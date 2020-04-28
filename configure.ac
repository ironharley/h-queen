AC_INIT([hqn-server hqn-client], [1.0])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_PROG_CXX
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
linux*)
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

AC_C_BIGENDIAN([AC_DEFINE_UNQUOTED(RESIP_BIG_ENDIAN, , RESIP_BIG_ENDIAN)])

CXXFLAGS="${ARCH_CPPFLAGS} ${CXXFLAGS} -std=c++14 -g -O3 -Wall -Wno-deprecated -Wnonnull -Wuninitialized -fpermissive  "

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_PID_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_TYPE_OFF_T
AC_TYPE_SSIZE_T
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T

AC_CHECK_FUNCS([bzero])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([strerror])
AC_CHECK_FUNCS([socket])
AC_CHECK_FUNCS([gethostname])
AC_CHECK_FUNCS([atexit])
AC_CHECK_HEADER([fcntl.h], [])
AC_CHECK_HEADER([stdlib.h], [])
AC_CHECK_HEADER([unistd.h], [])
AC_CHECK_HEADER([stddef.h], [])
AC_CHECK_HEADER([arpa/inet.h], [])
AC_CHECK_HEADER([netdb.h], [])
AC_CHECK_HEADER([netinet/in.h], [])
AC_CHECK_HEADER([sys/socket.h], [])

AC_LANG_PUSH([C++])
AC_CHECK_HEADER([boost/asio.hpp], [], [AC_MSG_ERROR(missing boost header(s))])
AC_CHECK_HEADER([boost/bind.hpp], [], [AC_MSG_ERROR(missing boost header(s))])
AC_CHECK_HEADER([boost/uuid/uuid.hpp], [], [AC_MSG_ERROR(missing boost header(s))])
AC_CHECK_HEADER([boost/property_tree/ptree.hpp], [], [AC_MSG_ERROR(missing boost header(s))])
AC_CHECK_HEADER([boost/program_options.hpp], [], [AC_MSG_ERROR(missing boost header(s))])

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
    ${LIBS} "


CXXFLAGS=" ${CXXFLAGS} -I/usr/include -I/usr/local/include -I/home/hkooper/local/include "
AC_CONFIG_FILES([
 Makefile
 src/tcp-client/Makefile
 src/tcp-server/Makefile

])
AC_OUTPUT