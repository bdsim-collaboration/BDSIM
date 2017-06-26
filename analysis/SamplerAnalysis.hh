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
  SamplerAnalysis(BDSOutputROOTEventSampler<float>* samplerIn,
		  BDSOutputROOTEventSampler<float>* samplerFirt,
		  bool debugIn = false);
#else 
  SamplerAnalysis(BDSOutputROOTEventSampler<double>* samplerIn,
		  BDSOutputROOTEventSampler<double>* samplerFirst,
		  bool debugIn = false);
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
  BDSOutputROOTEventSampler<float> *p;
#else 
  BDSOutputROOTEventSampler<double> *s;
  BDSOutputROOTEventSampler<double> *p;
#endif
protected:
  // sums - initialised to zero as that's what they start at
  long long int npart;
  double S;

  typedef std::vector<std::vector<double>>                           twoDArray;
  typedef std::vector<std::vector<std::vector<double>>>              threeDArray; 
  typedef std::vector<std::vector<std::vector<std::vector<double>>>> fourDArray;

  fourDArray    powSums;
  fourDArray    cenMoms;

  fourDArray    powSumsFirst;
  fourDArray    cenMomsFirst;
  
  threeDArray   covMats;
  threeDArray   derivMats;
  twoDArray     optical;     ///< emt, alf, bta, gma, eta, etapr, mean, sigma
  twoDArray     varOptical;  ///< variances of optical functions

  static double powSumToCentralMoment(fourDArray &powSum, long long int npartIn ,int i, int j, int m, int n);
  static double centMomToCovariance(fourDArray &centMoms, long long int npartIn ,int k, int i, int j);
  static double centMomToDerivative(fourDArray &centMoms, int k, int t, int i);

private:
  bool debug;
};

#endif
