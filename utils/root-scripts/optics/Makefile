CC=g++
CCFLAGS=-c -g -Wall `root-config --cflags`
LDFLAGS=`root-config --glibs`
SOURCES=SamplerLoop.C Sampler.C
OBJECTS=$(SOURCES:.C=.o)
EXECUTABLE=main

%.o : %.C
	$(CC) $(CCFLAGS) -c $<

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
