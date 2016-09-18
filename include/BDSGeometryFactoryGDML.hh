#ifndef BDSGEOMETRYFACTORYGDML_H
#define BDSGEOMETRYFACTORYGDML_H

#include "BDSGeometryFactoryBase.hh"

#include "globals.hh"

class BDSGeometryExternal;

class BDSGeometryFactoryGDML: public BDSGeometryFactoryBase
{
public:
  static BDSGeometryFactoryGDML* Instance();

  virtual ~BDSGeometryFactoryGDML();

  virtual BDSGeometryExternal* Build(G4String fileName);

private:
  /// Private default constructor as singleton.
  BDSGeometryFactoryGDML();
  
  /// Singleton instance
  static BDSGeometryFactoryGDML* instance;
};

#endif
