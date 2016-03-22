dnl Process this file with autoconf to produce a configure script. -*-m4-*-

AC_PREREQ([2.63])
AC_INIT([hark], [2.2.0], no-define, [hark])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/VERSION.cc])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile librecorder/Makefile libseparation/Makefile src/Makefile])
AM_INIT_AUTOMAKE([subdir-objects])
LT_INIT

dnl HARK PACKAGE NAME and VERSION
HARK_MAJOR_VERSION=2
HARK_MINOR_VERSION=2
HARK_MICRO_VERSION=0
HARK_VERSION=$HARK_MAJOR_VERSION.$HARK_MINOR_VERSION.$HARK_MICRO_VERSION
HARK_INTERFACE_AGE=0
HARK_BINARY_AGE=0
PACKAGE=hark
VERSION=$HARK_VERSION
AC_SUBST([HARK_MAJOR_VERSION])
AC_SUBST([HARK_MINOR_VERSION])
AC_SUBST([HARK_MICRO_VERSION])
AC_SUBST([HARK_VERSION])
AC_SUBST([HARK_INTERFACE_AGE])
AC_SUBST([HARK_BINARY_AGE])

dnl libtool versioning
LT_RELEASE=$HARK_VERSION
LT_CURRENT=`expr $HARK_MICRO_VERSION - $HARK_INTERFACE_AGE`
LT_REVISION=$HARK_INTERFACE_AGE
LT_AGE=`expr $HARK_BINARY_AGE - $HARK_INTERFACE_AGE`
AC_SUBST([LT_RELEASE])
AC_SUBST([LT_CURRENT])
AC_SUBST([LT_REVISION])
AC_SUBST([LT_AGE])

dnl TAGS VERSION and SVN REVISION
echo 'Searching SVN Revision Recursively ...'
SVN_REVISION=`\`flowdesigner-config --bindir\`/fdmodule_version.pl --printsvn`
AC_DEFINE_UNQUOTED([SVN_REVISION], "${SVN_REVISION}", [svn revision number])
AC_DEFINE_UNQUOTED([TAGS_VERSION], "${HARK_VERSION}", [tags version number])
echo "Found Revision: ${SVN_REVISION}"

dnl autoheader template was migrated from acconfig.h
AH_TEMPLATE([ENABLE_HARKIO], [])
AH_TEMPLATE([ENABLE_HARKIO1], [])
AH_TEMPLATE([ENABLE_HARKIO2], [])
AH_TEMPLATE([ENABLE_HARKIO3], [])
AH_TEMPLATE([ENABLE_HARKIO_NH], [])
AH_TEMPLATE([ENABLE_ASRASP], [])
AH_TEMPLATE([ENABLE_WS], [])
AH_TEMPLATE([ENABLE_RASP24], [])
AH_TEMPLATE([ENABLE_RASPLC], [])
AH_TEMPLATE([ENABLE_KANEKO], [])
AH_TEMPLATE([ENABLE_HARKNETAPI], [])
AH_TEMPLATE([ENABLE_RASP2], [])
AH_TEMPLATE([ENABLE_TDBD8CSUSB], [])
AH_TEMPLATE([ENABLE_TDBD16ADUSB], [])
AH_TEMPLATE([ENABLE_PLAYAUDIO], [])

AM_MAINTAINER_MODE

AC_CANONICAL_HOST

AC_PROG_CXX
AC_PROG_F77
AC_LANG([C++])
AC_PROG_MAKE_SET
AC_C_BIGENDIAN

dnl boost_thread judgement
BOOST_THREAD_JUDGE([1.46.0])

AC_PATH_PROG(FLOWDESIGNER_CONFIG,flowdesigner-config,no,$PATH:$FLOWDESIGNER_HOME/bin:$prefix/bin)
if test "$FLOWDESIGNER_CONFIG" = no; then
  AC_MSG_ERROR([Cannot find flowdesigner-config])
