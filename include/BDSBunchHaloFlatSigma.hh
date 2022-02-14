/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2022.

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
#ifndef BDSBUNCHHALOFLATSIGMA_H
#define BDSBUNCHHALOFLATSIGMA_H
#include "BDSBunch.hh"
#include "BDSBunchType.hh"
#include "BDSException.hh"

#include "G4Transform3D.hh"
#include "G4Types.hh"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <memory>
#include <numeric>
#include <vector>

namespace CLHEP
{
  class RandFlat;
}

namespace GMAD
{
  class Beam;
}

namespace detail
{

/**
* @brief Simple struct for storing action/angle pairs to aid readability.
* Implementation detail.
*
* @author Stuart Walker
*/

struct ActionAngleCoord
{
  double action;
  double angle;
};


/**
* @brief Simple struct for storing position/momentum pairs to aid readability.
* Implementation detail.
*
* @author Stuart Walker
*/

struct PhaseSpaceCoord
{
  double position;
  double momentum;
};

/**
* @brief Simple struct for storing Twiss alpha/beta pairs to aid readability.
* Implementation detail.
*
* @author Stuart Walker
*/

struct TwissPair
{
  double alpha;
  double beta;
};

PhaseSpaceCoord PhaseSpaceCoordFromActionAngle(ActionAngleCoord aa, TwissPair tp);

PhaseSpaceCoord GetCoordinatesOnPhaseSpaceEllipse(double action, TwissPair tp);

template <typename T> std::vector<double> CumulativeDistances(T pairs)
{
  if (pairs.size() < 2)
    {throw BDSException("At least two points needed to calculated adjacent distances.");}
  std::vector<double> distances;

  auto it = std::begin(pairs);
  auto next = std::next(it);
  for (; next != std::end(pairs); ++it, ++next)
    {
      double x0 = it->position;
      double xp0 = it->momentum;
      
      double x1 = next->position;
      double xp1 = next->momentum;
      
      double xd = std::pow((x0 - x1), 2);
      double xpd = std::pow((xp0 - xp1), 2);
      
      double distance = std::sqrt(xd + xpd);
      
      distances.push_back(distance);
    }
  std::vector<double> result;
  std::partial_sum(std::begin(distances), std::end(distances),
                   std::back_inserter(result));
  return result;
}


/**
* @brief Class for generating points uniformly on ellipse perimeters via
* interpolation.  Part of implementation.
*
* @author Stuart Walker
*/

class EllipsePointGenerator
{
public:
  EllipsePointGenerator(double actionIn, TwissPair tp);
  ~EllipsePointGenerator() = default;
  
  PhaseSpaceCoord GetRandomPointOnEllipse() const;
  inline double EllipsePerimeter() const {return pathLengths.back();};

private:
  double PathLengthToAngle(double pathLength) const;
  
  double action;
  TwissPair twisspair;
  std::unique_ptr<CLHEP::RandFlat> flatRandomGenerator;
  std::vector<double> angles;
  std::vector<double> pathLengths;
};

} // namespace detail

/**
* @brief Bunch halo distribution where the PDF is uniformly distribution in sigma.
*
* @author Stuart Walker
*/

class BDSBunchHaloFlatSigma: public BDSBunch
{
public:
  BDSBunchHaloFlatSigma();
  virtual ~BDSBunchHaloFlatSigma(){};
  virtual void SetOptions(const BDSParticleDefinition* beamParticle,
                          const GMAD::Beam& beam,
                          const BDSBunchType& distrType,
                          G4Transform3D beamlineTransformIn = G4Transform3D::Identity,
                          const G4double beamlineS = 0);
  virtual void CheckParameters();
  virtual BDSParticleCoordsFull GetNextParticleLocal();

private:
  /// @{ Twiss parameter
  G4double alphaX;
  G4double alphaY;
  G4double betaX;
  G4double betaY;
  G4double emitX;
  G4double emitY;
  G4double gammaX;
  G4double gammaY;
  G4double sigmaX;
  G4double sigmaY;
  /// @}

  G4double haloNSigmaXInner;
  G4double haloNSigmaXOuter;
  G4double haloNSigmaYInner;
  G4double haloNSigmaYOuter;

  CLHEP::RandFlat* flatGen;
};

#endif
