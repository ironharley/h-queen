AC_INIT([hqn], [1.0])
AM_PROG_LIBTOOL
AM_PROG_AR
AC_PROG_CXX
AM_INIT_AUTOMAKE([-Wall foreign subdir-objects])
AC_ENABLE_SHARED
AC_CANONICAL_HOST
# OS name, version and so on 
OSVER=`uname -r | tr '()/' " " | cut -f1 -d-`.0.0.0
OS_MAJOR_VER=`echo $OSVER | cut -f1 -d.`
OS_MINOR_VER=`echo $OSVER | cut -f2 -d.`
OS_POINT_VER=`echo $OSVER | cut -f3 -d.`
OS_PATCH_VER=`echo $OSVER | cut -f4 -d.`
AC_DEFINE_UNQUOTED(OS_MAJOR_VER, $OS_MAJOR_VER)
AH_TEMPLATE([OS_MAJOR_VER], [Major OS version])
AC_DEFINE_UNQUOTED(OS_MINOR_VER, $OS_MINOR_VER)
AH_TEMPLATE([OS_MINOR_VER], [Minor OS version])
AC_DEFINE_UNQUOTED(OS_POINT_VER, $OS_POINT_VER)
AH_TEMPLATE([OS_POINT_VER], [Point OS version])
AC_DEFINE_UNQUOTED(OS_PATCH_VER, $OS_PATCH_VER)
AH_TEMPLATE([OS_PATCH_VER], [Patch OS version])

PWDPATH=`pwd`
OSTYPE=`uname`
case $OSTYPE in
solaris*)
  OSTYPE=SunOS
  ;;
freebsd*)
  OSTYPE=FreeBSD
  ;;
Linux*)
  OSTYPE=Linux
  ;;
darwin*)
  OSTYPE=Darwin
  ;;
msys*)
  OSTYPE=MinGW
  ;;
MINGW*)
  OSTYPE=MinGW
  ;;
*)
  echo "Using native OSTYPE value: $OSTYPE"
  ;;
esac

ARCH=$host_cpu
case $host_cpu in
x86_64)
  ;;
i?86)
  LARCH=IA32
  ;;
*)
  LARCH="$host_cpu"
  echo "Using native ARCH/LARCH value from \$host_cpu: $host_cpu"
  ;;
esac
OSTYPE_UPPER=`echo $OSTYPE | sed 'y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/'`
ARCH_UPPER=`echo $host_cpu | sed 'y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/'`
LARCH_UPPER=`echo $LARCH | sed 'y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/'`
ARCH_CPPFLAGS="-DHQMQ_OSTYPE_${OSTYPE_UPPER} -DHQMQ_ARCH_${ARCH_UPPER} -DHQMQ_LARCH_${LARCH_UPPER}"

#AC_C_BIGENDIAN([AC_DEFINE_UNQUOTED(RESIP_BIG_ENDIAN, , RESIP_BIG_ENDIAN)])

CXXFLAGS="-g -O3 -std=c++14 ${ARCH_CPPFLAGS} -DBOOST_LOG_DYN_LINK -Wall -Wno-deprecated -Wnonnull -Wuninitialized -Wunused-function -fpermissive "

AC_LANG_PUSH([C++])
AC_CHECK_HEADER([boost/asio.hpp], [], [AC_MSG_ERROR(missing boost header(s))])
AC_CHECK_HEADER([boost/bind.hpp], [], [AC_MSG_ERROR(missing boost header(s))])
AC_CHECK_HEADER([boost/uuid/uuid.hpp], [], [AC_MSG_ERROR(missing boost header(s))])
AC_CHECK_HEADER([boost/property_tree/ptree.hpp], [], [AC_MSG_ERROR(missing boost header(s))])
AC_CHECK_HEADER([boost/property_tree/json_parser.hpp], [], [AC_MSG_ERROR(missing boost header(s))])
AC_CHECK_HEADER([boost/program_options.hpp], [], [AC_MSG_ERROR(missing boost header(s))])
AC_CHECK_HEADER([boost/log/trivial.hpp], [], [AC_MSG_ERROR(missing boost header(s))])
AC_CHECK_HEADER([boost/log/utility/setup.hpp], [], [AC_MSG_ERROR(missing boost header(s))])
AC_CHECK_HEADER([boost/log/expressions.hpp], [], [AC_MSG_ERROR(missing boost header(s))])
AC_CHECK_HEADER([boost/archive/iterators/base64_from_binary.hpp], [], [AC_MSG_ERROR(missing boost header(s))])
AC_CHECK_HEADER([boost/archive/iterators/insert_linebreaks.hpp], [], [AC_MSG_ERROR(missing boost header(s))])
AC_CHECK_HEADER([boost/archive/iterators/transform_width.hpp], [], [AC_MSG_ERROR(missing boost header(s))])
AC_CHECK_HEADER([boost/archive/iterators/ostream_iterator.hpp], [], [AC_MSG_ERROR(missing boost header(s))])
AC_CHECK_HEADER([boost/test/unit_test.hpp], [], [AC_MSG_ERROR(missing boost header(s))])
AC_CHECK_HEADER([boost/crc.hpp], [], [AC_MSG_ERROR(missing boost header(s))])

AC_CHECK_HEADER([openssl/pem.h],  [], [AC_MSG_FAILURE(missing openssl header(s))])
AC_CHECK_HEADER([openssl/x509.h],  [], [AC_MSG_FAILURE(missing openssl header(s))])
AC_CHECK_HEADER([openssl/x509v3.h],  [], [AC_MSG_FAILURE(missing openssl header(s))])
AC_LANG_POP([C++])
 

LIBS="-L/usr/local/lib -L/usr/lib  -L/usr/lib64 \   
    -lssl \
    -lcrypto \
    -lpthread \
    -lboost_thread \
    -lboost_system \
    -lboost_program_options \    
    -lboost_log \
    -lboost_log_setup \
    -lboost_chrono \
    -lboost_atomic \
    -lboost_date_time \
    -lboost_filesystem \
    -lboost_iostreams \
    -lboost_timer \
    -lboost_context \
    ${LIBS} "


CXXFLAGS=" ${CXXFLAGS} -I/usr/include -I/usr/local/include -I/home/hkooper/local/include "
AC_CONFIG_FILES([
 Makefile
 src/client/Makefile
 src/server/Makefile
 tests/Makefile
])
AC_OUTPUT