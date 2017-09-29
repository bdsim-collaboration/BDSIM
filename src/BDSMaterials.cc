#include "BDSDebug.hh"
#include "BDSMaterials.hh"
#include "BDSParser.hh"
#include "G4Version.hh"
#include "G4NistManager.hh"

#include <iomanip>
#include <list>
#include <map>

BDSMaterials* BDSMaterials::_instance = nullptr;

BDSMaterials* BDSMaterials::Instance()
{
  if(_instance==nullptr)
    {_instance = new BDSMaterials();}
  return _instance;
}

BDSMaterials::BDSMaterials()
{
#ifdef BDSDEBUG
  G4cout << "BDSMaterials: G4 predefined units: " << G4endl;
  G4cout << "g= "      << CLHEP::g << G4endl;
  G4cout << "m= "      << CLHEP::m << G4endl;
  G4cout << "mole= "   << CLHEP::mole << G4endl;
  G4cout << "kelvin= " << CLHEP::kelvin << G4endl;
#endif

  DefineElements();
  DefineMetals();
  DefineSuperconductors();
  DefineNonMetalSolids();
  DefineScintillators();
  DefineLHCComponents();
  DefineLiquids();
  DefineGases();
  DefinePlasmas();
  DefineVacuums();
}

void BDSMaterials::DefineElements()
{
  G4String name, symbol; //a=mass of a mole;
  G4double a, z;         //z=mean number of protons;

  AddElement(name="Hydrogen"   , symbol="H" , z=  1, a=   1.00  );
  AddElement(name="Helium"     , symbol="He", z=  2, a=   4.0026);
  AddElement(name="Beryllium"  , symbol="Be", z=  4, a=   9.0122);
  AddElement(name="Boron"      , symbol="B",  z=  5, a=  10.811 );
  AddElement(name="Carbon"     , symbol="C" , z=  6, a=  12.00  );
  AddElement(name="Nitrogen"   , symbol="N" , z=  7, a=  14.01  );
  AddElement(name="Oxygen"     , symbol="O" , z=  8, a=  16.00  );
  AddElement(name="Sodium"     , symbol="Na", z= 11, a=  22.98976928);
  AddElement(name="Magnesium"  , symbol="Mg", z= 12, a=  24.3050);
  AddElement(name="Aluminium"  , symbol="Al", z= 13, a=  26.98  );
  AddElement(name="Silicon"    , symbol="Si", z= 14, a=  28.09  );
  AddElement(name="Phosphorous", symbol="P" , z= 15, a=  30.973762);
  AddElement(name="Sulphur"    , symbol="S" , z= 16, a=  32.066 );
  AddElement(name="Potassium"  , symbol="K" , z= 19, a=  39.0983);
  AddElement(name="Calcium"    , symbol="Ca", z= 20, a=  40.078 );
  AddElement(name="Titanium"   , symbol="Ti", z= 22, a=  47.867 );
  AddElement(name="Vanadium"   , symbol="V" , z= 23, a=  50.9415);
  AddElement(name="Chromium"   , symbol="Cr", z= 24, a=  51.9961);
  AddElement(name="Manganese"  , symbol="Mn", z= 25, a=  54.93805);
  AddElement(name="Iron"       , symbol="Fe", z= 26, a=  55.847 );
  AddElement(name="Cobalt"     , symbol="Co", z= 27, a=  58.93  );
  AddElement(name="Nickel"     , symbol="Ni", z= 28, a=  58.693 );
  AddElement(name="Copper"     , symbol="Cu", z= 29, a=  63.546 );
  AddElement(name="Zinc"       , symbol="Zn", z= 30, a=  65.38  );
  AddElement(name="Rubidium"   , symbol="Rb", z= 37, a=  85.4678);
  AddElement(name="Strontium"  , symbol="Sr", z= 38, a=  87.62  );
  AddElement(name="Yttrium"    , symbol="Y",  z= 39, a=  88.906 );
  AddElement(name="Zirkonium"  , symbol="Zr", z= 40, a=  91.224 );
  AddElement(name="Niobium"    , symbol="Nb", z= 41, a=  92.906 );
  AddElement(name="Molybdenum" , symbol="Mo", z= 42, a=  95.94  );
  AddElement(name="Barium"     , symbol="Ba", z= 56, a= 137.327 );
  AddElement(name="Samarium"   , symbol="Sm", z= 62, a= 150.4   );
  AddElement(name="Europium"   , symbol="Eu", z= 63, a= 151.964 );
  AddElement(name="Tungsten"   , symbol="W" , z= 74, a= 183.84  );
  AddElement(name="Lead"       , symbol="Pb", z= 82, a= 207.2   );
  AddElement(name="Uranium"    , symbol="U",  z= 92, a= 238.0289);
}

