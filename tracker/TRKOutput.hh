#ifndef TRKOUTPUT_H
#define TRKOUTPUT_H

#include <string>
#include "BDSOutput.hh"

class TRKBunch;
// class BDSOutput;

class TRKOutput
{
public:
  TRKOutput(std::string file);
  ~TRKOutput();
  void NewFile();
  void CloseFile();
  void WritePrimaries(TRKBunch* bunch);

  void FillSamplerHitsTracker(G4int samplerIndex,
			      TRKParticle& particle,
			      double s);
  template<typename ...Ts> void FillEvent(Ts ...args)
  {
    output->FillEvent(args...);
  }

private:
  std::string filename;
  BDSOutput* output;

};

#endif /* TRKOUTPUT_H */
