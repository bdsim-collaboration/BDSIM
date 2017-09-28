#include "BDSBunchFactory.hh"

#include "BDSBunchCircle.hh"
#include "BDSBunchComposite.hh"
#include "BDSBunchEShell.hh"
#include "BDSBunchGaussian.hh"
#include "BDSBunch.hh"
#include "BDSBunchRing.hh"
#include "BDSBunchSquare.hh"
#include "BDSBunchUserFile.hh"
#include "BDSBunchTwiss.hh"
#include "BDSBunchPtc.hh"
#include "BDSBunchSixTrack.hh"
#include "BDSBunchHalo.hh"
#include "BDSBunchType.hh"
#include "BDSDebug.hh"

#include "parser/beam.h"

#ifdef USE_GZSTREAM
#include "gzstream.h"
#endif

BDSBunch* BDSBunchFactory::CreateBunch(const GMAD::Beam& beam,
				       G4Transform3D beamlineTransform)  
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << "> Instantiating chosen bunch distribution." << G4endl;
#endif
  G4String distrName = G4String(beam.distrType);

  // This will exit if no correct bunch type found.
  BDSBunchType distrType = BDS::DetermineBunchType(distrName);

  return CreateBunch(distrType, beam, beamlineTransform);
}

BDSBunch* BDSBunchFactory::CreateBunch(BDSBunchType      distrType,
				       const GMAD::Beam& beam,
				       G4Transform3D beamlineTransform)
{ 
  BDSBunch* bdsBunch = nullptr;

  switch (distrType.underlying())
    {
    case BDSBunchType::reference:
      {bdsBunch = new BDSBunch(); break;}
    case BDSBunchType::gaussian:
      {bdsBunch = new BDSBunchGaussian(); break;}
    case BDSBunchType::square:
      {bdsBunch = new BDSBunchSquare(); break;}
    case BDSBunchType::circle:
      {bdsBunch = new BDSBunchCircle(); break;}
    case BDSBunchType::ring:
      {bdsBunch = new BDSBunchRing(); break;}
    case BDSBunchType::eshell:
      {bdsBunch = new BDSBunchEShell(); break;}
    case BDSBunchType::sixtrack:
      {bdsBunch = new BDSBunchSixTrack(); break;}
    case BDSBunchType::twiss:
      {bdsBunch = new BDSBunchTwiss(); break;}
    case BDSBunchType::halo:
      {bdsBunch = new BDSBunchHalo(); break;}
    case BDSBunchType::userfile:
      {
	G4String distrFile = G4String(beam.distrFile);
	if(distrFile.rfind("gz") != std::string::npos)	  
#ifdef USE_GZSTREAM
	  {bdsBunch = new BDSBunchUserFile<igzstream>();}
#else
	{
	  G4cerr << __METHOD_NAME__ << beam.distrFile << " is a compressed file "
		 << "but BDSIM is compiled without GZIP." << G4endl;
	  exit(1);
	}
#endif
	else
	  {bdsBunch = new BDSBunchUserFile<std::ifstream>();}
	break;
      }
    case BDSBunchType::composite:
      {bdsBunch = new BDSBunchComposite(); break;}
    case BDSBunchType::ptc:
      {bdsBunch = new BDSBunchPtc(); break;}
    default:
      {
	G4cerr << "distrType \"" << distrType << "\" not found" << G4endl;
	exit(1);
	break;
      }
    }

  bdsBunch->SetOptions(beam, beamlineTransform);
  
  return bdsBunch;
}
