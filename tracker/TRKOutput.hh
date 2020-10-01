#ifndef TRKOUTPUT_H
#define TRKOUTPUT_H

#include <string>

#include "TRKOutputSamplers.hh"

class TRKBunch;
class BDSOutput;

class TRKOutput
{
public:
  TRKOutput(std::string file);
  ~TRKOutput();
  void NewFile();
  void CloseFile();
  void WritePrimaries(TRKBunch* bunch);

  void FillSamplerHitsTracker(int samplerIndex,
			      TRKParticle& particle,
			      double s);

  void RecordSamplerHit(int samplerIndex, TRKParticle const &particle, int turn,
                        double s);

  void PushBackSampler(const std::string& name);
  void PushBackSampler(const std::string& name, std::size_t reserved);
  

  void WriteEvents();
  int currentTurn;

private:
  std::string filename;
  BDSOutput* output;
  TRKSamplerData primaries;
  TRKOutputSamplers samplers;
  int nevents;

  void FillEventOutputStructures();
  void PrimaryFillOutput(int eventID);
  void SamplersFillOutput(int eventID);

};

#endif /* TRKOUTPUT_H */
