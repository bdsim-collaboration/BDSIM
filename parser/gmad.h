/*
 * GMAD interface 
 * I. Agapov 2005-2006
 * bdsim v.0.3
 *
 * modification history:
*/

#ifndef _GMAD_H
#define _GMAD_H

#define MAXFILENAMELENGTH 200

#define _RESERVED 1

#include <iostream>
#include <cstdio>
#include <list>
#include <string>
#include <cstring>

// types of elements

enum {
  _NONE = -1,
  _MARKER = 1,
  _DRIFT = 2,
  _PCLDRIFT = 63,
  _RF = 3,
  _SBEND = 4, 
  _QUAD  = 5,
  _SEXTUPOLE  = 6,
  _OCTUPOLE = 7,
  _MULT  = 8,
  _SOLENOID = 9,
  _ELEMENT = 10,
  _LINE = 11,
  _REV_LINE= -11, //for line inversion in sublines
  _COLLIMATOR = 12,
  _ECOL = 13,
  _MUSPOILER = 62,
  _RCOL = 14,
  _LASER=15,
  _MATERIAL=16,
  _RBEND=17,
  _ATOM = 18,
  _SEQUENCE = 19,
    
  _VKICK=31,
  _HKICK=32,
  
  _SAMPLER = 41,
  _CSAMPLER = 42,
  _DUMP = 43,
  _GAS = 51,
  _TUNNEL = 52,

  _TRANSFORM3D = 61
};

const char *typestr(int type);


// geometry and field formats
enum {
  _GMAD = 0,
  _MOKKA = 1
};

enum {
  _REFERENCE = 0,
  _GAUSSIAN = 1,
  _RING = 2,
  _SQUARE = 3,
  _CIRCLE = 4,
  _GUINEAPIG_BUNCH = 5,
  _GUINEAPIG_PAIRS = 6,
  _GUINEAPIG_SLAC = 7,
  _CAIN = 8,
  _ESHELL = 9,
  _GAUSSIAN_TWISS = 10,
  _GAUSSIAN_MATRIX = 11,
  _UDEF = 32
};

// options passed with option and beam command

struct Options {

  // list of pyhysics processes 
  std::string physicsList;

  // beam parameters
  std::string particleName;
  std::string distribType;
  std::string distribFile;

  int numberToGenerate;
  int nlinesIgnore; // ignore first lines in the input bunch file

  double elossHistoBinWidth;
  double elossHistoTransBinWidth;
  double defaultRangeCut;
  double ffact;
  double beamEnergy;

  // initial beam centroid
  double X0, Y0, Z0;
  double Xp0, Yp0, Zp0;
  double T0; 
  
  // bunch length
  double sigmaT;

  // initial twiss parameters
  double betx, bety, alfx, alfy, emitx, emity; 

  // for the gaussian beam distribution
  double sigmaX, sigmaXp, sigmaY, sigmaYp;

  // for the gaussian sigma matrix distribution
  double sigma11, sigma12, sigma13, sigma14, sigma15, sigma16;
  double          sigma22, sigma23, sigma24, sigma25, sigma26;
  double                   sigma33, sigma34, sigma35, sigma36;
  double                            sigma44, sigma45, sigma46;
  double                                     sigma55, sigma56;
  double                                              sigma66;
  
  // for the elliptic shell distribution
  double shellX, shellXp, shellY, shellYp;

  // for the ring beam distribution
  double Rmin, Rmax;

  // for the gaussian, elliptic shell, ring distributions
  double sigmaE;

  // bdsim options 
  int       doTwiss;
  int       doPlanckScattering;
  int       checkOverlaps;
  int       numberOfEventsPerNtuple;
  unsigned long int eventNumberOffset;
  double    vacuumPressure;
  double    planckScatterFe; 

  // for element specification
  double xsize, ysize;

  int backgroundScaleFactor;

  // default geometry parameters
  double    componentBoxSize;
  double    tunnelRadius;
  double    beampipeRadius;
  double    beampipeThickness;
  std::string pipeMaterial;
  std::string vacMaterial;
  std::string tunnelMaterial;
  std::string tunnelCavityMaterial;
  std::string soilMaterial;

