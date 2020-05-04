#!/bin/bash
libtoolize --force
aclocal
autoheader
automake --force-missing --add-missing
autoconf
autoreconf
mkdir -p src/client/.libs
./configure
