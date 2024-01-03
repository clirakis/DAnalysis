##################################################################
#
#	Makefile for Analysis using gcc on Linux. 
#       Analyze Raspberry Pi PiDA results. 
#
#
#	Modified	by	Reason
# 	--------	--	------
#	02-Jan-24       CBL     Original
#
#
######################################################################
# Machine specific stuff
#
#
TARGET = Analysis
#
# Compile time resolution.
#
INCLUDE = -I$(DRIVE)/common/utility -I$(COMMON)/SignalProcessing \
	-I/usr/include/hdf5/serial -I$(ROOT_INC) \

LIBS = -lutility -lhdf5_cpp -lhdf5 -lSignal
LIBS += -L$(HDF5LIB) -lconfig++ $(ROOT_LIBS)


# Rules to make the object files depend on the sources.
SRC     = 
SRCCPP  = main.cpp Analysis.cpp UserSignals.cpp
SRCS    = $(SRC) $(SRCCPP)

HEADERS = Analysis.hh UserSignals.hh Version.hh

# When we build all, what do we build?
all:      $(TARGET)

include $(DRIVE)/common/makefiles/makefile.inc


#dependencies
include make.depend 
# DO NOT DELETE