  int      includeIronMagFields;

  // tunnel geometry parameters
  int      buildTunnel;
  int      buildTunnelFloor;
  int      showTunnel;
  double   tunnelOffsetX;
  double   tunnelOffsetY;
  double   samplerDiameter;
  double   tunnelThickness;
  double   tunnelSoilThickness;
  double   tunnelFloorOffset;
  
  //Geometry biasing
  int      geometryBias;

  //BLM geometry
  double   blmRad;
  double   blmLength;

  //Cross section biasing parameters
  double   gammaToMuFe;
  double   annihiToMuFe;
  double   eeToHadronsFe;
 
  int      useEMLPB;
  int      useHadLPB;

  int      sensitiveBeamlineComponents;
  int      sensitiveBeamPipe;
  int      sensitiveBLMs;
 
  double   LPBFraction;

  double   thresholdCutCharged;
  double   thresholdCutPhotons;
  
  double   prodCutPhotons;
  double   prodCutPhotonsP;
  double   prodCutElectrons;
  double   prodCutElectronsP;
  double   prodCutPositrons;
  double   prodCutPositronsP;

  // Tracking related parameters 
  double    maximumTrackingTime;
  double   deltaChord;
  double   chordStepMinimum;
  double   deltaIntersection;
  double   minimumEpsilonStep;
  double   maximumEpsilonStep;
  double   deltaOneStep;
  int      turnOnCerenkov;
  int      synchRadOn;
  int      decayOn;
  int      synchRescale;
  int      synchTrackPhotons;
  double   synchLowX;
  double   synchLowGamE;
  int      synchPhotonMultiplicity;
  int      synchMeanFreeFactor;
  double   lengthSafety;
  long int randomSeed;

  int      useTimer;
  int      storeMuonTrajectories;
  double   trajCutGTZ;
  double   trajCutLTR;
  int      storeNeutronTrajectories;
  int      storeTrajectory;
  int      stopTracks;

  std::string fifo; // fifo for BDSIM-placet
  std::string refvolume; //initial starting volume
  int refcopyno; //initial starting volume copy number
};

// type of beamline list entries

struct Element {
  short type;
  int precisionRegion;
  const char * name;

  double inR; double bpRad; // inner radius and beam pipe radius of muon spoiler  
  double l,ks,k0,k1,k2,k3,angle,beampipeThickness,aper,aperX, aperY, tilt,xsize,ysize,r,outR,hgap,B, phiAngleIn, phiAngleOut;
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

  
};


// parameters - used in the parser

struct Parameters {

  // length, multipole coefficients

  double B;  int Bset;    // magnetic field
  double l;  int lset;    // length
  double k0; int k0set;   // dipole 
  double k1; int k1set;   // quadrupole
  double k2; int k2set;   // sextupole
  double k3; int k3set;   // octupole
  double ks; int ksset;   // solenoid

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

    beampipeThickness = 0;
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


extern std::list<Element> beamline_list;
extern std::list<Element> material_list;
extern std::list<Element> atom_list;

extern Options options;

// parse the input file and construct beamline_list and options 
int gmad_parser(FILE *f);

int gmad_parser(std::string name);


/** Python interface **/ 
extern "C" {   
  int    gmad_parser_c(char *name);

  /* Interface to extern std::list<Element> beamline_list */
  int    get_nelements(); // Length of list
  short  get_type(int);   // Type of element 
  const char*  get_name(int);   // Name of element
  double get_length(int); // Length of element
  double get_angle(int);  // Angle of element

  /* Interface to extern Options options*/
  /*
  char*  get_physicsList();
  char*  get_particleName();
  char*  get_distribType();
  char*  get_distribFile();
  int    get_numberToGenerate();
  
  double get_sigmaX();
  double get_sigmaY();
  double get_sigmaXp();
  double get_sigmaYp();
  double get_sigmaE();
  
  double get_shellX();
  double get_shellY();
  double get_shellXp();
  double get_shellYp();
  
  double get_rMin();
  double get_rMax();
 
  double get_betx();
  double get_bety();
  double get_alfx();
  double get_alfy();
  double get_emitx();
  double get_emity();
   
  */
  
  
}
#endif
