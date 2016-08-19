
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
  short type;
  int precisionRegion;
  std::string name;

  double inR; double bpRad; /// inner radius and beam pipe radius of muon spoiler  
  double l,ks,k0,k1,k2,k3,angle,beampipeThickness,aper,aperX, aperY, tilt,xsize,ysize,r,outR,hgap,B, phiAngleIn, phiAngleOut, tscint, screenPSize, windowScreenGap, twindow, tmount, bmapZOffset; // l in meter
  double screenXSize, screenYSize;
  double screenEndZ, poleStartZ, screenWidth;
  double xdir, ydir, zdir, waveLength; /// for laser wire and 3d transforms
  double flatlength,taperlength; ///for realistic collimators
  double gradient; /// for rf cavities
  double aperYUp, aperYDown, aperDy;  ///pcldrift
  double phi, theta, psi; /// for 3d transforms
  double tunnelRadius, tunnelOffsetX, floorBeamlineHeight, beamlineCeilingHeight, tunnelThickness, tunnelSoilThickness;
  int tunnelRadiusset, tunnelOffsetXset, floorBeamlineHeightset, beamlineCeilingHeightset, tunnelThicknessset, tunnelSoilThicknessset;
  int tunnelType, tunnelTypeset;
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

  std::list<double> layerThicknesses;
  std::list<const char*> layerMaterials;
  std::list<int> layerIsSampler;

  std::string geometryFile;
  std::string bmapFile;
  std::string material;
  std::string windowmaterial;
  std::string mountmaterial;
  std::string vacuummaterial;
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
