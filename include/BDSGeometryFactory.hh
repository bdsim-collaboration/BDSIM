#ifndef BDSGEOMETRYFACTORY_H
#define BDSGEOMETRYFACTORY_H

#include "BDSGeometryType.hh"

#include "globals.hh"

#include <string>
#include <unordered_map>

class BDSGeometry;

/**
 * @brief Interface to external geometry construction.
 *
 * @author Lawrence Deacon, Laurie Nevay
 */

class BDSGeometryFactory
{
public:
  /// Singleton accessor.
  static BDSGeometryFactory* Instance();
  
  ~BDSGeometryFactory();
  
  BDSGeometry* BuildGeometryOld(G4String formatAndFilePath);

  BDSGeometry* BuildGeometry(BDSGeometryType type, G4String file);
 
private:
  /// Private accessor as singleton
  BDSGeometryFactory();

  /// Singleton instance.
  static BDSGeometryFactory* instance;

  /// A registry of all previously constructed components. We must use an
  /// std::string (which G4String inherits from) so provide implicit hasher
  /// for the storage in the unordered map (which isn't provided for G4String).
  std::unordered_map<std::string, BDSGeometry*> registry;

  /// This is where the geometry components are stored. By storing the
  /// pointers in a vector, they may be more efficiently iterated over.
  std::vector<BDSGeometry*> storage;

  
  BDSGeometry* BuildGMAD(G4String fileName);
  
#ifdef USE_LCDD
  BDSGeometry* BuildLCDD(G4String fileName);
#endif
  
  BDSGeometry* BuildMokka(G4String fileName);
  
#ifdef USE_GDML
  BDSGeometry* BuildGDML(G4String fileName);
#endif
};

#endif
