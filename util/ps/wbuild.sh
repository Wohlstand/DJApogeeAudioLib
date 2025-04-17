#!/bin/bash

export WATCOM=/opt/watcom
export PATH=${WATCOM}/binl64:$PATH
export LIB=${WATCOM}/lib386/dos
export EDPATH=$WATCOM/eddat
export WIPFC=$WATCOM/wipfc
export INCLUDE="$WATCOM/h"
#WATCOM_FLAGS="-q -bdos4g -march=i386"
#export CFLAGS="$WATCOM_FLAGS -xc -std=wc"
#export CXXFLAGS="$WATCOM_FLAGS -xc++ -xs -feh -frtti -std=c++98"
export LFLAGS="$WATCOM_FLAGS"

if [[ ! -f usrhooks.c ]]; then
    cp -v ../../include/usrhooks/usrhooks.c .
fi
if [[ ! -f usrhooks.h ]]; then
    cp -v ../../include/usrhooks/usrhooks.h .
fi

mkdir -p obj
wmake -f Makefile

rm usrhooks.c
rm usrhooks.h
rm -Rf ./obj
