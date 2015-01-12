#---------------------------------------------------
# Olivier Dadoun Makefile for
# BDSIM display 
# Amended Steve Malton

ARCH         := $(shell root-config --arch)
ifeq ($(ARCH),macosx)
LIBFILES = libBDS.so libBDS.dylib
SOFLAGS  = -O -bundle -undefined dynamic_lookup
ifndef ROOTSYS
ROOTSYS       = /opt/local
endif
else
LIBFILES = libBDS.so
SOFLAGS  = -O -shared
endif

CXX           = g++
LD            = g++
CXXFLAGS      = $(shell $(ROOTSYS)/bin/root-config --cflags) -I../.. -fPIC
LDFLAGS       = 

ROOTLIBS      = $(shell $(ROOTSYS)/bin/root-config --libs)
LIBS          = $(ROOTLIBS) -L../../parser -lgmad

HDRS          = BDSCanvas.h BDSPad.h BDSAxis.h BDSParser.h BDSBox.h BDSPopUp.h

SRCS          = main.C BDSCanvas.C BDSPad.C BDSAxis.C BDSParser.C BDSBox.C BDSPopUp.C mydict.C 

OBJS          = BDSCanvas.o BDSPad.o BDSAxis.o BDSParser.o BDSBox.o BDSPopUp.o mydict.o

PROGRAM       = display

all:            $(PROGRAM) $(LIBFILES)

libBDS.so :
	@$(LD) $(SOFLAGS) $(LIBS) $(OBJS) -o $@

libBDS.dylib : 
	@$(LD) -O2 -dynamiclib -single_module -undefined dynamic_lookup	-install_name $@ $(LIBS) -o $@ $(OBJS)

$(PROGRAM):     $(OBJS) main.o
	@echo "Linking $(PROGRAM) ..."
	@/bin/rm -f $(PROGRAM)
	@$(LD) $(LDFLAGS) $(OBJS) main.o $(LIBS) -o $(PROGRAM)
	@chmod 555 $(PROGRAM)
	@echo "done"

clean:
	rm -f $(OBJS) $(PROGRAM) libBDS.so libBDS.dylib main.o mydict.*

###
BDSCanvas.o: BDSCanvas.h
BDSPad.o: BDSPad.h
BDSAxis.o: BDSAxis.h
BDSParser.o: BDSParser.h
BDSBox.o: BDSBox.h
BDSPopUp.o: BDSPopUp.h

mydict.C: BDSCanvas.h BDSPad.h BDSAxis.h BDSParser.h BDSBox.h BDSPopUp.h
	@echo "Generating dictionary ..."
	@rootcint mydict.C -c -I../.. BDSCanvas.h BDSPad.h BDSAxis.h BDSParser.h BDSBox.h BDSPopUp.h
#---------------------------------------------------
