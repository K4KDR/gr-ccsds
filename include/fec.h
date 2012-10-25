
/* User include file for libdsp2.0
 * Copyright 2009, Phil Karn, KA9Q
 * May be used under the terms of the GNU Lesser General Public License (LGPL)
 *
 * $Id$
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef _FEC_H_
#define _FEC_H_
#pragma once

/* r=1/2 k=7 convolutional encoder polynomials
 * The NASA-DSN convention is to use V27POLYA inverted, then V27POLYB
 * The CCSDS/NASA-GSFC convention is to use V27POLYB, then V27POLYA inverted
 */
#define	V27POLYA	0x6d
#define	V27POLYB	0x4f

void *create_viterbi27(int len);
void set_viterbi27_polynomial(int polys[2]);
int init_viterbi27(void *vp,int starting_state);
int update_viterbi27_blk(void *vp,unsigned char sym[],int npairs);
int chainback_viterbi27(void *vp, unsigned char *data,unsigned int nbits,unsigned int endstate);
void delete_viterbi27(void *vp);

/* r=1/2 k=9 convolutional encoder polynomials */
#define	V29POLYA	0x1af
#define	V29POLYB	0x11d

void *create_viterbi29(int len);
void set_viterbi29_polynomial(int polys[2]);
int init_viterbi29(void *vp,int starting_state);
int update_viterbi29_blk(void *vp,unsigned char syms[],int nbits);
int chainback_viterbi29(void *vp, unsigned char *data,unsigned int nbits,int endstate);
void delete_viterbi29(void *vp);

/* r=1/3 k=9 convolutional encoder polynomials */
#define	V39POLYA	0x1ed
#define	V39POLYB	0x19b
#define	V39POLYC	0x127

void *create_viterbi39(int len);
void set_viterbi39_polynomial(int polys[3]);
int init_viterbi39(void *vp,int starting_state);
int update_viterbi39_blk(void *vp,unsigned char syms[],int nbits);
int chainback_viterbi39(void *vp, unsigned char *data,unsigned int nbits,int endstate);
void delete_viterbi39(void *vp);

/* r=1/6 k=15 Cassini convolutional encoder polynomials without symbol inversion
 * dfree = 56
 * These bits may be left-right flipped from some textbook representations;
 * here I have the bits entering the shift register from the right (low) end
 *
 * Some other spacecraft use the same code, but with the polynomials in a different order.
 * E.g., Mars Pathfinder and STEREO swap POLYC and POLYD. All use alternate symbol inversion,
 * so use set_viterbi615_polynomial() as appropriate.
 */
#define	V615POLYA	042631
#define	V615POLYB	047245
#define V615POLYC       056507
#define V615POLYD       073363
#define V615POLYE       077267
#define V615POLYF       064537

void *create_viterbi615(int len);
void set_viterbi615_polynomial(int polys[6]);
int init_viterbi615(void *vp,int starting_state);
int update_viterbi615_blk(void *vp,unsigned char *syms,int nbits);
int chainback_viterbi615(void *vp, unsigned char *data,unsigned int nbits,int endstate);
void delete_viterbi615(void *vp);


/* General purpose RS codec, 8-bit symbols */
void encode_rs_char(void *rs,unsigned char *data,unsigned char *parity);
int decode_rs_char(void *rs,unsigned char *data,int *eras_pos,
		   int no_eras);
void *init_rs_char(int symsize,int gfpoly,
		   int fcr,int prim,int nroots,
		   int pad);
void free_rs_char(void *rs);

/* General purpose RS codec, integer symbols */
void encode_rs_int(void *rs,int *data,int *parity);
int decode_rs_int(void *rs,int *data,int *eras_pos,int no_eras);
void *init_rs_int(int symsize,int gfpoly,int fcr,
		  int prim,int nroots,int pad);
void free_rs_int(void *rs);

/* CCSDS standard (255,223) RS codec with conventional (*not* dual-basis)
 * symbol representation
 */
void encode_rs_8(unsigned char *data,unsigned char *parity,int pad);
int decode_rs_8(unsigned char *data,int *eras_pos,int no_eras,int pad);

/* CCSDS standard (255,223) RS codec with dual-basis symbol representation */
void encode_rs_ccsds(unsigned char *data,unsigned char *parity,int pad);
int decode_rs_ccsds(unsigned char *data,int *eras_pos,int no_eras,int pad);

/* Tables to map from conventional->dual (Taltab) and
 * dual->conventional (Tal1tab) bases
 */
extern unsigned char Taltab[],Tal1tab[];


