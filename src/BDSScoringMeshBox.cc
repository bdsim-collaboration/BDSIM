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
#include "BDSHistBinMapper.hh"
#include "BDSScoringMeshBox.hh"
#include "BDSScorerMeshInfo.hh"

#include "globals.hh"
#include "G4RotationMatrix.hh"

BDSScoringMeshBox::BDSScoringMeshBox(const G4String&          name,
                                     const BDSScorerMeshInfo& recipe,
                                     const G4Transform3D&     placementTransform):
    BDSScoringMeshBox(name, placementTransform.getTranslation(), placementTransform.getRotation())
{
  // size of the scoring mesh
  scorerSize[0] = recipe.ScoringBoxX();
  scorerSize[1] = recipe.ScoringBoxY();
  scorerSize[2] = recipe.ScoringBoxZ();
  scorerSize[3] = recipe.ScoringBoxE();
  SetSize(scorerSize);

  // divisions of the scoring mesh
  nSegment[0] = recipe.nBinsX;
  nSegment[1] = recipe.nBinsY;
  nSegment[2] = recipe.nBinsZ;
  nSegment[3] = recipe.nBinsE;
  SetNumberOfSegments(nSegment);

  mapper = new BDSHistBinMapper(fNSegment[0], fNSegment[1], fNSegment[2],nSegment[3]);
}

BDSScoringMeshBox::BDSScoringMeshBox(const G4String& name,
                                     const G4ThreeVector& translation,
                                     const G4RotationMatrix& rotation):
  G4ScoringBox(name),
  mapper(nullptr)
{
  fRotationMatrix = new G4RotationMatrix(rotation.inverse());
  fCenterPosition = translation;
}

const BDSHistBinMapper* BDSScoringMeshBox::Mapper() const
{
  if (!sizeIsSet)
    {throw BDSException(__METHOD_NAME__, "mesh \"" + fWorldName + "\" size not set but queried");}
  if (!mapper)
    {mapper = new BDSHistBinMapper(fNSegment[0], fNSegment[1], fNSegment[2], nSegment[3]);}
  return mapper;
}


BDSScoringMeshBox::~BDSScoringMeshBox()
{
  delete mapper;
}
