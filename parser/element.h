
#ifndef __ELEMENT_H
#define __ELEMENT_H

#include <list>
#include <string>

/**
 * @brief Element class
 * 
 * @author I. Agapov
 */

struct Element {
  int type; ///< element enum
  int precisionRegion;
  std::string name;

  double l; // l in metres
  double ks,k0,k1,k2,k3,angle;

  double aper,aperX, aperY; //old aperture model

  double beampipeThickness;
  double aper1, aper2, aper3, aper4; // new aperture model
  std::string apertureType;
  std::string beampipeMaterial;

  double boxSize; //outer *diameter / full width* of component
  
  double tilt,xsize,ysize,r,outR,hgap,B, phiAngleIn, phiAngleOut;
  double tscint, twindow, bmapZOffset; 
  double xdir, ydir, zdir, waveLength; /// for laser wire and 3d transforms
  double flatlength,taperlength; ///for realistic collimators
  double gradient; /// for rf cavities
  double phi, theta, psi; /// for 3d transforms
  double tunnelRadius;
  double tunnelOffsetX;
  std::list<double> knl;
  std::list<double> ksl;

  ///List of beam loss monitor locations
  std::list<double> blmLocZ;
  std::list<double> blmLocTheta;

  /// material properties
  double A; 
  double Z; 
  double density; 
  double temper;
  double pressure;
  std::string state;
  std::string symbol;
  std::list<const char*> components;
  std::list<double> componentsFractions;
  std::list<int> componentsWeights;

  std::string geometryFile;
  std::string bmapFile;
  std::string material;
  std::string windowmaterial;
  std::string scintmaterial;
  std::string airmaterial;
  std::string tunnelMaterial;
  std::string tunnelCavityMaterial;

  std::string spec;  /// arbitrary specification to pass to beamline builder
  
  /// in case the element is a list itself (line)
  std::list <Element> *lst;

  /// print method
  void print(int &ident)const;

  /// flush method
  void flush();

  /// property lookup by name (slow method)
  /// only for properties with type int/double!
  double property_lookup(char* property_name)const;

  /// constructor
  Element();
};

#endif
