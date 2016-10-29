#ifndef BDSGEOMETRYFACTORYGMAD_H
#define BDSGEOMETRYFACTORYGMAD_H

#include "BDSGeometryFactoryBase.hh"

#include "globals.hh"
#include "G4RotationMatrix.hh"

#include <fstream>
#include <map>
#include <vector>

class BDSGeometryExternal;
class G4Colour;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4VSolid;

/**
 * @brief Factory for GMAD geometry description.
 *
 * Originally part of BDSIM. Adapated for recent refactoring.
 */

class BDSGeometryFactoryGMAD: public BDSGeometryFactoryBase
{
public:
  /// Singleton accessor
  static BDSGeometryFactoryGMAD* Instance();

  virtual ~BDSGeometryFactoryGMAD();

  /// Main interface overrides base class method to construct a file with
  /// optional colour mapping.
  virtual BDSGeometryExternal* Build(G4String fileName,
				     std::map<G4String, G4Colour*>* colourMapping = nullptr,
				     G4double suggestedLength        = 0,
				     G4double suggestedOuterDiameter = 0);
  
private:
  /// Private default constructor as singleton.
  BDSGeometryFactoryGMAD();

  /// Singleton instance
  static BDSGeometryFactoryGMAD* instance;

  /// Simple there was an error print out.
  inline void Error() {G4cout << "ggmad: error in file format" << G4endl;}

  /// Read a single word from the ifstream.
  G4String GetWord(std::ifstream& infputf) const;

  /// Get a numerical parameter from the ifstream.
  void GetParameter(std::ifstream& inputf,
		    G4double&      x,
		    G4String       name,
		    G4String       lastToken) const;

  /// Get a string parameter from the ifstream.
  void GetParameter(std::ifstream& inputf,
		    G4String&      lval,
		    G4String       name,
		    G4String lastToken) const;

  /// Construct logical volume, rotation and placement.
  void Finish(G4String         name,
	      G4VSolid*        solid,
	      G4String         materialName,
	      G4LogicalVolume* mother,
	      G4double         phi   = 0,
	      G4double         theta = 0,
	      G4double         psi   = 0,
	      G4double         x0 = 0,
	      G4double         y0 = 0,
	      G4double         z0 = 0);
};

#endif
