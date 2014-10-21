#include "TRKPlacement.hh"

TRKPlacement::TRKPlacement() : x(0.0), y(0.0), z(0.0) 
{
}

TRKPlacement::TRKPlacement(double xIn, double yIn, double zIn) : x(xIn), y(yIn), z(zIn) 
{
}

TRKPlacement::~TRKPlacement() {}

vector3 TRKPlacement::GetGlobal()const
{
  return vector3(x,y,z);

}

std::ostream& operator<< (std::ostream& out, const TRKPlacement& /*placement*/) {
  out << "Placement - TBC ";
  return out;
}
