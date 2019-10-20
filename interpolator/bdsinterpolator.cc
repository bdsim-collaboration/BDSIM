/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2019.

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
#include "BDSIQuery.hh"
#include "BDSException.hh"
#include "BDSExecOptions.hh"
#include "BDSFieldFactory.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldLoader.hh"
#include "BDSFieldMag.hh"
#include "BDSFieldMagInterpolated.hh"
#include "BDSGlobalConstants.hh"
#include "BDSParser.hh"

#include "globals.hh"      // geant4 types / globals
#include "G4Field.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"

#include "parser/query.h"

int main(int argc, char** argv)
{
  /// Print header & program information
  G4cout<<"bdsinterpolator : version @BDSIM_VERSION@"<<G4endl;
  G4cout<<"                  (C) 2001-@CURRENT_YEAR@ Royal Holloway University London"<<G4endl;
  G4cout<<"                  http://www.pp.rhul.ac.uk/bdsim"<<G4endl;
  G4cout<<G4endl;

   /// Initialize executable command line options reader object
  const BDSExecOptions* execOptions = new BDSExecOptions(argc,argv);

  /// Parse lattice file
  auto fileName = execOptions->InputFileName();
  G4cout << __FUNCTION__ << "> Using input file : "<< fileName << G4endl;
  BDSParser::Instance(fileName);

  /// Update options generated by parser with those from executable options.
  BDSParser::Instance()->AmalgamateOptions(execOptions->Options());
  delete execOptions; /// No longer needed.

  /// Force construction of global constants
  BDSGlobalConstants::Instance();

  for (const auto& q : BDSParser::Instance()->GetQuery())
    {
      BDSFieldInfo* recipe = BDSFieldFactory::Instance()->GetDefinition(G4String(q.fieldObject));

      // We don't need to use the full interface of BDSFieldFactory to manufacture a complete
      // geant4 field - we only need the BDSFieldMag* instance.
      BDSFieldMag* field = nullptr;
      try
	{field = BDSFieldLoader::Instance()->LoadMagField(*recipe);}
      catch (const BDSException& e)
	{std::cerr << e.what() << std::endl;} // continue anyway to next one

      if (!field)
	{
	  G4cout << "No field constructed - skipping" << G4endl;
	  continue;
	}
      BDSI::Query(field, q, recipe->FieldType());
    }

  delete BDSFieldFactory::Instance();
  delete BDSFieldLoader::Instance();
  delete BDSGlobalConstants::Instance();
  delete BDSParser::Instance();

  return 0;
}
