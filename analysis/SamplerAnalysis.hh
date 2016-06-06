#ifndef SAMPLERANALYSIS_H
#define SAMPLERANALYSIS_H

#include "BDSOutputROOTEventSampler.hh"

/**
 * @brief Analysis routines for an individual sampler.
 *
 * @author Stewart Boogert, Andrey Abramov
 */

class SamplerAnalysis
{
public:
  SamplerAnalysis();

  // Note compile time float / double templating.
#ifndef __ROOTDOUBLE__
  SamplerAnalysis(BDSOutputROOTEventSampler<float>*);
#else 
  SamplerAnalysis(BDSOutputROOTEventSampler<double>*);
#endif
  /// Initialisation of arrays for optical function calculations
  void CommonCtor();
  virtual ~SamplerAnalysis();

  /// Initialise variables.
  void Initialise();

  /// Loop over all entries in the sampler and accumulate power sums over variuos moments.
  void Process();

  /// Calculate optical functions based on combinations of moments already accumulated.
  void Terminate();

  /// Accessor for optical functions
  std::vector<std::vector<double> > GetOpticalFunctions() {return optical;}

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

#endif
