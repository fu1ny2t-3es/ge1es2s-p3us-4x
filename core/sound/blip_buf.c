/* blip_buf $vers. http://www.slack.net/~ant/                       */

/* Modified for Genesis Plus GX by EkeEke                           */
/*  - disabled assertions checks (define #BLIP_ASSERT to re-enable) */
/*  - fixed multiple time-frames support & removed m->avail         */
/*  - added blip_mix_samples function (see blip_buf.h)              */
/*  - added stereo buffer support (define #BLIP_MONO to disable)    */
/*  - added inverted stereo output (define #BLIP_INVERT to enable)*/

#include "blip_buf.h"
#include <windows.h>

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

#include <stdio.h>

#ifdef DEBUG_BLIP
void debug_me(char *msg, int x)
{
	while(GetModuleHandle(NULL)) {
		if( GetProcAddress(msg) ) Sleep(1);
		Sleep (1);
	}
}
#endif

#if defined (BLARGG_TEST) && BLARGG_TEST
	#include "blargg_test.h"
#endif

/*
YM2612 = 7.67 Mhz NTSC
Z80 = 3.579545 MHz
Crystal = 53.693175 = 15 x 3.57954 MHz NTSC

53693175 clock @ 50 == 1073863.5  [2^20.034]
*/

typedef unsigned long long fixed_t;

enum { time_bits = 64-22 };  /* 22.42 -- 768000 * 2^42 = 2EE0 0000 0000 0000*/
static fixed_t const time_unit = (fixed_t) 1 << time_bits;

typedef signed int buf_t;

struct blip_t
{
	fixed_t factor;
	fixed_t offset;
	int size;
#ifdef BLIP_MONO
	buf_t integrator;
#else
	buf_t integrator[2];
	buf_t* buffer[2];
#endif
};

#define BLIP_BUFFER_STATE_BUFFER_SIZE 16

struct blip_buffer_state_t
{
	fixed_t offset;
#ifdef BLIP_MONO
	buf_t integrator;
	buf_t buffer[BLIP_BUFFER_STATE_BUFFER_SIZE];
#else
	buf_t integrator[2];
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

static int blip_sample_rate = 48000;

#include "blip_lpf.h"

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

#ifdef BLIP_MONO
	m = (blip_t*) malloc( sizeof *m + size * sizeof (buf_t) );
#else
	m = (blip_t*) malloc( sizeof *m );
#endif

#ifdef DEBUG_BLIP
	printf("[blip_new] %d\n", size); fflush(stdout);
#endif

