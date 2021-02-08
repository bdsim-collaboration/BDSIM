/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2021.

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
#include "BDSDebug.hh"
#include "BDSNavigatorPlacements.hh"

#include "G4AffineTransform.hh"
#include "G4Navigator.hh"
#include "G4ThreeVector.hh"

G4Navigator*       BDSNavigatorPlacement::navigator         = new G4Navigator();
G4int              BDSNavigatorPlacement::numberOfInstances = 0;
G4VPhysicalVolume* BDSNavigatorPlacement::worldPV           = nullptr;

BDSNavigatorPlacement::BDSNavigatorPlacement():
  globalToLocal(G4AffineTransform()),
  localToGlobal(G4AffineTransform())
{
  numberOfInstances++;
}

BDSNavigatorPlacement::~BDSNavigatorPlacement()
{
  // Only delete static navigator objects when last instance is deleted
  if (numberOfInstances == 1)
    {
      delete navigator; navigator   = nullptr;
    }
  numberOfInstances--;
}

void BDSNavigatorPlacement::ResetNavigatorStates()
{
  navigator->ResetStackAndState();
}

G4ThreeVector BDSNavigatorPlacement::ConvertToLocal(const G4ThreeVector& globalPosition,
                                                    G4bool& foundAPlacementVolume) const
{
  foundAPlacementVolume = InitialiseTransform(globalPosition);
  if (!foundAPlacementVolume)
    {return G4ThreeVector();}
  return globalToLocal.TransformPoint(globalPosition);
}

G4ThreeVector BDSNavigatorPlacement::ConvertToLocalNoSetup(const G4ThreeVector& globalPosition) const
{
  return globalToLocal.TransformPoint(globalPosition);
}

G4ThreeVector BDSNavigatorPlacement::ConvertAxisToGlobal(const G4ThreeVector& localAxis) const
{
  return localToGlobal.TransformAxis(localAxis);
}

G4bool BDSNavigatorPlacement::InitialiseTransform(const G4ThreeVector& globalPosition) const
{
  G4VPhysicalVolume* foundPVVolume = navigator->LocateGlobalPointAndSetup(globalPosition);
  if (foundPVVolume == worldPV)
    {return false;}
  globalToLocal = navigator->GetGlobalToLocalTransform();
  localToGlobal = navigator->GetLocalToGlobalTransform();
  return true; // found a placement volume ok
}
