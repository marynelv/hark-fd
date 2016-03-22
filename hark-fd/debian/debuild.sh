#!/bin/sh

# DISTRIB_CODENAME is trusty or precise
DISTRIB_CODENAME=$(sed -n "s/DISTRIB_CODENAME=//p" /etc/lsb-release)

# current directory check
if [ ! -d debian ]; then
  echo "no debian directory here"
  exit 1
fi

# in case [precise] but [no libsdl2-dev]
if [ "${DISTRIB_CODENAME}" = "precise" ]; then 
  if ! (which sdl2-config >/dev/null 2>&1); then
    echo ' ===  libsdl2-dev should be installed. Try this:  ==='
    echo " "
    echo "   sudo add-apt-repository ppa:zoogie/sdl2-snapshots"
    echo "   sudo apt-get update"
    echo "   sudo apt-get install libsdl2-dev"
    echo " "
    exit 1
  else
    echo "libsdl2-dev ok"
  fi
fi


# change dependency for [libsdl2-dev]
rm -f debian/control 
ln -s "_control_${DISTRIB_CODENAME}" debian/control

# run
if [ "$1" != "--no-debuild" ]; then
  debuild
fi