/* If x has odd parity, return 1; else 0 */
#if __i386__ || __x86_64__
static inline int parityb(unsigned char x){ /* parity(x & 0xff) */
  /* The setp instruction copies the parity flag (PF) into the low byte, leaving
   * the rest of the word unchanged. PF is 1 for even parity, so invert it for odd parity.
   * There doesn't seem to be any way to refer to the low
   * byte of a GCC-assigned register, so we have to hardwire x to %eax.
   */
  asm("and $255,%%eax; xor $1, %%eax; setp %%al" : "=a"(x) : "0" (x));
  return x;
}
#else
static inline int parityb(unsigned char x){
  x ^= x >> 4;
  x ^= x >> 2;
  x ^= x >> 1;
  return (x & 1);
}
#endif

#if __i386__ || __x86_64__
static inline int parity(unsigned int x){ /* Parity of 32-bit integer */
  x ^= x >> 16;
  asm("xor %%ah,%%al; and $255,%%eax; xor $1, %%eax; setp %%al" : "=a"(x) : "0" (x));
  return x;
}
#else
static inline int parity(unsigned int x){
  x ^= x >> 16;
  x ^= x >> 8;
  x ^= x >> 4;
  x ^= x >> 2;
  x ^= x >> 1;
  return(x & 1);
}
#endif

static inline int parityl(unsigned long long x){ /* Parity of 64-bit integer */
  x ^= x >> 32;
  return parity((unsigned int)x);
}

/* page 136, AMD Athlon Processor x86 Code Optimization Guide, AMD publication 22007, February 2002,
 * http://www.amd.com/us-en/assets/content_type/white_papers_and_tech_docs/22007.pdf
 *
 * see also "The Quest for an Accelerated Population Count", Henry S. Warren Jr, 
 * "Beautiful Code" chapter 10, p 151. (O'Reilly)
 */
static inline int popcnt(unsigned int x){ /* 32 bit integer */
  x -= (x >> 1) & 0x55555555; /* Each bit pair contains a count of the 1's in it */
  x = (x & 0x33333333) + ((x >> 2) & 0x33333333);  /* Sum bit pairs into nybbles */
  x = (x + (x >> 4)) & 0x0f0f0f0f;  /* Nybbles into bytes */
  return (x * 0x01010101) >> 24; /* Horizontal add of byte sums */
}

static inline int popcntl(unsigned long long x){ /* 64 bit version */
  x -= (x >> 1) & 0x5555555555555555LL;
  x = (x & 0x3333333333333333LL) + ((x >> 2) & 0x3333333333333333LL);
  x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0fLL;
  return (x * 0x0101010101010101LL) >> 56;
}

/* Bit reversal of 32-bit int
 * There has to be a better algorithm
 * Still, this 64-bit version is 8x faster than
 * a simple mask/or/left/right shift routine.
 */
static inline int bitreverse(int n){
  n = ((n >> 1) & 0x55555555) | ((n << 1) & 0xaaaaaaaa);
  n = ((n >> 2) & 0x33333333) | ((n << 2) & 0xcccccccc);
  n = ((n >> 4) & 0x0f0f0f0f) | ((n << 4) & 0xf0f0f0f0);
  n = ((n >> 8) & 0x00ff00ff) | ((n << 8) & 0xff00ff00);
  n = ((n >> 16) & 0x0000ffff) | ((n << 16) & 0xffff0000);
  return n;
}

static inline long long bitreversel(long long n){
  n = ((n >> 1) & 0x5555555555555555LL) | ((n << 1) & 0xaaaaaaaaaaaaaaaaLL);
  n = ((n >> 2) & 0x3333333333333333LL) | ((n << 2) & 0xccccccccccccccccLL);
  n = ((n >> 4) & 0x0f0f0f0f0f0f0f0fLL) | ((n << 4) & 0xf0f0f0f0f0f0f0f0LL);
  n = ((n >> 8) & 0x00ff00ff00ff00ffLL) | ((n << 8) & 0xff00ff00ff00ff00LL);
  n = ((n >> 16) & 0x0000ffff0000ffffLL) | ((n << 16) & 0xffff0000ffff0000LL);
  n = ((n >> 32) & 0x00000000ffffffffLL) | ((n << 32) & 0xffffffff00000000LL);

  return n;
}

/* Swap bits in arbitrary word size
 * n = 8 for 3 bit x
 * n = 16 for 4 bit x, etc
 */
static inline int bitreversen(int x,int n){
  int result = 0;

  while(1){
    result |= x & 1; // n = 8, result = 001; n = 4, result = 010; n = 2, result = 100; done
    if(n == 2)
      break;
    x >>= 1;
    result <<= 1;
    n >>= 1;
  }
  return result;
}

static inline unsigned long long rdtsc(void){
  unsigned long long tsc;
  asm volatile ("rdtsc" : "=A"(tsc));
  return tsc;
}

extern void addnoise(unsigned char *symbols,int length,int ampl,double noise);

#ifdef __cplusplus
}
#endif

#endif /* _FEC_H_ */

