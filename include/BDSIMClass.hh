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
#ifndef BDSIMCLASS_H
#define BDSIMCLASS_H

class BDSBunch;
class BDSComponentConstructor;
class BDSComponentFactoryUser;
class BDSOutput;
class BDSParser;
class BDSRunManager;

class BDSIM
{
public:
  BDSIM();
  BDSIM(int argc, char** argv, bool usualPrintOut=true);
  ~BDSIM();
  
  int Initialise();
  inline bool Initialised() const {return initialised;};

  void BeamOn(int nGenerate=-1);

  /*
  void TrackEnergy(double pdgID, double totalEnergy,
		   double x, double px,
		   double y, double py,
		   double s, double t);

  void TrackMomentum(double pdgID, double momentum,
		     double x, double px,
		     double y, double py,
		     double s, double t);
  */
private:
  bool   ignoreSIGINT;
  bool   usualPrintOut;
  bool   initialised;
  int    argcCache;
  char** argvCache;

  BDSParser*     parser;
  BDSOutput*     bdsOutput;
  BDSBunch*      bdsBunch;
  BDSRunManager* runManager;
  BDSComponentFactoryUser* userComponentFactory;
};

#endif
