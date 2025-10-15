/* blip_buf $vers. http://www.slack.net/~ant/                       */

/* Modified for Genesis Plus GX by EkeEke                           */
/*  - disabled assertions checks (define #BLIP_ASSERT to re-enable) */
/*  - fixed multiple time-frames support & removed m->avail         */
/*  - added blip_mix_samples function (see blip_buf.h)              */
/*  - added stereo buffer support (define #BLIP_MONO to disable)    */
/*  - added inverted stereo output (define #BLIP_INVERT to enable)*/

#include "blip_buf.h"

#ifdef BLIP_ASSERT
#include <assert.h>
#endif
#include <limits.h>
#include <string.h>
#include <stdlib.h>

/* Library Copyright (C) 2003-2009 Shay Green. This library is free software;
you can redistribute it and/or modify it under the terms of the GNU Lesser
General Public License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version. This
library is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
details. You should have received a copy of the GNU Lesser General Public
License along with this module; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA */

#if defined (BLARGG_TEST) && BLARGG_TEST
	#include "blargg_test.h"
#endif

/*
YM = 53693175
PSG = 32552
CDDA = 44100
External = 48000  [Paprium, YX-5000]
*/

#define BAND_LIMITED_WIDTH (1 << 6)  /* 64 */
#define BAND_LIMITED_PHASES (1 << 12)  /* 4096 */
#define BAND_LIMITED_ONE (1 << 13)  /* 1.18.13 = 7x overdrive */

#define M_PI 3.14159265358979323846

typedef unsigned long long fixed_t;
enum { pre_shift = 0 };

enum { time_bits = pre_shift + 40 };  /* 24.40 ~ 768000 */

static fixed_t const time_unit = (fixed_t) 1 << time_bits;

enum { bass_shift  = 9 }; /* affects high-pass filter breakpoint frequency */
enum { end_frame_extra = 2 }; /* allows deltas slightly after frame length */

enum { half_width  = 64/2 };
enum { buf_extra   = half_width*2 + end_frame_extra };
enum { phase_bits  = 12 };
enum { phase_count = 1 << phase_bits };
enum { delta_bits  = 13 };
enum { delta_unit  = 1 << delta_bits };
enum { frac_bits = time_bits - pre_shift };
enum { phase_shift = frac_bits - phase_bits };

/* We could eliminate avail and encode whole samples in offset, but that would
limit the total buffered samples to blip_max_frame. That could only be
increased by decreasing time_bits, which would reduce resample ratio accuracy.
*/

typedef int buf_t;

struct blip_t
{
	fixed_t factor;
	fixed_t offset;
	int size;
#ifdef BLIP_MONO
	int integrator;
#else
  int integrator[2];
  buf_t* buffer[2];
#endif
};

#define BLIP_BUFFER_STATE_BUFFER_SIZE 16

struct blip_buffer_state_t
{
	fixed_t offset;
#ifdef BLIP_MONO
	int integrator;
	buf_t buffer[BLIP_BUFFER_STATE_BUFFER_SIZE];
#else
	int integrator[2];
	buf_t buffer[2][BLIP_BUFFER_STATE_BUFFER_SIZE];
#endif
};

#ifdef BLIP_MONO
/* probably not totally portable */
#define SAMPLES( blip ) ((buf_t*) ((blip) + 1))
#endif

/* Arithmetic (sign-preserving) right shift */
#define ARITH_SHIFT( n, shift ) \
	((n) >> (shift))

enum { max_sample = +32767 };
enum { min_sample = -32768 };

#define CLAMP( n ) \
	{\
		if ( n > max_sample ) n = max_sample;\
    else if ( n < min_sample) n = min_sample;\
	}

#if 1
#include "blip-4096-64.h"

static void __attribute__((constructor)) band_limited_init(void) {}

#else
static int band_limited_steps[BAND_LIMITED_PHASES][BAND_LIMITED_WIDTH];

