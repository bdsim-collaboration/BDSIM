#include "BDSCavityFactory.hh"
#include "BDSCavityFactoryBase.hh"
#include "BDSCavityFactoryElliptical.hh"
#include "BDSCavityFactoryPillBox.hh"
#include "BDSCavityFactoryRectangular.hh"
#include "BDSCavityInfo.hh"
#include "BDSCavityType.hh"
#include "BDSDebug.hh"

#include "globals.hh"

BDSCavityFactory* BDSCavityFactory::instance = nullptr;

BDSCavityFactory* BDSCavityFactory::Instance()
{
  if (!instance)
    {instance = new BDSCavityFactory();}
  return instance;
}

BDSCavityFactory::BDSCavityFactory():
  elliptical(new BDSCavityFactoryElliptical()),
  rectangular(new BDSCavityFactoryRectangular()),
  pillBox(new BDSCavityFactoryPillBox())
{;}

BDSCavityFactory::~BDSCavityFactory()
{
  delete elliptical;
  delete rectangular;
  delete pillBox;
  instance = nullptr;
}

BDSCavityFactoryBase* BDSCavityFactory::GetAppropriateFactory(const BDSCavityType cavityType)
{
  switch (cavityType.underlying())
    {
    case BDSCavityType::elliptical:
      {return elliptical; break;}
    case BDSCavityType::rectangular:
      {return rectangular; break;}
    case BDSCavityType::pillbox:
      {return pillBox; break;}
    default:
      {
#ifdef BDSDEBUG
	G4cout << __METHOD_NAME__ << "unknown type \"" << cavityType << "\" - pill box by default" << G4endl;
#endif
	return pillBox;
	break;
      }
    }
}

BDSCavity* BDSCavityFactory::CreateCavity(G4String             name,
					  const BDSCavityInfo* info)
{
  BDSCavityFactoryBase* factory = GetAppropriateFactory(info->cavityType);

  return factory->CreateCavity(name, info);
}
