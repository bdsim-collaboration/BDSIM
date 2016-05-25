#include "BDSFieldE.hh"
#include "BDSFieldEM.hh"
#include "BDSFieldLoader.hh"
#include "BDSFieldMag.hh"


BDSFieldLoader* BDSFieldLoader::instance = nullptr;

BDSFieldLoader* BDSFieldLoader::Instance()
{
  if (!instance)
    {instance = new BDSFieldLoader();}
  return instance;
}

BDSFieldLoader::BDSFieldLoader()
{;}

BDSFieldLoader::~BDSFieldLoader()
{
  delete instance;
  instance = nullptr;
}

BDSFieldMag* BDSFieldLoader::LoadMagField(const BDSFieldInfo& info)
{
  G4String       filepath = info.MagneticFile();
  BDSFieldFormat format   = info.MagneticFormat();
  return LoadMagField(filepath, format);
}

BDSFieldMag* BDSFieldLoader::LoadMagField(G4String filePath, BDSFieldFormat format)
{
  BDSFieldMag* result = nullptr;
  switch (format.underlying())
    {
    case BDSFieldFormat::bdsim2d:
      result = LoadBDSIM2D(filePath); break;
    case BDSFieldFormat::bdsim3d:
      result = LoadBDSIM3D(filePath); break;
    default:
      break;
    }
  return result;
}

BDSFieldE* BDSFieldLoader::LoadEField(G4String /*filePath*/, BDSFieldFormat /*format*/)
{
  return nullptr;
}

BDSFieldEM* BDSFieldLoader::LoadEMField(G4String /*filePath*/, BDSFieldFormat /*format*/)
{
  return nullptr;
}

BDSFieldMag* BDSFieldLoader::LoadBDSIM2D(G4String filePath)
{
  return nullptr;
}

BDSFieldMag* BDSFieldLoader::LoadBDSIM3D(G4String filePath)
{
  return nullptr;
}