static void __attribute__((constructor)) band_limited_init(void)
{
	static int once = 0;

	if( once ) return;
	once = 1;

    const unsigned master_size = BAND_LIMITED_WIDTH * BAND_LIMITED_PHASES;
    double *master = malloc(master_size  * sizeof(*master));
    memset(master, 0, master_size  * sizeof(*master));
    
    const double lowpass = 15.0 / 16.0; // 1.0 means using Nyquist as the exact cutoff
    const double to_angle = M_PI / BAND_LIMITED_PHASES * lowpass;
    double sum = 0;

    for (signed i = 0; i < master_size; i++) {
        // Exact Blackman window
        const double a0 = 7938 / 18608.0;
        const double a1 = 9240 / 18608.0;
        const double a2 = 1430 / 18608.0;
        double window_angle = (2.0 * M_PI * i) / (master_size);
        double window = a0 - a1 * cos(window_angle) + a2 * cos(2 * window_angle);
        
        double angle = (i - (signed)master_size / 2) * to_angle;
        sum += master[i] = ((angle == 0)? 1 : sin(angle) / angle) * window;
    }
    
    for (signed i = 0; i < master_size; i++) {
        master[i] /= sum;
    }
    
    for (signed phase = 0; phase < BAND_LIMITED_PHASES; phase++) {
        int error = BAND_LIMITED_ONE;
        for (signed i = 0; i < BAND_LIMITED_WIDTH; i++) {
            double sum = 0;
            for (signed j = 0; j < BAND_LIMITED_PHASES; j++) {
                signed index = i * BAND_LIMITED_PHASES - phase + j;
                if (index >= 0) {
                    sum += master[index];
                }
            }
            int cur = sum * BAND_LIMITED_ONE;
            error -= cur;
            band_limited_steps[phase][i] = cur;
        }
        
        // Make sure the deltas sum to 1.0
        band_limited_steps[phase][BAND_LIMITED_WIDTH / 2] += error;
    }
    free(master);


	if(0) {
		FILE *fp_out = fopen("blip-.h", "w");

		fprintf(fp_out, "static int band_limited_steps[%d][%d] = {\n", BAND_LIMITED_PHASES, BAND_LIMITED_WIDTH);
		for( int lcv1 = 0; lcv1 < BAND_LIMITED_PHASES; lcv1++ ) {
			fprintf(fp_out, "\t{ ");
			for( int lcv2 = 0; lcv2 < BAND_LIMITED_WIDTH; lcv2++ ) {
				fprintf(fp_out, "%d, ", band_limited_steps[lcv1][lcv2]);
			}
			fprintf(fp_out, "},\n");
		}
		fprintf(fp_out, "};\n");
		fclose(fp_out);
	}
}
#endif

#ifdef BLIP_ASSERT
static void check_assumptions( void )
{
	int n;
	
	#if INT_MAX < 0x7FFFFFFF || UINT_MAX < 0xFFFFFFFF
		#error "int must be at least 32 bits"
	#endif
	
	assert( (-3 >> 1) == -2 ); /* right shift must preserve sign */
	
	n = max_sample * 2;
	CLAMP( n );
	assert( n == max_sample );
	
	n = min_sample * 2;
	CLAMP( n );
	assert( n == min_sample );
	
	assert( blip_max_ratio <= time_unit );
	assert( blip_max_frame <= (fixed_t) -1 >> time_bits );
}
#endif

blip_t* blip_new( int size )
{
	blip_t* m;
#ifdef BLIP_ASSERT
	assert( size >= 0 );
#endif
  
	band_limited_init();

#ifdef BLIP_MONO
	m = (blip_t*) malloc( sizeof *m + (size + buf_extra) * sizeof (buf_t) );
#else
	m = (blip_t*) malloc( sizeof *m );
#endif

	if ( m )
	{
#ifndef BLIP_MONO
    m->buffer[0] = (buf_t*) malloc( (size + buf_extra) * sizeof (buf_t));
    m->buffer[1] = (buf_t*) malloc( (size + buf_extra) * sizeof (buf_t));
    if ((m->buffer[0] == NULL) || (m->buffer[1] == NULL))
    {
      blip_delete(m);
      return 0;
    }
#endif
		m->factor = time_unit / blip_max_ratio;
		m->size   = size;
		blip_clear( m );
#ifdef BLIP_ASSERT
		check_assumptions();
#endif
  }
	return m;
}

void blip_delete( blip_t* m )
{
	if ( m != NULL )
	{
#ifndef BLIP_MONO
    if (m->buffer[0] != NULL)
      free(m->buffer[0]);
    if (m->buffer[1] != NULL)
      free(m->buffer[1]);
#endif
    /* Clear fields in case user tries to use after freeing */
		memset( m, 0, sizeof *m );
		free( m );
	}
}

