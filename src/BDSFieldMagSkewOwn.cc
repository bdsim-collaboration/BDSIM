#include "BDSFieldMag.hh"
#include "BDSFieldMagSkewOwn.hh"

BDSFieldMagSkewOwn::BDSFieldMagSkewOwn(BDSFieldMag* fieldIn,
				       G4double     angle):
  BDSFieldMagSkew(fieldIn, angle),
  field(fieldIn)
{;}

BDSFieldMagSkewOwn::~BDSFieldMagSkewOwn()
{
  delete field;
}
