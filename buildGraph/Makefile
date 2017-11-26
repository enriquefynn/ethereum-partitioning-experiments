THISDIR := $(realpath .)

EXECUTABLE = buildGraph

UNAME    := $(shell uname)

CPPFLAGS = 

INCLUDES = \
	-I $(THISDIR)/../libMETIS/include/ \
	-I $(THISDIR)/../libcommon/include/ \
	-I $(THISDIR)/../libutils/include/ \
	-I $(THISDIR)/../libpartitioning/include
	
LDFLAGS = \
	-L $(THISDIR)/../libMETIS/bin \
	-L $(THISDIR)/../libcommon/bin \
	-L $(THISDIR)/../libutils/bin \
	-L $(THISDIR)/../libpartitioning/bin \
	-L /usr/local/lib \
	-lMETIS \
	-lcommon \
	-lutils \
	-lpartitioning

include ../Makefile.exe
