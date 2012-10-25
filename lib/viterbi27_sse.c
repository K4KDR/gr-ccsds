/* K=7 r=1/2 Viterbi decoder for x86 SIMD
 * Revamped April 2009 to decode streams as well as blocks, run in 64-bit mode
 * Copyright 2009 Phil Karn, KA9Q
 * May be used under the terms of the GNU General Public License
 *
 * $Id$
 */
#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include "fec.h"
#include "sse.h"

#define V27MAGIC 0x0102fffe

typedef union { unsigned char c[64]; v16qi v[4]; } metric_t;
typedef union { unsigned char c[8]; unsigned short s[4]; } decision_t;
union branchtab27 { unsigned char c[32]; v16qi v[2];} Branchtab27[2];
static int Init = 0;

/* State info for instance of Viterbi decoder */
struct v27 {
  metric_t metrics; /* path metric buffer */
  decision_t *dp;          /* Pointer to current decision */
  int length;             /* Length of decision array, bits */
  int magic;

  /* Decision array follows this header */
};

/* Initialize Viterbi decoder for start of new frame */
int init_viterbi27(void *p,int starting_state){
  struct v27 *vp = p;
  int i;

  if(p == NULL)
    return -1;
  assert(vp->magic == V27MAGIC);
  for(i=0;i<64;i++)
    vp->metrics.c[i] = 63;

  vp->dp = (decision_t *)(vp + 1);
  vp->metrics.c[starting_state & 63] = 0; /* Bias known start state */
  return 0;
}

void set_viterbi27_polynomial(int polys[2]){
  int state;

  for(state=0;state < 32;state++){
    Branchtab27[0].c[state] = (polys[0] < 0) ^ parityb((2*state) & abs(polys[0])) ? 255 : 0;
    Branchtab27[1].c[state] = (polys[1] < 0) ^ parityb((2*state) & abs(polys[1])) ? 255 : 0;
  }
  Init++;
}

/* Create a new instance of a Viterbi decoder */
void *create_viterbi27(int len){
  struct v27 *vp;

  if(!Init){
    int polys[2] = { V27POLYA, V27POLYB };
    set_viterbi27_polynomial(polys);
  }
  /* Ordinary malloc() only returns 8-byte alignment, we need 16
   * The array of decisions immediately follows the struct v27
   */
  vp = (struct v27 *)MALLOC(sizeof(struct v27) + len*sizeof(decision_t));
  if(vp != NULL){
    vp->dp = (decision_t *)(vp + 1); /* Right after the structure */
    vp->length = len+6;
    vp->magic = V27MAGIC;
    init_viterbi27(vp,0);
  }

  return vp;
}

/* Viterbi chainback */
int chainback_viterbi27(
      void *p,
      unsigned char *data, /* Decoded output data, MSB first */
      unsigned int nbits, /* Number of data bits */
      unsigned int endstate){ /* Terminal encoder state */
  struct v27 *vp = p;
  decision_t *d;

  if(vp == NULL)
    return -1;

  assert(vp->magic == V27MAGIC);

  /* Endstate = -1 means find the best path metric */
  if(endstate == -1){
    int i;
    unsigned char bestmetric;

    endstate = 63;
    bestmetric = vp->metrics.c[63];

    for(i=62;i>=0;i--){
      if((int)(vp->metrics.c[i] - bestmetric) < 0){
	endstate = i;
	bestmetric = vp->metrics.c[i];
      }
    }
  }
  /* Make room beyond the end of the encoder register so we can
   * accumulate a full byte of decoded data
   */
  endstate %= 64;
  endstate <<= 2;

  /* The store into data[] only needs to be done every 8 bits.
   * But this avoids a conditional branch, and the writes will
   * combine in the cache anyway
   */
  d = vp->dp;
  while(nbits-- != 0){
    int k;

    if(--d < (decision_t *)(vp + 1))
      d = (decision_t *)(vp + 1) + vp->length - 1; /* Last element of decision array */
    k = (d->c[(endstate>>2)/8] >> ((endstate>>2)%8)) & 1;
    data[nbits>>3] = endstate = (endstate >> 1) | (k << 7);
  }
  return 0;
}

