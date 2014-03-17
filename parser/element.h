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

  double inR; double bpRad; // inner radius and beam pipe radius of muon spoiler  
  double l,ks,k0,k1,k2,k3,angle,beampipeThickness,aper,aperX, aperY, tilt,xsize,ysize,r,outR,hgap,B, phiAngleIn, phiAngleOut, tscint; // l in meter
  double xdir, ydir, zdir, waveLength; // for laser wire and 3d transforms
  double flatlength,taperlength; //for realistic collimators
  double gradient; // for rf cavities
  double aperYUp, aperYDown, aperDy;  //pcldrift
  double phi, theta, psi; // for 3d transforms
  double tunnelRadius;
  double tunnelOffsetX;

  std::list<double> knl;
  std::list<double> ksl;

  //List of beam loss monitor locations
  std::list<double> blmLocZ;
  std::list<double> blmLocTheta;

   // material properties
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
  std::string tunnelMaterial;
  std::string tunnelCavityMaterial;

  std::string spec;  // arbitrary specification to pass to beamline builder
  
  // in case the element is a list itself (line)
  std::list <Element> *lst;

  // print method
  void print(int &ident)const;
};

#endif
