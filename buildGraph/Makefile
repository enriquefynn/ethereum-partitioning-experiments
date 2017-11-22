THISDIR := $(realpath .)

EXECUTABLE = buildGraph

UNAME    := $(shell uname)

CPPFLAGS = 

INCLUDES = \
	-I $(THISDIR)/../libMETIS/include/ \
	-I $(THISDIR)/../libcommon/include/ \
	-I $(THISDIR)/../libutils/include/
	
LDFLAGS = \
	-L $(THISDIR)/../libMETIS/bin \
	-L $(THISDIR)/../libcommon/bin \
	-L /usr/local/lib \
	-lMETIS \
	-lcommon 

include ../Makefile.exe
