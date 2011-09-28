/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_primes.cc 13655 2010-11-15 15:34:57Z hannes $ */

#include <config.h>

#include "assert.h"

#include "cf_defs.h"
#include "cf_primes.h"
#include "cf_primetab.h"


int cf_getPrime( int i )
{
    ASSERT( i >= 0 && i < NUMPRIMES, "index to primes too high" );
    if ( i >= NUMSMALLPRIMES )
        return bigprimes[i-NUMSMALLPRIMES];
    else
        return smallprimes[i];
}

int cf_getNumPrimes()
{
    return NUMPRIMES;
}

int cf_getSmallPrime( int i )
{
    ASSERT( i >= 0 && i < NUMSMALLPRIMES, "index to primes too high" );
    return smallprimes[i];
}

int cf_getNumSmallPrimes()
{
    return NUMSMALLPRIMES;
}

int cf_getBigPrime( int i )
{
    ASSERT( i >= 0 && i < NUMBIGPRIMES, "index to primes too high" );
    return bigprimes[i];
}

int cf_getNumBigPrimes()
{
    return NUMBIGPRIMES;
}
