#ifndef BDSBEAMPIPEFACTORYBASE_H
#define BDSBEAMPIPEFACTORYBASE_H

#include "BDSBeamPipe.hh"

#include "globals.hh"         // geant4 globals / types
#include "G4Material.hh"      // materials

/**
 * @brief abstract base class for beampipe factory classes
 * 
 * Abstract base class that defines the interface of each factory
 * to build any type of beampipe required. There are four general
 * possibilities apart from the shape (each derived class will give
 * a different cross-section) and these dictate the angled faces.
 * 
 * Uses Geant4 default units by default, ie mm, rad (not metres)
 * 
 * Note, any type of aperture (in MADX) requires up to 4 parameters
 * 
 * Default values in constructors result in global defaults being used
 * ie beampipe material will be steel - the default from options
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSBeamPipeFactoryBase
{
public:
  BDSBeamPipeFactoryBase();
  virtual ~BDSBeamPipeFactoryBase();

  // NOTE each derived class is expected to be a singleton but this can't be specified here
  // as it'd refer to the abstract base class - must be implemented in each derived class
  // utilised in BDSBeamPipeFactory.cc

  // 'In' in argument variables is used to distinguish between that and possible
  // internal member variables with the same name - avoiding 'itsVariable'
  
  /// create a flat ended beampipe
  virtual BDSBeamPipe* CreateBeamPipe(G4String    nameIn,                    // name
				      G4double    lengthIn,                  // length [mm]
				      G4Material* vacuumMaterialIn = NULL,   // vacuum material
				      G4double    beamPipeThicknessIn = 0,   // beampipe thickness [mm]
				      G4Material* beamPipeMaterialIn = NULL, // beampipe material
				      G4double    aper1 = 0,                 // aperture parameter 1
				      G4double    aper2 = 0,                 // aperture parameter 2
				      G4double    aper3 = 0,                 // aperture parameter 3
				      G4double    aper4 = 0                  // aperture parameter 4
				      ) = 0;

  /// create beampipe with an angled face on input side only
  virtual BDSBeamPipe* CreateBeamPipeAngledIn(   G4String    nameIn,
					         G4double    lengthIn,
					         G4double    angleInIn,   // the normal angle of the input face
					         G4Material* vacuumMaterialIn = NULL,
					         G4double    beamPipeThicknessIn = 0,
					         G4Material* beamPipeMaterialIn = NULL,
					         G4double    aper1 = 0,
					         G4double    aper2 = 0,
					         G4double    aper3 = 0,
					         G4double    aper4 = 0
					         ) = 0;

  /// create beampipe with an angled face on output side only
  virtual BDSBeamPipe* CreateBeamPipeAngledOut(  G4String    nameIn,
					         G4double    lengthIn,
					         G4double    angleOutIn, // the normal angle of the output face
					         G4Material* vacuumMaterialIn = NULL,
					         G4double    beamPipeThicknessIn = 0,
					         G4Material* beamPipeMaterialIn = NULL,
					         G4double    aper1 = 0,
					         G4double    aper2 = 0,
					         G4double    aper3 = 0,
					         G4double    aper4 = 0
					         ) = 0;
  
  /// create beampipe with an angled face on both input adn output sides
  virtual BDSBeamPipe* CreateBeamPipeAngledInOut(G4String    nameIn,
						 G4double    lengthIn,
						 G4double    angleInIn,  // the normal angle of the input face
						 G4double    angleOutIn, // the normal angle of the output face
						 G4Material* vacuumMaterialIn = NULL,
						 G4double    beamPipeThicknessIn = 0,
						 G4Material* beamPipeMaterialIn = NULL,
						 G4double    aper1 = 0,
						 G4double    aper2 = 0,
						 G4double    aper3 = 0,
						 G4double    aper4 = 0
						 ) = 0;
};

#endif
