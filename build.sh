#!/usr/bin/bash
set -e
CXXFLAGS="${CXXFLAGS} -D_LIBCPP_DISABLE_AVAILABILITY" make
mkdir -p $PREFIX/bin
cp RemoveCramTags $PREFIX/bin/RemoveCramTags
chmod +x $PREFIX/bin/RemoveCramTags