fi

FLOWDESIGNER_LIB=`$FLOWDESIGNER_CONFIG --libs libflow`
FLOWDESIGNER_INCLUDE=`$FLOWDESIGNER_CONFIG --cflags config libflow | sed 's/\"/\\\"/g'`
FLOWDESIGNER_DATA=`$FLOWDESIGNER_CONFIG --datadir`/$PACKAGE
FLOWDESIGNER_BIN=`$FLOWDESIGNER_CONFIG --bindir`

if test "x$prefix" = "xNONE"; then
  echo $FLOWDESIGNER_DATA
else
  FLOWDESIGNER_DATA=$prefix/lib/flowdesigner/toolbox/$PACKAGE
fi

AC_SUBST([FLOWDESIGNER_LIB])
AC_SUBST([FLOWDESIGNER_INCLUDE])
AC_SUBST([FLOWDESIGNER_DATA])
AC_SUBST([FLOWDESIGNER_BIN])

PKG_CHECK_MODULES(GTK, gtk+-2.0)
AC_SUBST([GTK_CFLAGS])
AC_SUBST([GTK_LIBS])

dnl for libharkio1
AC_ARG_ENABLE(
  [harkio1],
  [  --enable-harkio1         Enable harkio1 library],
  [
    case "${enableval}" in
      yes) harkio1=true ;;
      no)  harkio1=false ;;
      *) AC_MSG_ERROR([bad value ${enableval} for --enable-harkio1]) ;;
    esac
  ],
  [harkio1=false]
)
AM_CONDITIONAL([ENABLE_HARKIO1], [test x$harkio1 = xtrue])
if test "$enable_harkio1" = "yes"; then
  AC_DEFINE([ENABLE_HARKIO1])
  AC_DEFINE([ENABLE_HARKIO])
fi
AC_ARG_WITH(
  [harkio1-inc],
  [  --with-harkio1-inc=DIR      location of libharkio1 include files],
  [CPPFLAGS="${CPPFLAGS} -I ${withval}"]
)
AC_ARG_WITH(
  [harkio1-lib],
  [  --with-harkio1-lib=DIR      location of libharkio1 library files],
  [LDFLAGS="${LDFLAGS} -L${withval}"]
)

dnl for libharkio2
AC_ARG_ENABLE(
  [harkio2],
  [  --enable-harkio2        Enable harkio2 library],
  [
    case "${enableval}" in
      yes) harkio2=true ;;
      no)  harkio2=false ;;
      *) AC_MSG_ERROR([bad value ${enableval} for --enable-harkio2]) ;;
    esac
  ],
  [harkio2=false]
)
AM_CONDITIONAL([ENABLE_HARKIO2], [test x$harkio2 = xtrue])
if test "$enable_harkio2" = "yes"; then
  AC_DEFINE([ENABLE_HARKIO2])
  AC_DEFINE([ENABLE_HARKIO])
fi
AC_ARG_WITH(
  [harkio2-inc],
  [  --with-harkio2-inc=DIR      location of libharkio2 include files],
  [CPPFLAGS="${CPPFLAGS} -I ${withval}"]
)
AC_ARG_WITH(
  [harkio2-lib],
  [  --with-harkio2-lib=DIR      location of libharkio2 library files],
  [LDFLAGS="${LDFLAGS} -L${withval}"]
)



dnl for libharkio3
AC_ARG_ENABLE(
  [harkio3],
  [  --enable-harkio3        Enable harkio3 library],
  [
    case "${enableval}" in
      yes) harkio3=true ;;
      no)  harkio3=false ;;
      *) AC_MSG_ERROR([bad value ${enableval} for --enable-harkio3]) ;;
    esac
  ],
  [harkio3=false]
)
AM_CONDITIONAL([ENABLE_HARKIO3], [test x$harkio3 = xtrue])
if test "$enable_harkio3" = "yes"; then
  AC_DEFINE([ENABLE_HARKIO3])
  AC_DEFINE([ENABLE_HARKIO_NH])
