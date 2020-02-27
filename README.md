# GNU-RNG-tausx                  

GNU Scientific Library - new random number generator, tausx
Tausx is an extended version of taus2 with a Bays-Durham shuffle.

This repository shows how to add tausx to the GNU Scientific Library.

See the etaus repository for more information about the extended
taus algorithm with a Bays-Durham shuffle.  The test programs in
the etaus repository also apply to the tausx generator, here.

This repository contains difference files for implementing the extended
taus algorithm in the random number generator section of the GNU
Scientific Library.  A new source file, tausx.c, is added to the
rng directory in the GNU Scientific Library.  See tausx.c in this
repository.  The difference file for tausx.c vs taus.c is called
ext.diff.  Tausx.c is on the left (< sign), and taus.c is on
the right (> sign).

Four other files require the following changes in order to compile
tausx.c into the GNU Scientific Library.  The updated code is on the
left.  The original code is on the right.

---------------------------------------------

* Makefile.am - add the name of tausx.c to the list or source programs.

7c7

< libgslrng_la_SOURCES = borosh13.c cmrg.c coveyou.c default.c file.c fishman18.c fishman20.c fishman2x.c gfsr4.c knuthran2.c knuthran.c knuthran2002.c lecuyer21.c minstd.c mrg.c mt.c r250.c ran0.c ran1.c ran2.c ran3.c rand48.c rand.c random.c randu.c ranf.c ranlux.c ranlxd.c ranlxs.c ranmar.c rng.c slatec.c taus.c taus113.c tausx.c transputer.c tt.c types.c uni32.c uni.c vax.c waterman14.c zuf.c inline.c

---

> libgslrng_la_SOURCES = borosh13.c cmrg.c coveyou.c default.c file.c fishman18.c fishman20.c fishman2x.c gfsr4.c knuthran2.c knuthran.c knuthran2002.c lecuyer21.c minstd.c mrg.c mt.c r250.c ran0.c ran1.c ran2.c ran3.c rand48.c rand.c random.c randu.c ranf.c ranlux.c ranlxd.c ranlxs.c ranmar.c rng.c slatec.c taus.c taus113.c transputer.c tt.c types.c uni32.c uni.c vax.c waterman14.c zuf.c inline.c

---------------------------------------------

* Makefile.in - add the name of tausx.lo to the list of object
  modules.

104,105c104,105

< 	taus113.lo tausx.lo transputer.lo tt.lo types.lo uni32.lo \
< 	uni.lo vax.lo waterman14.lo zuf.lo inline.lo

---

> 	taus113.lo transputer.lo tt.lo types.lo uni32.lo uni.lo vax.lo \
> 	waterman14.lo zuf.lo inline.lo

---------------------------------------------

* gsl_rng.h - add the RNG name, gsl_rng_tausx, to the list of
  of generators in the header file for random number generators.

116d115

< GSL_VAR const gsl_rng_type *gsl_rng_tausx;

---------------------------------------------

* types.c - add "tausx" to the list of random number generators.

89d88

<   ADD(gsl_rng_tausx);

---------------------------------------------

Installation instructions.

The latest version of the GNU Scientific Library is version
2.6.  The source code for the GNU Scientific Library may be
downloaded at the following address.

ftp://ftp.gnu.org/gnu/gsl/

ftp://ftp.gnu.org/gnu/gsl/gsl-2.6.tar.gz

README and INSTALL in the main directory explain how to compile
the GNU Scientific Library.

The compile sequence for the GNU Scientific Library is

./configure

make

sudo make install

The static library, libgsl.a, and the shared library, libgsl.so,
are placed in the .libs directory after the compile has completed.

unzip GSL-rng-tausx-master.zip

change directory to GSL-rng-tausx-master

chmod 700 *.sh

ln -s [path]/gsl-2.6 ./gsl-2.6

./inst.sh

If you receive a compile error from inst.sh, check speed.mak to
make sure that the gsl_rng.h header file is pointing to the correct
path in CFLAGS.

Run the speed test.

Make sure that the LD_LIBRARY_PATH variable in runspeed.sh is
pointing to the correct address path.

./runspeed.sh

To de-install the speed test,

./deinst.sh

---------------------------------------------

Speed test of tausx vs taus2.

Program speed.c in this repository runs a speed test on tausx
and taus2, to see which generator runs faster.

After 100 million gets, the following times were calculated for
tausx and taus2.

Generator           #Ticks

tausx duration        178

taus2 duration        158

Tausx takes roughly 12% longer than taus2 to generate numbers
in this test.

In another test, the timings were much closer.

Generator           #Ticks

tausx duration        206

taus2 duration        200

The timings change from one run to the next, depending on what else
is happening in the linux system.  The first test above is a more
accurate representation of the relative differences between the
two generators.
