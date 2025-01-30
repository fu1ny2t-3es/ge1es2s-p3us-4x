/* https://fiiir.com/ */


/* 1.16.15 */
enum { lpf_frac = 15 };
enum { lpf_scale = 32768 };


#define LPF_TAPS(x) (buf_t) ((double) (x) * (double) (1UL << lpf_frac) * (double) lpf_scale)


#include "blip_lpf_768K.h"
#include "blip_lpf_384K.h"
#include "blip_lpf_192K.h"
#include "blip_lpf_96K.h"
#include "blip_lpf_48K.h"
