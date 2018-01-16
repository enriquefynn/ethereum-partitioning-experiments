THISDIR := $(realpath .)
LIBRARY = libmetismethods.so
UNAME    := $(shell uname)
CPPFLAGS = 
INCLUDES = \
	-I /usr/local/include\
	-I $(THISDIR)/../libcommon/include \
	-I $(THISDIR)/../libutils/include \
	-I $(THISDIR)/../libpartitioning/include
LDFLAGS = \
	-L /usr/local/lib \
	-L $(THISDIR)/../libpartitioning/bin \
	-L $(THISDIR)/../libutils/bin \
	-L ~/GIT/hmetis-1.5-osx-i686 \
	-lpartitioning \
	-lmetis \
	-lutils
	# -lhmetis

include ../Makefile.lib
