#include "BDSOutputROOTEventSampler.hh"

class SamplerAnalysis
{
public:
  SamplerAnalysis();
#ifndef __ROOTDOUBLE__
  SamplerAnalysis(BDSOutputROOTEventSampler<float>*);
#else 
  SamplerAnalysis(BDSOutputROOTEventSampler<double>*);
#endif
  void CommonCtor();
  virtual ~SamplerAnalysis();

  void Initialise();
  void Process();
  void Terminate();
#ifndef __ROOTDOUBLE__
  BDSOutputROOTEventSampler<float> *s;
#else 
  BDSOutputROOTEventSampler<double> *s;
#endif
protected:
  // sums - initialised to zero as that's what they start at
  double npart;

  typedef std::vector<std::vector<double>>                           twoDArray;
  typedef std::vector<std::vector<std::vector<std::vector<double>>>> fourDArray;
  std::vector<double>               means;
  std::vector<double>               sums;
  fourDArray                        powSums;
  twoDArray                         covMats;
  fourDArray                        cenMoms;

  static double powSumToCentralMoment(fourDArray &powSum, int i, int j, int m, int n);
};
