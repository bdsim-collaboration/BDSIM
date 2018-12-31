/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2019.

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
#ifndef TRKPARTICLEDEFINITION_h
#define TRKPARTICLEDEFINITION_h 1

#include <map>
#include <string>

/**
 * @brief class that holds constants related to elementary particles
 */

class TRKParticleDefinition {
public:
  ~TRKParticleDefinition();
  static TRKParticleDefinition* Instance();   ///> access method for instance
  std::pair<double,int> GetParticleMassAndCharge(const std::string particlename);
  double GetParticleMass  (const std::string particlename);
  int    GetParticleCharge(const std::string particlename);

protected:
  TRKParticleDefinition();
private:
  static TRKParticleDefinition* _instance; ///> singleton pattern
  /// map of particle name and mass in MeV and elementary charge
  std::map<std::string,std::pair<double, int> > particleMassCharge;
  void PrepareDefinitions();
};

#endif
