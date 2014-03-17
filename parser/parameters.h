#ifndef __PARAMETERS_H
#define __PARAMETERS_H

#include <cstring>
#include <list>

/**
 * @brief Parameters - used in the parser
 * 
 * @author I. Agapov
 */

struct Parameters {

  // length, multipole coefficients

  double B;  int Bset;    // magnetic field
  double l;  int lset;    // length
  double k0; int k0set;   // dipole 
  double k1; int k1set;   // quadrupole
  double k2; int k2set;   // sextupole
  double k3; int k3set;   // octupole
  double ks; int ksset;   // solenoid

  double tscint; int tscintset; //thickness of scintillating part of screen

  std::list<double> knl;           // multipole expansion coefficients
  std::list<double> ksl;           // skew multipole expansion
  
  int knlset; int kslset;

  //List of beam loss monitor locations
  std::list<double> blmLocZ;
  std::list<double> blmLocTheta;
  int blmLocZset; int blmLocThetaset;


  //For MADX style sequences
  double at; int atset;//"at" and "from" define position in beam line
  char from[256]; int fromset;
  char refer[64]; int referset;
  double absp; int abspset;//pos. from beg. of line
  
  // placement, geometrical sizes etc.

  double r; int rset; //radius, i.e cylindrical sampler
  
  double angle; int angleset;   // bending angle
  double phiAngleIn; int phiAngleInset;   // incoming bending angle for element
  double phiAngleOut; int phiAngleOutset;   // outgoing bending angle for element
  double beampipeThickness; int beampipeThicknessset;  
  double aper; int aperset;   // aperture (circular)
  double aperX; int aperXset;   // aperture (elliptical)
  double aperY; int aperYset;   
  double phi, theta, psi; // for 3d transforms
  int phiset, thetaset, psiset;
  double tunnelRadius;
  int tunnelRadiusset;
  double tunnelOffsetX;
  int tunnelOffsetXset;

  //which precision physics region the element is in (0 = none)
  int precisionRegion; int precisionRegionset;

  double aperYUp; int aperYUpset;  
  double aperYDown; int aperYDownset; 
  double aperDy; int aperDyset;
  
  double flatlength; int flatlengthset;
  double taperlength; int taperlengthset;
  double gradient; int gradientset;

  double outR; int outRset; // outer radius of magnets
  double inR, bpRad; int inRset, bpRadset;// inner radius and beam pipe radius of muon spoiler
  double hgap, hgapset;
  double xsize, ysize; int xsizeset, ysizeset; // aperture (or laser spotsize for laser)
  double xdir, ydir, zdir, waveLength; int xdirset, ydirset, zdirset, waveLengthset;

  double tilt; int tiltset;   // tilt

  // twiss parameters
  

  // for external geometry and field definition files
  char geometry[256]; int geomset;
  char bmap[256]; int bmapset;
  char emap[256];
  char material[256]; int materialset;
  char tunnelMaterial[256]; int tunnelmaterialset;
  char tunnelCavityMaterial[256]; int tunnelcavitymaterialset;


  // string to pass a custom type specification
  char spec[1024]; int specset;

  // material properties
  double A; int Aset;
  double Z; int Zset;
  double density; int densityset;
  double temper; int temperset;
  double pressure; int pressureset;
  char state[64]; int stateset;
  char symbol[64]; int symbolset;
  std::list<const char*> components; int componentsset;
  std::list<double> componentsFractions; int componentsFractionsset;
  std::list<int> componentsWeights; int componentsWeightsset;

  // reset the parameters to defaults
  void flush() {

    l=0; lset = 0;
    r = 0; rset = 0;
    tscint = 0.0003; tscintset = 0;

    // materials' parameters
    A = 0; Aset = 0; //g*mol^-1
    Z = 0; Zset = 0; 
    density = 0; densityset = 0; //g*cm-3
    temper = 300; temperset = 0; //kelvin
    pressure = 1; pressureset = 0; //atm
    strcpy(state,"solid"); stateset = 0; // "solid", "liquid", or "gas"
    strcpy(symbol,""); symbolset = 0;
    componentsset = 0; componentsFractionsset = 0; componentsWeightsset = 0;
    components.erase(components.begin(),components.end());
    componentsFractions.erase(componentsFractions.begin(),componentsFractions.end());
    componentsWeights.erase(componentsWeights.begin(),componentsWeights.end());

    flatlength = 0; flatlengthset = 0;
    taperlength = 0; taperlengthset = 0;
    angle = 0; angleset = 0;
    phiAngleIn = 0; phiAngleInset = 0;
    phiAngleOut = 0; phiAngleOutset = 0;
    xsize = 0; xsizeset = 0;
    ysize = 0; ysizeset = 0;
    hgap = 0; hgapset = 0;    
    xdir = 0; xdirset = 0;
    ydir = 0; ydirset = 0;
    zdir = 0; zdirset = 0; 
    waveLength = 0; waveLengthset = 0; 

    phi = 0; phiset = 0;
    theta = 0; thetaset = 0;
    psi = 0; psiset = 0;

    tunnelRadius = 0; tunnelRadiusset = 0;
    tunnelOffsetX = 0; tunnelOffsetXset = 0;

    precisionRegion = 0; precisionRegionset = 0;

    beampipeThickness = 0; beampipeThicknessset = 0;
    aper = 0; aperset = 0;
    outR = 0; outRset = 0;
    tilt = 0; tiltset = 0;

    aperX = 0; aperXset = 0;
    aperY = 0; aperYset = 0;
    aperYUp = 0; aperYUp = 0;
    aperYDown = 0; aperYDownset = 0;
    aperDy=0; aperDyset = 0;

    B  = 0; Bset  = 0;
    k0 = 0; k0set = 0;
    k1 = 0; k1set = 0;
    k2 = 0; k2set = 0;
    k3 = 0; k3set = 0;
    ks = 0; ksset = 0;

    gradient = 0; gradientset = 0;
    
    knlset = 0; kslset=0;

    knl.erase(knl.begin(),knl.end());
    ksl.erase(ksl.begin(),ksl.end());

    //Beam loss monitor locations
    blmLocZset = 0;  blmLocThetaset = 0;
    blmLocZ.erase(blmLocZ.begin(), blmLocZ.end());
    blmLocTheta.erase(blmLocTheta.begin(), blmLocTheta.end());

    //precisionRegion
    precisionRegion = 0; precisionRegionset=0;

    strcpy(geometry,"");  geomset = 0;

    strcpy(bmap,""); bmapset = 0;

    strcpy(material,""); materialset = 0;
    strcpy(tunnelMaterial,""); tunnelmaterialset = 0;
    strcpy(tunnelCavityMaterial,""); tunnelcavitymaterialset = 0;
    strcpy(spec,""); specset = 0;
  }
  
};

#endif
