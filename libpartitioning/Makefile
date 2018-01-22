THISDIR := $(realpath .)
LIBRARY = libpartitioning.so
UNAME    := $(shell uname)
CPPFLAGS = 
INCLUDES = \
	-I /usr/local/include \
	-I $(THISDIR)/../libcommon/include \
	-I $(THISDIR)/../libutils/include
LDFLAGS = \
	-L /usr/local/lib \
	-L $(THISDIR)/../libutils/bin \
	-L $(THISDIR)/../libMETIS/bin \
	-lutils \
	-lmetis

include ../Makefile.lib
