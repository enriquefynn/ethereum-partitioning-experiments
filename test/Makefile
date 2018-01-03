THISDIR := $(realpath .)

EXECUTABLE = run_tests 

UNAME    := $(shell uname)

CPPFLAGS =

INCLUDES = \
	-I $(THISDIR)/../libcommon/include \
	-I $(THISDIR)/../libutils/include \
 	-I $(THISDIR)/../libcommon/include \
 	-I $(THISDIR)/../libpartitioning/include

LDFLAGS = \
	-L $(THISDIR)/../libpartitioning/bin \
	-L $(THISDIR)/../libutils/bin \
	-L $(THISDIR)/../libcommon/bin \
	-lgtest \
	-lpartitioning \
	-lutils \
	-lcommon

ifeq ($(UNAME), Linux)
LDFLAGS += \
	-Wl,-rpath,$(THISDIR)/../libpartitioning/bin
endif

include ../Makefile.exe
