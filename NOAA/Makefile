##################################################################
#
#	Makefile for ReadAK using gcc on Linux. 
#
#
#	Modified	by	Reason
# 	--------	--	------
#	10-Feb-24       CBL     Original
#
#
######################################################################
# Machine specific stuff
#
#
TARGET = ReadAK
#
# Compile time resolution.
#
INCLUDE = -I$(DRIVE)/common/utility
LIBS = -lutility 
LIBS += -lconfig++


# Rules to make the object files depend on the sources.
SRC     = 
SRCCPP  = main.cpp AKRead.cpp Plotting.cpp UserSignals.cpp
SRCS    = $(SRC) $(SRCCPP)

HEADERS = AKRead.hh Plotting.hh UserSignals.hh Version.hh

# When we build all, what do we build?
all:      $(TARGET)

include $(DRIVE)/common/makefiles/makefile.inc


#dependencies
include make.depend 
# DO NOT DELETE
