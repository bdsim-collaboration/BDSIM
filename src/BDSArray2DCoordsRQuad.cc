#include "BDSArray2DCoordsRQuad.hh"
#include "BDSFieldValue.hh"

#include "globals.hh"

#include <algorithm>
#include <cmath>
#include <ostream>


BDSArray2DCoordsRQuad::BDSArray2DCoordsRQuad(BDSArray2DCoords* arrayIn):
  BDSArray2DCoords(*arrayIn),
  returnValue(BDSFieldValue())
{;}

G4bool BDSArray2DCoordsRQuad::OutsideCoords(const G4double x,
					    const G4double y,
					    const G4double z,
					    const G4double t) const
{
  G4bool rx = x < -xMax || x > xMax;
  G4bool ry = y < -yMax || y > yMax;
  G4bool rz = z <  zMin || z > zMax;
  G4bool rt = t <  tMin || t > tMax;
  return rx || ry || rz || rt;
}

G4double BDSArray2DCoordsRQuad::ArrayCoordsFromX(const G4double x) const
{
  // xmin becomes -xmax
  return (x + xMax) / xStep;
}

G4double BDSArray2DCoordsRQuad::ArrayCoordsFromY(const G4double y) const
{
  return (y + yMax) / yStep;
}

G4int BDSArray2DCoordsRQuad::NearestX(const G4double x) const
{
  return (G4int)round((x+xMax)/xStep);
}

G4int BDSArray2DCoordsRQuad::NearestY(const G4double y) const
{
  return (G4int)round((y+yMax)/yStep);
}

const BDSFieldValue& BDSArray2DCoordsRQuad::GetConst(const G4int x,
						     const G4int y,
						     const G4int z,
						     const G4int t) const
{
  if (Outside(x,y,z,t))
    {return defaultValue;}

  // Data is really
  // A

  // This interface makes it look like (0,0) bottom left, top right (2*xsize, 2*ysize)
  // in array coords - ie int number of elements.
  // B | A
  // -----
  // C | D

  // factors to multiply result by - saves duplicate testing of which quadrant etc
  G4double xr = 1;
  G4double yr = 1;
  G4int    xi = x; // copy of x index that can be modified.
  G4int    yi = y; 
  
  if (x < nX)
    {
      if (y < nY)
	{// C quadrant - coordinate flip is a subtraction in array coords
	  xi = nX - 1 - x; xr = -1; // flip
	  yi = nY - 1 - y; yr = -1; // flip
	}
      else
	{// B quadrant
	  xi = nX - 1 - x; // flip
	  yi = y - nY; yr = -1; // just offset index, but flip result
	}
    }
  else
    {
      if (y < nY)
	{// D quadrant
	  xi = x - nX; xr = -1;// just offset index, flip x component
	  yi = nY - 1 - y;
	}
      else
	{// A quadrant
	  xi = x - nX;
	  yi = y - nY;
	}
    }

  G4bool swapResult = false;
  if (yi > xi) //lies above y = x line (even in array coords)
    {
      std::swap(xi, yi);
      swapResult = true;
    }

  returnValue = BDSArray2DCoords::GetConst(xi,yi,z,t);

  if (swapResult)
    {
      G4double xv = returnValue.x();
      G4double yv = returnValue.y();
      returnValue = BDSFieldValue(yv,xv,returnValue.z());
    }
  
  returnValue[0] = returnValue.x() * xr;
  returnValue[1] = returnValue.y() * yr;

  return returnValue;
}

G4bool BDSArray2DCoordsRQuad::Outside(const G4int x,
				      const G4int y,
				      const G4int z,
				      const G4int t) const
{
  G4bool rx = x < 0 || x > 2*(nX-1);
  G4bool ry = y < 0 || y > 2*(nY-1);
  G4bool rz = z < 0 || z > 2*(nZ-1);
  G4bool rt = t < 0 || t > 2*(nT-1);
  return rx || ry || rz || rt;
}

std::ostream& BDSArray2DCoordsRQuad::Print(std::ostream& out) const
{
  std::ostream& out2 = BDSArray2DCoords::Print(out);

  out2 << G4endl << "REFLECTED VERSION" << G4endl;
  
  for (G4int t = 0; t < nT; t++)
    {
      for (G4int z = 0; z < nZ; z++)
	{
	  for (G4int y = 0; y < 2*nY; y++)
	    {
	      for (G4int x = 0; x < 2*nX; x++)
		{out2 << GetConst(x,y,z,t) << G4endl;}
	    }
	}
    }
  return out2;
}

std::ostream& operator<< (std::ostream& out, BDSArray2DCoordsRQuad const &a)
{
  return a.Print(out);
}