fi
AC_ARG_WITH(
  [harkio3-inc],
  [  --with-harkio3-inc=DIR      location of libharkio3 include files],
  [CPPFLAGS="${CPPFLAGS} -I ${withval}"]
)
AC_ARG_WITH(
  [harkio3-lib],
  [  --with-harkio3-lib=DIR      location of libharkio3 library files],
  [LDFLAGS="${LDFLAGS} -L${withval}"]
)



dnl for checking the existence of pkg-config
if test "$enable_harkio1" != "yes" -a "$enable_harkio2" != "yes"; then
   AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
   if test x"$PKG_CONFIG" == "xno"; then
      AC_MSG_ERROR([You have to install pkg-config])
   fi
fi

dnl load libharkio1 by pkg-config
if test "$enable_harkio1" != "yes"; then
   PKG_CHECK_MODULES([LIBHARKIO], [libharkio],
   [
      have_libharkio=yes
      AC_DEFINE([ENABLE_HARKIO1])
      AC_DEFINE([ENABLE_HARKIO])
      AM_CONDITIONAL([ENABLE_HARKIO1], [test x$have_libharkio = xyes])
      CPPFLAGS="${CPPFLAGS} `pkg-config libharkio --cflags-only-I`"      
      LDFLAGS="${LDFLAGS} `pkg-config libharkio --libs-only-L`"      
      echo "Enable  have_libharkio=$have_libharkio"
   ],
   [ 
      have_libharkio=no
      echo "Disable  have_libharkio=$have_libharkio"
   ])
fi

dnl load libharkio2 by pkg-config
if test "$enable_harkio2" != "yes"; then
   PKG_CHECK_MODULES([LIBHARKIO2], [libharkio2],
   [
      have_libharkio2=yes
      AC_DEFINE([ENABLE_HARKIO2])
      AC_DEFINE([ENABLE_HARKIO])
      AM_CONDITIONAL([ENABLE_HARKIO2], [test x$have_libharkio2 = xyes])
      CPPFLAGS="${CPPFLAGS} `pkg-config libharkio2 --cflags-only-I`"      
      LDFLAGS="${LDFLAGS} `pkg-config libharkio2 --libs-only-L`"      
      echo "Enable  have_libharkio2=$have_libharkio2"
   ],
   [ 
      have_libharkio2=no
      echo "Disable  have_libharkio2=$have_libharkio2"
   ])
fi


# load libharkio3 by pkg-config
if test "$enable_harkio3" != "yes"; then
   PKG_CHECK_MODULES([LIBHARKIO3], [libharkio3],
   [
      have_libharkio3=yes
      AC_DEFINE(ENABLE_HARKIO3)
      AC_DEFINE(ENABLE_HARKIO_NH)
      AM_CONDITIONAL(ENABLE_HARKIO3, test x$have_libharkio3 = xyes)
      CPPFLAGS="${CPPFLAGS} `pkg-config libharkio3 --cflags-only-I`"      
      LDFLAGS="${LDFLAGS} `pkg-config libharkio3 --libs-only-L`"      
      echo "Enable  have_libharkio3=$have_libharkio3"
   ],
   [ 
      have_libharkio3=no
      echo "Disable  have_libharkio3=$have_libharkio3"
   ])
fi


# load libzip by pkg-config
if test "$enable_harkio3" != "yes"; then
   PKG_CHECK_MODULES([LIBZIP], [libzip],
   [
      CPPFLAGS="${CPPFLAGS} `pkg-config libzip --cflags-only-I`"      
   ],
   [ 
      echo "libzip not found"
   ])
fi



