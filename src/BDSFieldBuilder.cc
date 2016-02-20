#include "BDSFieldBuilder.hh"
#include "BDSFieldFactory.hh"
#include "BDSFieldInfo.hh"

#include "G4LogicalVolume.hh"

#include <vector>

BDSFieldBuilder* BDSFieldBuilder::instance = nullptr;

BDSFieldBuilder* BDSFieldBuilder::Instance()
{
  if (!instance)
    {instance = new BDSFieldBuilder();}
  return instance;
}

BDSFieldBuilder::~BDSFieldBuilder()
{
  instance = nullptr;
}

BDSFieldBuilder::BDSFieldBuilder()
{
  size_t defaultSize = 30;
  infos.reserve(defaultSize);
  lvs.reserve(defaultSize);
  propagators.reserve(defaultSize);
}
  
void BDSFieldBuilder::RegisterFieldForConstruction(BDSFieldInfo*    info,
						   G4LogicalVolume* logicalVolume,
						   G4bool           propagateToDaughters)
{
  if (info)
    {
      infos.push_back(info);
      lvs.push_back(logicalVolume);
      propagators.push_back(propagateToDaughters);
    }
}

std::vector<BDSFieldObjects*> BDSFieldBuilder::CreateAndAttachAll()
{
  std::vector<BDSFieldObjects*> fields;
  fields.reserve(infos.size());
  for (G4int i = 0; i < (G4int)infos.size(); i++)
    {
      BDSFieldObjects* field = BDSFieldFactory::Instance()->CreateField(*(infos[i]));
      fields.push_back(field);
      field->AttachToVolume(lvs[i], propagators[i]);
    }
  return fields;
}
