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

  void AddNSamplers(int);

private:
  std::string filename;
  BDSOutput* output;
  TRKOutputSamplers samplers;

};

#endif /* TRKOUTPUT_H */