void blip_set_rates( blip_t* m, double clock_rate, double sample_rate )
{
	double factor = time_unit * sample_rate / clock_rate;
	m->factor = (fixed_t) factor;
	
#if DEBUG
	if(!debug) debug = fopen("debug.txt","w");
	fprintf(debug, "rates = %llu - %f %f %f\n", m->factor, clock_rate, sample_rate, factor);
#endif

#ifdef BLIP_ASSERT
	/* Fails if clock_rate exceeds maximum, relative to sample_rate */
	assert( 0 <= factor - m->factor && factor - m->factor < 1 );
#endif
  
/* Avoid requiring math.h. Equivalent to
	m->factor = (int) ceil( factor ) */
	if ( m->factor < factor )
		m->factor++;
	
	/* At this point, factor is most likely rounded up, but could still
	have been rounded down in the floating-point calculation. */
}

void blip_clear( blip_t* m )
{
	/* We could set offset to 0, factor/2, or factor-1. 0 is suitable if
	factor is rounded up. factor-1 is suitable if factor is rounded down.
	Since we don't know rounding direction, factor/2 accommodates either,
	with the slight loss of showing an error in half the time. Since for
	a 64-bit factor this is years, the halving isn't a problem. */

	m->offset = m->factor / 2;
#ifdef BLIP_MONO
	m->integrator = 0;
	memset( SAMPLES( m ), 0, (m->size + buf_extra) * sizeof (buf_t) );
#else
	m->integrator[0] = 0;
	m->integrator[1] = 0;
	memset( m->buffer[0], 0, (m->size + buf_extra) * sizeof (buf_t) );
	memset( m->buffer[1], 0, (m->size + buf_extra) * sizeof (buf_t) );
#endif
}

int blip_clocks_needed( const blip_t* m, int samples )
{
	fixed_t needed;

#ifdef BLIP_ASSERT
	/* Fails if buffer can't hold that many more samples */
	assert( (samples >= 0) && (((m->offset >> time_bits) + samples) <= m->size) );
#endif

  needed = (fixed_t) samples * time_unit;
	if ( needed < m->offset )
		return 0;

	return (needed - m->offset + m->factor - 1) / m->factor;
}

void blip_end_frame( blip_t* m, unsigned t )
{
	m->offset += t * m->factor;

#ifdef BLIP_ASSERT
	/* Fails if buffer size was exceeded */
  assert( (m->offset >> time_bits) <= m->size );
#endif
}

int blip_samples_avail( const blip_t* m )
{
	return (m->offset >> time_bits);
}

static void remove_samples( blip_t* m, int count )
{
#ifdef BLIP_MONO
	buf_t* buf = SAMPLES( m );
#else
	buf_t* buf = m->buffer[0];
#endif
  int remain = (m->offset >> time_bits) + buf_extra - count;
  m->offset -= count * time_unit;

	memmove( &buf [0], &buf [count], remain * sizeof (buf_t) );
	memset( &buf [remain], 0, count * sizeof (buf_t) );
#ifndef BLIP_MONO
	buf = m->buffer[1];
	memmove( &buf [0], &buf [count], remain * sizeof (buf_t) );
	memset( &buf [remain], 0, count * sizeof (buf_t) );
#endif
}

int blip_discard_samples_dirty(blip_t* m, int count)
{
	if (count > (m->offset >> time_bits))
		count = m->offset >> time_bits;

	m->offset -= count * time_unit;
}

int blip_read_samples( blip_t* m, short out [], int count)
{
#ifdef BLIP_ASSERT
	assert( count >= 0 );

	if ( count > (m->offset >> time_bits) )
		count = m->offset >> time_bits;

	if ( count )
#endif
  {
#ifdef BLIP_MONO
		buf_t const* in = SAMPLES( m );
		int sum = m->integrator;
#else
		buf_t const* in = m->buffer[0];
		buf_t const* in2 = m->buffer[1];
		int sum = m->integrator[0];
		int sum2 = m->integrator[1];
#endif
		buf_t const* end = in + count;
		do
		{
			/* Eliminate fraction */
			int s = ARITH_SHIFT( sum, delta_bits );

			sum += *in++;

			CLAMP( s );

			*out++ = s;

			/* High-pass filter */
			//sum -= s << (delta_bits - bass_shift);

#ifndef BLIP_MONO
			/* Eliminate fraction */
			s = ARITH_SHIFT( sum2, delta_bits );

			sum2 += *in2++;

			CLAMP( s );

			*out++ = s;

			/* High-pass filter */
			//sum2 -= s << (delta_bits - bass_shift);
#endif
		}
		while ( in != end );

#ifdef BLIP_MONO
		m->integrator = sum;
#else
		m->integrator[0] = sum;
		m->integrator[1] = sum2;
#endif
		remove_samples( m, count );
	}

	return count;
}

