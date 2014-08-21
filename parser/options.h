#ifndef __OPTIONS_H
#define __OPTIONS_H

#include <string>

/**
 * @brief Options class
 * 
 * options passed with option and beam command
 *
 * @author I. Agapov
 */

struct Options {

  Options();

  /// list of pyhysics processes 
  std::string physicsList;

  /// beam parameters
  std::string particleName;
  std::string distribType;
  std::string distribFile;
  std::string distribFileFormat;

  int numberToGenerate;
  int nlinesIgnore; /// ignore first lines in the input bunch file

  double elossHistoBinWidth;
  double elossHistoTransBinWidth;
  double defaultRangeCut;
  double ffact;
  double beamEnergy;

  /// initial beam centroid
  double X0, Y0, Z0;
  double Xp0, Yp0, Zp0;
  double T0; 
  
  /// bunch length
  double sigmaT;

  /// initial twiss parameters
  double betx, bety, alfx, alfy, emitx, emity; 

  /// for the gaussian beam distribution
  double sigmaX, sigmaXp, sigmaY, sigmaYp;

  /// for the circle/square beam distribution
  double envelopeX, envelopeXp, envelopeY, envelopeYp, envelopeT, envelopeE;

  /// for the gaussian sigma matrix distribution
  double sigma11, sigma12, sigma13, sigma14, sigma15, sigma16;
  double          sigma22, sigma23, sigma24, sigma25, sigma26;
  double                   sigma33, sigma34, sigma35, sigma36;
  double                            sigma44, sigma45, sigma46;
  double                                     sigma55, sigma56;
  double                                              sigma66;
  
  /// for the elliptic shell distribution
  double shellX, shellXp, shellY, shellYp;

  /// for the ring beam distribution
  double Rmin, Rmax;

  /// for the gaussian, elliptic shell, ring distributions
  double sigmaE;

  /// bdsim options 
  int       doPlanckScattering;
  int       checkOverlaps;
  int       numberOfEventsPerNtuple;
  unsigned long int eventNumberOffset;
  double    vacuumPressure;
  double    planckScatterFe; 

  /// for element specification
  double xsize, ysize;

  ///  int backgroundScaleFactor;

  /// default geometry parameters
  std::string magnetGeometry;
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

  /// tunnel geometry parameters
  int      buildTunnel;
  int      buildTunnelFloor;
  int      showTunnel;
  double   tunnelOffsetX;
  double   tunnelOffsetY;
  double   samplerDiameter;
  double   tunnelThickness;
  double   tunnelSoilThickness;
  double   tunnelFloorOffset;
  
  ///Geometry biasing
  int      geometryBias;

  ///BLM geometry
  double   blmRad;
  double   blmLength;

  ///Cross section biasing parameters
  double   gammaToMuFe;
  double   annihiToMuFe;
  double   eeToHadronsFe;

  double scintYieldFactor;
 
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
  double   prodCutPhotonsA;
  double   prodCutElectrons;
  double   prodCutElectronsP;
  double   prodCutElectronsA;
  double   prodCutPositrons;
  double   prodCutPositronsP;
  double   prodCutPositronsA;


  /// Tracking related parameters 
  double   maximumTrackingTime; /// maximum tracking time per volume [s]
  double   deltaChord;
  double   chordStepMinimum;
  double   deltaIntersection;
  double   minimumEpsilonStep;
  double   maximumEpsilonStep;
  double   deltaOneStep;
  int      turnOnCerenkov;
  int turnOnOpticalAbsorption;
  int turnOnMieScattering;
  int turnOnRayleighScattering;
  int turnOnOpticalSurface;
  int turnOnBirksSaturation;
  int      synchRadOn;
  int      decayOn;
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

  std::string fifo; /// fifo for BDSIM-placet
  std::string refvolume; ///initial starting volume
  int refcopyno; ///initial starting volume copy number
  
  /// Ring parameters
  int      nturns;

  /// print some properties
  void print()const;

  /// set methods by property name
  void set_value(std::string name, double value);
  void set_value(std::string name, std::string value);
};

#endif
