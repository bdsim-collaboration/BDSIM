#ifndef BDSGEOMETRYFACTORY_H
#define BDSGEOMETRYFACTORY_H

#include "BDSGeometryFactoryBase.hh"
#include "BDSGeometryType.hh"

#include "globals.hh"

#include <string>
#include <unordered_map>

class BDSGeometry;
class BDSGeometryExternal;

/**
 * @brief Interface to external geometry construction.
 *
 * A factory that uses the correct derived singleton factory of
 * BDSGeometryFactoryBase to construct the required geometry with
 * extents.
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

  BDSGeometryExternal* BuildGeometry(G4String formatAndFilePath,
				     std::map<G4String, G4Colour*>* colourMapping = nullptr);
  
  //BDSGeometry* BuildGeometry(BDSGeometryType type, G4String file);
 
private:
  /// Private accessor as singleton
  BDSGeometryFactory();

  /// Singleton instance.
  static BDSGeometryFactory* instance;

  /// A registry of all previously constructed components. We must use an
  /// std::string (which G4String inherits from) so provide implicit hasher
  /// for the storage in the unordered map (which isn't provided for G4String).
  std::unordered_map<std::string, BDSGeometryExternal*> registry;

  /// This is where the geometry components are stored. By storing the
  /// pointers in a vector, they may be more efficiently iterated over.
  std::vector<BDSGeometryExternal*> storage;

  BDSGeometryFactoryBase* GetAppropriateFactory(BDSGeometryType type);
  
  BDSGeometry* BuildGMAD(G4String fileName);
  
#ifdef USE_LCDD
  BDSGeometry* BuildLCDD(G4String fileName);
#endif
  
  BDSGeometry* BuildMokka(G4String fileName);
};

#endif
