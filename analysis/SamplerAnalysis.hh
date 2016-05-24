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
  std::vector<std::vector<double>> GetOpticalFunctions();

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
  std::vector<double>               means;       // x, xp, y, yp, E, t 
  std::vector<double>               sums;        // not used
  fourDArray                        powSums;
  fourDArray                        cenMoms;
  twoDArray                         covMats;
  twoDArray                         optical;     // e, ax, gx, etax, bx, ay, gy, by, etay

  static double powSumToCentralMoment(fourDArray &powSum, int npart ,int i, int j, int m, int n);

  static double centMomToCovariance(fourDArray &centMoms, int npart ,int a, int i, int j);
};
