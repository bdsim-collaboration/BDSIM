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
#ifndef BDSSDMANAGER_H
#define BDSSDMANAGER_H

class BDSEnergyCounterSD;
class BDSSamplerSD;
class BDSTerminatorSD;

/**
 * @brief A singleton class that holds all required sensitive
 * detector class instances.  
 * 
 * Each sensitive detector class
 * need only be instantiated once and attached to the relevant
 * volume. This instantiates all necessary SDs and holds them.
 * 
 * @author Laurie Nevay
 */

class BDSSDManager
{
public:
  static BDSSDManager* Instance(); /// Singleton accessor.

  ~BDSSDManager();

  /// SD for samplers (plane type)
  inline BDSSamplerSD*       GetSamplerPlaneSD() const
  {return samplerPlane;}

  /// SD for samplers (cylinder type)
  inline BDSSamplerSD*       GetSamplerCylinderSD() const
  {return samplerCylinder;}

  /// SD for measuring turns around circular machine and terminating
  /// particles appropriately.
  inline BDSTerminatorSD*    GetTerminatorSD() const
  {return terminator;}
  
  /// SD for general energy counter.
  inline BDSEnergyCounterSD* GetEnergyCounterSD() const
  {return eCounter;}

  /// SD for tunnel energy counter
  inline BDSEnergyCounterSD* GetEnergyCounterTunnelSD() const
  {return tunnelECounter;}

private:
  /// Private default constructor for singleton.
  BDSSDManager();

  BDSSDManager(const BDSSDManager&) = delete;
  BDSSDManager& operator=(const BDSSDManager&) = delete;
 
  static BDSSDManager* _instance;

  /// @{ SD instance.
  BDSSamplerSD*       samplerPlane;
  BDSSamplerSD*       samplerCylinder;
  BDSTerminatorSD*    terminator;
  BDSEnergyCounterSD* eCounter;
  BDSEnergyCounterSD* tunnelECounter;
  /// @}
};

#endif