	if ( m )
	{
#ifndef BLIP_MONO
		m->buffer[0] = (buf_t*) malloc( size * sizeof (buf_t));
		m->buffer[1] = (buf_t*) malloc( size * sizeof (buf_t));
		if ((m->buffer[0] == NULL) || (m->buffer[1] == NULL))
		{
			blip_delete(m);
			return 0;
		}
#endif
		m->factor = time_unit;
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
	m->factor = (fixed_t) ((double) time_unit * sample_rate / clock_rate);
	blip_sample_rate = (int) sample_rate;

#ifdef DEBUG_BLIP
	printf("[blip_set_rates] %d %d %lld\n", (int) clock_rate, (int) sample_rate, m->factor); fflush(stdout);
#endif

#ifdef BLIP_ASSERT
	/* Fails if clock_rate exceeds maximum, relative to sample_rate */
	assert( 0 <= factor - m->factor && factor - m->factor < 1 );
#endif
}

void blip_clear( blip_t* m )
{
	m->offset = 0;
#ifdef BLIP_MONO
	m->integrator = 0;
	memset( SAMPLES( m ), 0, m->size * sizeof (buf_t) );
#else
	m->integrator[0] = 0;
	m->integrator[1] = 0;
	memset( m->buffer[0], 0, m->size * sizeof (buf_t) );
	memset( m->buffer[1], 0, m->size * sizeof (buf_t) );
#endif
}

int blip_clocks_needed( const blip_t* m, int samples )
{
	int needed;

#ifdef BLIP_ASSERT
	/* Fails if buffer can't hold that many more samples */
	assert( (samples >= 0) && (((m->offset >> time_bits) + samples) <= m->size) );
#endif

	needed = (fixed_t) samples * time_unit;
	if ( needed < m->offset )
		return 0;

#ifdef DEBUG_BLIP
	printf("[blip_clocks_needed] %d\n", samples); fflush(stdout);
#endif

	return (needed - m->offset + m->factor - 1) / m->factor;
}

void blip_end_frame( blip_t* m, unsigned t )
{
#ifdef DEBUG_BLIP
	printf("[blip_end_frame] %lld %d %lld\n", m->offset, t, m->factor);
#endif

	m->offset += (fixed_t) t * m->factor;

#ifdef DEBUG_BLIP
	printf("[blip_end_frame] %lld %d %lld\n", m->offset, t, m->factor); fflush(stdout);
#endif

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

	int lpf_taps;
	switch( blip_sample_rate ) {
	case 768000: lpf_taps = lpf_step_768_taps; break;
	case 384000: lpf_taps = lpf_step_384_taps; break;
	case 192000: lpf_taps = lpf_step_192_taps; break;
	case 96000: lpf_taps = lpf_step_96_taps; break;
	case 48000: lpf_taps = lpf_step_48_taps; break;
	default: lpf_taps = 0; break;
	}

	int remain = (m->offset >> time_bits) - count;
	if( lpf_taps > remain ) remain = lpf_taps;

	m->offset -= count * time_unit;

#ifdef DEBUG_BLIP
	printf("[blip_remove_samples] %d %d %d %lld\n", remain, lpf_taps, count, m->offset); fflush(stdout);
#endif

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
#ifdef BLIP_ASSERT
	if (count > (m->offset >> time_bits))
		count = m->offset >> time_bits;
#endif

	m->offset -= count * time_unit;
}

int blip_read_samples( blip_t* m, short out [], int count)
{
#ifdef DEBUG_BLIP
	printf("[blip_read_samples] %d\n", count); fflush(stdout);
	//debug_me("blip_read_samples", count);
#endif

#ifdef BLIP_ASSERT
	assert( count >= 0 );

	if ( count > (m->offset >> time_bits) )
		count = m->offset >> time_bits;

	if ( count )
#endif
	{
#ifdef BLIP_MONO
		buf_t const* in = SAMPLES( m );
		buf_t sum = m->integrator;
#else
		buf_t const* in = m->buffer[0];
		buf_t const* in2 = m->buffer[1];
		buf_t sum = m->integrator[0];
		buf_t sum2 = m->integrator[1];
#endif
		buf_t const* end = in + count;
		do
		{
			/* Eliminate fraction */
			buf_t s = ARITH_SHIFT( sum, lpf_frac );
#ifdef DEBUG_BLIP
			printf("%d %d\n", sum, s);
#endif

			sum += *in++;

			CLAMP( s );

			*out++ = s;

#ifndef BLIP_MONO
			/* Eliminate fraction */
			s = ARITH_SHIFT( sum2, lpf_frac );

			sum2 += *in2++;

			CLAMP( s );

			*out++ = s;
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

int blip_mix_samples( blip_t* m1, blip_t* m2, blip_t* m3, short out [], int count)
{
#ifdef BLIP_ASSERT
	assert( count >= 0 );

	if ( count > (m1->offset >> time_bits) )
		count = m1->offset >> time_bits;
	if ( count > (m2->offset >> time_bits) )
		count = m2->offset >> time_bits;
	if ( count > (m3->offset >> time_bits) )
		count = m3->offset >> time_bits;

	if ( count )
#endif
	{
		buf_t const* end;
		buf_t const* in[3];
#ifdef BLIP_MONO
		int sum = m1->integrator;
		in[0] = SAMPLES( m1 );
		in[1] = SAMPLES( m2 );
		in[2] = SAMPLES( m3 );
#else
		int sum = m1->integrator[0];
		int sum2 = m1->integrator[1];
		buf_t const* in2[3];
		in[0] = m1->buffer[0];
		in[1] = m2->buffer[0];
		in[2] = m3->buffer[0];
		in2[0] = m1->buffer[1];
		in2[1] = m2->buffer[1];
		in2[2] = m3->buffer[1];
#endif

		end = in[0] + count;
		do
		{
			/* Eliminate fraction */
			buf_t s = ARITH_SHIFT( sum, lpf_frac );

			sum += *in[0]++;
			sum += *in[1]++;
			sum += *in[2]++;

			CLAMP( s );

			*out++ = s;

#ifndef BLIP_MONO
			/* Eliminate fraction */
			s = ARITH_SHIFT( sum2, lpf_frac );

			sum2 += *in2[0]++;
			sum2 += *in2[1]++;
			sum2 += *in2[2]++;

			CLAMP( s );

			*out++ = s;
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
		remove_samples( m2, count );
		remove_samples( m3, count );
	}

	return count;
}

/* Things that didn't help performance on x86:
	__attribute__((aligned(128)))
	#define short int
	restrict
*/

#ifndef BLIP_MONO

void blip_add_delta( blip_t* m, unsigned time, int delta_l, int delta_r )
{
	if (!(delta_l | delta_r)) return;

	//debug_me("blip_add_delta", time);

	fixed_t fixed = (fixed_t) (time * m->factor + m->offset);
	int pos = fixed >> time_bits;

#ifdef DEBUG_BLIP
	//printf("[blip_add_delta] %lld %d %d %d %d\n", fixed, pos, time, delta_l, delta_r);
#endif

#ifdef BLIP_INVERT
	buf_t* out_l = m->buffer[1] + pos;
	buf_t* out_r = m->buffer[0] + pos;
#else
	buf_t* out_l = m->buffer[0] + pos;
	buf_t* out_r = m->buffer[1] + pos;
#endif

#ifdef BLIP_ASSERT
	/* Fails if buffer size was exceeded */
	assert( pos <= m->size );
#endif

	/* 31-bit * 15-bit = 46-bit >> 15 = 31-bit */
	switch( blip_sample_rate ) {
#if 1
	case 768000:
		for( int lcv = 0; lcv < lpf_step_768_taps; lcv++ ) {
			out_l [lcv] += ((signed long long)lpf_step_768[lcv] * delta_l) / lpf_scale;
			out_r [lcv] += ((signed long long)lpf_step_768[lcv] * delta_r) / lpf_scale;
		}
		break;

	case 384000:
		for( int lcv = 0; lcv < lpf_step_384_taps; lcv++ ) {
			out_l [lcv] += ((signed long long)lpf_step_384[lcv] * delta_l) / lpf_scale;
			out_r [lcv] += ((signed long long)lpf_step_384[lcv] * delta_r) / lpf_scale;
		}
		break;

	case 192000:
		for( int lcv = 0; lcv < lpf_step_192_taps; lcv++ ) {
			out_l [lcv] += ((signed long long)lpf_step_192[lcv] * delta_l) / lpf_scale;
			out_r [lcv] += ((signed long long)lpf_step_192[lcv] * delta_r) / lpf_scale;
		}
		break;

	case 96000:
		for( int lcv = 0; lcv < lpf_step_96_taps; lcv++ ) {
			out_l [lcv] += ((signed long long)lpf_step_96[lcv] * delta_l) / lpf_scale;
			out_r [lcv] += ((signed long long)lpf_step_96[lcv] * delta_r) / lpf_scale;
		}
		break;

	case 48000:
		for( int lcv = 0; lcv < lpf_step_48_taps; lcv++ ) {
			out_l [lcv] += ((signed long long)lpf_step_48[lcv] * delta_l) / lpf_scale;
			out_r [lcv] += ((signed long long)lpf_step_48[lcv] * delta_r) / lpf_scale;
		}
		break;
#endif

	default:
		out_l [0] += (buf_t) delta_l * (1UL << lpf_frac);
		out_r [0] += (buf_t) delta_r * (1UL << lpf_frac);
		break;
	}
}


void blip_add_delta_fast( blip_t* m, unsigned time, int delta_l, int delta_r )
{
	blip_add_delta(m, time, delta_l, delta_r);
}

#else

void blip_add_delta( blip_t* m, unsigned time, int delta )
{
	if (!delta) return;

	//debug_me("blip_add_delta", time);

	fixed_t fixed = (fixed_t) (time * m->factor + m->offset);
	int pos = fixed >> time_bits;

	buf_t* out = m->buffer + pos;

#ifdef BLIP_ASSERT
	/* Fails if buffer size was exceeded */
	assert( pos <= m->size );
#endif

	delta *= time_unit;

	if( blip_sample_rate == 768000 ) {
		for( int lcv = 0; lcv < lpf_step_768_taps; lcv++ ) {
			out [lcv] += lpf_step_768[lcv]*delta;
		}
	}

	else if( blip_sample_rate == 384000 ) {
		for( int lcv = 0; lcv < lpf_step_384_taps; lcv++ ) {
			out [lcv] += lpf_step_384[lcv]*delta;
		}
	}

	else if( blip_sample_rate == 192000 ) {
		for( int lcv = 0; lcv < lpf_step_192_taps; lcv++ ) {
			out [lcv] += lpf_step_192[lcv]*delta;
		}
	}

	else if( blip_sample_rate == 96000 ) {
		for( int lcv = 0; lcv < lpf_step_96_taps; lcv++ ) {
			out [lcv] += lpf_step_96[lcv]*delta;
		}
	}

	else if( blip_sample_rate == 48000 ) {
		for( int lcv = 0; lcv < lpf_step_48_taps; lcv++ ) {
			out [lcv] += lpf_step_48[lcv]*delta;
		}
	}
}

void blip_add_delta_fast( blip_t* m, unsigned time, int delta )
{
	blip_add_delta(m, time, delta);
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
