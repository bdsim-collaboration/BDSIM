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
#include "BDSExtent.hh"
#include "BDSScorerMeshInfo.hh"
#include "BDSUtilities.hh"
#include "BDSBHTypedefs.hh"

#include "parser/scorermesh.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <fstream>

BDSScorerMeshInfo::BDSScorerMeshInfo(const GMAD::ScorerMesh& mesh)
{
  name = G4String(mesh.name);
  nBinsX = mesh.nx;
  nBinsY = mesh.ny;
  nBinsZ = mesh.nz;
  nBinsE = mesh.ne;

  if (!BDS::IsFinite(mesh.xsize))
    {throw BDSException(__METHOD_NAME__, "xsize must be finite in mesh \"" + mesh.name + "\"");}
  if (!BDS::IsFinite(mesh.ysize))
    {throw BDSException(__METHOD_NAME__, "ysize must be finite in mesh \"" + mesh.name + "\"");}
  if (!BDS::IsFinite(mesh.zsize))
    {throw BDSException(__METHOD_NAME__, "zsize must be finite in mesh \"" + mesh.name + "\"");}

  xLow  = -0.5*mesh.xsize * CLHEP::m;
  xHigh =  0.5*mesh.xsize * CLHEP::m;
  yLow  = -0.5*mesh.ysize * CLHEP::m;
  yHigh =  0.5*mesh.ysize * CLHEP::m;
  zLow  = -0.5*mesh.zsize * CLHEP::m;
  zHigh =  0.5*mesh.zsize * CLHEP::m;
  eLow  =  mesh.eLow* CLHEP::GeV;
  eHigh =  mesh.eHigh* CLHEP::GeV;
  eScale = mesh.eScale;

  if (!BDS::IsFinite(nBinsX))
    {throw BDSException(__METHOD_NAME__, "nx must be finite in mesh \"" + mesh.name + "\"");}
  if (!BDS::IsFinite(nBinsY))
    {throw BDSException(__METHOD_NAME__, "ny must be finite in mesh \"" + mesh.name + "\"");}
  if (!BDS::IsFinite(nBinsZ))
    {throw BDSException(__METHOD_NAME__, "nz must be finite in mesh \"" + mesh.name + "\"");}

  extent = BDSExtent(xLow, xHigh,
		     yLow, yHigh,
		     zLow, zHigh);

  if (eScale == "user")
    {
      std::string const BinsEdgesFile(mesh.eBinsEdgesFilenamePath);
      std::ifstream file(BinsEdgesFile.c_str());
      
      if (file)
	{
	  // Reading of the bins edges file.
          std::istream_iterator<double> it(file);
          std::istream_iterator<double> end;
          std::back_insert_iterator<std::vector<double>> it2(eBinsEdges);
	  
          copy(it, end, it2);
	}
      else
	{throw BDSException(__METHOD_NAME__, "eBinsEdgesFilenamePath must be the path to a .txt file");}

      nBinsE = eBinsEdges.size()-1;
      eLow = eBinsEdges[0];
      eHigh = eBinsEdges[nBinsE];
    }
  
  if (nBinsE > 1)
    {
      if (eScale == "linear")
	{energyAxis = new boost_histogram_linear_axis(nBinsE, eLow, eHigh, "energy");}
      else if (eScale == "log")
	{energyAxis = new boost_histogram_log_axis(nBinsE, eLow, eHigh, "energy");}
      else if (eScale == "user")
	{
	  std::vector<double> eBinsEdgesEnergyAxis;
	  for (int i=0; i < (int)eBinsEdges.size(); i++)
	    {eBinsEdgesEnergyAxis.push_back(eBinsEdges[i]*CLHEP::GeV);}
	  energyAxis = new boost_histogram_variable_axis(eBinsEdgesEnergyAxis, "energy");
	}
      else
	{throw BDSException(__METHOD_NAME__, "eScale must be 'linear', 'log' or 'user' in mesh \"" + mesh.name + "\"");}
    }
}
