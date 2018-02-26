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
#include "BDSFieldMag.hh"
#include "BDSFieldMagMultipoleOuter.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include <cmath>

BDSFieldMagMultipoleOuter::BDSFieldMagMultipoleOuter(const G4int              orderIn,
						     const BDSMagnetStrength* stIn,
						     const G4double&          poleTipRadiusIn,						     
						     BDSFieldMag*             innerFieldIn,
						     const G4double&          beamPipeRadius):
  order(orderIn),
  normalisation(1),
  m(G4TwoVector(0,1)),
  innerField(innerFieldIn),
  useInnerField(false),
  poleTipRadius(poleTipRadiusIn),
  transitionLengthScale(1)
{
  G4double angle = CLHEP::pi/(2.*order);

  if (order > 1)
    {// don't do for dipole
      rotation = new G4RotationMatrix();
      antiRotation = new G4RotationMatrix();
      rotation->rotateZ(angle);
      antiRotation->rotateZ(-angle);
    }
  factor = (order + 1)/2.;
  negativeField = (*stIn)["k1"] < 0;
  
  // query inner field
  G4ThreeVector poleTipPoint = G4ThreeVector(0, poleTipRadius, 0);
  poleTipPoint.rotateZ(angle); // rotate from 0,1 to the pole position (different for each magnet).
  G4ThreeVector fieldAtPoleTip = innerField->GetField(poleTipPoint,/*t=*/0);
  G4double fieldAtPoleTipMag = fieldAtPoleTip.mag();
  
  // query outer field
  // we query this field object but with the normalisation initialised to 1 so it won't
  // affect the result desipte using the same code
  // the inner field is by default OFF just now so won't affect this
  G4ThreeVector rawOuterlFieldAtPoleTip = GetField(poleTipPoint,/*t=*/0);
  G4double rawOuterlFieldAtPoleTipMag = rawOuterlFieldAtPoleTip.mag();
  
  // normalisation
  normalisation = fieldAtPoleTipMag / rawOuterlFieldAtPoleTipMag;

  if (BDS::IsFinite(beamPipeRadius))
    {
      useInnerField = true; // must be after normalisation calculation
      // fade between inner and outer field over 10% of distance between
      // beam pipe and pole tip.
      transitionLengthScale = 0.3*(poleTipRadius - beamPipeRadius);
    }
  else
    {
      useInnerField = false;
      delete innerField;
    }      
}

BDSFieldMagMultipoleOuter::~BDSFieldMagMultipoleOuter()
{
  delete rotation;
  delete antiRotation;
  delete innerField;
}

G4ThreeVector BDSFieldMagMultipoleOuter::GetField(const G4ThreeVector& position,
						  const G4double       t) const
{
  // rotate from dipole frame to npole frame
  G4ThreeVector rotatedPosition(position);
  if (rotation)
    {rotatedPosition = rotatedPosition.transform(*rotation);}

  // construct 2D vector of position
  G4TwoVector r(rotatedPosition.x(), rotatedPosition.y());
  G4double rmag = r.mag(); // calculate magnitude

  // we can't deal with positions smaller than r of 1mm as the magnetic dipole is defined
  // with (0,1). set limit to 2mm and query as if further out
  if (rmag <= 2)
    {
      G4double scale = 2./rmag;
      rmag = 2;
      r*= scale;
    }

  // calculate angle in 2D polar coordinates from y axis vertical clockwise
  G4double angle = std::atan2(r.x(),r.y());
  if (angle < 0)
    {angle = CLHEP::twopi + angle;}

  // the point to query in the nominal dipole equation
  G4TwoVector query(0,rmag);
  query.rotate(-factor*angle);

  // calculate the field according to a magnetic dipole m at position r.
  G4TwoVector b = 3*query*(m.dot(query))/std::pow(rmag,5) - m/std::pow(rmag,3);

  // package in 3-vector
  G4ThreeVector result = G4ThreeVector(b.x(), b.y(), 0);

  // rotate back to n-pole frame
  G4ThreeVector rotatedResult(result);
  if (antiRotation)
    {rotatedResult = (*antiRotation)*result;}
  rotatedResult *= normalisation;

  rotatedResult *= negativeField ? -1. : 1.;

  if (useInnerField)
    {
      G4ThreeVector innerB = innerField->GetField(position, t);
      // normalise with tanh (sigmoid-type function) between -3 and 3 in x and 0,1 in y.
      // scaled spatially in r across the transition length scale
      // tanh(x) goes from -1 to 1 -> change to 0,1 via +1 and /2
      G4double weight = (std::tanh(3*(rmag - std::abs(poleTipRadius))/(transitionLengthScale)) + 1) / 2.0;
      //if (poleTipRadius < 0)
      //{weight = 1-weight;} // rmag is +ve so the subtraction doesn't work for both sides
      if (std::abs(weight) > 0.99 || std::abs(weight) < 0.01)
	{weight = std::round(weight);} // tanh is asymptotic - snap to 1.0 when beyond 0.99
      rotatedResult = weight*rotatedResult + (1-weight)*innerB; // weighted sum of two components
    }
  
  return rotatedResult;
}
