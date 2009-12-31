#!/bin/bash -x

builddir=$PWD

build_fail() {
    echo ERROR: BUILD FAILED
    build_fail
}

echo asserting version 3.0.1 of antlr3
if [ `antlr3 -v 2>&1 | perl -ne 's/.*Version ([\d\.]+).*/$1/; print $_'` != "3.0.1" ] ; then
    echo ERROR: did not find version 3.0.1 of antlr3
    build_fail
else
    echo found version 3.0.1 of antlr3
fi

antlr3 Censor.g || build_fail

# get version 3.0.1 of libantlrc from http://www.antlr.org/download/C
if [ ! -e libantlr3c-3.0.1.tar.gz ] ; then 
    wget http://www.antlr.org/download/C/libantlr3c-3.0.1.tar.gz || build_fail
fi

rm -rf libantlr3c-3.0.1
tar xvzf libantlr3c-3.0.1.tar.gz || build_fail

antlrc_home=$builddir/antlrc
rm -rf $antlrc_home
cd libantlr3c-3.0.1
./configure --with-pic --prefix=$antlrc_home || build_fail
make -j4 || build_fail
make -j4 install || build_fail
cd ..

gcc -I . -I $antlrc_home/include -o censor_file_loader -g3 -O0 -fno-inline \
    -L$antlrc_home/lib/ \
    censor_file_loader.c  CensorLexer.c  CensorParser.c \
    $antlrc_home/lib/libantlr3c.a || build_fail
    

echo testing parser
./censor_file_loader test_input.cnsr

echo build passed