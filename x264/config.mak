SRCPATH=.
prefix=./ffmpeg_build
exec_prefix=${prefix}
bindir=./bin
libdir=${exec_prefix}/lib
includedir=${prefix}/include
SYS_ARCH=X86_64
SYS=LINUX
CC=gcc
CFLAGS=-Wno-maybe-uninitialized -Wshadow -O3 -ffast-math -m64  -Wall -I. -I$(SRCPATH) -std=gnu99 -D_GNU_SOURCE -mpreferred-stack-boundary=6 -fPIC -fomit-frame-pointer -fno-tree-vectorize -fvisibility=hidden
CFLAGSSO=
CFLAGSCLI=
COMPILER=GNU
COMPILER_STYLE=GNU
DEPMM=-MM -g0
DEPMT=-MT
LD=gcc -o 
LDFLAGS=-m64  -lm -lpthread -ldl
LDFLAGSCLI=-ldl 
LIBX264=libx264.a
CLI_LIBX264=$(LIBX264)
AR=gcc-ar rc 
RANLIB=gcc-ranlib
STRIP=strip
INSTALL=install
AS=nasm
ASFLAGS= -I. -I$(SRCPATH) -DARCH_X86_64=1 -I$(SRCPATH)/common/x86/ -f elf64 -DSTACK_ALIGNMENT=64 -DPIC
RC=
RCFLAGS=
EXE=
HAVE_GETOPT_LONG=1
DEVNULL=/dev/null
PROF_GEN_CC=-fprofile-generate
PROF_GEN_LD=-fprofile-generate
PROF_USE_CC=-fprofile-use
PROF_USE_LD=-fprofile-use
HAVE_OPENCL=yes
CC_O=-o $@
default: cli
install: install-cli
default: lib-static
install: install-lib-static