void BDSMaterials::DefineMetals()
{
  G4String name;
  G4double density;

  //default Geant4 temperature = 293.15 K (NTP_Temperature)
  //default Geant4 pressure = 1atm

#if G4VERSION_NUMBER < 1011
  G4double NTP_Temperature = 293.15;
#endif

  // solid materials
  // metals
  std::list<int> singleElement = {1};
  AddMaterial(name="aluminium"     , density=  2.700, kStateSolid, NTP_Temperature, 1, {"Al"}, singleElement);
  AddMaterial(name="beryllium"     , density=  1.848, kStateSolid, NTP_Temperature, 1, {"Be"}, singleElement);
  AddMaterial(name="copper"        , density=  8.96 , kStateSolid, NTP_Temperature, 1, {"Cu"}, singleElement);
  AddMaterial(name="carbon"        , density=  1.88 , kStateSolid, NTP_Temperature, 1, {"C"} , singleElement);
  AddMaterial(name="graphite"      , density=  2.265, kStateSolid, NTP_Temperature, 1, {"C"} , singleElement);
  AddMaterial(name="graphitefoam"  , density=  0.61 , kStateSolid, NTP_Temperature, 1, {"C"} , singleElement);
  AddMaterial(name="iron"          , density=  7.874, kStateSolid, NTP_Temperature, 1, {"Fe"}, singleElement);
  AddMaterial(name="weightiron"    , density=  7.87 , kStateSolid, NTP_Temperature, 1, {"Fe"}, singleElement);
  AddMaterial(name="lead"          , density= 11.35 , kStateSolid, NTP_Temperature, 1, {"Pb"}, singleElement);
  AddMaterial(name="silicon"       , density=  2.33 , kStateSolid, NTP_Temperature, 1, {"Si"}, singleElement);
  AddMaterial(name="titanium"      , density=  4.54 , kStateSolid, NTP_Temperature, 1, {"Ti"}, singleElement);
  AddMaterial(name="tungsten"      , density= 19.3  , kStateSolid, NTP_Temperature, 1, {"W"} , singleElement);
  AddMaterial(name="uranium"       , density= 18.9  , kStateSolid, NTP_Temperature, 1, {"U"} , singleElement);
  AddMaterial(name="vanadium"      , density=  5.800, kStateSolid, NTP_Temperature, 1, {"V"} , singleElement);
  AddMaterial(name="solidhydrogen" , density=  8.96 , kStateSolid, NTP_Temperature, 1, {"H"} , singleElement);
  AddMaterial(name="solidnitrogen" , density=  8.96 , kStateSolid, NTP_Temperature, 1, {"N"} , singleElement);
  AddMaterial(name="solidoxygen"   , density=  8.96 , kStateSolid, NTP_Temperature, 1, {"O"} , singleElement);

  // composites and alloys

  AddMaterial(name="leadtungstate" , density=  8.27 , kStateSolid, NTP_Temperature, 1,
              {"Pb","W" ,"O" }, std::list<int>{1, 1, 4});
  AddMaterial(name="smco"          , density=  8.4  , kStateSolid, 300, 1,
              {"Sm","Co"}     , std::list<double>{0.338, 0.662});

  //Titanium alloy (BDS collimator material SLAC-TN-03-069 p25) deacon 15 Jun 2007
  AddMaterial(name="titaniumalloy" , density=  4.48 , kStateSolid, 300, 1,
              {"V" ,"Al","Ti"}, std::list<double>{0.025, 0.03, 0.945});

  //Carbon Steel (shell of cryomodule). LDeacon 21 Feb 2006
  AddMaterial(name="carbonsteel"   , density=  7.87 , kStateSolid, 100, 1,
              {"C","Mn","P","S","Fe"}, std::list<double>{0.0017, 0.0045, 0.0004, 0.0005, 0.9929});

  // Copper Alloy 17410 (C17410) "Beryllium Copper"
  AddMaterial(name="berylliumcopper", density=8.8 * CLHEP::g/CLHEP::cm3,
	      kStateSolid, 300, 1,
      {"G4_Cu", "G4_Be", "G4_Co", "G4_Al", "G4_Fe", "G4_Ni"},
	      std::list<double>{0.991, 0.0031, 0.00500,
		  0.0004, 0.0003, 0.0002});

  //Stainless Steel 316L
  AddMaterial(name="stainlesssteel", density=  8.000, kStateSolid, 295, 1,
              {"C","Mn","Si","P","S","Cr","Mo","Ni","N","Fe"},
	      std::list<double>{0.0003, 0.02, 0.0075, 0.00045, 0.0003, 0.17, 0.025, 0.12, 0.001, 0.65545});

  // Stainless Steel AISI code 304L (low-carbon) @ 300K
  AddMaterial(name="stainless_steel_304L",
	      density=8.02 * CLHEP::g/CLHEP::cm3,
	      kStateSolid, 300, 1,
      {"G4_Fe", "G4_Cr", "G4_Ni", "G4_Mn", "G4_Si", "G4_P", "G4_S", "G4_C"},
	      std::list<double>{0.67145, 0.185, 0.1125, 0.02,
		  0.01, 0.00045, 0.0003, 0.0003});

  // Stainless Steel AISI code 304L (low-carbon) @ 87K
  AddMaterial(name="stainless_steel_304L_87K",
	      density=8.02 * CLHEP::g/CLHEP::cm3,
	      kStateSolid, 87, 1,
      {"G4_Fe", "G4_Cr", "G4_Ni", "G4_Mn", "G4_Si", "G4_P", "G4_S", "G4_C"},
	      std::list<double>{0.67145, 0.185, 0.1125, 0.02,
		  0.01, 0.00045, 0.0003, 0.0003});

 // Stainless Steel AISI code 316LN
 // (Type 316, low carbon, nitrogen-enhanced) @ 300K
  AddMaterial(name="stainless_steel_316LN",
	      density=8.03 * CLHEP::g/CLHEP::cm3,
	      kStateSolid, 300, 1,
      {"G4_Fe", "G4_Cr", "G4_Ni", "G4_Mo", "G4_Mn", "G4_Si", "G4_Ti", "G4_N",
	  "G4_Nb", "G4_Cu", "G4_Co", "G4_P", "G4_C", "G4_S", "G4_Ta", "G4_B"},
	      std::list<double>{
		0.65093, 0.1700, 0.12000, 0.02500, 0.0200, 0.00750,
		  0.00150, 0.0014, 0.00100, 0.00100, 0.0005, 0.00045,
		  0.00030, 0.0003, 0.00010, 0.00002});

 // Stainless Steel AISI code 316LN
 // (Type 316, low-carbon nitrogen-enhanced) @ 87K
  AddMaterial(name="stainless_steel_316LN_87K",
	      density=8.03 * CLHEP::g/CLHEP::cm3,
	      kStateSolid, 87, 1,
      {"G4_Fe", "G4_Cr", "G4_Ni", "G4_Mo", "G4_Mn", "G4_Si", "G4_Ti", "G4_N",
	  "G4_Nb", "G4_Cu", "G4_Co", "G4_P", "G4_C", "G4_S", "G4_Ta", "G4_B"},
	      std::list<double>{
		0.65093, 0.1700, 0.12000, 0.02500, 0.0200,
		  0.00750, 0.00150, 0.0014, 0.00100, 0.00100,
		  0.0005, 0.00045, 0.00030, 0.0003, 0.00010,
		  0.00002});

  // Mild Steel
  AddMaterial(name="mild_steel", density=  8.000, kStateSolid, 295, 1,
      {"G4_C", "G4_Mn", "G4_Si", "G4_Fe"},
	      std::list<double>{0.002, 0.005, 0.0015, 0.99150});

   // Pure tungsten is not typically used, but instead instead as part of "heavy
   // alloy."  I think "heavy" in the sense that the tungsten makes up almost
   // all of the composition, and tungsten is a very dense metal.
  AddMaterial(name="tungsten_heavy_alloy",
	      density=18.5 * CLHEP::g/CLHEP::cm3,
	      kStateSolid, 87, 1,
      {"G4_W", "G4_Ni", "G4_Fe"},
	      std::list<double>{0.97, 0.02, 0.01});
}

void BDSMaterials::DefineSuperconductors()
{
  G4String name;
  G4double density;
  // niobium at 2K
  AddMaterial(name="niobium"       , density=  8.57 , kStateSolid, 2, 1, {"Nb"}, std::list<int>{1});
  // niobium titanium at 4K
  AddMaterial(name="nbti"          , density=  5.6  , kStateSolid, 4, 1, {"Nb","Ti"}, std::list<int>{1,1});
}

