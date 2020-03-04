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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSLinkOpaqueBox.hh"
#include "BDSLinkRegistry.hh"

#include "G4Transform3D.hh"
#include "G4Types.hh"

#include <map>
#include <string>

BDSLinkRegistry::BDSLinkRegistry()
{;}

BDSLinkRegistry::~BDSLinkRegistry()
{;}

void BDSLinkRegistry::Register(BDSLinkOpaqueBox*    componentIn,
			       const G4Transform3D& globalToInputIn)
{
  G4int newID = (G4int)byName.size();
  G4cout << "New ID " << newID << " for " << componentIn->GetName() << G4endl;
  G4bool noRotation = !(componentIn->Angled());
  BDSLinkRegistry::LinkEntry le = {noRotation, componentIn, globalToInputIn, globalToInputIn.inverse(), newID};
  byName[componentIn->GetName()] = le;
  byID[newID] = le;
  componentIn->PlaceOutputSampler(newID);
}

G4Transform3D BDSLinkRegistry::Transform(const std::string& name) const
{
  auto search = byName.find(name);
  if (search != byName.end())
    {return search->second.transform;}
  else
    {throw BDSException(__METHOD_NAME__, "unknown link element name \"" + name + "\"");}
}

G4Transform3D BDSLinkRegistry::Transform(const G4int ID) const
{
  auto search = byID.find(ID);
  if (search != byID.end())
    {return search->second.transform;}
  else
    {throw BDSException(__METHOD_NAME__, "unknown link element ID " + std::to_string(ID));}
}

G4Transform3D BDSLinkRegistry::TransformInverse(const std::string& name) const
{
  auto search = byName.find(name);
  if (search != byName.end())
    {return search->second.transformInverse;}
  else
    {throw BDSException(__METHOD_NAME__, "unknown link element name \"" + name + "\"");}
}

G4Transform3D BDSLinkRegistry::TransformInverse(const G4int ID) const
{
  auto search = byID.find(ID);
  if (search != byID.end())
    {return search->second.transformInverse;}
  else
    {throw BDSException(__METHOD_NAME__, "unknown link element ID " + std::to_string(ID));}
}

G4bool BDSLinkRegistry::NoRotation(const std::string& name) const
{
  auto search = byName.find(name);
  if (search != byName.end())
    {return search->second.noRotation;}
  else
    {throw BDSException(__METHOD_NAME__, "unknown link element name \"" + name + "\"");}
}

G4bool BDSLinkRegistry::NoRotation(G4int ID) const
{
  auto search = byID.find(ID);
  if (search != byID.end())
    {return search->second.noRotation;}
  else
    {throw BDSException(__METHOD_NAME__, "unknown link element ID " + std::to_string(ID));}
}