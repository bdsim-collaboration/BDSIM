#include "BDSArray3D.hh"

#include "G4Types.hh"

#include <ostream>

BDSArray3D::BDSArray3D(G4int nX, G4int nY, G4int nZ):
  BDSArray4D(nX,nY,nZ,1)
{;}

std::ostream& operator<< (std::ostream& out, BDSArray3D const &a)
{
  out << "#x = " << a.sizeX() << ", ";
  out << "#y = " << a.sizeY() << ", ";
  out << "#z = " << a.sizeZ() << G4endl;
  
  for (G4int z = 0; z < a.sizeZ(); z++)
    {
      out << "[ ";
      for (G4int y = 0; y < a.sizeY(); y++)
	{
	  out << "[ ";
	  for (G4int x = 0; x < a.sizeX(); x++)
	    {out << a(x,y,z,0) << "\t";}
	  out << "]" << G4endl;
	}
      out << G4endl << "]" << G4endl;
    }
  return out;
}