void BDSMaterials::DefineNonMetalSolids()
{
  G4String name;
  G4double density;

  //Boron Nickel (absorber)
  AddMaterial(name="bn5000"        , density=  1.925, kStateSolid, 300, 1, {"B","Ni","O","Ca","Si"}, 
	      std::list<double>{0.383249242, 0.472071387, 0.0366276887, 0.0228923054, 0.0851593762});

  //Calcium carbonate (calcite)
  AddMaterial(name="calciumCarbonate", density=2.711, kStateSolid, 300, 1, {"Ca","O","C"}, std::list<int>{1,3,1});

  //Clay
  AddMaterial(name="clay"          , density=  1.746, kStateSolid, 300, 1, {"Al","O","Si","H"}, std::list<int>{1,9,2,4});

  AddMaterial(name="concrete"      , density=  2.3  , kStateSolid, 300, 1, {"Si","O","H","Ca","Al","Fe"},
	      std::list<double>{0.227915, 0.60541, 0.09972, 0.04986, 0.014245, 0.00285});
  
  G4Material* tmpMaterial = new G4Material
    (name="fusedsilica", density=1.032*CLHEP::g/CLHEP::cm3, 2, kStateSolid);
  tmpMaterial->AddElement(elements["O"],2);
  tmpMaterial->AddElement(elements["Si"],1);
  const G4int FusedSilica_NUMENTRIES = 3; //Number of entries in the material properties table
  G4double FusedSilica_RIND[FusedSilica_NUMENTRIES]={1.49,1.49,1.49};
  G4double FusedSilica_AbsLength[FusedSilica_NUMENTRIES]={420.*CLHEP::cm,420.*CLHEP::cm,420.*CLHEP::cm};
  G4double FusedSilica_Energy[FusedSilica_NUMENTRIES] = {2.0*CLHEP::eV,7.0*CLHEP::eV,7.14*CLHEP::eV};
  G4MaterialPropertiesTable* fsMaterialPropertiesTable = CreatePropertiesTable();
  fsMaterialPropertiesTable->AddProperty("ABSLENGTH",FusedSilica_Energy,FusedSilica_AbsLength,FusedSilica_NUMENTRIES);
  fsMaterialPropertiesTable->AddProperty("RINDEX",FusedSilica_Energy,FusedSilica_RIND,FusedSilica_NUMENTRIES);
  tmpMaterial->SetMaterialPropertiesTable(fsMaterialPropertiesTable);
  AddMaterial(tmpMaterial,name);

  //Perspex.
  AddMaterial(name="perspex", density= 1.18, kStateSolid, 300, 1, {"G4_C","G4_O","G4_H"}, 
	      std::list<double>{0.59984,0.31961,0.08055});

  //Invar.Temperature 2 kelvin. LDeacon 6th Feburary 2006
  AddMaterial(name="invar" , density=  8.1 , kStateSolid, 2, 1, {"Ni","Fe"}, std::list<double>{0.35,0.65});

  //Kapton polyimide film
  AddMaterial(name="kapton", density=  1.42, kStateSolid, 295, 1, {"H","C","N","O"}, 
	      std::list<double>{0.026362,0.691133,0.073270,0.209235});

  //n-bk7
  tmpMaterial = new G4Material 
    (name="n-bk7", density=1.032*CLHEP::g/CLHEP::cm3, 2, kStateSolid);
  tmpMaterial->AddElement(elements["O"],2);
  tmpMaterial->AddElement(elements["Si"],1);
  const G4int N_Bk7_NUMENTRIES = 3; //Number of entries in the material properties table
  G4double N_Bk7_RIND[N_Bk7_NUMENTRIES]={1.51680,1.51680,1.51680};
  G4double N_Bk7_AbsLength[N_Bk7_NUMENTRIES]={420.*CLHEP::cm,420.*CLHEP::cm,420.*CLHEP::cm};
  G4double N_Bk7_Energy[N_Bk7_NUMENTRIES] = {2.0*CLHEP::eV,7.0*CLHEP::eV,7.14*CLHEP::eV};
  G4MaterialPropertiesTable* nbk7MaterialPropertiesTable = CreatePropertiesTable();
  nbk7MaterialPropertiesTable->AddProperty("ABSLENGTH",N_Bk7_Energy,N_Bk7_AbsLength,N_Bk7_NUMENTRIES);
  nbk7MaterialPropertiesTable->AddProperty("RINDEX",N_Bk7_Energy,N_Bk7_RIND,N_Bk7_NUMENTRIES);
  tmpMaterial->SetMaterialPropertiesTable(nbk7MaterialPropertiesTable);
  AddMaterial(tmpMaterial,name);

  //quartz
  AddMaterial(name="quartz", density= 2.655, kStateSolid, 300, 1, {"Si","O"}, std::list<int>{1,2});

  // marl
  AddMaterial(name="marl"  , density= 1.474, kStateSolid, 300, 1, {"clay","calciumCarbonate"},
	      std::list<double>{0.5,0.5});

  //clayousMarl
  AddMaterial(name="clayousMarl", density=1.555, kStateSolid, 300, 1, {"clay","calciumCarbonate"},
	      std::list<double>{0.65,0.35});

  //limousMarl
  AddMaterial("limousMarl"      , density=1.392, kStateSolid, 300, 1, {"clay", "calciumCarbonate"},
	      std::list<double>{0.35,0.65});

  // "standard" soil (dry)
  AddMaterial(name="soil"  , density= 1.9, kStateSolid, 300, 1, {"Si","O","H","Al"},
	      std::list<double>{0.33377483443708611, 0.57218543046357617, 0.022516556291390728, 0.071523178807947022});

  // Epoxy Resin components
  //The main component of epoxy resin commonly used to insulate magnet coils.
  AddMaterial(name="aralditef", density = 1.175, kStateSolid, 300, 1, {"C","H","O"},std::list<int>{12,18,4});

  //A hardener for the epoxy resin
  AddMaterial(name="hy906"    , density = 1.225, kStateSolid, 300, 1, {"C","H","O"},std::list<int>{10,5,3});

  //An accelerator for epoxy resin
  AddMaterial(name="dy061"    , density = 1.025, kStateSolid, 300, 1, {"C","H","O","N"},std::list<int>{15,25,1,3});

  //Material type 3 from CERN 81-05, "The Selection and Properties of Epoxide Resins Used for the Insulation of Magnet Systems in Radiation Environments".
  AddMaterial(name="epoxyresin3", density = 1.20, kStateSolid, 300, 1, {"aralditef","hy906","dy061"},
	      std::list<double>{0.497512,0.497512,0.004976});

  //Cellulose
  tmpMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_CELLULOSE_CELLOPHANE",true,true);
  name="cellulose";
  const G4int Cellulose_NUMENTRIES = 3; //Number of entries in the material properties table
  G4double Cellulose_RIND[Cellulose_NUMENTRIES] = {1.532,1.532,1.532};//Assume constant refractive index.
  G4double Cellulose_Energy[Cellulose_NUMENTRIES] = {2.0*CLHEP::eV,7.0*CLHEP::eV,7.14*CLHEP::eV}; //The energies.
  G4MaterialPropertiesTable* celluloseMaterialPropertiesTable = CreatePropertiesTable();
  celluloseMaterialPropertiesTable->AddProperty("RINDEX",Cellulose_Energy, Cellulose_RIND, Cellulose_NUMENTRIES);
  tmpMaterial->SetMaterialPropertiesTable(celluloseMaterialPropertiesTable);
  AddMaterial(tmpMaterial,name);

  //Polyurethane
  AddMaterial(name="polyurethane", density=1.05, kStateSolid, NTP_Temperature, 1, {"C","H","N","O"}, std::list<int>{6,10,2,4});
}

