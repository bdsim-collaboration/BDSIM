#ifndef __PARAMETERS_H
#define __PARAMETERS_H

#include <list>
#include <string>

struct Element;

/**
 * @brief Parameters - used in the parser
 * 
 * @author I. Agapov
 */

struct Parameters {

  /// length, multipole coefficients

  double B;  int Bset;    /// magnetic field
  double l;  int lset;    /// length
  double bmapZOffset; int bmapZOffsetset; //offset of the field map magnet field
  double k0; int k0set;   /// dipole 
  double k1; int k1set;   /// quadrupole
  double k2; int k2set;   /// sextupole
  double k3; int k3set;   /// octupole
  double ks; int ksset;   /// solenoid

  double tscint; int tscintset; ///thickness of scintillating part of screen
  double twindow; int twindowset; ///thickness of window

  std::list<double> knl;           /// multipole expansion coefficients
  std::list<double> ksl;           /// skew multipole expansion
  
  int knlset; int kslset;

  ///List of beam loss monitor locations
  std::list<double> blmLocZ;
  std::list<double> blmLocTheta;
  int blmLocZset; int blmLocThetaset;


  ///For MADX style sequences
  /* double at; int atset;///"at" and "from" define position in beam line */
  /* char from[256]; int fromset; */
  /* char refer[64]; int referset; */
  /* double absp; int abspset;///pos. from beg. of line */
  
  /// placement, geometrical sizes etc.

  double r; int rset; ///radius, i.e cylindrical sampler
  
  double angle; int angleset;   /// bending angle
  double phiAngleIn; int phiAngleInset;   /// incoming bending angle for element
  double phiAngleOut; int phiAngleOutset;   /// outgoing bending angle for element
  double beampipeThickness; int beampipeThicknessset;  
  double aper; int aperset;   /// aperture (circular)
  double aperX; int aperXset;   /// aperture (elliptical)
  double aperY; int aperYset;   
  double phi, theta, psi; /// for 3d transforms
  int phiset, thetaset, psiset;
  double tunnelRadius;
  int tunnelRadiusset;
  double tunnelOffsetX;
  int tunnelOffsetXset;

  ///which precision physics region the element is in (0 = none)
  int precisionRegion; int precisionRegionset;

  double aperYUp; int aperYUpset;  
  double aperYDown; int aperYDownset; 
  double aperDy; int aperDyset;
  
  double flatlength; int flatlengthset;
  double taperlength; int taperlengthset;
  double gradient; int gradientset;

  double outR; int outRset; /// outer radius of magnets
  double inR, bpRad; int inRset, bpRadset;/// inner radius and beam pipe radius of muon spoiler
  double hgap, hgapset;
  double xsize, ysize; int xsizeset, ysizeset; /// aperture (or laser spotsize for laser)
  double xdir, ydir, zdir, waveLength; int xdirset, ydirset, zdirset, waveLengthset;

  double tilt; int tiltset;   /// tilt

  /// for external geometry and field definition files
  std::string geometry; int geomset;
  std::string bmap; int bmapset;
  //  std::string emap; int emapset;
  std::string material; int materialset;
  std::string tunnelMaterial; int tunnelmaterialset;
  std::string tunnelCavityMaterial; int tunnelcavitymaterialset;

  /// string to pass a custom type specification
  std::string spec; int specset;

  /// material properties
  double A; int Aset;
  double Z; int Zset;
  double density; int densityset;
  double temper; int temperset;
  double pressure; int pressureset;
  std::string state; int stateset;
  std::string symbol; int symbolset;
  std::list<const char*> components; int componentsset;
  std::list<double> componentsFractions; int componentsFractionsset;
  std::list<int> componentsWeights; int componentsWeightsset;

  std::string  scintmaterial;  int scintmaterialset;
  std::string  windowmaterial;  int windowmaterialset;
  std::string  airmaterial;  int airmaterialset;

  // position of an element withing a sequence
  double at; int atset;


  //// reset the parameters to defaults
  void flush();

  /// copy parameters into temporary buffer params from element e
  /// parameters already set in params have priority and are not overridden
  void inherit_properties(struct Element& e);

  /// print multipole expansion array
  void print()const;

  /// constructor
  Parameters();
};

#endif
