#ifndef BDSMAGNETOUTERFACTORYPOLESSQUARE_H
#define BDSMAGNETOUTERFACTORYPOLESSQUARE_H

#include "BDSGeometryComponent.hh"
#include "BDSMagnetOuterFactoryPolesBase.hh"
#include "BDSBeamPipe.hh"

#include "globals.hh"           // geant4 globals / types
#include "G4VSolid.hh"
#include <vector>

class G4Colour;
class G4Material;

/**
 * @brief Factory class for outer volume of magnets. 
 * 
 * Produces magnets
 * with 2N-poles around the beampipe with a square yoke.
 * 
 * Most magnets are 2N poles, but sector- and r-bends as well as 
 * muon spoilers, and h/v kickers are unique.
 *
 * @author Laurie Nevay
 */

class BDSMagnetOuterFactoryPolesSquare: public BDSMagnetOuterFactoryPolesBase
{
public:
  /// Singelton accessor.
  static BDSMagnetOuterFactoryPolesSquare* Instance();
  
  virtual ~BDSMagnetOuterFactoryPolesSquare();

private:
  /// Private constructor as singleton - nothing special here - all in parent class
  BDSMagnetOuterFactoryPolesSquare(); 
  static BDSMagnetOuterFactoryPolesSquare* _instance;

  virtual void CleanUp();

  /// Create yoke that connects poles and container to put them in
  virtual void CreateYokeAndContainerSolid(G4String name,
					   G4double length,
					   G4int    order,
					   G4double magnetContainerRadius);

  virtual void IntersectPoleWithYoke(G4String name,
				     G4double length,
				     G4int    orderIn);

  /// Build the logical volumes from the solids assigning materials and colours and cuts.
  /// This doesn't make use of any base class implementation as this class creates a
  /// vector of unique poles that must all be built individually into logical volumes.
  /// It does however make use of BDSMagnetOuterFactoryPolesBase::CreateLogicalVolumesCoil.
  virtual void CreateLogicalVolumes(G4String    name,
				    G4double    length,
				    G4Colour*   colour,
				    G4Material* outerMaterial);

  /// Place the poles and yoke in the container volume
  virtual void PlaceComponents(G4String name,
			       G4int    order);

  /// Common construction tasks to all methods - assemble yoke and poles in container
  /// have to override this as we have multiple pole logical volumes and they need to
  /// be registered
  virtual BDSMagnetOuter* CommonConstructor(G4String     name,
					    G4double     length,
					    BDSBeamPipe* beamPipe,
					    G4int        order,
					    G4double     outerDiameter,
					    G4Material*  outerMaterial,
					    G4double     magnetContainerLength);
  
  /// Poles have to be different lengths to fit in a square - have to be unique
  /// therefore keep a vector of them
  std::vector<G4RotationMatrix*> poleRotations;
  std::vector<G4VSolid*> poleSolids;
  std::vector<G4LogicalVolume*> poleLVs;

  /// Need to pass order to CreateLogicalVolumes but should override for neatness and
  /// therefore can't change the signature of the function, so pass by internal variable.
  G4int order;
  
};

#endif