void BDSMaterials::DefineScintillators()
{
  G4String name;
  G4double density;
  //YAG
  G4Material* tmpMaterial = new G4Material(name="yag", density=4.56*CLHEP::g/CLHEP::cm3, 3);
  tmpMaterial->AddElement(elements["Y"],3);
  tmpMaterial->AddElement(elements["Al"],5);
  tmpMaterial->AddElement(elements["O"],12);
  G4double birks = 0.08*CLHEP::mm/CLHEP::MeV; 
  tmpMaterial->GetIonisation()->SetBirksConstant(birks);
  G4MaterialPropertiesTable* mpt_YAG = CreatePropertiesTable();
  //  const int nEntries3=60;
  const G4int nEntries = 9;
  G4double PhotonEnergyYAG[nEntries];
  G4double dNEntries2=(G4double)nEntries;
  G4double energyMin=1.91*CLHEP::eV;
  G4double energyMax=2.76*CLHEP::eV;
  G4double deltaEnergy=(energyMax-energyMin)/(dNEntries2-1.0);
  G4double energy=energyMin;
  for(G4int i=0; i<nEntries; energy += deltaEnergy, i++){
    PhotonEnergyYAG[i]=energy;
  }
  G4double RefractiveIndexYAG[nEntries] = //Approximately correct, but check for different wavelengths
    { 1.82, 1.82, 1.82, 1.82, 1.82, 1.82, 1.82,
      1.82, 1.82 };
  G4double scintFastYAG[nEntries] = //Approximately correct
    { 0, 0.25, 2.0, 14.0, 13.0, 7.0, 4.0, 2.0, 0.0 };
  //  const G4int nEntries2 = 32;
  /*  G4double PhotonEnergy[nEntries2] =
    { 2.034*CLHEP::eV, 2.068*CLHEP::eV, 2.103*CLHEP::eV, 2.139*CLHEP::eV,
      2.177*CLHEP::eV, 2.216*CLHEP::eV, 2.256*CLHEP::eV, 2.298*CLHEP::eV,
      2.341*CLHEP::eV, 2.386*CLHEP::eV, 2.433*CLHEP::eV, 2.481*CLHEP::eV,
      2.532*CLHEP::eV, 2.585*CLHEP::eV, 2.640*CLHEP::eV, 2.697*CLHEP::eV,
      2.757*CLHEP::eV, 2.820*CLHEP::eV, 2.885*CLHEP::eV, 2.954*CLHEP::eV,
      3.026*CLHEP::eV, 3.102*CLHEP::eV, 3.181*CLHEP::eV, 3.265*CLHEP::eV,
      3.353*CLHEP::eV, 3.446*CLHEP::eV, 3.545*CLHEP::eV, 3.649*CLHEP::eV,
      3.760*CLHEP::eV, 3.877*CLHEP::eV, 4.002*CLHEP::eV, 4.136*CLHEP::eV };
  */
  mpt_YAG->AddProperty("FASTCOMPONENT",PhotonEnergyYAG, scintFastYAG, nEntries)->SetSpline(true);
  mpt_YAG->AddProperty("RINDEX",PhotonEnergyYAG, RefractiveIndexYAG, nEntries);
  mpt_YAG->AddConstProperty("SCINTILLATIONYIELD",8000./CLHEP::MeV); //Approximately correct
  mpt_YAG->AddConstProperty("RESOLUTIONSCALE",2.0); //Check this
  mpt_YAG->AddConstProperty("FASTTIMECONSTANT",70.*CLHEP::ns); //Approximately correct
  mpt_YAG->AddConstProperty("YIELDRATIO",1.0);
  tmpMaterial->SetMaterialPropertiesTable(mpt_YAG);
  AddMaterial(tmpMaterial,name);

 
  //UPS-923A  - see http://www.amcrys-h.com/
  //Define the material properties (copy from NIST table of materials).
  G4NistManager* nistManager = G4NistManager::Instance();
  G4Material* polystyrene = nistManager->FindOrBuildMaterial("G4_POLYSTYRENE",true,true);
  tmpMaterial = new G4Material(name="ups923a",density=polystyrene->GetDensity(),1);
  tmpMaterial->AddMaterial(polystyrene,1);
  tmpMaterial->SetName(name.c_str());
  //Define the optical properties.
  const G4int ups923a_numentries = 67;
  
  G4double ups923a_PhotonEnergy[ups923a_numentries]   = {
    3.35,    3.31,    3.28,    3.26,    3.25,    3.23,    3.23,
    3.22,    3.21,    3.19,    3.18,    3.17,    3.16,    3.15,
    3.14,    3.14,    3.13,    3.11,    3.1,     3.09,    3.09,
    3.08,    3.07,    3.04,    3.02,    3.02,    3.01,    2.99,
    2.98,    2.97,    2.97,    2.95,    2.95,    2.93,    2.93,
    2.92,    2.92,    2.91,    2.89,    2.88,    2.87,    2.86,
    2.85,    2.83,    2.81,    2.8,     2.79,    2.78,    2.76,
    2.74,    2.72,    2.71,    2.68,    2.66,    2.64,    2.62,
    2.61,    2.58,    2.55,    2.53,    2.5,     2.48,    2.46,
    2.44,    2.41,    2.38,    2.35  };      
  
  G4double ups923a_emission[ups923a_numentries]   = {
    0,       0.04,    0.11,    0.2,     0.3,     0.4,     0.52,
    0.62,    0.67,    0.68,    0.67,    0.62,    0.53,    0.48,
    0.44,    0.42,    0.4,     0.41,    0.42,    0.51,    0.46,
    0.57,    0.67,    0.78,    0.91,    0.93,    0.95,    0.96,
    0.94,    0.91,    0.85,    0.76,    0.67,    0.61,    0.57,
    0.55,    0.52,    0.51,    0.52,    0.54,    0.57,    0.58,
    0.6,     0.6,     0.59,    0.58,    0.55,    0.48,    0.42,
    0.37,    0.33,    0.31,    0.29,    0.28,    0.26,    0.24,
    0.2,     0.17,    0.12,    0.09,    0.08,    0.07,
    0.06,    0.04,    0.02,    0.01,    0.01  };
  
  G4MaterialPropertiesTable* ups923a_mt = CreatePropertiesTable();
  ups923a_mt->AddConstProperty("RESOLUTIONSCALE",2.0); //Check this
  ups923a_mt->AddConstProperty("FASTTIMECONSTANT",3.3*CLHEP::ns);
  ups923a_mt->AddConstProperty("YIELDRATIO",1.0);
  //Birk's constant
  birks = (0.014/1.06)*CLHEP::cm/CLHEP::MeV; 
  tmpMaterial->GetIonisation()->SetBirksConstant(birks);
  ups923a_mt->AddProperty("FASTCOMPONENT",ups923a_PhotonEnergy, ups923a_emission, ups923a_numentries)->SetSpline(true);
  ups923a_mt->AddConstProperty("RINDEX", 1.52);
  ups923a_mt->AddConstProperty("ABSLENGTH", 1*CLHEP::m);
  G4double scintYieldAnthracene=14200; //Anthracene yield per 1 CLHEP::MeV
  G4double scintYieldUPS923A=scintYieldAnthracene*0.60;//60% of anthracene
  ups923a_mt->AddConstProperty("SCINTILLATIONYIELD",scintYieldUPS923A/CLHEP::MeV);
  tmpMaterial->SetMaterialPropertiesTable(ups923a_mt);
  //Put into the materials array.
  AddMaterial(tmpMaterial,name);

  //PET 
  G4double pet_density=1.4*CLHEP::g/CLHEP::cm3;
  G4int pet_nelements=3;
  G4State pet_state=kStateSolid;
  name="pet";
  tmpMaterial= new G4Material(name,pet_density,
			      pet_nelements,
			      pet_state
			      );
  tmpMaterial->AddElement(nistManager->FindOrBuildElement("C",true),10);
  tmpMaterial->AddElement(nistManager->FindOrBuildElement("H",true),8);
  tmpMaterial->AddElement(nistManager->FindOrBuildElement("O",true),4);
  const G4int Pet_NUMENTRIES = 3; //Number of entries in the material properties table
  G4double Pet_RIND[Pet_NUMENTRIES] = {1.570,1.570,1.570};//Assume constant refractive index.
  G4double Pet_Energy[Pet_NUMENTRIES] = {2.0*CLHEP::eV,7.0*CLHEP::eV,7.14*CLHEP::eV}; //The energies.
  G4MaterialPropertiesTable* petMaterialPropertiesTable = CreatePropertiesTable();
  petMaterialPropertiesTable->AddProperty("RINDEX",Pet_Energy, Pet_RIND, Pet_NUMENTRIES);
  tmpMaterial->SetMaterialPropertiesTable(petMaterialPropertiesTable);
  AddMaterial(tmpMaterial,name);


  //Opaque PET (Dacron)
  name="pet_opaque";
  tmpMaterial= new G4Material(name,pet_density,
			      pet_nelements,
			      pet_state
			      );
  tmpMaterial->AddElement(nistManager->FindOrBuildElement("C",true),10);
  tmpMaterial->AddElement(nistManager->FindOrBuildElement("H",true),8);
  tmpMaterial->AddElement(nistManager->FindOrBuildElement("O",true),4);
  const G4int Pet_Opaque_NUMENTRIES = 3; //Number of entries in the material properties table
  G4double Pet_Opaque_RIND[Pet_Opaque_NUMENTRIES] = {1.570,1.570,1.570};//Assume constant refractive index.
  G4double Pet_Opaque_Energy[Pet_Opaque_NUMENTRIES] = {2.0*CLHEP::eV,7.0*CLHEP::eV,7.14*CLHEP::eV}; //The energies.
  G4double Pet_Opaque_abslen[]={1*CLHEP::um, 1*CLHEP::um, 1*CLHEP::um};
  G4MaterialPropertiesTable* pet_opaqueMaterialPropertiesTable = CreatePropertiesTable();
  pet_opaqueMaterialPropertiesTable->AddProperty("RINDEX",Pet_Opaque_Energy, Pet_Opaque_RIND, Pet_Opaque_NUMENTRIES);
  pet_opaqueMaterialPropertiesTable->AddProperty("ABSLENGTH",Pet_Opaque_Energy, Pet_Opaque_abslen, Pet_Opaque_NUMENTRIES);
  tmpMaterial->SetMaterialPropertiesTable(pet_opaqueMaterialPropertiesTable);
  materials[name]=tmpMaterial;

  //Gadolinium oxysulphate Gd_2 O_2 S
  G4Material* GOS = nistManager->FindOrBuildMaterial("G4_GADOLINIUM_OXYSULFIDE",true,true);

  //Ganolinium oxysulphate in a polyurethane elastomer (lanex)
  G4double fill_factor=0.5;
  G4double lanex_density=fill_factor*GOS->GetDensity()+(1-fill_factor)*GetMaterial("polyurethane")->GetDensity();
  G4double gos_fraction_by_mass=fill_factor*GOS->GetDensity()/lanex_density;
  G4double pur_fraction_by_mass=1-gos_fraction_by_mass;
  tmpMaterial = new G4Material(name="lanex", density=lanex_density, 2);
  tmpMaterial->AddMaterial(GOS, gos_fraction_by_mass);
  tmpMaterial->AddMaterial(GetMaterial("polyurethane"), pur_fraction_by_mass);
  G4MaterialPropertiesTable* mptLanex = CreatePropertiesTable();
  const G4int nentLanex=2;
  G4double rindex=1.50;//(1.82+1.50)/2.0;
  G4double energytab[]={2.239*CLHEP::eV, 2.241*CLHEP::eV};
  G4double rindextab[]={rindex, rindex};
  G4double emitspec[]={1.0, 1.0};
  G4double abslen[]={7*CLHEP::mm, 7*CLHEP::mm};
  mptLanex->AddProperty("RINDEX",energytab, rindextab, nentLanex); //Average refractive index of bulk material
  mptLanex->AddProperty("ABSLENGTH", energytab, abslen, nentLanex);
  mptLanex->AddProperty("FASTCOMPONENT",energytab, emitspec, nentLanex);
  G4double scintScalingFactor=1;
  mptLanex->AddConstProperty("SCINTILLATIONYIELD",7.8e4/CLHEP::MeV);
  mptLanex->AddConstProperty("RESOLUTIONSCALE",1.0);
  mptLanex->AddConstProperty("FASTTIMECONSTANT", 1.*CLHEP::ns);
  mptLanex->AddConstProperty("MIEHG", 60.3e-3*CLHEP::mm);
  mptLanex->AddConstProperty("MIEHG_FORWARD", 0.91);
  mptLanex->AddConstProperty("MIEHG_BACKWARD", 0.91);
  mptLanex->AddConstProperty("MIEHG_FORWARD_RATIO", 1.0);
  tmpMaterial->SetMaterialPropertiesTable(mptLanex);
  AddMaterial(tmpMaterial,name);

  //Ganolinium oxysulphate in a polyurethane elastomer (lanex) - version 2
  tmpMaterial = new G4Material(name="lanex2", density=lanex_density, 2);
  tmpMaterial->AddMaterial(GOS, gos_fraction_by_mass);
  tmpMaterial->AddMaterial(GetMaterial("polyurethane"), pur_fraction_by_mass);
  G4MaterialPropertiesTable* mptLanex2 = CreatePropertiesTable();
  mptLanex2->AddProperty("RINDEX",energytab, rindextab, nentLanex); //Average refractive index of bulk material
  mptLanex2->AddProperty("ABSLENGTH", energytab, abslen, nentLanex);
  mptLanex2->AddProperty("FASTCOMPONENT",energytab, emitspec, nentLanex);
  mptLanex2->AddConstProperty("SCINTILLATIONYIELD",8.9e4/CLHEP::MeV);
  mptLanex2->AddConstProperty("RESOLUTIONSCALE",1.0);
  mptLanex2->AddConstProperty("FASTTIMECONSTANT", 1.*CLHEP::ns);
  mptLanex2->AddConstProperty("MIEHG", 60.3e-3*CLHEP::mm);
  mptLanex2->AddConstProperty("MIEHG_FORWARD", 0.91);
  mptLanex2->AddConstProperty("MIEHG_BACKWARD", 0.91);
  mptLanex2->AddConstProperty("MIEHG_FORWARD_RATIO", 0.5);
  tmpMaterial->SetMaterialPropertiesTable(mptLanex);
  AddMaterial(tmpMaterial,name);

  //gos_lanex - GOS with the bulk optical transport properties of lanex particles suspended in an elastomer but the atomic, density and scintillation properties of GOS
  G4double gos_lanex_density=GOS->GetDensity();
  tmpMaterial = new G4Material(name="gos_lanex", density=gos_lanex_density, 1);
  tmpMaterial->AddMaterial(GOS, 1.0);
  G4MaterialPropertiesTable* mptGOSLanex = CreatePropertiesTable();
  const G4int nentGOSLanex=2;
  G4double rindexGOSLanex=1.50;
  G4double energyGOSLanexTab[]={2.239*CLHEP::eV, 2.241*CLHEP::eV};
  G4double rindexGOSLanexTab[]={rindexGOSLanex, rindexGOSLanex};
  G4double emitspecGOSLanex[]={1.0, 1.0};
  G4double abslenGOSLanex[]={7*CLHEP::mm, 7*CLHEP::mm};
  G4double mieScatteringLengthGOSLanex=60.3*CLHEP::um;
  G4double gosLanexMiehgForward=0.911;
  G4double gosLanexMiehgBackward=0.911;
  G4double gosLanexMiehgForwardRatio=0.5;
  G4double mieHgTimeConst=1.0*CLHEP::ns;
  mptGOSLanex->AddProperty("FASTCOMPONENT",energyGOSLanexTab, emitspecGOSLanex, nentGOSLanex);
  mptGOSLanex->AddConstProperty("SCINTILLATIONYIELD",8.9e4/CLHEP::MeV); //Intrinisic scintilation yield of GOS
  mptGOSLanex->AddConstProperty("RESOLUTIONSCALE", 1.0);
  mptGOSLanex->AddConstProperty("FASTTIMECONSTANT", mieHgTimeConst);
  mptGOSLanex->AddConstProperty("YIELDRATIO", 1.0);
  mptGOSLanex->AddConstProperty("MIEHG", mieScatteringLengthGOSLanex);
  mptGOSLanex->AddConstProperty("MIEHG_FORWARD", gosLanexMiehgForward);
  mptGOSLanex->AddConstProperty("MIEHG_BACKWARD", gosLanexMiehgBackward);
  mptGOSLanex->AddConstProperty("MIEHG_FORWARD_RATIO", gosLanexMiehgForwardRatio);
  mptGOSLanex->AddProperty("RINDEX",energyGOSLanexTab, rindexGOSLanexTab, nentGOSLanex); //Average refractive index of bulk material
  mptGOSLanex->AddProperty("ABSLENGTH", energyGOSLanexTab, abslenGOSLanex, nentGOSLanex);
  tmpMaterial->SetMaterialPropertiesTable(mptGOSLanex);
  AddMaterial(tmpMaterial,name);

  //Same as gos_lanex but refractive index = 1
  tmpMaterial = new G4Material(name="gos_ri1", density=gos_lanex_density, 1);
  tmpMaterial->AddMaterial(GOS, 1.0);
  G4MaterialPropertiesTable* mptGOSLanexRi1 = CreatePropertiesTable();
  G4double rindexGOSLanexRi1Tab[]={1.0, 1.0};
  mptGOSLanexRi1->AddProperty("FASTCOMPONENT",energyGOSLanexTab, emitspecGOSLanex, nentGOSLanex);
  mptGOSLanexRi1->AddConstProperty("SCINTILLATIONYIELD",8.9e4/CLHEP::MeV); //Intrinisic scintilation yield of GOS
  mptGOSLanexRi1->AddConstProperty("RESOLUTIONSCALE", 1.0);
  mptGOSLanexRi1->AddConstProperty("FASTTIMECONSTANT", mieHgTimeConst);
  mptGOSLanexRi1->AddConstProperty("YIELDRATIO", 1.0);
  mptGOSLanexRi1->AddConstProperty("MIEHG", mieScatteringLengthGOSLanex);
  mptGOSLanexRi1->AddConstProperty("MIEHG_FORWARD", gosLanexMiehgForward);
  mptGOSLanexRi1->AddConstProperty("MIEHG_BACKWARD", gosLanexMiehgBackward);
  mptGOSLanexRi1->AddConstProperty("MIEHG_FORWARD_RATIO", gosLanexMiehgForwardRatio);
  mptGOSLanexRi1->AddProperty("RINDEX",energyGOSLanexTab, rindexGOSLanexRi1Tab, nentGOSLanex); //Average refractive index of bulk material
  mptGOSLanexRi1->AddProperty("ABSLENGTH", energyGOSLanexTab, abslenGOSLanex, nentGOSLanex);
  tmpMaterial->SetMaterialPropertiesTable(mptGOSLanexRi1);
  AddMaterial(tmpMaterial,name);

  //pet_lanex - PET with the bulk optical transport properties of lanex particles suspended in an elastomer but the atomic, density and scintillation properties of PET
  G4double pet_lanex_density=GetMaterial("polyurethane")->GetDensity();
  tmpMaterial = new G4Material(name="pet_lanex", density=pet_lanex_density, 1);
  tmpMaterial->AddMaterial(GetMaterial("polyurethane"), 1.0);
  G4MaterialPropertiesTable* mptPETLanex = CreatePropertiesTable();
  mptPETLanex->AddConstProperty("MIEHG", mieScatteringLengthGOSLanex);
  mptPETLanex->AddConstProperty("MIEHG_FORWARD", gosLanexMiehgForward);
  mptPETLanex->AddConstProperty("MIEHG_BACKWARD", gosLanexMiehgBackward);
  mptPETLanex->AddConstProperty("MIEHG_FORWARD_RATIO", gosLanexMiehgForwardRatio);
  mptPETLanex->AddProperty("RINDEX",energyGOSLanexTab, rindexGOSLanexTab, nentGOSLanex); //Average refractive index of bulk material
  mptPETLanex->AddProperty("ABSLENGTH", energyGOSLanexTab, abslenGOSLanex, nentGOSLanex);
  tmpMaterial->SetMaterialPropertiesTable(mptPETLanex);
  AddMaterial(tmpMaterial,name);


  //Medex (larger grained lanex)
  //  G4double medex_fill_factor=0.5;
  G4double medex_density=fill_factor*GOS->GetDensity()+(1-fill_factor)*GetMaterial("polyurethane")->GetDensity();
  G4double medex_gos_fraction_by_mass=fill_factor*GOS->GetDensity()/medex_density;
  G4double medex_pur_fraction_by_mass=1-medex_gos_fraction_by_mass;
  tmpMaterial = new G4Material(name="medex", density=medex_density, 2);
  tmpMaterial->AddMaterial(GOS, medex_gos_fraction_by_mass);
  tmpMaterial->AddMaterial(GetMaterial("polyurethane"), medex_pur_fraction_by_mass);
  G4MaterialPropertiesTable* mptMedex = CreatePropertiesTable();
  const G4int nentMedex=2;
  //  G4double medexRindex=(1.82+1.50)/2.0;
  //  G4double medexEnergytab[]={2.239*CLHEP::eV, 2.241*CLHEP::eV};
  G4double medexRindextab[]={rindex, rindex};
  G4double medexEmitspec[]={1.0, 1.0};
  G4double medexAbslen[]={7*CLHEP::mm, 7*CLHEP::mm};
  mptMedex->AddProperty("RINDEX",energytab, medexRindextab, nentMedex); //Average refractive index of bulk material
  mptMedex->AddProperty("ABSLENGTH", energytab, medexAbslen, nentMedex);
  mptMedex->AddProperty("FASTCOMPONENT",energytab, medexEmitspec, nentMedex);
  mptMedex->AddConstProperty("SCINTILLATIONYIELD",scintScalingFactor*2.94e4/CLHEP::MeV);
  mptMedex->AddConstProperty("RESOLUTIONSCALE",1.0);
  mptMedex->AddConstProperty("FASTTIMECONSTANT", 1.*CLHEP::ns);
  mptMedex->AddConstProperty("MIEHG", 230e-3*CLHEP::mm);
  mptMedex->AddConstProperty("MIEHG_FORWARD", 0.93);
  mptMedex->AddConstProperty("MIEHG_BACKWARD", 0.93);
  mptMedex->AddConstProperty("MIEHG_FORWARD_RATIO", 1.0);
  tmpMaterial->SetMaterialPropertiesTable(mptMedex);
  AddMaterial(tmpMaterial,name);

  //carbon fiber
  G4double frac_graph=0.5;
  G4double frac_poly=0.5;
  G4Material* graph=GetMaterial("G4_GRAPHITE");
  G4Material* poly=GetMaterial("G4_POLYACRYLONITRILE");
  G4double dens_graph=graph->GetDensity();
  G4double dens_poly=poly->GetDensity();

  G4double dens_cf =frac_graph*dens_graph+frac_poly*dens_poly;
  G4double frac_graph_bw=frac_graph*dens_graph/dens_cf;
  G4double frac_poly_bw=frac_poly*dens_poly/dens_cf;
  tmpMaterial = new G4Material(name="carbonfiber", density=dens_cf, 2);
  tmpMaterial->AddMaterial(graph, frac_graph_bw);
  tmpMaterial->AddMaterial(poly, frac_poly_bw);
  G4MaterialPropertiesTable* mptCarbonfiber = CreatePropertiesTable();
  const G4int nentCarbonfiber=2;
  G4double energytab_cf[]={2.239*CLHEP::eV, 2.241*CLHEP::eV};
  G4double carbonfiberRindextab[]={2.6, 2.6};
  G4double carbonfiberAbslen[]={2.1*CLHEP::um, 2.1*CLHEP::um};
  mptCarbonfiber->AddProperty("RINDEX",energytab_cf, carbonfiberRindextab, nentCarbonfiber); //Average refractive index of bulk material
  mptCarbonfiber->AddProperty("ABSLENGTH", energytab_cf, carbonfiberAbslen, nentCarbonfiber);
  tmpMaterial->SetMaterialPropertiesTable(mptCarbonfiber);
  materials[name]=tmpMaterial;
}

