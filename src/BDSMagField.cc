#include "BDSMagField.hh"

#include "globals.hh"  // geant4 globals / types
#include "G4RotationMatrix.hh"

BDSMagField::BDSMagField(G4String bmap,
			 G4double bmapZOffset):
  translation(0,0,0),
  rotation(nullptr)
  bmap(bmap),
  bmapZOffset(bmapZOffset)
  bFile(""),
  bFormat(""),
  isPrepared(false)
{
  ParseBMapFormatAndFile();  
}

BDSMagField::~BDSMagField()
{
  delete rotation;
}

void BDSMagField::SetOriginRotation(G4RotationMatrix* rot)
{rotation = rot;}

void BDSMagField::SetOriginRotation(G4RotationMatrix rot)
{rotation = new G4RotationMatrix(rot);}

G4RotationMatrix BDSMagField::Rotation() const
{return *rotation;}

void BDSMagField::SetOriginTranslation(G4ThreeVector trans)
{translation = trans;}

G4bool HasNPoleFields()
{return false;}

G4bool HasUniformField()
{return false;}

G4bool HasFieldMap()
{return false;}

void BDSMagField::CheckPrepared() const
{
  //Field must be prepared!
  if(!_isPrepared)
    {
      G4String exceptionString = __METHOD_NAME__ + "- field has not been prepared. Rotation not set. Aborting.";
      G4Exception(exceptionString.c_str(), "-1", FatalErrorInArgument, "");   
    }
}

void BDSMagField::ParseBMapFormatAndFile()
{
  _bFormat="none";
  if(_bmap != "")
    {
      G4int pos = _bmap.find(":");
      if(pos<0)
	{G4cerr << "WARNING: invalid B map reference format : " << _bmap << G4endl;}
    else
      {
	_bFormat = _bmap.substr(0,pos);
	_bFile = BDSExecOptions::Instance()->GetBDSIMPATH() + _bmap.substr(pos+1,_bmap.length() - pos); 
#ifdef BDSDEBUG
	G4cout << "BDSElement::PlaceComponents bmap file is " << _bFile << G4endl;
#endif
      }
    }
}