dnl for AS-RASP
AC_ARG_ENABLE(
  [asrasp],
  [  --enable-asrasp         Enable AS-RASP in librecord],
  [
    case "${enableval}" in
      yes) asrasp=true ;;
      no)  asrasp=false ;;
      *) AC_MSG_ERROR([bad value ${enableval} for --enable-asrasp]) ;;
    esac
  ],
  [asrasp=false]
)
AM_CONDITIONAL([ENABLE_ASRASP], [test x$asrasp = xtrue])
if test "$enable_asrasp" = "yes"; then
  AC_DEFINE(ENABLE_ASRASP)
fi
AC_ARG_WITH(
  [asrasp-inc],
  [  --with-asrasp-inc=DIR      location of AS-RASP include files],
  [CPPFLAGS="${CPPFLAGS} -I ${withval}"]
)
AC_ARG_WITH(
  [asrasp-lib],
  [  --with-asrasp-lib=DIR      location of AS-RASP library files],
  [LDFLAGS="${LDFLAGS} -L${withval}"]
)

dnl for Wireless System
AC_ARG_ENABLE(
  [ws],
  [  --enable-ws             Enable Wireless System in librecord],
  [
    case "${enableval}" in
      yes) ws=true ;;
      no)  ws=false ;;
      *) AC_MSG_ERROR([bad value ${enableval} for --enable-ws]) ;;
    esac
  ],
  [ws=false]
)
AM_CONDITIONAL([ENABLE_WS], [test x$ws = xtrue])
if test "$enable_ws" = "yes"; then
  AC_DEFINE([ENABLE_WS])
fi
AC_ARG_WITH(
  [ws-inc],
  [  --with-ws-inc=DIR          location of Wireless System include files],
  [CPPFLAGS="${CPPFLAGS} -I ${withval}"]
)
AC_ARG_WITH(
  [ws-lib],
  [  --with-ws-lib=DIR          location of Wireless System library files],
  [LDFLAGS="${LDFLAGS} -L${withval}"]
)

dnl for RASP24 System
AC_ARG_ENABLE(
  [rasp24],
  [  --enable-rasp24         Enable RASP24 System in librecord],
  [
    case "${enableval}" in
      yes) rasp24=true ;;
      no)  rasp24=false ;;
      *) AC_MSG_ERROR([bad value ${enableval} for --enable-rasp24]) ;;
    esac
  ],
  [rasp24=false]
)
AM_CONDITIONAL([ENABLE_RASP24], [test x$rasp24 = xtrue])
if test "$enable_rasp24" = "yes"; then
  AC_DEFINE([ENABLE_RASP24])
fi
AC_ARG_WITH(
  [rasp24-inc],
  [  --with-rasp24-inc=DIR      location of RASP24 System include files],
  [CPPFLAGS="${CPPFLAGS} -I ${withval}"]
)
AC_ARG_WITH(
  [rasp24-lib],
  [  --with-rasp24-lib=DIR      location of RASP24 System library files],
  [LDFLAGS="${LDFLAGS} -L${withval}"]
)

dnl for RASPLC System
AC_ARG_ENABLE(
  [rasplc],
  [  --enable-rasplc         Enable RASPLC System in librecord],
  [
    case "${enableval}" in
      yes) rasplc=true ;;
      no)  rasplc=false ;;
      *) AC_MSG_ERROR([bad value ${enableval} for --enable-rasplc]) ;;
    esac
  ],
  [rasplc=false]
)
AM_CONDITIONAL([ENABLE_RASPLC], [test x$rasplc = xtrue])
if test "$enable_rasplc" = "yes"; then
  AC_DEFINE([ENABLE_RASPLC])
fi
AC_ARG_WITH(
  [rasplc-inc],
  [  --with-rasplc-inc=DIR      location of RASPLC System include files],
  [CPPFLAGS="${CPPFLAGS} -I ${withval}"]
)
AC_ARG_WITH(
  [rasplc-lib],
  [  --with-rasplc-lib=DIR      location of RASPLC System library files],
  [LDFLAGS="${LDFLAGS} -L${withval}"]
)