void BDSMaterials::DefineLHCComponents()
{
  G4String name;
  G4double density;
  AddMaterial(name="lhcconcrete"   , density=  2.42 , kStateSolid, 300, 1, 
      {"H","C","O","Na","Mg","Al","Si","K","Ca","Fe","P","S","Ti","Mn","Zn","Zr","Ba","Pb","Sr","Eu"},
	      std::list<double>{0.59785345499811 *CLHEP::perCent,
		  5.59989402848226 *CLHEP::perCent,
		  49.1111702720319 *CLHEP::perCent,
		  0.45137935852357 *CLHEP::perCent,
		  0.66062806777291 *CLHEP::perCent,
		  2.05561946276849 *CLHEP::perCent,
		  18.7995018924154 *CLHEP::perCent,
		  0.65365311079793 *CLHEP::perCent,
		  20.0191229406116 *CLHEP::perCent,
		  1.11400027114647 *CLHEP::perCent,
		  0.04782827639985 *CLHEP::perCent,
		  0.01195706909996 *CLHEP::perCent,
		  0.3457585814739  *CLHEP::perCent,
		  0.03856154784738 *CLHEP::perCent,
		  0.02401378044242 *CLHEP::perCent,
		  0.00737352594498 *CLHEP::perCent,
		  0.01783596140744 *CLHEP::perCent,
		  0.04623400051985 *CLHEP::perCent,
		  0.39757254757374 *CLHEP::perCent,
		  4.184974185E-05  *CLHEP::perCent});

  // LHC Rock
  AddMaterial(name="lhc_rock",
	      density=2*CLHEP::g/CLHEP::cm3,
	      kStateSolid, 300, 1,
      {"G4_H", "G4_Na", "G4_Si", "G4_Fe", "G4_C",
	  "G4_Mg", "G4_K", "G4_O", "G4_Al", "G4_Ca"},
	      std::list<double>{
		0.006, 0.01, 0.2, 0.014, 0.03, 0.005, 0.01, 0.5, 0.03, 0.195});

  // Superconducting components of LHC magnet elements
  // Definitions taken from FLUKA
  
  std::list<int> singleElement = {1};
  // Liquid helium at 1.9K  
  AddMaterial(name="lhe_1.9k", density=0.1472, kStateLiquid, 1.9, 1, {"He"}, singleElement);

  // Niobium @ 87K
  AddMaterial(name="nb_87k"  , density=8.902 , kStateSolid , 87 , 1, {"Nb"}, singleElement);
  
  // Titanium @ 87K
  AddMaterial(name="ti_87k"  , density=4.54  , kStateSolid , 87 , 1, {"Ti"}, singleElement);

  // superconductor NbTi with Ti = 47% by weight
  AddMaterial(name="nbti_87k", density=6.0471, kStateSolid , 87 , 1, {"nb_87k","ti_87k"},
	      std::list<double>{0.53,0.47});

  // copper at 4 Kelvin
  AddMaterial(name="cu_4k", density=8.96, kStateSolid, 4, 1, {"Cu"}, singleElement);
  
  // naked superconductor NbTi wire with Cu/SC volume ratio (>= 4.0 and <4.8)
  AddMaterial(name="nbti.1", density=8.4206, kStateSolid, 4, 1, {"nbti_87k","cu_4k"},
	      std::list<double>{1.0/5.4, 4.4/5.4});
}

