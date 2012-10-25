#ifdef __cplusplus
extern "C" {
#endif

#pragma once
/* Include file for DSP code that uses SIMD instructions and data types
 * this stuff isn't well documented in gcc, so it might be redundant to
 * standard gcc include files I should be using
 *
 * $Id$
 */


#include <stdio.h>
#include <string.h>

#if 0
#if __SSE4_2__
#include <nmmintrin.h>
#elif __SSE4_1__
#include <smmintrin.h>
#elif __SSSE3__
#include <tmmintrin.h>
#elif __SSE3__
#include <pmmintrin.h>
#elif __SSE2__
#include <emmintrin.h>
#elif __SSE__
#include <xmmintrin.h>
#elif __MMX__
#include <mmintrin.h>
#else
#define NO_SSE 1
#endif
#endif

#if __SSE4_2__
#include <nmmintrin.h>
#endif
#if __SSE4_1__
#include <smmintrin.h>
#endif
#if __SSSE3__
#include <tmmintrin.h>
#endif
#if __SSE3__
#include <pmmintrin.h>
#endif
#if __SSE2__
#include <emmintrin.h>
#endif
#if __SSE__
#include <xmmintrin.h>
#endif
#if __MMX__
#include <mmintrin.h>
#endif

#if defined(__APPLE__) || !defined(__SSE__)
#define MALLOC(x) (malloc(x))
#define CALLOC(x,y) (calloc(x,y))
#else
#include <malloc.h>
/* Linux libc stubbornly doesn't guarantee 16-byte alignment, so we have to do it ourselves */
#define MALLOC(x) (memalign(16,(x)))
#define CALLOC(x,y) (memalign(16,(x*y)), memset(x,0,x*y)) /* memset() returns its first argument, the allocated memory */
#endif


#ifdef __SSE__
/* 128-bit vectors */
typedef __v16qi v16qi; /* 16 8-bit bytes */
typedef __v8hi v8hi;  /* 8 16-bit words */
typedef __v4si v4si;  /* 4 32-bit words */
typedef __v2di v2di;  /* 2 64-bit words */

typedef __v4sf v4sf;  /* 4 32-bit single precision floats */
typedef __v2df v2df;  /* 2 64-bit double precision floats */

#define ZERO_SSE (_mm_setzero_si128())

union v16 {
  unsigned char i[16];
  v16qi v;
};

union v8 {
  unsigned short i[8];
  v8hi v;
};
union v4 {
  unsigned int i[4];
  v4si v;
};
union v2 {
  long long i[2];
  v2di v;
};

static inline void print2v(char *label,v2di *v){
  union v2 u;
  u.v = *v;
  fprintf(stderr,"%s %016llx %016llx\n",label,u.i[1],u.i[0]);
}

static inline void print4(char *label,v4si *v){
  union v4 u;
  u.v = *v;
  fprintf(stderr,"%s %08x %08x %08x %08x\n",label,u.i[3],u.i[2],u.i[1],u.i[0]);
}

static inline void print8(char *label,v8hi *v){
  union v8 u;
  u.v = *v;
  fprintf(stderr,"%s %04x %04x %04x %04x %04x %04x %04x %04x\n",label,u.i[7],u.i[6],u.i[5],u.i[4],u.i[3],u.i[2],u.i[1],u.i[0]);
}

static inline void print16(char *label,v16qi *v){
  union v16 u;
  u.v = *v;

  fprintf(stderr,"%s %02x %02x %02x %02x %02x %02x %02x %02x",label,u.i[15],u.i[14],u.i[13],u.i[12],u.i[11],u.i[10],u.i[9],u.i[8]);
  fprintf(stderr," %02x %02x %02x %02x %02x %02x %02x %02x\n",u.i[7],u.i[6],u.i[5],u.i[4],u.i[3],u.i[2],u.i[1],u.i[0]);
}
#endif

#ifdef __MMX__
/* 64-bit vectors */
typedef __m64 v2sf;   /* 2 32-bit floats (3DNow!) */
typedef __m64 v2si;   /* 2 32-bit integers */
typedef __m64 v4hi;   /* 4 16-bit integers */
typedef __m64 v8qi;   /* 8 8-bit bytes */
typedef __m64 v1di;   /* 1 64-bit integer */
#endif

#if 0
/* Multiply two pairs of floating point complex numbers */
/* Incomplete */
inline v4sf cmultiply(v4sf a,v4sf b){
  v4sf m0,m1,m2;

  /* A and B are complex with Bimag Breal Aimag Areal */
  m0 = __mm_moveldup_ps(a); /* Ar       Ar       Ar       Ar */
  m0 = __mm_mul_ps(m0,b);   /* Br*Bi    Br*Br    Ar*Ai    Ar*Ar */

}
#endif

// #if 0
#ifdef __SSE__
/* Popcount of a 128-bit SSE register */
static inline int popcnt128(v8hi x){
  int w;

  /*
  x -= (x>>1) & 0x5555;
  x = (x & 0x3333) + ((x >>2) & 0x3333);
  x = (x + (x >> 4)) & 0x0f0f;
  x = x + (x >> 8)
  horizontal sum bytes
  */

  // x -= (x>>1) & 0x5555;
  x -= (v8hi)(_mm_srli_epi16(x,1) & _mm_set1_epi16(0x5555));

  //  x = (x & 0x3333) + ((x >> 2) & 0x3333);
  x = (v8hi)(x & (v8hi)_mm_set1_epi16(0x3333)) + (v8hi)(_mm_srli_epi16(x,2) & _mm_set1_epi16(0x3333));

  // x = (x + (x >> 4)) & 0x0f0f;
  x = (x + (v8hi)_mm_srli_epi16(x,4)  )    &      (v8hi) _mm_set1_epi16(0x0f0f);

  // x = x + (x >> 8)
  x += (v8hi)_mm_srli_epi16(x,8);

  /* f   e   d   c   b   a   9   8   7   6   5   4   3   2   1   0 */
#ifdef __SSSE3__
  x = _mm_hadd_epi16(x,x); /* 8 words to 4 */
  x = _mm_hadd_epi16(x,x); /* 4 words to 2 */
  x = _mm_hadd_epi16(x,x); /* 2 words to 1 */
#else
  /* SSE2 doesn't have hadd */
  x += (v8hi)_mm_srli_si128(x,8);
  x += (v8hi)_mm_srli_si128(x,4);
  x += (v8hi)_mm_srli_si128(x,2);
#endif

  w = _mm_extract_epi16(x,0); /* Extract last 16-bit word and pull sum from low byte - OK in SSE2?? */
  return w & 0xff;
}

#endif /* NO_SSE */
// #endif /* 0 */

/* Cycle timing stuff */
#ifdef __x86_64__
static inline unsigned long long getticks(void){
     unsigned a, d; 
     asm volatile("rdtsc" : "=a" (a), "=d" (d)); 
     return ((unsigned long long)a) | (((unsigned long long)d) << 32); 
}
#endif

#if 0
#ifdef __i386__
static inline long long getticks(void){
  unsigned long long ticks;

  volatile asm("rdtsc" : "=A"(ticks));
  return ticks;
}
#endif
#endif

#define POLY 0xd800000000000000LL

#ifdef __cplusplus
}
#endif