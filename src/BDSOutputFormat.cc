#include "BDSDebug.hh"
#include "BDSOutputFormat.hh"

#include "globals.hh" // geant4 types  globals

#include <map>

template<>
std::map<BDSOutputFormat,std::string>* BDSOutputFormat::dictionary=
  new std::map<BDSOutputFormat,std::string>
  ({{BDSOutputFormat::none,"none"}
    ,{BDSOutputFormat::rootevent,"rootevent"}});

BDSOutputFormat BDS::DetermineOutputFormat(G4String outputFormat)
{
  std::map<G4String, BDSOutputFormat> formats;
  formats["none"]               = BDSOutputFormat::none;
  formats["rootevent"]          = BDSOutputFormat::rootevent;

  outputFormat.toLower();

  if (formats.find(outputFormat) == formats.end())
    {
      G4cerr << __METHOD_NAME__ << "unknown output format \"" << outputFormat << "\" please specify one of:" << G4endl;
      G4cout << "Available output types are:" << G4endl;
      for (auto keyvalue : formats)
	{G4cout << "\"" << keyvalue.first << "\"" << G4endl;}
      exit(1);
    }
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined format \"" << outputFormat << "\" to be "
	 << formats[outputFormat] << G4endl;
#endif
  
  return formats[outputFormat];
}
