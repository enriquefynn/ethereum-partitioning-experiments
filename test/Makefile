THISDIR := $(realpath .)

EXECUTABLE = run_tests 

UNAME    := $(shell uname)

CPPFLAGS =

INCLUDES = \
	-I $(THISDIR)/../libcommon/include \
	-I $(THISDIR)/../libutils/include \
 	-I $(THISDIR)/../libcommon/include \
	-I $(THISDIR)/../libMETIS/include \
	-I $(THISDIR)/../libfuture/include \
 	-I $(THISDIR)/../libpartitioning/include

LDFLAGS = \
	-L $(THISDIR)/../libpartitioning/bin \
	-L $(THISDIR)/../libMETIS/bin \
	-L $(THISDIR)/../libfuture/bin \
	-L $(THISDIR)/../libutils/bin \
	-L $(THISDIR)/../libcommon/bin \
	-lgtest \
	-lmetismethods \
	-lfuture \
	-lpartitioning \
	-lutils \
	-lcommon

ifeq ($(UNAME), Linux)
LDFLAGS += \
	-Wl,-rpath,$(THISDIR)/../libpartitioning/bin
endif

include ../Makefile.exe
