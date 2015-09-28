#ifndef ELEMENT_H
#define ELEMENT_H

#include <list>
#include <string>

namespace GMAD {

enum class ElementType;

/**
 * @brief Element class
 * 
 * @author I. Agapov
 */

struct Element {
  ElementType type; ///< element enum
  int precisionRegion;
  std::string name;

  double l; ///< l in metres
  double ks,k0,k1,k2,k3,k4,angle;

  // beampipe information
  double beampipeThickness;
  double aper1, aper2, aper3, aper4; // new aperture model
  std::string apertureType;
  std::string beampipeMaterial;

  // magnet geometry
  std::string magnetGeometryType;
  std::string outerMaterial;
  double outerDiameter;

  double tilt,xsize,ysize,r,B, phiAngleIn, phiAngleOut;
  double offsetX, offsetY;
  double tscint, twindow, bmapZOffset; 
  double xdir, ydir, zdir, waveLength; ///< for laser wire and 3d transforms
  double gradient; ///< for rf cavities
  double phi, theta, psi; ///< for 3d transforms

  std::list<double> knl;
  std::list<double> ksl;

  ///List of beam loss monitor locations
  std::list<double> blmLocZ;
  std::list<double> blmLocTheta;

  /// physics biasing process
  std::string bias;
  
  /// material properties
  double A; 
  double Z; 
  double density; 
  double temper;
  double pressure;
  std::string state;
  std::string symbol;
  std::list<std::string> components;
  std::list<double> componentsFractions;
  std::list<int> componentsWeights;

  std::string geometryFile;
  std::string bmapFile;
  std::string material;
  std::string windowmaterial;
  std::string scintmaterial;
  std::string airmaterial;
  std::string spec;  ///< arbitrary specification to pass to beamline builder
  
  /// in case the element is a list itself (line)
  std::list <Element> *lst;

  /// print method
  void print(int &ident)const;

  /// flush method
  void flush();

  /// property lookup by name (slow method)
  /// only for properties with type int/double!
  double property_lookup(std::string property_name)const;

  /// constructor
  Element();
};
}
 
#endif
