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
  double S;

  typedef std::vector<std::vector<double>>                           twoDArray;
  typedef std::vector<std::vector<std::vector<double>>>              threeDArray; 
  typedef std::vector<std::vector<std::vector<std::vector<double>>>> fourDArray;

  fourDArray                        powSums;
  fourDArray                        cenMoms;
  threeDArray                       covMats;
  threeDArray                       derivMats;
  twoDArray                         optical;     // emt, alf, bta, gma, eta, etapr, mean, sigma
  twoDArray                         varOptical;  // variances of optical functions

  static double powSumToCentralMoment(fourDArray &powSum, int npart ,int i, int j, int m, int n);
  static double centMomToCovariance(fourDArray &centMoms, int npart ,int k, int i, int j);
  static double centMomToDerivative(fourDArray &centMoms, int k, int t, int i);
};
