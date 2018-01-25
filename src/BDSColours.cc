/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "BDSColours.hh"
#include "BDSDebug.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Colour.hh"

#include <iomanip>
#include <map>
#include <sstream>

BDSColours* BDSColours::_instance = nullptr;

BDSColours* BDSColours::Instance()
{
  if (_instance == nullptr)
    {_instance = new BDSColours();}
  return _instance;
}

BDSColours::~BDSColours()
{
  _instance = nullptr;
}

BDSColours::BDSColours()
{
  magnetName[1] = "sectorbend";
  magnetName[2] = "quadrupole";
  magnetName[3] = "sextupole";
  magnetName[4] = "octupole";
  magnetName[5] = "decapole";

  // colour definitions (red,green,blue)
  // some additional info:
  // http://geant4.web.cern.ch/geant4/G4UsersDocuments/UsersGuides/ForApplicationDeveloper/html/Visualization/attributes.html
  // https://en.wikipedia.org/wiki/Web_colors

  // special
  colours["default"]          = new G4Colour(1,     1,     1);     // white
  colours["warning"]          = new G4Colour(1,     0.078, 0.576); // hot warning pink

  // tunnel
  colours["tunnel"]           = new G4Colour(0.545, 0.533, 0.470); // a nice gray
  colours["tunnelfloor"]      = new G4Colour(0.5,   0.5,   0.45);  // another gray
  colours["soil"]             = new G4Colour(0.545, 0.353, 0, 0.4); // brown 
  
  // elements
  colours["beampipe"]         = new G4Colour(0.4,   0.4,   0.4);   // dark gray
  colours["sectorbend"]       = new G4Colour(0,     0.4,   0.8);   // blue
  colours["rectangularbend"]  = new G4Colour(0,     0.4,   0.8);   // blue
  colours["quadrupole"]       = new G4Colour(0.82,  0.1,   0.1);   // red
  colours["sextupole"]        = new G4Colour(1,     0.8,   0);     // yellow
  colours["octupole"]         = new G4Colour(0,     0.6,   0.3);   // green
  colours["decapole"]         = new G4Colour(0.3,   0.2,   0.7);   // purple
  colours["solenoid"]         = new G4Colour(1,     0.549, 0);     // orange
  colours["multipole"]        = new G4Colour(0.466, 0.533, 0.6);   // slate gray
  colours["rfcavity"]         = new G4Colour(0.466, 0.533, 0.6);   // slate gray
  colours["srfcavity"]        = new G4Colour(0.69,  0.769, 0.871); // light steel blue
  colours["collimator"]       = new G4Colour(0.3,   0.4,   0.2);   // dark green
  colours["muspoiler"]        = new G4Colour(0,     0.807, 0.819); // "light blue" / tab blue
  colours["vkicker"]          = new G4Colour(0.73,  0.33,  0.83);  // blue
  colours["hkicker"]          = new G4Colour(0.3,   0.2,   0.7);   // blue
  colours["degrader"]         = new G4Colour(0.625, 0.625, 0.625); // silver
  colours["shield"]           = colours["tunnel"];
  
  // element parts
  colours["coil"]             = new G4Colour(0.722, 0.451, 0.2);   // copper
  colours["LHCcopperskin"]    = new G4Colour(0.722, 0.525, 0.043); // copper
  colours["LHCcoil"]          = new G4Colour(0.9,   0.75,  0);     // gold-ish
  colours["LHCcollar"]        = new G4Colour(0.9,   0.9,   0.9);   // gainsboro
  colours["LHCyoke"]          = new G4Colour(0,     0.5,   1.0);   // LHC blue
  colours["LHCyokered"]       = new G4Colour(*(colours["quadrupole"])); // quadrupole red
  colours["gdml"]             = new G4Colour(0.4,   0.2,   0);     // poo brown
  colours["screenframe"]      = new G4Colour(0.7,   0.7,   0.7, 0.4);   // light gray, semi-transparent

  // general
  colours["white"]   = new G4Colour(G4Colour::White());
  colours["gray"]    = new G4Colour(G4Colour::Gray());
  colours["grey"]    = new G4Colour(G4Colour::Grey());
  colours["black"]   = new G4Colour(G4Colour::Black());
  colours["brown"]   = new G4Colour(G4Colour::Brown());
  colours["red"]     = new G4Colour(G4Colour::Red());
  colours["green"]   = new G4Colour(G4Colour::Green());
  colours["blue"]    = new G4Colour(G4Colour::Blue());
  colours["cyan"]    = new G4Colour(G4Colour::Cyan());
  colours["magenta"] = new G4Colour(G4Colour::Magenta());
  colours["yellow"]  = new G4Colour(G4Colour::Yellow());

#ifdef BDSDEBUG
  // auto-generate the manual coolour table in rst syntax
  G4cout << __METHOD_NAME__ << "Colour Table" << G4endl;
  G4cout << "+-----------------+-----+-----+-----+" << G4endl;
  G4cout << "| Name            |  R  |  G  |  B  |" << G4endl;
  G4cout << "+=================+=====+=====+=====+" << G4endl;
  for (const auto& col : colours)
    {
      int r = (int)(col.second->GetRed() * 255);
      int g = (int)(col.second->GetGreen() * 255);
      int b = (int)(col.second->GetBlue() * 255);
      G4cout << "| " << std::setw(16) << col.first << "| "
	     << std::setw(3) << r << " | "
	     << std::setw(3) << g << " | "
	     << std::setw(3) << b << " |" << G4endl;
      G4cout << "+-----------------+-----+-----+-----+" << G4endl;
    }
#endif
}

G4Colour* BDSColours::GetColour(G4String type)
{
  G4String colourName = type;
  G4bool   canDefine  = false;
  if (type.contains(":"))
    {
      colourName = type.substr(0, type.find(":"));
      canDefine  = true;
      if (colours.find(colourName) != colours.end())
	{
	  G4cerr << "Colour \"" << colourName
		 << "\" is already defined - clashing definitions" << G4endl;
	  exit(1);
	}
    }
    
  auto it = colours.find(colourName);
  if (it != colours.end())
    {// colour must therefore exist
      return it->second;
    }
  else if (it == colours.end() && canDefine)
    {
      G4double r = 240;
      G4double g = 240;
      G4double b = 240; // default rgb is almost white but visible
      G4String rgb = type.substr(type.find(":")+1); // everything after ':'
      std::stringstream ss(rgb);
      ss >> r >> g >> b;
      BDS::EnsureInLimits(r,0,255);
      BDS::EnsureInLimits(g,0,255);
      BDS::EnsureInLimits(b,0,255);
      G4Colour* newColour = new G4Colour(r/255.,g/255.,b/255.);
      colours[colourName] = newColour;
      return newColour;
    }
  else
    {// colour not found
      G4cout << __METHOD_NAME__ << "WARNING: invalid colour" << type << G4endl;
      return colours.at("warning");
    }
}

G4Colour* BDSColours::GetMagnetColour(G4int magnetOrder) const
{
  if (magnetOrder < 1)
    { // too low order
      return colours.at("warning");
    }
  else if (magnetOrder > 5)
    { // high order multipole
      return colours.at("multipole");
    }
  else
    { // specified order
      return colours.at(magnetName.at(magnetOrder));
    }
}
