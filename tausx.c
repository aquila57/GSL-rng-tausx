/* rng/tausx.c
 * 
 * Copyright (C) 2020  aquila57 at github.com
 * 
 * This is an an extension to taus.c with a Bays-Durham shuffle.
 *
 * Copyright for taus.c:
 *
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 James Theiler, Brian Gough
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <config.h>
#include <stdlib.h>
#include <gsl/gsl_rng.h>

/* This is a maximally equidistributed combined Tausworthe
   generator. The sequence for tausx_get() is,

   x_{n-2} = x_{n-1}
   x_{n-1} = x_n

   x_n = (s1_n ^ s2_n ^ s3_n) 

   s1_{n+1} = (((s1_n & 4294967294) <<12) ^ (((s1_n <<13) ^ s1_n) >>19))
   s2_{n+1} = (((s2_n & 4294967288) << 4) ^ (((s2_n << 2) ^ s2_n) >>25))
   s3_{n+1} = (((s3_n & 4294967280) <<17) ^ (((s3_n << 3) ^ s3_n) >>11))

   index = x_{x-2} >> 18
   temp = sx[index]
   sx[index] = x_n
   x_n = temp

   computed modulo 2^32. In the three formulas above '^' means
   exclusive-or (C-notation), not exponentiation. Note that the
   algorithm relies on the properties of 32-bit unsigned integers (it
   is formally defined on bit-vectors of length 32). I have added a
   bitmask to make it work on 64 bit machines.

   sx is a random list of 16384 32-bit numbers.
   temp is a temporary 32-bit number.
   index is a 32-bit signed integer as a subscript to sx.

   We initialize the generator with s1_1 .. s3_1 = s_n MOD m, where
   s_n = (69069 * s_{n-1}) mod 2^32, and s_0 = s is the user-supplied
   seed.

   After s1, s2, and s3 have been initialized as with taus2, the array sx
   is initialized by performing the Tausworthe algorithm for each element
   in the array.

   The theoretical value of x_{17391} is 1132370745.  The subscript
   17391 means (1) seed the generator with s=1 (2) do six warm-up
   iterations, (3) initialize sx, (4) then do 10000 actual iterations.

   The period of this generator approximates the factorial of 16384,
   or 10^61036.  It is orders of magnitude greater than the period
   of the original taus generator.

   From: P. L'Ecuyer, "Maximally Equidistributed Combined Tausworthe
   Generators", Mathematics of Computation, 65, 213 (1996), 203--213.

   This is available on the net from L'Ecuyer's home page,

   http://www.iro.umontreal.ca/~lecuyer/myftp/papers/tausme.ps
   ftp://ftp.iro.umontreal.ca/pub/simulation/lecuyer/papers/tausme.ps 

   Update: April 2002

   There is an erratum in the paper "Tables of Maximally
   Equidistributed Combined LFSR Generators", Mathematics of
   Computation, 68, 225 (1999), 261--269:
   http://www.iro.umontreal.ca/~lecuyer/myftp/papers/tausme2.ps

        ... the k_j most significant bits of z_j must be non-
        zero, for each j. (Note: this restriction also applies to the 
        computer code given in [4], but was mistakenly not mentioned in
        that paper.)
   
   This affects the seeding procedure by imposing the requirement
   s1 > 1, s2 > 7, s3 > 15.

   The generator taus2 has been added to satisfy this requirement.
   The original taus generator is unchanged.

   Update: November 2002

   There was a bug in the correction to the seeding procedure for s2.
   It affected the following seeds 254679140 1264751179 1519430319
   2274823218 2529502358 3284895257 3539574397 (s2 < 8).

*/

static inline unsigned long int tausx_get (void *vstate);
static double tausx_get_double (void *vstate);
static void tausx_set (void *state, unsigned long int s);

typedef struct
  {
    unsigned long int s1, s2, s3;
    unsigned long int xpp;        /* x_{n-2}           */
    unsigned long int xp;         /* x_{n-1}           */       
    unsigned long int xn;         /* s1^s2^s3          */
    unsigned long int ix;         /* index to sx array */       
    unsigned long int tx;         /* temporary xn      */       
    unsigned long int sx[16384];     /* shuffle array  */
  }
tausx_state_t;

static inline unsigned long tausx_get (void *vstate)
{

  tausx_state_t *state = (tausx_state_t *) vstate;

#define MASK 0xffffffffUL
#define TAUSWORTHE(s,a,b,c,d) (((s &c) <<d) &MASK) ^ ((((s <<a) &MASK)^s) >>b)

  state->xpp = state->xp;
  state->xp  = state->xn;

  state->s1 = TAUSWORTHE (state->s1, 13, 19, 4294967294UL, 12);
  state->s2 = TAUSWORTHE (state->s2, 2, 25, 4294967288UL, 4);
  state->s3 = TAUSWORTHE (state->s3, 3, 11, 4294967280UL, 17);

  state->xn  = (state->s1 ^ state->s2 ^ state->s3);
  state->ix = state->xpp >> 18;
  state->tx = state->sx[state->ix];
  state->sx[state->ix] = state->xn;
  state->xn = state->tx;

  return state->xn;
}

static double tausx_get_double (void *vstate)
{
  return tausx_get (vstate) / 4294967296.0 ;
}

static void tausx_set (void *vstate, unsigned long int s)
{
  tausx_state_t *state = (tausx_state_t *) vstate;

  if (s == 0)
    s = 1;      /* default seed is 1 */

/* The following is the taus2 adjustment to the initial seeds */

#define LCG(n) ((69069 * n) & 0xffffffffUL)
  state->s1 = LCG (s);
  if (state->s1 < 2) state->s1 += 2UL;
  state->s2 = LCG (state->s1);
  if (state->s2 < 8) state->s2 += 8UL;
  state->s3 = LCG (state->s2);
  if (state->s3 < 16) state->s3 += 16UL;

  inline void tausx_get_simple(tausx_state_t *state)
     {
#define MASK 0xffffffffUL
#define TAUSWORTHE(s,a,b,c,d) (((s &c) <<d) &MASK) ^ ((((s <<a) &MASK)^s) >>b)
     state->s1 = TAUSWORTHE (state->s1, 13, 19, 4294967294UL, 12);
     state->s2 = TAUSWORTHE (state->s2, 2, 25, 4294967288UL, 4);
     state->s3 = TAUSWORTHE (state->s3, 3, 11, 4294967280UL, 17);
     } /* tausx_get_simple(state) */

  inline void tausx_init_array(tausx_state_t *state)
     {
     unsigned long int *p,*q;
     p = (unsigned long int *) state->sx;
     q = (unsigned long int *) state->sx + 16384;
     while (p < q)
        {
        tausx_get_simple (state);
	*p++ = (state->s1 ^ state->s2 ^ state->s3);
	} /* for each element in shuffle array */
     } /* tausx_init_array(state) */

  /* "warm it up" */
  tausx_get_simple (state);
  tausx_get_simple (state);
  tausx_get_simple (state);
  tausx_get_simple (state);
  tausx_get_simple (state);
  tausx_get_simple (state);
  /* initialize the Bays-Durham shuffle array  */
  /* to random 32-bit non-zero numbers         */
  tausx_init_array (state);
  return;
}

static const gsl_rng_type tausx_type =
{"tausx",                       /* name */
 0xffffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (tausx_state_t),
 &tausx_set,
 &tausx_get,
 &tausx_get_double};

const gsl_rng_type *gsl_rng_tausx = &tausx_type;
