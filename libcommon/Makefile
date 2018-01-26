THISDIR := $(realpath .)
LIBRARY = libcommon.so
UNAME    := $(shell uname)
CPPFLAGS = 
INCLUDES = \
	-I /usr/local/include \
	-I $(THISDIR)/../libpartitioning/include \
	-I $(THISDIR)/../libutils/include
LDFLAGS = \
	-L /usr/local/lib \
	-I $(THISDIR)/../libpartitioning/bin \
	-L $(THISDIR)/../libutils/bin

include ../Makefile.lib
