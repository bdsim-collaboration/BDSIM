#ifndef __PARAMETERS_H
#define __PARAMETERS_H

#include <list>
#include <string>

namespace GMAD {

struct Array;
struct Element;

/**
 * @brief Parameters - used in the parser
 * 
 * @author I. Agapov
 */

struct Parameters {

  /// length, multipole coefficients

  double B;  int Bset;    ///< magnetic field
  double l;  int lset;    ///< length
  double bmapZOffset; int bmapZOffsetset; //offset of the field map magnet field
  double k0; int k0set;   ///< dipole 
  double k1; int k1set;   ///< quadrupole
  double k2; int k2set;   ///< sextupole
  double k3; int k3set;   ///< octupole
  double k4; int k4set;   ///< decapole
  double ks; int ksset;   ///< solenoid

  double tscint; int tscintset; ///<thickness of scintillating part of screen
  double twindow; int twindowset; ///<thickness of window

  std::list<double> knl;           ///< multipole expansion coefficients
  std::list<double> ksl;           ///< skew multipole expansion
  
  int knlset; int kslset;

  std::string bias; ///< physics biasing
  int biasset;
  
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

  double r; int rset; ///<radius, i.e. cylindrical sampler
  
  double angle; int angleset;   ///< bending angle
  double phiAngleIn; int phiAngleInset;   ///< incoming bending angle for element
  double phiAngleOut; int phiAngleOutset;   ///< outgoing bending angle for element
  
  //new aperture model
  double beampipeThickness; int beampipeThicknessset;
  double aper1; int aper1set;
  double aper2; int aper2set;
  double aper3; int aper3set;
  double aper4; int aper4set;
  std::string apertureType; int apertureTypeset;
  std::string beampipeMaterial; int beampipeMaterialset;

  //magnet geometry
  std::string magnetGeometryType; int magnetGeometryTypeset;
  std::string outerMaterial;      int outerMaterialset;
  double      outerDiameter;      int outerDiameterset;
  
  double phi, theta, psi; ///< for 3d transforms
  int phiset, thetaset, psiset;

  ///which precision physics region the element is in (0 = none)
  int precisionRegion; int precisionRegionset;

  double gradient; int gradientset; ///> gradient for RF
  std::string cavityModel; int cavityModelset; ///> cavitymodel for RF

  double xsize, ysize; int xsizeset, ysizeset; ///< aperture (or laser spotsize for laser)
  double xdir, ydir, zdir, waveLength; int xdirset, ydirset, zdirset, waveLengthset;

  double tilt; int tiltset;   ///< tilt
  double offsetX; int offsetXset;   ///< offset X
  double offsetY; int offsetYset;   ///< offset Y

  /// for external geometry and field definition files
  std::string geometry; int geomset;
  std::string bmap; int bmapset;
  //  std::string emap; int emapset;
  std::string material; int materialset;

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
  std::list<std::string> components; int componentsset;
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

  /// set methods by property name, numeric values
  void set_value(std::string property, double value);
  /// set methods by property name, string values
  void set_value(std::string property, std::string value);
  /// set methods by property name, Array values
  void set_value(std::string property, Array* value);

  /// constructor
  Parameters();
};
}

#endif
