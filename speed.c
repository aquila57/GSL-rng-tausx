/* speed.c - Speed test for tausx vs taus2  Version 0.1.0            */
/* Copyright (C) 2020 aquila57 at github.com                         */

/* This program is free software; you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License as    */
/* published by the Free Software Foundation; either version 2 of    */
/* the License, or (at your option) any later version.               */

/* This program is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of    */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the      */
/* GNU General Public License for more details.                      */

/* You should have received a copy of the GNU General Public License */
/* along with this program; if not, write to:                        */

   /* Free Software Foundation, Inc.                                 */
   /* 59 Temple Place - Suite 330                                    */
   /* Boston, MA 02111-1307, USA.                                    */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
/* this is the new version of gsl_rng.h */
#include <gsl_rng.h>

#define SMPLS 100000000

int main(void)
   {
   int i;                      /* loop counter */
   unsigned long int x;        /* result of get */
   clock_t begclk;             /* current number of ticks */
   clock_t endclk;             /* current number of ticks */
   int duration;               /* duration of test in ticks */
   struct tms t;               /* structure used by times() */
   gsl_rng *rng;               /* RNG structure (state) */
   /* print column headings */
   printf("Generator           #Ticks\n");
   /************************************************/
   /* initialize the random number generator tausx */
   /* tausx is taus2 with a Bays-Durham shuffle.   */
   /************************************************/
   rng = (gsl_rng *) gsl_rng_alloc(gsl_rng_tausx);
   /* set the initial seed to 1 [the default] */
   gsl_rng_set(rng,1);
   /* get #clock ticks since boot */
   begclk = times(&t);         /* starting time in ticks */
   i = SMPLS;                  /* number of samples */
   while (i--)
      {
      x = gsl_rng_get(rng);
      } /* for each iteration of get */
   endclk = times(&t);         /* ending time in ticks */
   duration = endclk - begclk;   /* calculate duration */
   printf("tausx duration %10d\n", duration);
   gsl_rng_free(rng);    /* de-allocate the RNG */
   /************************************************/
   /* initialize the random number generator taus2 */
   /************************************************/
   rng = (gsl_rng *) gsl_rng_alloc(gsl_rng_taus2);
   /* set the initial seed to 1 [the default] */
   gsl_rng_set(rng,1);
   /* get #clock ticks since boot */
   begclk = times(&t);         /* starting time in ticks */
   i = SMPLS;                  /* number of samples */
   while (i--)
      {
      x = gsl_rng_get(rng);
      } /* for each iteration of get */
   endclk = times(&t);         /* ending time in ticks */
   duration = endclk - begclk;   /* calculate duration */
   printf("taus2 duration %10d\n", duration);
   gsl_rng_free(rng);    /* de-allocate the RNG */
   x = x;                /* to avoid a warning message */
   return(0);
   } /* main */
