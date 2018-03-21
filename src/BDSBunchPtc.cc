/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

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
#include "BDSBunchPtc.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSUtilities.hh"

#include "parser/beam.h"

#include "CLHEP/Units/PhysicalConstants.h"

#include <fstream>
#include <regex>

BDSBunchPtc::BDSBunchPtc():
  nRays(0),
  fileName("./inrays.madx"),
  iRay(0),
  loopedOver(false)
{;}

BDSBunchPtc::~BDSBunchPtc()
{
  for(std::vector<double*>::iterator i = ptcData.begin();i!=ptcData.end();++i)
    {delete[] *i;}
}

void BDSBunchPtc::LoadPtcFile()
{ 
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  // open file and read line by line and extract values
  std::ifstream ifstr(fileName);

  if (!ifstr)
    {
      G4cout << __METHOD_NAME__ << "\"" << fileName << "\" file doesn't exist - exiting as no input" << G4endl;
      exit(1);
    }

  std::string line; 
  // read single line 
  while(std::getline(ifstr,line)) { 
    
    // variable for storage
    double x=0.0;
    double y=0.0;
    double px=0.0;
    double py=0.0; 
    double t=0.0;
    double pt=0.0;
    
    // create regular expressions 
    std::regex rex("\\sx\\s*=\\s*([0-9eE.+-]+)");
    std::regex rey("\\sy\\s*=\\s*([0-9eE.+-]+)");
    std::regex repx("px\\s*=\\s*([0-9eE.+-]+)");
    std::regex repy("py\\s*=\\s*([0-9eE.+-]+)");
    std::regex ret("\\st\\s*=\\s*([0-9eE.+-]+)");
    std::regex rept("pt\\s*=\\s*([0-9eE.+-]+)");

    // return search match objects 
    std::smatch smx;
    std::smatch smy;
    std::smatch smpx;
    std::smatch smpy;
    std::smatch smt;    
    std::smatch smpt;

    // perform search
    std::regex_search(line,smx, rex);
    std::regex_search(line,smy, rey);
    std::regex_search(line,smpx,repx);
    std::regex_search(line,smpy,repy);
    std::regex_search(line,smt, ret);
    std::regex_search(line,smpt, rept);

    if(smx.size() == 2)  x  = std::stod(smx[1]);
    if(smy.size() == 2)  y  = std::stod(smy[1]);
    if(smpx.size() == 2) px = std::stod(smpx[1]);
    if(smpy.size() == 2) py = std::stod(smpy[1]);
    if(smt.size() == 2)  t  = std::stod(smt[1]);
    if(smpt.size() == 2) pt = std::stod(smpt[1]);

#ifdef BDSDEBUG 
    G4cout << __METHOD_NAME__ << "read line " << line << G4endl;
    G4cout << __METHOD_NAME__ << "values    " << x << " " << px << " " << y << " " << py << " " << t << " " << pt << G4endl;   
#endif 
    
    double *values = new double[6]; 
    values[0] = x;
    values[1] = px;
    values[2] = y; 
    values[3] = py;
    values[4] = t; 
    values[5] = pt; 
    
    // append values to storage vector
    ptcData.push_back(values);
  }

  // set number of available rays in options
  nRays = ptcData.size();

  BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  if (globals->MatchDistrFileLength())
    {globals->SetNumberToGenerate(nRays);}

  return;
}

void BDSBunchPtc::SetOptions(const BDSParticleDefinition* beamParticle,
			     const GMAD::Beam& beam,
			     G4Transform3D beamlineTransformIn)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << " " << beam.distrFile << G4endl;
#endif

  BDSBunch::SetOptions(beamParticle, beam, beamlineTransformIn);
  SetDistrFile((G4String)beam.distrFile); 
  LoadPtcFile();
}

void BDSBunchPtc::SetDistrFile(G4String distrFileName)
{
  fileName = BDS::GetFullPath(distrFileName);
}

void BDSBunchPtc::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
				  G4double& xp, G4double& yp, G4double& zp,
				  G4double& t , G4double&  E, G4double& weight)
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // if all particles are read, start at 0 again and note it down in bool flag
  if (iRay == nRays)
    {
      iRay       = 0;
      loopedOver = true;
    }
  
  x0     = (ptcData[iRay][0] + X0) * CLHEP::m;
  y0     = (ptcData[iRay][2] + Y0) * CLHEP::m;
  z0     = (ptcData[iRay][4] + Z0) * CLHEP::m;
  xp     = ptcData[iRay][1] * CLHEP::rad+Xp0;
  yp     = ptcData[iRay][3] * CLHEP::rad+Yp0;
  t      = (z0-Z0)*CLHEP::m / CLHEP::c_light + T0 * CLHEP::s;
  E      = E0 * CLHEP::GeV * (ptcData[iRay][5]+1.0);
  zp     = CalculateZp(xp,yp,Zp0);

  ApplyTransform(x0,y0,z0,xp,yp,zp);
  
  weight = 1.0; 

  iRay++;
  
  if (loopedOver)
    {
      G4cout << __METHOD_NAME__ << "End of file reached. Returning to beginning of file." << G4endl;
      loopedOver = false; // reset flag until next time
    }

  return;
}

