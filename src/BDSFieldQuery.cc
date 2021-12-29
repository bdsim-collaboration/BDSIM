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
#include "BDSFieldQuery.hh"
#include "BDSFieldQueryInfo.hh"

#include "G4String.hh"
#include "G4Transform3D.hh"
#include "G4Types.hh"

#include <vector>

BDSFieldQuery::BDSFieldQuery()
{;}

BDSFieldQuery::~BDSFieldQuery()
{;}

void BDSFieldQuery::QueryFields(const std::vector<BDSFieldQueryInfo*>& fieldQueries)
{
  for (const auto& fieldQuery : fieldQueries)
    {QueryField(fieldQuery);}
}

void BDSFieldQuery::QueryField(const BDSFieldQueryInfo* query)
{
  G4double xLocal = query->xInfo.min;
  G4double xStep  = (query->xInfo.max - query->xInfo.min) / (G4double)query->xInfo.n;
  G4double yLocal = query->yInfo.min;
  G4double yStep  = (query->yInfo.max - query->yInfo.min) / (G4double)query->yInfo.n;
  G4double zLocal = query->zInfo.min;
  G4double zStep  = (query->zInfo.max - query->zInfo.min) / (G4double)query->zInfo.n;
  G4double tLocal = query->tInfo.min;
  G4double tStep  = (query->tInfo.max - query->tInfo.min) / (G4double)query->tInfo.n;
  G4double xGlobal, yGlobal, zGlobal, tGlobal = 0;
  const G4Transform3D& globalTransform = query->globalTransform;
  
  OpenFiles(query);
  
  G4double fieldValue[6];
  for (G4int i = 0; i < query->xInfo.n; i++)
    {
      for (G4int j = 0; j < query->yInfo.n; j++)
	{
	  for (G4int k = 0; k < query->zInfo.n; k++)
	    {
	      for (G4int l = 0; l < query->tInfo.n; l++)
		{
		  ApplyTransform(globalTransform, xLocal, yLocal, zLocal, tLocal,
				 xGlobal, yGlobal, zGlobal, tGlobal);
		  GetFieldValue(xGlobal, yGlobal, zGlobal, tGlobal, fieldValue);
		  WriteFieldValue(xGlobal, yGlobal, zGlobal, tGlobal, fieldValue);
		  
		  tLocal += tStep;
		}
	      zLocal += zStep;
	    }
	  yLocal += yStep;
	}
      xLocal += xStep;
    }
  
  CloseFiles();
}

void BDSFieldQuery::OpenFiles(const BDSFieldQueryInfo* query)
{;}

void BDSFieldQuery::CloseFiles()
{;}

void BDSFieldQuery::ApplyTransform(const G4Transform3D& globalTransform,
				   G4double xLocal, G4double yLocal, G4double zLocal, G4double tLocal,
				   G4double& xGlobal, G4double& yGlobal, G4double& zGlobal, G4double& tGlobal)
{;}

void BDSFieldQuery::GetFieldValue(G4double xGlobal, G4double yGlobal, G4double zGlobal, G4double tGlobal,
				  G4double fieldValue[6])
{;}

void BDSFieldQuery::WriteFieldValue(G4double xGlobal, G4double yGlobal, G4double zGlobal, G4double tGlobal,
				    G4double fieldValue[6])
{;}