void BDSMaterials::DefineLiquids()
{
  G4String name;
  G4double density;
  AddMaterial(name="liquidhelium"  , density=  0.12498, kStateLiquid, 4.15, 1, {"He"}, std::list<int>{1});
  AddMaterial(name="water"         , density=  1.00   , kStateLiquid, 300 , 1, {"O","H"}, std::list<int>{1,2});
  
  //Water for Cherenkov radiation detector
  G4Material* tmpMaterial = new G4Material(name="waterCkov", density=1.*CLHEP::g/CLHEP::cm3, 2);
  tmpMaterial->AddElement(elements["O"],1);
  tmpMaterial->AddElement(elements["H"],2);
  const G4int nEntries = 9;
  G4MaterialPropertiesTable* mpt_waterCkov = CreatePropertiesTable();
  G4double PhotonEnergy[nEntries];
  G4double dNEntries=(G4double)nEntries;
  G4double energyMin=1.*CLHEP::eV;
  G4double energyMax=3.*CLHEP::eV;
  G4double deltaEnergy=(energyMax-energyMin)/(dNEntries-1.0);
  G4double energy=energyMin;
  for(G4int i=0; i<nEntries; energy += deltaEnergy, i++){
    PhotonEnergy[i]=energy;
  }
  G4double RefractiveIndex[nEntries] = { 1.325, 1.325, 1.326, 1.327, 1.328, 1.33, 1.333, 1.336, 1.343};
  mpt_waterCkov->AddProperty("RINDEX",PhotonEnergy, RefractiveIndex,nEntries);
  tmpMaterial->SetMaterialPropertiesTable(mpt_waterCkov);
  AddMaterial(tmpMaterial,name);  
}