/* Delete instance of a Viterbi decoder */
void delete_viterbi27(void *p){
  struct v27 *vp = p;

  if(vp != NULL){
    free(vp);
  }
}

/* I wrote this version with compiler intrinsics for the vector operations a long time ago, but didn't use
 * it because my handcrafted asm version was faster. But times have changed. Bit rot set in and the asm
 * code no longer assembled with gasm. It would also have to be rewritten to work in 64-bit mode, so instead
 * I've retired the asm version and am now maintaining this one.
 */
int update_viterbi27_blk(void *p,unsigned char *syms,int nbits){
  struct v27 *vp = p;
  register v16qi current_metric0,current_metric1,current_metric2,current_metric3;
  register v16qi new_metric1;

  if(p == NULL)
    return -1;
  
  assert(vp->magic == V27MAGIC);
  /* Registers version speed: 43.4 Mb/s on laptop */
  current_metric0 = vp->metrics.v[0];
  current_metric1 = vp->metrics.v[1];
  current_metric2 = vp->metrics.v[2];
  current_metric3 = vp->metrics.v[3];

  while(nbits--){
    register v16qi sym0v,sym1v;
    register v16qi decision0,decision1,metric,m0,m1,m2,m3;
    
    /* Splat the 0th symbol across sym0v, the 1st symbol across sym1v */
    sym0v = _mm_set1_epi8(syms[0]);
    sym1v = _mm_set1_epi8(syms[1]);
    syms += 2;

    /* Unrolling the loop that used to be here gave a dramatic speedup in 64-bit mode, from ~27 Mb/s to 43 Mb/s! */

    /* Form branch metrics
     * Only the high 3 bits of each symbol are used
     */
    
    metric = _mm_avg_epu8(_mm_xor_si128(Branchtab27[0].v[0],sym0v),_mm_xor_si128(Branchtab27[1].v[0],sym1v));
    /* There's no packed bytes right shift in SSE2, so we use the word version and mask
     * (I'm *really* starting to miss Altivec...)
     */
    //metric = _mm_srli_epi16(metric,3);
    //metric = _mm_srli_epi16(metric,4);
    metric = _mm_srli_epi16(metric,5);

    //metric = _mm_and_si128(metric,_mm_set1_epi8(31));
    //metric = _mm_and_si128(metric,_mm_set1_epi8(15));
    metric = _mm_and_si128(metric,_mm_set1_epi8(7));
    
    /* Add branch metrics to path metrics */
    m0 = _mm_add_epi8(current_metric0,metric);
    m3 = _mm_add_epi8(current_metric2,metric);

    /* Inverse metrics */
    //metric = _mm_sub_epi8(_mm_set1_epi8(31),metric);
    //metric = _mm_sub_epi8(_mm_set1_epi8(15),metric);
    metric = _mm_sub_epi8(_mm_set1_epi8(7),metric);

    m1 = _mm_add_epi8(current_metric2,metric);
    m2 = _mm_add_epi8(current_metric0,metric);
    
    /* Compare and select, using modulo arithmetic */
    decision0 = _mm_cmpgt_epi8(_mm_sub_epi8(m0,m1),_mm_setzero_si128());
    decision1 = _mm_cmpgt_epi8(_mm_sub_epi8(m2,m3),_mm_setzero_si128());

#ifdef __SSE4_1__
    /* Speed on MacBook Pro, Core 2 Duo T9300 @ 2.50 GHz, SSE 4.1 enabled: 42.93 Mb/s */
    /* Altivec has had a select instruction since day 1... */
    m0 = _mm_blendv_epi8(m0,m1,decision0);
    m1 = _mm_blendv_epi8(m2,m3,decision1);
#else
    /* SSE2 speed: 42.26 Mb/s */
    m0 = _mm_or_si128(_mm_and_si128(decision0,m1),_mm_andnot_si128(decision0,m0));
    m1 = _mm_or_si128(_mm_and_si128(decision1,m3),_mm_andnot_si128(decision1,m2));
#endif
    
    /* Pack each set of decisions into 16 bits */
    vp->dp->s[0] = _mm_movemask_epi8(_mm_unpacklo_epi8(decision0,decision1));
    vp->dp->s[1] = _mm_movemask_epi8(_mm_unpackhi_epi8(decision0,decision1));
    
    /* Store surviving metrics */
    current_metric0 = _mm_unpacklo_epi8(m0,m1);
    new_metric1 = _mm_unpackhi_epi8(m0,m1); /* Save temporarily as we still need current_metric1 */

    /* Second set of 16 butterflies */

    /* Form branch metrics */
    metric = _mm_avg_epu8(_mm_xor_si128(Branchtab27[0].v[1],sym0v),_mm_xor_si128(Branchtab27[1].v[1],sym1v));

    //metric = _mm_srli_epi16(metric,3);
    //metric = _mm_srli_epi16(metric,4);
    metric = _mm_srli_epi16(metric,5);

    //metric = _mm_and_si128(metric,_mm_set1_epi8(31));
    //metric = _mm_and_si128(metric,_mm_set1_epi8(15));
    metric = _mm_and_si128(metric,_mm_set1_epi8(7));

    /* Add branch metrics to path metrics */
    m0 = _mm_add_epi8(current_metric1,metric);
    m3 = _mm_add_epi8(current_metric3,metric);

    /* Inverse metrics */
    //metric = _mm_sub_epi8(_mm_set1_epi8(31),metric);
    //metric = _mm_sub_epi8(_mm_set1_epi8(15),metric);
    metric = _mm_sub_epi8(_mm_set1_epi8(7),metric);

    m1 = _mm_add_epi8(current_metric3,metric);
    m2 = _mm_add_epi8(current_metric1,metric);
    
    /* Compare and select, using modulo arithmetic */
    decision0 = _mm_cmpgt_epi8(_mm_sub_epi8(m0,m1),_mm_setzero_si128());
    decision1 = _mm_cmpgt_epi8(_mm_sub_epi8(m2,m3),_mm_setzero_si128());
    
#ifdef __SSE4_1__
    /* Speed on MacBook Pro, Core 2 Duo T9300 @ 2.50 GHz, SSE 4.1 enabled: 42.93 Mb/s */
    /* Altivec has had a select instruction since day 1... */
    m0 = _mm_blendv_epi8(m0,m1,decision0);
    m1 = _mm_blendv_epi8(m2,m3,decision1);
#else
    /* SSE2 speed: 42.26 Mb/s */
    m0 = _mm_or_si128(_mm_and_si128(decision0,m1),_mm_andnot_si128(decision0,m0));
    m1 = _mm_or_si128(_mm_and_si128(decision1,m3),_mm_andnot_si128(decision1,m2));
#endif

    /* Pack each set of decisions into 16 bits */
    vp->dp->s[2] = _mm_movemask_epi8(_mm_unpacklo_epi8(decision0,decision1));
    vp->dp->s[3] = _mm_movemask_epi8(_mm_unpackhi_epi8(decision0,decision1));
    
    /* Store surviving metrics */
    current_metric1 = new_metric1;
    current_metric2 = _mm_unpacklo_epi8(m0,m1);
    current_metric3 = _mm_unpackhi_epi8(m0,m1);

    /* Wrap around decision memory if necessary */
    if(++(vp->dp) == (decision_t *)(vp + 1) + vp->length)
      vp->dp = (decision_t *)(vp + 1);

  }
  vp->metrics.v[0] = current_metric0;
  vp->metrics.v[1] = current_metric1;
  vp->metrics.v[2] = current_metric2;
  vp->metrics.v[3] = current_metric3;

  return 0;
}
#ifdef __cplusplus
}
#endif