dnl for KANEKO System
AC_ARG_ENABLE(
  [kaneko],
  [  --enable-kaneko         Enable KANEKO System in librecord],
  [
    case "${enableval}" in
      yes) kaneko=true ;;
      no)  kaneko=false ;;
      *) AC_MSG_ERROR([bad value ${enableval} for --enable-kaneko]) ;;
    esac
  ],
  [kaneko=false]
)
AM_CONDITIONAL([ENABLE_KANEKO], [test x$kaneko = xtrue])
if test "$enable_kaneko" = "yes"; then
  AC_DEFINE([ENABLE_KANEKO])
fi
AC_ARG_WITH(
  [kaneko-inc],
  [  --with-kaneko-inc=DIR      location of KANEKO System include files],
  [CPPFLAGS="${CPPFLAGS} -I ${withval}"]
)
AC_ARG_WITH(
  [kaneko-lib],
  [  --with-kaneko-lib=DIR      location of KANEKO System library files],
  [LDFLAGS="${LDFLAGS} -L${withval}"]
)

dnl for HARKNETAPI System
AC_ARG_ENABLE(
  [harknetapi],
  [  --enable-harknetapi         Enable HARKNETAPI System in librecord],
  [
    case "${enableval}" in
      yes) harknetapi=true ;;
      no)  harknetapi=false ;;
      *) AC_MSG_ERROR([bad value ${enableval} for --enable-harknetapi]) ;;
    esac
  ],
  [harknetapi=false]
)
AM_CONDITIONAL([ENABLE_HARKNETAPI], [test x$harknetapi = xtrue])
if test "$enable_harknetapi" = "yes"; then
  AC_DEFINE([ENABLE_HARKNETAPI])
fi
AC_ARG_WITH(
  [harknetapi-inc],
  [  --with-harknetapi-inc=DIR      location of HARKNETAPI System include files],
  [CPPFLAGS="${CPPFLAGS} -I ${withval}"]
)
AC_ARG_WITH(
  [harknetapi-lib],
  [  --with-harknetapi-lib=DIR      location of HARKNETAPI System library files],
  [LDFLAGS="${LDFLAGS} -L${withval}"]
)

dnl for TDBD8CSUSB
#AC_ARG_ENABLE(
#  [tdbd8csusb],
#  [  --enable-tdbd8csusb     Enable TD-BD-8CS-USB (Tokyo Electron Device Co., Ltd.)  in librecord],
#  [
#    case "${enableval}" in
#      yes) tdbd8csusb=true ;;
#      no)  tdbd8csusb=false ;;
#      *) AC_MSG_ERROR([bad value ${enableval} for --enable-tdbd8csusb]) ;;
#    esac
#  ],
#  [tdbd8csusb=false]
#)
#AM_CONDITIONAL([ENABLE_TDBD8CSUSB], [test x$tdbd8csusb = xtrue])
#if test "$enable_tdbd8csusb" = "yes"; then
#  AC_DEFINE([ENABLE_TDBD8CSUSB])
#fi
#AC_ARG_WITH(
#  [tdbd8csusb-inc],
#  [  --with-tdbd8csusb-inc=DIR  location of TD-BD-8CS-USB include files],
#  [CPPFLAGS="${CPPFLAGS} -I ${withval}"]
#)
#AC_ARG_WITH(
#  [tdbd8csusb-lib],
#  [  --with-tdbd8csusb-lib=DIR  location of TD-BD-8CS-USB library files],
#  [LDFLAGS="${LDFLAGS} -L${withval}"]
#)
#
dnl for TDBD16ADUSB
#AC_ARG_ENABLE(
#  [tdbd16adusb],
#  [  --enable-tdbd16adusb    Enable TD-BD-16AD-USB (Tokyo Electron Device Co., Ltd.)  in librecord],
#  [
#    case "${enableval}" in
#      yes) tdbd16adusb=true ;;
#      no)  tdbd16adusb=false ;;
#      *) AC_MSG_ERROR(bad value ${enableval} for --enable-tdbd16adusb) ;;
#    esac
#  ],
#  [tdbd16adusb=false]
#)
#AM_CONDITIONAL([ENABLE_TDBD16ADUSB], [test x$tdbd16adusb = xtrue])
#if test "$enable_tdbd16adusb" = "yes"; then
#  AC_DEFINE([ENABLE_TDBD16ADUSB])
#fi
#AC_ARG_WITH(
#  [tdbd16adusb-inc],
#  [  --with-tdbd16adusb-inc=DIR location of TD-BD-16AD-USB include files],
#  [CPPFLAGS="${CPPFLAGS} -I ${withval}"]
#)
#AC_ARG_WITH(
#  [tdbd16adusb-lib],
#  [  --with-tdbd16adusb-lib=DIR location of TD-BD-16AD-USB library files],
#  [LDFLAGS="${LDFLAGS} -L${withval}"]
#)