void BDSMaterials::DefineGases()
{
  G4String name;
  G4double fractionmass;
  // gaseous materials
  // For air: do we want it at p=1atm or at p inside beampipe?
  // 1st case:
  // tmpMaterial = new G4Material
  //    (name="Air"           , density=  1.205*CLHEP::g/dm3, 2, kStateGas, 300*CLHEP::kelvin);
  // 2nd case:
  // ideal gas: 
  //   1) density * temperature / pressure = constant
  //   2) at T=STP_Temperature, p=1*atmosphere, n=1mole occupies a volume
  //      V=22.4l=22.4e-3*m3 and therefore has density=molar weight/(22.4e-3*m3)
  
  //Room temperature temperature and standard pressure
  G4double temperature = 300*CLHEP::kelvin;
  G4double pressure = 1.0*CLHEP::atmosphere;
  //Air
  G4double density = (CLHEP::STP_Temperature/temperature) * (pressure/(1.*CLHEP::atmosphere))* 30*CLHEP::g/(22.4e-3*CLHEP::m3) ;
  //  G4cout << "Air: temperature = " << temperature/CLHEP::kelvin << " kelvin, pressure = " << pressure/CLHEP::atmosphere << " atm, density = " << density/(CLHEP::g/CLHEP::m3) << " g/m3" << G4endl;
  G4Material* tmpMaterial = new G4Material
    (name="air", density, 2, kStateGas, temperature, pressure);
  tmpMaterial->AddElement(elements["O"], fractionmass=0.2);
  tmpMaterial->AddElement(elements["N"], fractionmass=0.8);
  const G4int Air_NUMENTRIES = 3; //Number of entries in the material properties table
  G4double Air_RIND[Air_NUMENTRIES] = {1.000292,1.000292,1.000292};//Source: NPL Tables of Physical & Chemical Constants. Refractive indices at different energies.
  G4double Air_Energy[Air_NUMENTRIES] = {2.0*CLHEP::eV,7.0*CLHEP::eV,7.14*CLHEP::eV}; //The energies.
  G4MaterialPropertiesTable* airMaterialPropertiesTable = CreatePropertiesTable();
  airMaterialPropertiesTable->AddProperty("RINDEX",Air_Energy, Air_RIND, Air_NUMENTRIES);
  tmpMaterial->SetMaterialPropertiesTable(airMaterialPropertiesTable);
  AddMaterial(tmpMaterial,name);

  //Carbon monoxide
  density = (CLHEP::STP_Temperature/temperature) * (pressure/(1.*CLHEP::atmosphere))
    * (12.+16.)*CLHEP::g/(22.4*1.e-3*CLHEP::m3) ;
  AddMaterial(name="carbonmonoxide", density, kStateGas, temperature, pressure, {"C","O"},
	      std::list<int>{1,1});

  //Carbon monoxide beam pipe gas
  double bp_pressure=0.0133e-9*CLHEP::bar; //10 nTorr pressure
  density = (CLHEP::STP_Temperature/temperature) * (bp_pressure/(1.*CLHEP::atmosphere))
    * (12.+16.)*CLHEP::g/(22.4*1.e-3*CLHEP::m3) ;
  AddMaterial(name="bp_carbonmonoxide", density, kStateGas, temperature, bp_pressure, {"C","O"},
	      std::list<int>{1,1});

  //Nitrogen
  density = (CLHEP::STP_Temperature/temperature) * (pressure/(1.*CLHEP::atmosphere))
    * (14.)*CLHEP::g/(22.4*1.e-3*CLHEP::m3) ;
  AddMaterial(name="nitrogen", density, kStateGas, temperature, pressure, {"N"}, std::list<int>{1});
}

void BDSMaterials::DefinePlasmas()
{
  G4String name;
  G4double z;
  //Awake plasma - rubidium at density of 7e14 atoms/cm3 
  //  G4double numberDensity = 7.0e14/CLHEP::cm3;
  G4double a = 85.4678*CLHEP::g/CLHEP::mole;
  G4double density = 1e-7*CLHEP::g/CLHEP::cm3;
  //density = a*numberDensity;
  G4Material* tmpMaterial = new G4Material(name="awakeplasma", z=37., a, density);
  AddMaterial(tmpMaterial,name);
}

void BDSMaterials::DefineVacuums()
{
  G4String name;
  G4double fractionmass;
  //Default vacuum (same composition as residual vacuum in warm sections of LHC).
  // can be overridden by vacMaterial option
  G4double vacpressure;
  // check if parser is initialised (in case list materials is asked by command line) 
  if (BDSParser::IsInitialised())
    {vacpressure=BDSParser::Instance()->GetOptions().vacuumPressure*CLHEP::bar;}
  else
    {vacpressure=1e-12*CLHEP::bar;}
  G4double temperature = 300*CLHEP::kelvin;
  G4double density = (CLHEP::STP_Temperature/temperature) * (vacpressure/(1.*CLHEP::atmosphere)) * 29*CLHEP::g/(22.4*1.e-3*CLHEP::m3);
#ifdef BDSDEBUG 
  G4cout<< " ***************** defining Vacuum"<<G4endl;
  G4cout<< "pressure="<<vacpressure/CLHEP::bar<<" bar"<<G4endl;
  G4cout<< "temp="<<temperature/CLHEP::kelvin<<" K"<<G4endl;
  G4cout<< "density="<<density/(CLHEP::g/CLHEP::m3)<<"g/m^3"<<G4endl;
#endif
  G4Material* tmpMaterial = new G4Material
    (name="vacuum", density, 3, kStateGas, temperature, vacpressure);
  tmpMaterial->AddElement(elements["H"], fractionmass=0.482);
  tmpMaterial->AddElement(elements["C"], fractionmass=0.221);
  tmpMaterial->AddElement(elements["O"], fractionmass=0.297);
  AddMaterial(tmpMaterial,name);

  const G4int Vac_NUMENTRIES = 3; //Number of entries in the material properties table
  G4double Vac_RIND[Vac_NUMENTRIES] = {1.000,1.000,1.000};//Assume refractive index = 1 in a vacuum.
  G4double Vac_Energy[Vac_NUMENTRIES] = {2.0*CLHEP::eV,7.0*CLHEP::eV,7.14*CLHEP::eV}; //The energies.
  G4MaterialPropertiesTable* vacMaterialPropertiesTable = CreatePropertiesTable();
  vacMaterialPropertiesTable->AddProperty("RINDEX",Vac_Energy, Vac_RIND, Vac_NUMENTRIES);
  tmpMaterial->SetMaterialPropertiesTable(vacMaterialPropertiesTable);

  AddMaterial(name="laservac" , density, kStateGas, temperature, vacpressure, {"vacuum"}, std::list<int>{1});

  //High density carbon monoxide, density chosen such that 1mm length gives ~ one interaction
  density=37.403/10.*CLHEP::g/CLHEP::cm3;
  G4double pressure = 1.0*CLHEP::atmosphere;
  AddMaterial(name="beamgasplugmat", density, kStateGas, temperature, pressure, {"C","O"}, 
	      std::list<int>{1,1});

  // Empty material (space vacuum - real empty material does not exist in Geant4)
  tmpMaterial = G4NistManager::Instance()->FindOrBuildMaterial(name="G4_Galactic");
  tmpMaterial->SetMaterialPropertiesTable(vacMaterialPropertiesTable);
  AddMaterial(tmpMaterial,name);
}

void BDSMaterials::AddMaterial(G4Material* aMaterial, G4String aName)
{
  aName.toLower();
  if(materials.insert(make_pair(aName,aMaterial)).second)
    {
#ifdef BDSDEBUG
      G4cout << "New material : " << aName << " added to material table" << G4endl;
#endif
    }
  else
    {G4cout << __METHOD_NAME__ << "Material \"" << aName << "\" already exists" << G4endl; exit(1);}
}

void BDSMaterials::AddMaterial(G4String aName,
			       G4double itsZ,
			       G4double itsA,
			       G4double itsDensity,
			       G4State  itsState,    //solid,gas
			       G4double itsTemp,     //temperature
			       G4double itsPressure) //pressure
{
  // convention: material name in small letters (to be able to find materials regardless of capitalisation)
  aName.toLower();
  G4Material* tmpMaterial = new G4Material(aName, itsZ, itsA*CLHEP::g/CLHEP::mole, itsDensity*CLHEP::g/CLHEP::cm3, itsState, itsTemp*CLHEP::kelvin, itsPressure*CLHEP::atmosphere);
  AddMaterial(tmpMaterial,aName);
}

