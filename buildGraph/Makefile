THISDIR := $(realpath .)

EXECUTABLE = buildGraph

UNAME    := $(shell uname)

CPPFLAGS = 

INCLUDES = \
	-I $(THISDIR)/../libMETIS/include/ \
	-I $(THISDIR)/../libcommon/include/ \
	-I $(THISDIR)/../libutils/include/ \
	-I $(THISDIR)/../libfuture/include \
	-I $(THISDIR)/../libpartitioning/include
	
LDFLAGS = \
	-L $(THISDIR)/../libMETIS/bin \
	-L $(THISDIR)/../libcommon/bin \
	-L $(THISDIR)/../libutils/bin \
	-L $(THISDIR)/../libfuture/bin \
	-L $(THISDIR)/../libpartitioning/bin \
	-L /usr/local/lib \
	-lmetismethods \
	-lutils \
	-lfuture \
	-lpartitioning \
	-lcommon

ifeq ($(UNAME), Linux)
    LDFLAGS += \
    -lmetis \
		-Wl,-rpath,/home/fynn/.local/lib \
		-Wl,-rpath,$(THISDIR)/../libMETIS/bin \
		-Wl,-rpath,$(THISDIR)/../libcommon/bin \
		-Wl,-rpath,$(THISDIR)/../libutils/bin \
		-Wl,-rpath,$(THISDIR)/../libfuture/bin \
		-Wl,-rpath,$(THISDIR)/../libpartitioning/bin
endif

include ../Makefile.exe
