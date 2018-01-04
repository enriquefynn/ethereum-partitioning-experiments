THISDIR := $(realpath .)
LIBRARY = libutils.so
UNAME    := $(shell uname)
CPPFLAGS = 
INCLUDES = \
	-I /usr/local/include \
	-I $(THISDIR)/../libcommon/include \
	-I $(THISDIR)/../libMETIS/include/ \
	-I $(THISDIR)/../libpartitioning/include
LDFLAGS = \
	-L /usr/local/lib

include ../Makefile.lib
