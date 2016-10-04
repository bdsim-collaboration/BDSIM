#include "BDSArray4D.hh"
#include "BDSFieldValue.hh"

#include "globals.hh" // geant4 types / globals

#include <ostream>
#include <vector>


BDSArray4D::BDSArray4D(G4int nXIn, G4int nYIn, G4int nZIn, G4int nTIn):
  nX(nXIn), nY(nYIn), nZ(nZIn), nT(nTIn),
  data(std::vector<BDSFieldValue>(nTIn*nZIn*nYIn*nXIn)),
  defaultValue(BDSFieldValue())
{;}

BDSFieldValue& BDSArray4D::operator()(const G4int x,
				      const G4int y,
				      const G4int z,
				      const G4int t)
{
  OutsideWarn(x,y,z,t); // keep as a warning as can't assign to invalid index
  return data[t*nT + z*nZ + y*nY + z];
}

const BDSFieldValue& BDSArray4D::GetConst(const G4int x,
					  const G4int y,
					  const G4int z,
					  const G4int t) const
{
  if (Outside(x,y,z,t))
    {return defaultValue;}
  return data[t*nT + z*nZ + y*nY + z];
}
  
const BDSFieldValue& BDSArray4D::operator()(const G4int x,
					    const G4int y,
					    const G4int z,
					    const G4int t) const
{
  return GetConst(x,y,z,t);
}

G4bool BDSArray4D::Outside(const G4int x,
			   const G4int y,
			   const G4int z,
			   const G4int t) const
{
  G4bool rx = x < 0 || x > nX-1;
  G4bool ry = y < 0 || y > nY-1;
  G4bool rz = z < 0 || z > nZ-1;
  G4bool rt = t < 0 || t > nT-1;
  return rx || ry || rz || rt;
}

void BDSArray4D::OutsideWarn(const G4int x,
			     const G4int y,
			     const G4int z,
			     const G4int t) const
{
  if (Outside(x,y,z,t))
    {
      G4cerr << "(" << x << ", " << y << ", " << z << ", " << t
	     << ") is outside array" << G4endl;
      exit(1);
    }
}

std::ostream& operator<< (std::ostream& out, BDSArray4D const &a)
{
  out << "#x = " << a.nX << ", ";
  out << "#y = " << a.nY << ", ";
  out << "#z = " << a.nZ << ", ";
  out << "#t = " << a.nT << G4endl;
  
  for (G4int t = 0; t < a.nT; t++)
    {
      out << "[ " << G4endl;
      for (G4int z = 0; z < a.nZ; z++)
	{
	  out << "[ ";
	  for (G4int y = 0; y < a.nY; y++)
	    {
	      out << "[ ";
	      for (G4int x = 0; x < a.nX; x++)
		{out << a(x,y,z,t) << "\t";}
	      out << "]" << G4endl;
	    }
	  out << "]" << G4endl;
	}
      out << G4endl << "]" << G4endl;
    }
  return out;
}
