THISDIR := $(realpath .)
LIBRARY = libMETIS.so
UNAME    := $(shell uname)
CPPFLAGS = 
INCLUDES = \
	-I /usr/local/include\
	-I $(THISDIR)/../libcommon/include \
	-I $(THISDIR)/../libpartitioning/include
LDFLAGS = \
	-L /usr/local/lib \
	-L $(THISDIR)/../libpartitioning/bin \
	-lpartitioning \
	-lmetis

include ../Makefile.lib
