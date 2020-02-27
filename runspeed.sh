#!/bin/bash
if [ -z $1 ]
then
echo "Usage: $0 GSL_RNG_name"
echo "Example: $0 taus2"
echo "Example: $0 tausx"
echo "To see a list of GSL generators"
echo "$0 ?"
exit 1
fi
LD_LIBRARY_PATH=gsl-2.6/.libs
export LD_LIBRARY_PATH
GSL_RNG_TYPE=$1
export GSL_RNG_TYPE
time speed
