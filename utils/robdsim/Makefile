GCC=g++
ROOTCINT=rootcint 
ROOT_CFLAGS=`root-config --cflags`
ROOT_LIBS=`root-config --glibs`
TARGET=robdsim.so

COBJS=Sampler.o Eloss.o PrecisionEloss.o SamplerDict.o ElossDict.o PrecisionElossDict.o robdsimOutput.o robdsimOutputDict.o SamplerAnalysis.o


%.o : %.C
	$(GCC) $(ROOT_CFLAGS) -c $< 

%Dict.C : %.h
	$(ROOTCINT) $@ -c $<

robdsim.so : $(COBJS) 
	$(GCC) -shared $(ROOT_LIBS) -o $@ $(COBJS)

clean : 
	$(RM) -rf $(COBJS) $(TARGET) *Dict.C *Dict.h