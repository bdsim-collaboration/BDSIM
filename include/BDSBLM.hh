/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef BDSBLM_H
#define BDSBLM_H

#include "BDSExtent.hh"
#include "BDSGeometryComponent.hh"

#include "G4String.hh"

class G4LogicalVolume;
class G4VSolid;

/**
 * @brief A beam loss monitor.
 *
 * This is a wrapper class for BDSGeometryComponent. Currently,
 * this provides no additional members, but experience with beam pipes
 * and magnet outer geometry says that this class will undoubtedly be
 * required. Easier to implement now than rewrite all factories later.
 * The base class copy constructor doesn't update any registered objects
 * so we keep a copy of the pointer to query in select functions.
 * 
 * @author Laurie Nevay
 */

class BDSBLM: public BDSGeometryComponent
{
public:
  /// Use constructors of BDSGeometryComponent
  //using BDSGeometryComponent::BDSGeometryComponent;
  /// No default constructor
  BDSBLM() = delete;

  BDSBLM(const BDSGeometryComponent* geometry);
  BDSBLM(G4VSolid*         containerSolidIn,
         G4LogicalVolume*  containerLVIn,
         BDSExtent         extentIn          = BDSExtent());

  virtual ~BDSBLM(){;}

  /// Accessor.
  G4String Bias() const {return bias;}

  /// Use a setter function rather than add to the constructor as for now we
  /// can get away with using the base class constructor.
  void SetBias(const G4String& biasIn) {bias = biasIn;}

  virtual std::set<G4VPhysicalVolume*> GetAllPhysicalVolumes()  const {return geometry->GetAllPhysicalVolumes();}
  virtual std::set<G4RotationMatrix*>  GetAllRotationMatrices() const {return geometry->GetAllRotationMatrices();}
  virtual std::set<G4VisAttributes*>   GetAllVisAttributes()    const {return geometry->GetAllVisAttributes();}
  virtual std::set<G4UserLimits*>      GetAllUserLimits()       const {return geometry->GetAllUserLimits();}
  virtual std::set<BDSGeometryComponent*> GetAllDaughters()     const {return geometry->GetAllDaughters();}
  virtual std::set<G4VSolid*>          GetAllSolids()           const {return geometry->GetAllSolids();}
  virtual std::set<G4LogicalVolume*>   GetAllLogicalVolumes()   const {return geometry->GetAllLogicalVolumes();}

private:
  const BDSGeometryComponent* geometry;
  G4String bias = "";
};

#endif
