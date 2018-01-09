THISDIR := $(realpath .)
LIBRARY = libcommon.so
UNAME    := $(shell uname)
CPPFLAGS = 
INCLUDES = \
	-I /usr/local/include \
	-I $(THISDIR)/../libutils/include
LDFLAGS = \
	-L /usr/local/lib \
	-L $(THISDIR)/../libutils/bin

include ../Makefile.lib
