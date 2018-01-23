THISDIR := $(realpath .)
LIBRARY = libfuture.so
UNAME    := $(shell uname)
CPPFLAGS = 
INCLUDES = \
	-I /usr/local/include \
	-I $(THISDIR)/../libcommon/include \
	-I $(THISDIR)/../libutils/include \
	-I $(THISDIR)/../libpartitioning/include
LDFLAGS = \
	-L /usr/local/lib \
	-L $(THISDIR)/../libutils/bin \
	-L $(THISDIR)/../libpartitioning/bin \
	-lpartitioning \
	-lutils

include ../Makefile.lib
