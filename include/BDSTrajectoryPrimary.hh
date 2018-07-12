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
#ifndef BDSTRAJECTORYPRIMARY_H
#define BDSTRAJECTORYPRIMARY_H

#include "BDSTrajectory.hh"

#include <ostream>

class BDSTrajectoryPoint;

class G4Step;
class G4Track;

class BDSTrajectoryPrimary; // forward declaration so namespaced method can be at top
namespace BDS
{
  /// Search the trajectory container for the primary trajectory.
  BDSTrajectoryPrimary* GetPrimaryTrajectory(G4TrajectoryContainer* trajCon);
}

/**
 * @brief Trajectory information for only the primary.
 *
 * Similar to BDSTrajectory but keeps a running track of the end point
 * as well as the first scattering point.
 *
 * @author L. Nevay
 */

class BDSTrajectoryPrimary: public BDSTrajectory
{
public:
  BDSTrajectoryPrimary(const G4Track* aTrack,
		       const G4bool& interactiveIn,
		       const G4bool& suppressTransportationStepsIn,
		       const G4bool& storeTrajectoryPoints);
  
  /// copy constructor is not needed
  BDSTrajectoryPrimary(BDSTrajectoryPrimary &) = delete;

  virtual ~BDSTrajectoryPrimary();

  virtual G4bool IsPrimary() const {return true;}
  
  inline int operator == (const BDSTrajectoryPrimary& right) const {return (this==&right);}

  /// Append a step point to this trajectory. This is required for the trajectory
  /// points to show up in the visualisation correctly.
  virtual void AppendStep(const G4Step* aStep);
  
  /// Output stream
  friend std::ostream& operator<< (std::ostream &out, BDSTrajectoryPrimary const &t);

  /// @{ Accessor.
  const BDSTrajectoryPoint* FirstHit()  const {return firstHit;}
  const BDSTrajectoryPoint* LastPoint() const {return lastPoint;}
  /// @}
  
protected:
  BDSTrajectoryPoint* firstHit;  ///< Point owned by this class for the first scattering point.
  BDSTrajectoryPoint* lastPoint; ///< Point owned by this class for the last-most point.

private:
  const G4bool storeTrajectoryPoints; ///< Whether to use base class to store all points.
  
  BDSTrajectoryPrimary() = delete; ///< No default constructor required.
};

#endif