dnl for RASP2
AC_ARG_ENABLE(
  [rasp2],
  [  --enable-rasp2          Enable RASP2 (JEOL SYSTEM TECHNOLOGY CO., LTD.) in librecord],
  [
    case "${enableval}" in
      yes) rasp2=true ;;
      no)  rasp2=false ;;
      *) AC_MSG_ERROR(bad value ${enableval} for --enable-rasp2) ;;
    esac
  ],
  [rasp2=false]
)
AM_CONDITIONAL([ENABLE_RASP2], [test x$rasp2 = xtrue])
if test "$enable_rasp2" = "yes"; then
  AC_DEFINE([ENABLE_RASP2])
fi
AC_ARG_WITH(
  [rasp2-inc],
  [  --with-rasp2-inc=DIR       location of RASP2 include files],
  [CPPFLAGS="${CPPFLAGS} -I ${withval}"]
)
AC_ARG_WITH(
  [rasp2-lib],
  [  --with-rasp2-lib=DIR       location of RASP2 library files],
  [LDFLAGS="${LDFLAGS} -I ${withval}"]
)

# for PlayAudio
AC_ARG_ENABLE(
  [playaudio],
  [  --enable-playaudio          Enable PlayAudio],
  [
    case "${enableval}" in
      yes) playaudio=true ;;
      no)  playaudio=false ;;
      *) AC_MSG_ERROR(bad value ${enableval} for --enable-playaudio) ;;
    esac
  ],
  [playaudio=false]
)
AM_CONDITIONAL([ENABLE_PLAYAUDIO], [test x$playaudio = xtrue])
if test "$enable_playaudio" = "yes"; then
  AC_DEFINE([ENABLE_PLAYAUDIO])
fi

AC_OUTPUT

dnl ## obsolete macros ##

dnl Stolen from gtk+:

dnl includedir=$includedir/flowdesigner
dnl datadir=$datadir/flowdesigner
dnl libdir=$datadir/lib

dnl ## legacy arguments
dnl AM_CONFIG_HEADER(config.h)
dnl AM_INIT_AUTOMAKE([$PACKAGE], $VERSION, no-define)

dnl ## already included libtool macros in m4 search path
dnl AC_DISABLE_STATIC
dnl AM_PROG_LIBTOOL

dnl AM_C_PROTOTYPES

dnl ## replaced to AC_LANG([C++])
dnl AC_LANG_CPLUSPLUS

dnl AC_INST_EXTENSIONS

dnl AM_PATH_GTK_2_0(2.0.0,,AC_MSG_ERROR([GTK+ is required.]))

dnl AX_BOOST_BASE(1.33.1)

dnl AC_OVERFLOW_CHECKS
dnl LDFLAGS=`flowdesigner-config config libflow audio_blocks --libs`
dnl CXXFLAGS=`flowdesigner-config config libflow audio_blocks --cflags`