int blip_mix_samples( blip_t* m1, blip_t** m2, int num, short out [], int count)
{
  int i;
#ifdef BLIP_ASSERT
  assert( num <= 3 );
  assert( count >= 0 );

  if ( count > (m1->offset >> time_bits) )
    count = m1->offset >> time_bits;
  for (i=0; i<num; i++)
    if ( count > (m2[i]->offset >> time_bits) )
      count = m2[i]->offset >> time_bits;

  if ( count )
#endif
  {
    buf_t const* end;
    buf_t const* in[4];
#ifdef BLIP_MONO
    int sum = m1->integrator;
    in[0] = SAMPLES( m1 );
    for (i=0; i<num; i++)
      in[i+1] = SAMPLES( m2[i] );
#else
    int sum = m1->integrator[0];
    int sum2 = m1->integrator[1];
    buf_t const* in2[4];
    in[0] = m1->buffer[0];
    in2[0] = m1->buffer[1];
    for (i=0; i<num; i++)
    {
      in[i+1] = m2[i]->buffer[0];
      in2[i+1] = m2[i]->buffer[1];
    }
#endif

    end = in[0] + count;
    do
    {
      /* Eliminate fraction */
      int s = ARITH_SHIFT( sum, delta_bits );

      sum += *in[0]++;
      for (i=1; i<num+1; i++)
        sum += *in[i]++;

      CLAMP( s );

      *out++ = s;

      /* High-pass filter */
      //sum -= s << (delta_bits - bass_shift);

#ifndef BLIP_MONO
      /* Eliminate fraction */
      s = ARITH_SHIFT( sum2, delta_bits );

      sum2 += *in2[0]++;
      for (i=1; i<num+1; i++)
        sum2 += *in2[i]++;

      CLAMP( s );

      *out++ = s;

      /* High-pass filter */
      //sum2 -= s << (delta_bits - bass_shift);
#endif
    }
    while ( in[0] != end );

#ifdef BLIP_MONO
    m1->integrator = sum;
#else
    m1->integrator[0] = sum;
    m1->integrator[1] = sum2;
#endif
    remove_samples( m1, count );
    for (i=0; i<num; i++)
      remove_samples( m2[i], count );
  }

  return count;
}

#ifndef BLIP_MONO

void blip_add_delta( blip_t* m, unsigned time, int delta_l, int delta_r )
{
  if (delta_l | delta_r)
  {
    fixed_t fixed = (fixed_t) ((time * m->factor + m->offset) >> pre_shift);
    int phase = (fixed >> phase_shift) & (phase_count - 1);
    int const* in = band_limited_steps [phase];
    int interp = (fixed >> (phase_shift - delta_bits)) & (delta_unit - 1);
    int pos = fixed >> frac_bits;

#ifdef BLIP_INVERT
    buf_t* out_l = m->buffer[1] + pos;
    buf_t* out_r = m->buffer[0] + pos;
#else
    buf_t* out_l = m->buffer[0] + pos;
    buf_t* out_r = m->buffer[1] + pos;
#endif

#ifdef BLIP_ASSERT
    /* Fails if buffer size was exceeded */
    assert( pos <= m->size + end_frame_extra );
#endif

	for( int lcv = 0; lcv < BAND_LIMITED_WIDTH; lcv++ ) {
		out_l [lcv] += in[lcv]*delta_l;
		out_r [lcv] += in[lcv]*delta_r;
	}
  }
}