template <typename Type>
void BDSMaterials::AddMaterial(G4String aName,
			       G4double itsDensity,
			       G4State  itsState,
			       G4double itsTemp,
			       G4double itsPressure,
			       std::list<G4String> itsComponents,
			       std::list<Type> itsComponentsFractions)
{
  aName.toLower();
  G4Material* tmpMaterial = new G4Material(aName, itsDensity*CLHEP::g/CLHEP::cm3, 
		(G4int)itsComponents.size(),itsState, itsTemp*CLHEP::kelvin, itsPressure*CLHEP::atmosphere);
  std::list<G4String>::iterator sIter;
  typename std::list<Type>::iterator dIter;
  for(sIter = itsComponents.begin(), dIter = itsComponentsFractions.begin();
      sIter != itsComponents.end();
      ++sIter, ++dIter)
  {
#ifdef BDSDEBUG
    G4cout << "BDSMaterials::AddMaterial - Adding element: " << *sIter << G4endl;
#endif
    if(CheckElement(*sIter)){
      tmpMaterial->AddElement(GetElement(*sIter),(*dIter));
    } else tmpMaterial->AddMaterial(GetMaterial(*sIter),(*dIter));
  }
  AddMaterial(tmpMaterial,aName);
}

void BDSMaterials::AddElement(G4Element* aElement, G4String aSymbol)
{
  if(elements.insert(make_pair(aSymbol,aElement)).second)
    {
#ifdef BDSDEBUG
      G4cout << "New atom : " << aSymbol << G4endl;
#endif
    }
  else
    {G4cout << __METHOD_NAME__ << "Atom \"" << aSymbol << "\" already exists" << G4endl; exit(1);}
}

void BDSMaterials::AddElement(G4String aName, G4String aSymbol, G4double itsZ, G4double itsA)
{
  G4Element* tmpElement = new G4Element(aName, aSymbol, itsZ, itsA*CLHEP::g/CLHEP::mole);
  AddElement(tmpElement,aSymbol);
}

G4Material* BDSMaterials::GetMaterial(G4String aMaterial)const
{
  G4String cmpStr1 ("G4_");
  G4String cmpStr2 (aMaterial, 3);

  if (!cmpStr1.compareTo(cmpStr2))
    {
#ifdef BDSDEBUG
      G4cout << "Using NIST material " << aMaterial << G4endl;
#endif
      G4Material* mat = G4NistManager::Instance()->FindOrBuildMaterial(aMaterial, true, true);
      if (mat == nullptr)
        {
          G4cout << __METHOD_NAME__ << "\"" << aMaterial << "\" could not be found by NIST." << G4endl;
          exit(1);
        }
      return mat;
    }
  else
    {
      // find material regardless of capitalisation
      aMaterial.toLower();
      auto iter = materials.find(aMaterial);
      if(iter != materials.end()) return (*iter).second;
      else
	{
	  ListMaterials();
	  G4cout << __METHOD_NAME__ << "\"" << aMaterial << "\" is unknown." << G4endl;
	  exit(1);
	}
    }
}

G4Element* BDSMaterials::GetElement(G4String aSymbol)const
{
  G4String cmpStr1 ("G4_");
  G4String cmpStr2 (aSymbol, 3);
// #ifdef BDSDEBUG
//   G4cout << cmpStr1 << " " << cmpStr2 << " " << cmpStr1.compareTo(cmpStr2) << G4endl;
// #endif
  if (!cmpStr1.compareTo(cmpStr2))
    {
#ifdef BDSDEBUG
      G4cout << "Using NIST material " << aSymbol << G4endl;
#endif
      G4Element* element = G4NistManager::Instance()->FindOrBuildElement(aSymbol, true);
      if (element == nullptr)
        {
          G4cout << __METHOD_NAME__ << "\"" << aSymbol << "\" could not be found by NIST." << G4endl;
          exit(1);
        }
      return element;
    }
  else
    {
      auto iter = elements.find(aSymbol);
      if(iter != elements.end())
	{return (*iter).second;}
      else
	{
	  G4cout << __METHOD_NAME__ << "\"" <<  aSymbol << " unknown." << G4endl;
	  exit(1);
	}
    }
}

G4bool BDSMaterials::CheckMaterial(G4String aMaterial)const
{
  aMaterial.toLower();
  auto iter = materials.find(aMaterial);
  if(iter != materials.end()) return true;
  else return false;
}

G4bool BDSMaterials::CheckElement(G4String aSymbol)const
{
  auto iter = elements.find(aSymbol);
  if(iter != elements.end()) return true;
  else return false;
}

void BDSMaterials::ListMaterials()const
{
  G4cout << "Available elements are:" << G4endl;
  for (auto element : elements) {
    G4cout << std::left << std::setw(12) << element.second->GetName() << " - " << element.second->GetSymbol() << G4endl;
  }
  G4cout << "***************" << G4endl;
  G4cout << "Available materials are:" << G4endl;
  for (auto material : materials) {
    G4cout << material.second->GetName() << G4endl;
  }
  G4cout << "****************************" << G4endl;
  G4cout << "Available NIST materials are:" << G4endl;
  G4NistManager::Instance()->ListMaterials("all");
}

BDSMaterials::~BDSMaterials()
{
  for(auto material : materials)
    {delete material.second;}
  materials.clear();

  for(auto element : elements)
    {delete element.second;}
  elements.clear();
  for(G4MaterialPropertiesTable* table : propertiesTables)
    {delete table;}

  _instance = nullptr;
}

void BDSMaterials::PrepareRequiredMaterials(G4bool verbose)
{
  // This function uses the list from the parser and prepares
  // the necessary materials for this run.
#ifdef BDSDEBUG
  G4bool debug = true;
#else
  G4bool debug = false;
#endif

  // convert the parsed atom list to list of Geant4 G4Elements
  
  if (verbose || debug) G4cout << "parsing the atom list..."<< G4endl;
  for(auto it : BDSParser::Instance()->GetAtoms())
  {
#ifdef BDSDEBUG
    G4cout << "---->adding Atom, ";
    it.print();
#endif

    AddElement(it.name,it.symbol,it.Z,it.A);
  }
  if (verbose || debug) G4cout << "size of atom list: "<< BDSParser::Instance()->GetAtoms().size() << G4endl;
  
  // convert the parsed material list to list of Geant4 G4Materials
  if (verbose || debug) G4cout << "parsing the material list..."<< G4endl;
  for(auto it : BDSParser::Instance()->GetMaterials())
  {
    G4State itsState;
    if      (it.state=="solid")  itsState = kStateSolid;
    else if (it.state=="liquid") itsState = kStateLiquid;
    else if (it.state=="gas")    itsState = kStateGas;
    else {
      G4cout << "Unknown material state "<< it.state 
	     << ", setting it to default (solid)"
	     << G4endl;
      it.state="solid";
      itsState = kStateSolid;
    }

#ifdef BDSDEBUG  
    G4cout << "---->adding Material, ";
    it.print();
#endif

    if(it.Z != 0) {
      AddMaterial(it.name,
		  it.Z,
		  it.A,
		  it.density,
		  itsState,
		  it.temper,
		  it.pressure);
    }
    else if(it.components.size() != 0){
      std::list<G4String> tempComponents;
      for (auto jt : it.components)
	{tempComponents.push_back(G4String(jt));}

      if(it.componentsWeights.size()==it.components.size()) {
	
	AddMaterial(it.name,
		    it.density,
		    itsState,
		    it.temper,
		    it.pressure,
		    tempComponents,
		    it.componentsWeights);
      }
      else if(it.componentsFractions.size()==it.components.size()) {

        AddMaterial(it.name,
		    it.density,
		    itsState,
		    it.temper,
		    it.pressure,
		    tempComponents,
		    it.componentsFractions);
      }
      else
	{
	  G4cout << __METHOD_NAME__
		 << "Badly defined material - number of components is not equal to number of weights or mass fractions!" << G4endl;
	  exit(1);
	}
    }
    else
      {
	G4cout << __METHOD_NAME__ << "Badly defined material - need more information!" << G4endl;
	exit(1);
      }
  }
  if (verbose || debug)
    {G4cout << "size of material list: "<< BDSParser::Instance()->GetMaterials().size() << G4endl;}
}

G4MaterialPropertiesTable* BDSMaterials::CreatePropertiesTable()
{
  G4MaterialPropertiesTable* table = new G4MaterialPropertiesTable();
  propertiesTables.push_back(table);
  return table;
}
