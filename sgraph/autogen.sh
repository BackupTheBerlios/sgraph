#!/bin/sh
#
# Regenerate configuration files
aclocal
automake -a
autoheader
autoconf

# Run configure for this platform
#./configure $*
echo "Now you are ready to run ./configure"