void blip_add_delta_fast( blip_t* m, unsigned time, int delta_l, int delta_r )
{
  if (delta_l | delta_r)
  {
    fixed_t fixed = (fixed_t) ((time * m->factor + m->offset) >> pre_shift);
    int interp = (fixed >> (frac_bits - delta_bits)) & (delta_unit - 1);
    int pos = fixed >> frac_bits;

#ifdef STEREO_INVERT
    buf_t* out_l = m->buffer[1] + pos;
    buf_t* out_r = m->buffer[0] + pos;
#else
    buf_t* out_l = m->buffer[0] + pos;
    buf_t* out_r = m->buffer[1] + pos;
#endif

    int delta = delta_l * interp;

#ifdef BLIP_ASSERT
    /* Fails if buffer size was exceeded */
    assert( pos <= m->size + end_frame_extra );
#endif

    if (delta_l == delta_r)
    {
      delta_l = delta_l * delta_unit - delta;
      out_l[7] += delta_l;
      out_l[8] += delta;
      out_r[7] += delta_l;
      out_r[8] += delta;
    }
    else
    {
      out_l[7] += delta_l * delta_unit - delta;
      out_l[8] += delta;
      delta = delta_r * interp;
      out_r[7] += delta_r * delta_unit - delta;
      out_r[8] += delta;
    }
  }
}

#else

void blip_add_delta( blip_t* m, unsigned time, int delta )
{
	fixed_t fixed = (fixed_t) ((time * m->factor + m->offset) >> pre_shift);
	buf_t* out = SAMPLES( m ) + (fixed >> frac_bits);
	
	int phase = (fixed >> phase_shift) & (phase_count - 1);
	int const* in  = bl_step [phase];
	int const* rev = bl_step [phase_count - phase];
	
	int interp = (fixed >> (phase_shift - delta_bits)) & (delta_unit - 1);

#ifdef BLIP_ASSERT
	/* Fails if buffer size was exceeded */
	assert( out <= &SAMPLES( m ) [m->size + end_frame_extra] );
#endif

	for( int lcv = 0; lcv < BAND_LIMITED_WIDTH; lcv++ ) {
		out [lcv] += in[lcv]*delta;
	}
}

void blip_add_delta_fast( blip_t* m, unsigned time, int delta )
{
	fixed_t fixed = (fixed_t) ((time * m->factor + m->offset) >> pre_shift);
	buf_t* out = SAMPLES( m ) + (fixed >> frac_bits);
	
	int interp = fixed >> (frac_bits - delta_bits) & (delta_unit - 1);
	int delta2 = delta * interp;
	
#ifdef BLIP_ASSERT
  /* Fails if buffer size was exceeded */
	assert( out <= &SAMPLES( m ) [m->size + end_frame_extra] );
#endif
  
	out [7] += delta * delta_unit - delta2;
	out [8] += delta2;
}
#endif

void blip_save_buffer_state(const blip_t *buf, blip_buffer_state_t *state)
{
#ifdef BLIP_MONO
	state->integrator = buf->integrator;
	if (buf->buffer && buf->size >= BLIPSTATE_BUFFER_SIZE)
	{
		memcpy(state->buffer, buf->buffer, sizeof(state->buffer));
	}
#else
	int c;
	for (c = 0; c < 2; c++)
	{
		state->integrator[c] = buf->integrator[c];
		if (buf->buffer[c] && buf->size >= BLIP_BUFFER_STATE_BUFFER_SIZE)
		{
			memcpy(state->buffer[c], buf->buffer[c], sizeof(state->buffer[c]));
		}
	}
#endif
	state->offset = buf->offset;
}

void blip_load_buffer_state(blip_t *buf, const blip_buffer_state_t *state)
{
#ifdef BLIP_MONO
	state->integrator = buf->integrator;
	if (buf->buffer && buf->size >= BLIPSTATE_BUFFER_SIZE)
	{
		memcpy(state->buffer, buf->buffer, sizeof(state->buffer));
	}
#else
	int c;
	for (c = 0; c < 2; c++)
	{
		buf->integrator[c] = state->integrator[c];
		if (buf->buffer[c] && buf->size >= BLIP_BUFFER_STATE_BUFFER_SIZE)
		{
			memcpy(buf->buffer[c], state->buffer[c], sizeof(state->buffer[c]));
		}
	}
#endif
	buf->offset = (fixed_t)state->offset;
}

blip_buffer_state_t* blip_new_buffer_state()
{
	return (blip_buffer_state_t*)calloc(1, sizeof(blip_buffer_state_t));
}

void blip_delete_buffer_state(blip_buffer_state_t *state)
{
	if (state == NULL) return;
	memset(state, 0, sizeof(blip_buffer_state_t));
	free(state);
}
