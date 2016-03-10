#ifndef __OPTIONSBASE_H
#define __OPTIONSBASE_H

#include <iomanip>
#include <iostream>
#include <string>

namespace GMAD
{
/*
 * @brief Basic options class independent of Geant4.
 *
 * @author Stewart Boogert.
 */
  
  class OptionsBase
  {
  public:
    OptionsBase();

    /// list of pyhysics processes
    std::string physicsList;
    int modularPhysicsListsOn;

    /// beam parameters
    std::string particleName;
    std::string distribType;
    std::string xDistribType;
    std::string yDistribType;
    std::string zDistribType;
    std::string distribFile;
    std::string distribFileFormat;

    int numberToGenerate;
    int nlinesIgnore; ///< ignore first lines in the input bunch file

    double elossHistoBinWidth;
    double elossHistoTransBinWidth;
    double defaultRangeCut;
    /// magnetic field flip (+1 default, -1: flip sign)
    double ffact;
    double beamEnergy;

    /// initial beam centroid
    double X0, Y0, Z0, S0;
    double Xp0, Yp0, Zp0;
    double T0;
    double E0;

    /// bunch length
    double sigmaT;

    /// initial twiss parameters
    double betx, bety, alfx, alfy, emitx, emity, dispx, dispy, dispxp, dispyp;

    /// for the gaussian beam distribution
    double sigmaX, sigmaXp, sigmaY, sigmaYp;

    /// for the circle/square beam distribution
    double envelopeX, envelopeXp, envelopeY, envelopeYp, envelopeT, envelopeE;
    double envelopeR, envelopeRp;

    /// for the gaussian sigma matrix distribution
    double sigma11, sigma12, sigma13, sigma14, sigma15, sigma16;
    double          sigma22, sigma23, sigma24, sigma25, sigma26;
    double                   sigma33, sigma34, sigma35, sigma36;
    double                            sigma44, sigma45, sigma46;
    double                                     sigma55, sigma56;
    double                                              sigma66;

    /// for the elliptic shell distribution
    double shellX, shellXp, shellY, shellYp;
    double shellXWidth, shellXpWidth, shellYWidth, shellYpWidth;

    /// for the ring beam distribution
    double Rmin, Rmax;

    /// for the halo distribution
    double      haloPSWeightParameter;
    std::string haloPSWeightFunction;

    /// for the gaussian, elliptic shell, ring distributions
    double sigmaE;

    /// bdsim options
    int       doPlanckScattering;
    int       checkOverlaps;
    int       numberOfEventsPerNtuple;
    int       eventNumberOffset;
    double    vacuumPressure;
    double    planckScatterFe;

    /// for element specification
    double xsize, ysize;

    /// default magnet geometry parameters
    std::string magnetGeometryType;
    std::string outerMaterialName;
    double      outerDiameter;

    /// geometry debug
    bool        dontSplitSBends;

    int         includeIronMagFields;

    /// default beampipe parameters
    double      beampipeThickness;
    std::string apertureType;
    double      aper1;
    double      aper2;
    double      aper3;
    double      aper4;
    std::string beampipeMaterial;
    std::string vacMaterial;

    /// tunnel geometry parameters
    bool        buildTunnel;
    bool        buildTunnelStraight;
    std::string tunnelType;
    double      tunnelThickness;
    double      tunnelSoilThickness;
    std::string tunnelMaterial;
    std::string soilMaterial;
    bool        buildTunnelFloor;
    double      tunnelFloorOffset;
    double      tunnelAper1;
    double      tunnelAper2;
    bool        tunnelSensitive;
    bool        tunnelVisible;
    double      tunnelOffsetX;
    double      tunnelOffsetY;

    /// Sampler
    double   samplerDiameter;

    ///BLM geometry
    double   blmRad;
    double   blmLength;

    double   scintYieldFactor;

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
    double   prodCutProtons;
    double   prodCutProtonsP;
    double   prodCutProtonsA;

    /// Tracking related parameters
    double   maximumTrackingTime; ///< maximum tracking time per volume [s]
    double   deltaChord;
    double   chordStepMinimum;
    double   deltaIntersection;
    double   minimumEpsilonStep;
    double   maximumEpsilonStep;
    double   deltaOneStep;
    int      turnOnCerenkov;
    int      turnOnOpticalAbsorption;
    int      turnOnMieScattering;
    int      turnOnRayleighScattering;
    int      turnOnOpticalSurface;
    int      turnOnBirksSaturation;
    double   lengthSafety;
    int      randomSeed;

    bool     storeMuonTrajectories;
    double   trajCutGTZ;
    double   trajCutLTR;
    bool     storeNeutronTrajectories;
    bool     storeTrajectory;
    bool     stopSecondaries;
    bool     stopTracks;

    /// Ring parameters
    int      nturns;

    double   printModuloFraction;

    /// print some properties
    void print() const;

  };
}


#endif //__OPTIONSBASE_H
