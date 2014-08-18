/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 

   Modified 22.03.05 by J.C.Carter, Royal Holloway, Univ. of London.
   Added GABs LCBeamGasPlugMat Material
   Added LCLead

   Modified 13.09.07 by S.P.Malton, Royal Holloway, Univ. of London.
   Created maps of materials and elements
   Added Initialise function
   Added Add/GetElement/Material functions
   Cleanup in destructor
   Added ListMaterials function
*/

#include "BDSGlobalConstants.hh" 
#include "BDSExecOptions.hh"
#include "BDSMaterials.hh"
#include "G4NistManager.hh"

#include <list>
#include <map>

#include "parser/element.h"
#include "parser/elementlist.h"

BDSMaterials* BDSMaterials::_instance = 0;

BDSMaterials* BDSMaterials::Instance(){
  if(_instance==0) {
    _instance = new BDSMaterials();
  }
  return _instance;
}

BDSMaterials::BDSMaterials(){
  Initialise();
}

void BDSMaterials::Initialise()
{
  G4String name, symbol;             //a=mass of a mole;
  G4double a, z, density;            //z=mean number of protons;
  //G4int iz, n;                       //iz=number of protons  in an isotope;
                                   // n=number of nucleons in an isotope;


  //G4double abundance;
  G4double fractionmass;
 //G4int ncomponents;
  G4double temperature, pressure;

  //
  // Define elements
  //
#ifdef BDSDEBUG
  G4cout << "BDSMaterials: G4 predefined units: " << G4endl;
  G4cout << "g= " << CLHEP::g << G4endl;
  G4cout << "kg= " << CLHEP::kg << G4endl;
  G4cout << "cm= " << CLHEP::cm << G4endl;
  G4cout << "m= " << CLHEP::m << G4endl;
  G4cout << "mm= " << CLHEP::mm << G4endl;
  G4cout << "um= " << CLHEP::um << G4endl;
  G4cout << "nm= " << CLHEP::nm << G4endl;
  G4cout << "mole= " << CLHEP::mole << G4endl;
  G4cout << "kelvin= " << CLHEP::kelvin << G4endl;
  G4cout << "tesla= " << CLHEP::tesla << G4endl;
#endif
  
  G4Element* tmpElement = new G4Element
    (name="Hydrogen"   , symbol="H" , z=  1., a=   1.00*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Helium"     , symbol="He", z=  2., a=   4.0026*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Beryllium"  , symbol="Be", z=  4., a=   9.0122*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Boron"  , symbol="B", z=  5., a=   10.811*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Carbon"     , symbol="C" , z=  6., a=  12.00*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Nitrogen"   , symbol="N" , z=  7., a=  14.01*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Oxygen"     , symbol="O" , z=  8., a=  16.00*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element("Yttrium", symbol="Y", z=39 , a=88.906*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Aluminium"  , symbol="Al", z= 13., a=  26.98*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement =  new G4Element
    (name="Silicon"    , symbol="Si", z= 14., a=  28.09*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Phosphorous", symbol="P" , z= 15., a=  30.973762*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Sulphur"    , symbol="S" , z= 16., a=  32.066*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Calcium"    , symbol="Ca", z= 20., a=  40.078*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Titanium"   , symbol="Ti", z= 22., a=  47.867*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Zirkonium"   , symbol="Zr", z= 40., a=  91.224*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Rubidium"   , symbol="Rb", z= 37., a=  85.4678*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;


  tmpElement = new G4Element
    (name="Strontium"   , symbol="Sr", z= 38., a=  87.62*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Magnesium"   , symbol="Mg", z= 12., a=  24.3050*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Potassium"   , symbol="K", z= 19., a=  39.0983*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Vanadium"   , symbol="V" , z= 23., a=  50.9415*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Manganese"  , symbol="Mn", z= 25., a=  54.93805*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Iron"       , symbol="Fe", z= 26., a=  55.847*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Cobalt"     , symbol="Co", z= 27., a=  58.93*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Nickel"     , symbol="Ni", z= 28., a=  58.693*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Copper"     , symbol="Cu", z= 29., a=  63.546*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Niobium"    , symbol="Nb", z= 41., a=  92.906*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Samarium"   , symbol="Sm", z= 62., a= 150.4*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Tungsten"   , symbol="W" , z= 74., a= 183.84*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Lead"       , symbol="Pb", z= 82., a= 207.2*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Molybdenum"       , symbol="Mo", z= 42., a= 95.94*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Chromium"       , symbol="Cr", z= 24., a= 51.9961*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Sodium"       , symbol="Na", z= 11., a= 22.98976928*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Zinc"       , symbol="Zn", z= 30., a= 65.38*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Barium"       , symbol="Ba", z= 56., a= 137.327*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Europium"       , symbol="Eu", z= 63., a= 151.964*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  tmpElement = new G4Element
    (name="Uranium"       , symbol="U", z= 92., a=  238.0289*CLHEP::g/CLHEP::mole); elements[symbol] = tmpElement;

  //
  // Define materials
  //

  //default Geant4 temperature = 273.15 K
  //default Geant4 pressure = 1atm

  // solid materials

  G4Material* tmpMaterial = new G4Material
    (name="aluminium"     , density=  2.700*CLHEP::g/CLHEP::cm3, 1, kStateSolid);
  tmpMaterial->AddElement(elements["Al"],1);
  materials[name] = tmpMaterial; 

  tmpMaterial = new G4Material
    (name="vanadium"      , density=  5.800*CLHEP::g/CLHEP::cm3, 1, kStateSolid);
  tmpMaterial->AddElement(elements["V"],1);
  materials[name] = tmpMaterial; 

  tmpMaterial = new G4Material
    (name="tungsten"      , density= 19.3  *CLHEP::g/CLHEP::cm3, 1, kStateSolid);
  tmpMaterial->AddElement(elements["W"],1);
  materials[name] = tmpMaterial; 

  tmpMaterial = new G4Material
    (name="iron"          , density=  7.874 *CLHEP::g/CLHEP::cm3, 1, kStateSolid);
  tmpMaterial->AddElement(elements["Fe"],1);
  materials[name] = tmpMaterial; 

  tmpMaterial = new G4Material
    (name="uranium"          , density=  18.9 *CLHEP::g/CLHEP::cm3, 1, kStateSolid);
  tmpMaterial->AddElement(elements["U"],1);
  materials[name] = tmpMaterial; 

  tmpMaterial = new G4Material
    (name="copper"        , density=  8.96 *CLHEP::g/CLHEP::cm3, 1, kStateSolid);
  tmpMaterial->AddElement(elements["Cu"],1);
  materials[name] = tmpMaterial; 

  tmpMaterial = new G4Material
    (name="weightiron"    , density=  7.87 *CLHEP::g/CLHEP::cm3, 1, kStateSolid);
  tmpMaterial->AddElement(elements["Fe"],1);
  materials[name] = tmpMaterial; 

  tmpMaterial = new G4Material
    (name="titanium"      , density=  4.54 *CLHEP::g/CLHEP::cm3, 1, kStateSolid);
  tmpMaterial->AddElement(elements["Ti"],1);
  materials[name] = tmpMaterial; 

  tmpMaterial = new G4Material
    (name="graphite"      , density=  2.265*CLHEP::g/CLHEP::cm3, 1, kStateSolid);
  tmpMaterial->AddElement(elements["C"],1);
  materials[name] = tmpMaterial; 

  tmpMaterial = new G4Material
    (name="graphitefoam"      , density=  0.61*CLHEP::g/CLHEP::cm3, 1, kStateSolid);
  tmpMaterial->AddElement(elements["C"],1);
  materials[name] = tmpMaterial; 

  tmpMaterial = new G4Material
    (name="lead"          , density= 11.35 *CLHEP::g/CLHEP::cm3, 1, kStateSolid);
  tmpMaterial->AddElement(elements["Pb"],1);
  materials[name] = tmpMaterial; 

  tmpMaterial = new G4Material
    (name="silicon"       , density=  2.33 *CLHEP::g/CLHEP::cm3, 1, kStateSolid);
  tmpMaterial->AddElement(elements["Si"],1);
  materials[name] = tmpMaterial; 

  tmpMaterial = new G4Material
    (name="beryllium"     , density=  1.848*CLHEP::g/CLHEP::cm3, 1, kStateSolid);
  tmpMaterial->AddElement(elements["Be"],1);
  materials[name] = tmpMaterial; 

  tmpMaterial = new G4Material
    (name="smco"          , density=  8.4  *CLHEP::g/CLHEP::cm3, 2, kStateSolid, 300*CLHEP::kelvin);
  tmpMaterial->AddElement(elements["Sm"],fractionmass=0.338);
  tmpMaterial->AddElement(elements["Co"],fractionmass=0.662);
  materials[name] = tmpMaterial; 

  tmpMaterial = new G4Material
    (name="niobium"       , density=  8.57 *CLHEP::g/CLHEP::cm3, 1, kStateSolid, 2*CLHEP::kelvin);
  tmpMaterial->AddElement(elements["Nb"],1);
  materials[name] = tmpMaterial; 

  //Titanium alloy (BDS collimator material SLAC-TN-03-069 p25) deacon 15 Jun 2007
  tmpMaterial = new G4Material
    (name="titaniumalloy" , density=  4.48 *CLHEP::g/CLHEP::cm3, 3, kStateSolid, 300*CLHEP::kelvin);
  tmpMaterial->AddElement(elements["V"],  fractionmass=0.025);
  tmpMaterial->AddElement(elements["Al"], fractionmass=0.03 );
  tmpMaterial->AddElement(elements["Ti"], fractionmass=0.945);
  materials[name] = tmpMaterial; 

  //Carbon Steel (shell of cryomodule). LDeacon 21 Feb 2006
  tmpMaterial = new G4Material
    (name="carbonsteel"   , density=  7.87 *CLHEP::g/CLHEP::cm3, 5, kStateSolid, 100*CLHEP::kelvin);
  tmpMaterial->AddElement(elements["C"], fractionmass=0.0017);
  tmpMaterial->AddElement(elements["Mn"], fractionmass=0.0045);
  tmpMaterial->AddElement(elements["P"], fractionmass=0.0004);
  tmpMaterial->AddElement(elements["S"], fractionmass=0.0005);
  tmpMaterial->AddElement(elements["Fe"], fractionmass=0.9929);
  materials[name] = tmpMaterial; 

  //Stainless Steel 316L
  tmpMaterial = new G4Material
    (name="stainlesssteel"   , density=  8000 *CLHEP::kg/CLHEP::m3, 10, kStateSolid, 295*CLHEP::kelvin);
  tmpMaterial->AddElement(elements["C"], fractionmass=0.0003);
  tmpMaterial->AddElement(elements["Mn"], fractionmass=0.02);
  tmpMaterial->AddElement(elements["Si"], fractionmass=0.0075);
  tmpMaterial->AddElement(elements["P"], fractionmass=0.00045);
  tmpMaterial->AddElement(elements["S"], fractionmass=0.0003);
  tmpMaterial->AddElement(elements["Cr"], fractionmass=0.17);
  tmpMaterial->AddElement(elements["Mo"], fractionmass=0.025);
  tmpMaterial->AddElement(elements["Ni"], fractionmass=0.12);
  tmpMaterial->AddElement(elements["N"], fractionmass=0.001);
  tmpMaterial->AddElement(elements["Fe"], fractionmass=0.65545);
  materials[name] = tmpMaterial; 

  //Kapton polyimide film
  tmpMaterial = new G4Material
    (name="kapton", density=  1.42 *CLHEP::g/CLHEP::cm3, 4, kStateSolid, 295*CLHEP::kelvin);
  tmpMaterial->AddElement(elements["H"], fractionmass=0.026362);
  tmpMaterial->AddElement(elements["C"], fractionmass=0.691133);
  tmpMaterial->AddElement(elements["N"], fractionmass=0.073270);
  tmpMaterial->AddElement(elements["O"], fractionmass=0.209235);
  materials[name] = tmpMaterial; 


  tmpMaterial = new G4Material
    (name="nbti"          , density=  5.6  *CLHEP::g/CLHEP::cm3, 2, kStateSolid, 4*CLHEP::kelvin);
  tmpMaterial->AddElement(elements["Nb"], 1);
  tmpMaterial->AddElement(elements["Ti"], 1);
  materials[name] = tmpMaterial; 

  tmpMaterial = new G4Material
    (name="concrete"      , density=  2.3  *CLHEP::g/CLHEP::cm3, 6, kStateSolid, 300*CLHEP::kelvin);
  tmpMaterial->AddElement(elements["Si"], 0.227915);
  tmpMaterial->AddElement(elements["O"], 0.60541);
  tmpMaterial->AddElement(elements["H"], 0.09972);
  tmpMaterial->AddElement(elements["Ca"], 0.04986);
  tmpMaterial->AddElement(elements["Al"], 0.014245);
  tmpMaterial->AddElement(elements["Fe"], 0.00285);
  materials[name] = tmpMaterial; 

  
  tmpMaterial = new G4Material(name="lhcconcrete", density= 2.42*CLHEP::g/CLHEP::cm3,
                               20, kStateSolid, 300*CLHEP::kelvin);
  tmpMaterial->AddElement(elements["H"], 0.59785345499811  *CLHEP::perCent);
  tmpMaterial->AddElement(elements["C"], 5.59989402848226  *CLHEP::perCent);
  tmpMaterial->AddElement(elements["O"], 49.1111702720319  *CLHEP::perCent);
  tmpMaterial->AddElement(elements["Na"], 0.45137935852357 *CLHEP::perCent);
  tmpMaterial->AddElement(elements["Mg"], 0.66062806777291 *CLHEP::perCent);
  tmpMaterial->AddElement(elements["Al"], 2.05561946276849 *CLHEP::perCent);
  tmpMaterial->AddElement(elements["Si"], 18.7995018924154 *CLHEP::perCent);
  tmpMaterial->AddElement(elements["K"], 0.65365311079793  *CLHEP::perCent);
  tmpMaterial->AddElement(elements["Ca"], 20.0191229406116 *CLHEP::perCent);
  tmpMaterial->AddElement(elements["Fe"], 1.11400027114647 *CLHEP::perCent);
  tmpMaterial->AddElement(elements["P"], 0.04782827639985  *CLHEP::perCent);
  tmpMaterial->AddElement(elements["S"], 0.01195706909996  *CLHEP::perCent);
  tmpMaterial->AddElement(elements["Ti"], 0.3457585814739  *CLHEP::perCent);
  tmpMaterial->AddElement(elements["Mn"], 0.03856154784738 *CLHEP::perCent);
  tmpMaterial->AddElement(elements["Zn"], 0.02401378044242 *CLHEP::perCent);
  tmpMaterial->AddElement(elements["Zr"], 0.00737352594498 *CLHEP::perCent);
  tmpMaterial->AddElement(elements["Ba"], 0.01783596140744 *CLHEP::perCent);
  tmpMaterial->AddElement(elements["Pb"], 0.04623400051985 *CLHEP::perCent);
  tmpMaterial->AddElement(elements["Sr"], 0.39757254757374 *CLHEP::perCent);
  tmpMaterial->AddElement(elements["Eu"], 4.184974185E-05  *CLHEP::perCent);
  materials[name] = tmpMaterial; 

  
  tmpMaterial = new G4Material
    (name="bn5000"      , density=  1.925  *CLHEP::g/CLHEP::cm3, 5, kStateSolid, 300*CLHEP::kelvin);
  tmpMaterial->AddElement(elements["B"], 0.383249242);//0.418535);
  tmpMaterial->AddElement(elements["Ni"], 0.472071387);//0.515535);
  tmpMaterial->AddElement(elements["O"], 0.0366276887);//0.04);
  tmpMaterial->AddElement(elements["Ca"], 0.0228923054);//0.025);
  tmpMaterial->AddElement(elements["Si"], 0.0851593762);//0.093);
  materials[name] = tmpMaterial; 
  G4int natoms;
  //The main component of epoxy resin commonly used to insulate magnet coils.
  tmpMaterial = new G4Material
    (name="aralditef", density = 1.175*CLHEP::g/CLHEP::cm3, 3, kStateSolid, 300*CLHEP::kelvin);
  tmpMaterial->AddElement(elements["C"], natoms=12);
  tmpMaterial->AddElement(elements["H"], natoms=18);
  tmpMaterial->AddElement(elements["O"], natoms=4);
  materials[name] = tmpMaterial;

  //A hardener for the epoxy resin
  tmpMaterial = new G4Material
    (name="hy906", density = 1.225*CLHEP::g/CLHEP::cm3, 3, kStateSolid, 300*CLHEP::kelvin);
  tmpMaterial->AddElement(elements["C"], natoms=10);
  tmpMaterial->AddElement(elements["H"], natoms=5);
  tmpMaterial->AddElement(elements["O"], natoms=3);
  materials[name] = tmpMaterial;

  //An accelerator for epoxy resin
  tmpMaterial = new G4Material
    (name="dy061", density = 1.025*CLHEP::g/CLHEP::cm3, 4, kStateSolid, 300*CLHEP::kelvin);
  tmpMaterial->AddElement(elements["C"], natoms=15);
  tmpMaterial->AddElement(elements["H"], natoms=25 );
  tmpMaterial->AddElement(elements["O"], natoms=1 );
  tmpMaterial->AddElement(elements["N"], natoms=3 );
  materials[name] = tmpMaterial;

  //Material type 3 from CERN 81-05, "The Selection and Properties of Epoxide Resins Used for the Insulation of Magnet Systems in Radiation Environments".
  tmpMaterial = new G4Material
    (name="epoxyresin3", density = 1.20*CLHEP::g/CLHEP::cm3, 3, kStateSolid, 300*CLHEP::kelvin);
  tmpMaterial->AddMaterial(materials["aralditef"],fractionmass=0.497512);
  tmpMaterial->AddMaterial(materials["hy906"],fractionmass=0.497512);
  tmpMaterial->AddMaterial(materials["dy061"],fractionmass=0.004976);
  materials[name] = tmpMaterial;

  //Invar.Temperature 2 kelvin. LDeacon 6th Feburary 2006
  tmpMaterial = new G4Material
    (name="invar"         , density=  8.1  *CLHEP::kg/CLHEP::m3, 2, kStateSolid, 2*CLHEP::kelvin);
  tmpMaterial->AddElement(elements["Ni"], fractionmass=0.35);
  tmpMaterial->AddElement(elements["Fe"], fractionmass=0.65);
  materials[name] = tmpMaterial; 

  // "standard" soil (dry)
  tmpMaterial = new G4Material
    (name="soil"          , density=  1.9  *CLHEP::g/CLHEP::cm3, 4, kStateSolid, 300*CLHEP::kelvin);
  tmpMaterial->AddElement(elements["Si"],0.33377483443708611 );
  tmpMaterial->AddElement(elements["O"], 0.57218543046357617);
  tmpMaterial->AddElement(elements["H"], 0.022516556291390728);
  tmpMaterial->AddElement(elements["Al"], 0.071523178807947022);
  materials[name] = tmpMaterial; 

  //quartz
  tmpMaterial = new G4Material
    (name="quartz", density=  2.655  *CLHEP::g/CLHEP::cm3, 2, kStateSolid, 300*CLHEP::kelvin);
  tmpMaterial->AddElement(elements["Si"],1);
  tmpMaterial->AddElement(elements["O"], 2);
  materials[name] = tmpMaterial; 

  //Clay
  tmpMaterial = new G4Material
    (name="clay", density=  1746  *CLHEP::kg/CLHEP::m3, 4, kStateSolid, 300*CLHEP::kelvin);
  tmpMaterial->AddElement(elements["Al"],1);
  tmpMaterial->AddElement(elements["O"], 9);
  tmpMaterial->AddElement(elements["Si"], 2);
  tmpMaterial->AddElement(elements["H"], 4);
  materials[name] = tmpMaterial; 

  //Calcium carbonate
  tmpMaterial = new G4Material
    (name="calciumCarbonate", density=  1201  *CLHEP::kg/CLHEP::m3, 3, kStateSolid, 300*CLHEP::kelvin);
  tmpMaterial->AddElement(elements["Ca"],1);
  tmpMaterial->AddElement(elements["O"], 3);
  tmpMaterial->AddElement(elements["C"], 1);
  materials[name] = tmpMaterial; 

  // marl
  tmpMaterial = new G4Material
    (name="marl"          , density=1474*CLHEP::kg/CLHEP::m3, 2, kStateSolid, 300*CLHEP::kelvin);
  tmpMaterial->AddMaterial(materials["clay"], 0.5 );
  tmpMaterial->AddMaterial(materials["calciumCarbonate"],0.5);
  materials[name] = tmpMaterial; 

  //clayousMarl
  tmpMaterial = new G4Material
    (name="clayousMarl"          , density=1555*CLHEP::kg/CLHEP::m3, 2, kStateSolid, 300*CLHEP::kelvin);
  tmpMaterial->AddMaterial(materials["clay"], 0.65 );
  tmpMaterial->AddMaterial(materials["calciumCarbonate"],0.35);
  materials[name] = tmpMaterial; 

  //limousMarl
  tmpMaterial = new G4Material
    (name="limousMarl"          , density=1392*CLHEP::kg/CLHEP::m3, 2, kStateSolid, 300*CLHEP::kelvin);
  tmpMaterial->AddMaterial(materials["clay"], 0.35 );
  tmpMaterial->AddMaterial(materials["calciumCarbonate"],0.65);
  materials[name] = tmpMaterial; 

  tmpMaterial = new G4Material
    (name="leadtungstate" , density=  8.27 *CLHEP::g/CLHEP::cm3, 3, kStateSolid);
  tmpMaterial->AddElement(elements["Pb"], 1);
  tmpMaterial->AddElement(elements["W"], 1);
  tmpMaterial->AddElement(elements["O"], 4);
  materials[name] = tmpMaterial; 

  
  tmpMaterial = new G4Material 
    (name="fusedsilica", density=1.032*CLHEP::g/CLHEP::cm3, 2, kStateSolid);
  tmpMaterial->AddElement(elements["O"],2);
  tmpMaterial->AddElement(elements["Si"],1);
  const G4int FusedSilica_NUMENTRIES = 3; //Number of entries in the material properties table
  G4double FusedSilica_RIND[FusedSilica_NUMENTRIES]={1.49,1.49,1.49};
  G4double FusedSilica_AbsLength[FusedSilica_NUMENTRIES]={420.*CLHEP::cm,420.*CLHEP::cm,420.*CLHEP::cm};
  G4double FusedSilica_Energy[FusedSilica_NUMENTRIES] = {2.0*CLHEP::eV,7.0*CLHEP::eV,7.14*CLHEP::eV};
  fsMaterialPropertiesTable= new G4MaterialPropertiesTable();
  fsMaterialPropertiesTable->AddProperty("ABSLENGTH",FusedSilica_Energy,FusedSilica_AbsLength,FusedSilica_NUMENTRIES);
  fsMaterialPropertiesTable->AddProperty("RINDEX",FusedSilica_Energy,FusedSilica_RIND,FusedSilica_NUMENTRIES);
  tmpMaterial->SetMaterialPropertiesTable(fsMaterialPropertiesTable);
  materials[name] = tmpMaterial; 

  //n-bk7
  tmpMaterial = new G4Material 
    (name="n-bk7", density=1.032*CLHEP::g/CLHEP::cm3, 2, kStateSolid);
  tmpMaterial->AddElement(elements["O"],2);
  tmpMaterial->AddElement(elements["Si"],1);
  const G4int N_Bk7_NUMENTRIES = 3; //Number of entries in the material properties table
  G4double N_Bk7_RIND[N_Bk7_NUMENTRIES]={1.51680,1.51680,1.51680};
  G4double N_Bk7_AbsLength[N_Bk7_NUMENTRIES]={420.*CLHEP::cm,420.*CLHEP::cm,420.*CLHEP::cm};
  G4double N_Bk7_Energy[N_Bk7_NUMENTRIES] = {2.0*CLHEP::eV,7.0*CLHEP::eV,7.14*CLHEP::eV};
  G4MaterialPropertiesTable* nbk7MaterialPropertiesTable= new G4MaterialPropertiesTable();
  nbk7MaterialPropertiesTable->AddProperty("ABSLENGTH",N_Bk7_Energy,N_Bk7_AbsLength,N_Bk7_NUMENTRIES);
  nbk7MaterialPropertiesTable->AddProperty("RINDEX",N_Bk7_Energy,N_Bk7_RIND,N_Bk7_NUMENTRIES);
  tmpMaterial->SetMaterialPropertiesTable(nbk7MaterialPropertiesTable);
  materials[name] = tmpMaterial; 
  
  //scintillator materials
  //YAG
  tmpMaterial = new G4Material(name="yag", density=4.56*CLHEP::g/CLHEP::cm3, 3);
  tmpMaterial->AddElement(elements["Y"],3);
  tmpMaterial->AddElement(elements["Al"],5);
  tmpMaterial->AddElement(elements["O"],12);
  G4double birks = 0.08*CLHEP::mm/CLHEP::MeV; 
  tmpMaterial->GetIonisation()->SetBirksConstant(birks);
  G4MaterialPropertiesTable* mpt_YAG = new G4MaterialPropertiesTable();
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
#if G4VERSION_NUMBER < 950
  mpt_YAG->AddProperty("FASTCOMPONENT",PhotonEnergyYAG, scintFastYAG, nEntries);
#else
  mpt_YAG->AddProperty("FASTCOMPONENT",PhotonEnergyYAG, scintFastYAG, nEntries)->SetSpline(true);
#endif
  mpt_YAG->AddProperty("RINDEX",PhotonEnergyYAG, RefractiveIndexYAG, nEntries);
  mpt_YAG->AddConstProperty("SCINTILLATIONYIELD",8000./CLHEP::MeV); //Approximately correct
  mpt_YAG->AddConstProperty("RESOLUTIONSCALE",2.0); //Check this
  mpt_YAG->AddConstProperty("FASTTIMECONSTANT",70.*CLHEP::ns); //Approximately correct
  mpt_YAG->AddConstProperty("YIELDRATIO",1.0);
  tmpMaterial->SetMaterialPropertiesTable(mpt_YAG);
  materials[name] = tmpMaterial;

  //UPS-923A  - see http://www.amcrys-h.com/
  //Define the material properties (copy from NIST table of materials).
  G4Material* polystyrene = G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYSTYRENE",true,true);
  tmpMaterial = new G4Material(name="ups923a",density=polystyrene->GetDensity(),1);
  tmpMaterial->AddMaterial(polystyrene,1);
  tmpMaterial->SetName(name.c_str());
  //Define the optical properties.
  const G4int ups923a_numentries = 67;
  
  G4double ups923a_PhotonEnergy[ups923a_numentries]   = {
    3.35,    3.31,    3.28,    3.26,    3.25,    3.23,    3.23,
    3.22,    3.21,    3.19,    3.18,    3.17,    3.16,    3.15,
    3.14,    3.14,    3.13,    3.11,    3.1,    3.09,    3.09,
    3.08,    3.07,    3.04,    3.02,    3.02,    3.01,    2.99,
    2.98,    2.97,    2.97,    2.95,    2.95,    2.93,    2.93,
    2.92,    2.92,    2.91,    2.89,    2.88,    2.87,    2.86,
    2.85,    2.83,    2.81,    2.8,    2.79,    2.78,    2.76,
    2.74,    2.72,    2.71,    2.68,    2.66,    2.64,    2.62,
    2.61,    2.58,    2.55,    2.53,    2.5,    2.48,    2.46,
    2.44,    2.41,    2.38,    2.35  };      
  
  G4double ups923a_emission[ups923a_numentries]   = {
    0,    0.04,    0.11,    0.2,    0.3,    0.4,    0.52,
    0.62,    0.67,    0.68,    0.67,    0.62,    0.53,    0.48,
    0.44,    0.42,    0.4,    0.41,    0.42,    0.51,    0.46,
    0.57,    0.67,    0.78,    0.91,    0.93,    0.95,    0.96,
    0.94,    0.91,    0.85,    0.76,    0.67,    0.61,    0.57,
    0.55,    0.52,    0.51,    0.52,    0.54,    0.57,    0.58,
    0.6,    0.6,    0.59,    0.58,    0.55,    0.48,    0.42,
    0.37,    0.33,    0.31,    0.29,    0.28,    0.26,    0.24,
    0.2,    0.17,    0.12,    0.09,    0.08,    0.07,
    0.06,    0.04,    0.02,    0.01,    0.01  };
  
  G4MaterialPropertiesTable* ups923a_mt = new G4MaterialPropertiesTable();
  ups923a_mt->AddConstProperty("RESOLUTIONSCALE",2.0); //Check this
  ups923a_mt->AddConstProperty("FASTTIMECONSTANT",3.3*CLHEP::ns);
  ups923a_mt->AddConstProperty("YIELDRATIO",1.0);
  //Birk's constant
  birks = (0.014/1.06)*CLHEP::cm/CLHEP::MeV; 
  tmpMaterial->GetIonisation()->SetBirksConstant(birks);
#if G4VERSION_NUMBER < 950
  ups923a_mt->AddProperty("FASTCOMPONENT",ups923a_PhotonEnergy, ups923a_emission, ups923a_numentries);
#else
  ups923a_mt->AddProperty("FASTCOMPONENT",ups923a_PhotonEnergy, ups923a_emission, ups923a_numentries)->SetSpline(true);
#endif
  ups923a_mt->AddConstProperty("RINDEX", 1.52);
  ups923a_mt->AddConstProperty("ABSLENGTH", 1*CLHEP::m);
  G4double scintYieldAnthracene=14200; //Anthracene yield per 1 CLHEP::MeV
  G4double scintYieldUPS923A=scintYieldAnthracene*0.60;//60% of anthracene
  ups923a_mt->AddConstProperty("SCINTILLATIONYIELD",scintYieldUPS923A/CLHEP::MeV);
  tmpMaterial->SetMaterialPropertiesTable(ups923a_mt);
  //Put into the materials array.
  materials[name]=tmpMaterial;

  //PET (Dacron)
  tmpMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_DACRON",true,true);
  name="pet";
  const G4int Pet_NUMENTRIES = 3; //Number of entries in the material properties table
  G4double Pet_RIND[Pet_NUMENTRIES] = {1.570,1.570,1.570};//Assume constant refractive index.
  G4double Pet_Energy[Pet_NUMENTRIES] = {2.0*CLHEP::eV,7.0*CLHEP::eV,7.14*CLHEP::eV}; //The energies.
  petMaterialPropertiesTable=new G4MaterialPropertiesTable();
  petMaterialPropertiesTable->AddProperty("RINDEX",Pet_Energy, Pet_RIND, Pet_NUMENTRIES);
  tmpMaterial->SetMaterialPropertiesTable(petMaterialPropertiesTable);
  materials[name]=tmpMaterial;

  //Cellulose
  tmpMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_CELLULOSE_CELLOPHANE",true,true);
  name="cellulose";
  const G4int Cellulose_NUMENTRIES = 3; //Number of entries in the material properties table
  G4double Cellulose_RIND[Cellulose_NUMENTRIES] = {1.532,1.532,1.532};//Assume constant refractive index.
  G4double Cellulose_Energy[Cellulose_NUMENTRIES] = {2.0*CLHEP::eV,7.0*CLHEP::eV,7.14*CLHEP::eV}; //The energies.
  celluloseMaterialPropertiesTable=new G4MaterialPropertiesTable();
  celluloseMaterialPropertiesTable->AddProperty("RINDEX",Cellulose_Energy, Cellulose_RIND, Cellulose_NUMENTRIES);
  tmpMaterial->SetMaterialPropertiesTable(celluloseMaterialPropertiesTable);
  materials[name] = tmpMaterial;


  //Polyurethane
  tmpMaterial = new G4Material(name="polyurethane", density=1.05*CLHEP::g/CLHEP::cm3, 4);
  tmpMaterial->AddElement(elements["C"],6);
  tmpMaterial->AddElement(elements["H"],10);
  tmpMaterial->AddElement(elements["N"],2);
  tmpMaterial->AddElement(elements["O"],4);
  materials[name]=tmpMaterial;

  //Gadolinium oxysulphate Gd_2 O_2 S
  G4Material* GOS = G4NistManager::Instance()->FindOrBuildMaterial("G4_GADOLINIUM_OXYSULFIDE",true,true);

  //Ganolinium oxysulphate in a polyurethane elastomer (lanex)
  G4double fill_factor=0.5;
  G4double lanex_density=fill_factor*GOS->GetDensity()+(1-fill_factor)*GetMaterial("polyurethane")->GetDensity();
  G4double gos_fraction_by_mass=fill_factor*GOS->GetDensity()/lanex_density;
  G4double pur_fraction_by_mass=1-gos_fraction_by_mass;
  tmpMaterial = new G4Material(name="lanex", density=lanex_density, 2);
  tmpMaterial->AddMaterial(GOS, gos_fraction_by_mass);
  tmpMaterial->AddMaterial(GetMaterial("polyurethane"), pur_fraction_by_mass);
  G4MaterialPropertiesTable* mptLanex = new G4MaterialPropertiesTable();
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
  materials[name]=tmpMaterial;

  //Ganolinium oxysulphate in a polyurethane elastomer (lanex) - version 2
  tmpMaterial = new G4Material(name="lanex2", density=lanex_density, 2);
  tmpMaterial->AddMaterial(GOS, gos_fraction_by_mass);
  tmpMaterial->AddMaterial(GetMaterial("polyurethane"), pur_fraction_by_mass);
  G4MaterialPropertiesTable* mptLanex2 = new G4MaterialPropertiesTable();
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
  materials[name]=tmpMaterial;

  //gos_lanex - GOS with the bulk optical transport properties of lanex particles suspended in an elastomer but the atomic, density and scintillation properties of GOS
  G4double gos_lanex_density=GOS->GetDensity();
  tmpMaterial = new G4Material(name="gos_lanex", density=gos_lanex_density, 1);
  tmpMaterial->AddMaterial(GOS, 1.0);
  G4MaterialPropertiesTable* mptGOSLanex = new G4MaterialPropertiesTable();
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
  materials[name]=tmpMaterial;

  //Same as gos_lanex but refractive index = 1
  tmpMaterial = new G4Material(name="gos_ri1", density=gos_lanex_density, 1);
  tmpMaterial->AddMaterial(GOS, 1.0);
  G4MaterialPropertiesTable* mptGOSLanexRi1 = new G4MaterialPropertiesTable();
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
  materials[name]=tmpMaterial;

  //pet_lanex - PET with the bulk optical transport properties of lanex particles suspended in an elastomer but the atomic, density and scintillation properties of PET
  G4double pet_lanex_density=GetMaterial("polyurethane")->GetDensity();
  tmpMaterial = new G4Material(name="pet_lanex", density=pet_lanex_density, 1);
  tmpMaterial->AddMaterial(GetMaterial("polyurethane"), 1.0);
  G4MaterialPropertiesTable* mptPETLanex = new G4MaterialPropertiesTable();
  mptPETLanex->AddConstProperty("MIEHG", mieScatteringLengthGOSLanex);
  mptPETLanex->AddConstProperty("MIEHG_FORWARD", gosLanexMiehgForward);
  mptPETLanex->AddConstProperty("MIEHG_BACKWARD", gosLanexMiehgBackward);
  mptPETLanex->AddConstProperty("MIEHG_FORWARD_RATIO", gosLanexMiehgForwardRatio);
  mptPETLanex->AddProperty("RINDEX",energyGOSLanexTab, rindexGOSLanexTab, nentGOSLanex); //Average refractive index of bulk material
  mptPETLanex->AddProperty("ABSLENGTH", energyGOSLanexTab, abslenGOSLanex, nentGOSLanex);
  tmpMaterial->SetMaterialPropertiesTable(mptPETLanex);
  materials[name]=tmpMaterial;


  //Medex (larger grained lanex)
  //  G4double medex_fill_factor=0.5;
  G4double medex_density=fill_factor*GOS->GetDensity()+(1-fill_factor)*GetMaterial("polyurethane")->GetDensity();
  G4double medex_gos_fraction_by_mass=fill_factor*GOS->GetDensity()/medex_density;
  G4double medex_pur_fraction_by_mass=1-medex_gos_fraction_by_mass;
  tmpMaterial = new G4Material(name="medex", density=medex_density, 2);
  tmpMaterial->AddMaterial(GOS, medex_gos_fraction_by_mass);
  tmpMaterial->AddMaterial(GetMaterial("polyurethane"), medex_pur_fraction_by_mass);
  G4MaterialPropertiesTable* mptMedex = new G4MaterialPropertiesTable();
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
  materials[name]=tmpMaterial;

  // liquid materials

  tmpMaterial = new G4Material
    (name="liquidhelium"  , density=  0.12498*CLHEP::g/CLHEP::cm3, 1, kStateLiquid, 4.15*CLHEP::kelvin);
  tmpMaterial->AddElement(elements["He"],1);
  materials[name] = tmpMaterial; 

  tmpMaterial=new G4Material
    (name="water"         , density=  1.00 *CLHEP::g/CLHEP::cm3, 2, kStateLiquid, 300*CLHEP::kelvin);
  tmpMaterial->AddElement(elements["O"], 1);
  tmpMaterial->AddElement(elements["H"], 2);
  materials[name] = tmpMaterial; 

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
  temperature = 300*CLHEP::kelvin;
  pressure = 1.0*CLHEP::atmosphere;
  //Air
  density = (CLHEP::STP_Temperature/temperature) * (pressure/(1.*CLHEP::atmosphere))* 30*CLHEP::g/(22.4e-3*CLHEP::m3) ;
  //  G4cout << "Air: temperature = " << temperature/CLHEP::kelvin << " kelvin, pressure = " << pressure/CLHEP::atmosphere << " atm, density = " << density/(CLHEP::g/CLHEP::m3) << " g/m3" << G4endl;
  tmpMaterial = new G4Material
    (name="air", density, 2, kStateGas, temperature, pressure);
  tmpMaterial->AddElement(elements["O"], fractionmass=0.2);
  tmpMaterial->AddElement(elements["N"], fractionmass=0.8);
  const G4int Air_NUMENTRIES = 3; //Number of entries in the material properties table
  G4double Air_RIND[Air_NUMENTRIES] = {1.000292,1.000292,1.000292};//Source: NPL Tables of Physical & Chemical Constants. Refractive indices at different energies.
  G4double Air_Energy[Air_NUMENTRIES] = {2.0*CLHEP::eV,7.0*CLHEP::eV,7.14*CLHEP::eV}; //The energies.
  airMaterialPropertiesTable=new G4MaterialPropertiesTable();
  airMaterialPropertiesTable->AddProperty("RINDEX",Air_Energy, Air_RIND, Air_NUMENTRIES);
  tmpMaterial->SetMaterialPropertiesTable(airMaterialPropertiesTable);
  materials[name] = tmpMaterial; 

  //Carbon monoxide
  density = (CLHEP::STP_Temperature/temperature) * (pressure/(1.*CLHEP::atmosphere))
    * (12.+16.)*CLHEP::g/(22.4*1.e-3*CLHEP::m3) ;
  tmpMaterial =  new G4Material
    (name="carbonmonoxide", density, 2, kStateGas, temperature, pressure);
  tmpMaterial->AddElement(elements["C"], 1);
  tmpMaterial->AddElement(elements["O"], 1);
  materials[name] = tmpMaterial; 

  //Awake plasma - rubidium at density of 7e14 atoms/cm3 
  //  G4double numberDensity = 7.0e14/CLHEP::cm3;
  a = 85.4678*CLHEP::g/CLHEP::mole;
  density = 1e-7*CLHEP::g/CLHEP::cm3;
  //density = a*numberDensity;
  tmpMaterial =  new G4Material
    (name="awakeplasma", z=37., a, density);
  materials[name] = tmpMaterial; 

  //Carbon monoxide beam pipe gas
  double bp_pressure=0.0133e-9*CLHEP::bar; //10 nTorr pressure
  density = (CLHEP::STP_Temperature/temperature) * (bp_pressure/(1.*CLHEP::atmosphere))
    * (12.+16.)*CLHEP::g/(22.4*1.e-3*CLHEP::m3) ;
  tmpMaterial =  new G4Material
    (name="bp_carbonmonoxide", density, 2, kStateGas, temperature, bp_pressure);
  tmpMaterial->AddElement(elements["C"], 1);
  tmpMaterial->AddElement(elements["O"], 1);
  materials[name] = tmpMaterial; 

  //Nitrogen
  density = (CLHEP::STP_Temperature/temperature) * (pressure/(1.*CLHEP::atmosphere))
    * (14.)*CLHEP::g/(22.4*1.e-3*CLHEP::m3) ;
  tmpMaterial =  new G4Material
    (name="nitrogen", density, 1, kStateGas, temperature, pressure);
  tmpMaterial->AddElement(elements["N"], fractionmass=1.0);
  materials[name] = tmpMaterial; 

  //Vacuum (same composition as residual vacuum in warm sections of LHC).
  pressure=BDSGlobalConstants::Instance()->GetVacuumPressure();
  density = (CLHEP::STP_Temperature/temperature) * (pressure/(1.*CLHEP::atmosphere))  * 29*CLHEP::g/(22.4*1.e-3*CLHEP::m3) ;
#ifdef BDSDEBUG 
  G4cout<< " ***************** defining Vacuum"<<G4endl;
  G4cout<< "pressure="<<pressure/CLHEP::bar<<" bar"<<G4endl;
  G4cout<< "temp="<<temperature/CLHEP::kelvin<<" K"<<G4endl;
  G4cout<< "density="<<density/(CLHEP::g/CLHEP::m3)<<"g/m^3"<<G4endl;
#endif
  tmpMaterial = new G4Material
    (name="vacuum", density, 3, kStateGas, temperature, pressure);
  tmpMaterial->AddElement(elements["H"], fractionmass=0.482);
  tmpMaterial->AddElement(elements["C"], fractionmass=0.221);
  tmpMaterial->AddElement(elements["O"], fractionmass=0.297);
  materials[name] = tmpMaterial; 

  const G4int Vac_NUMENTRIES = 3; //Number of entries in the material properties table
  G4double Vac_RIND[Vac_NUMENTRIES] = {1.000,1.000,1.000};//Assume refractive index = 1 in a vacuum.
  G4double Vac_Energy[Vac_NUMENTRIES] = {2.0*CLHEP::eV,7.0*CLHEP::eV,7.14*CLHEP::eV}; //The energies.
  vacMaterialPropertiesTable=new G4MaterialPropertiesTable();
  vacMaterialPropertiesTable->AddProperty("RINDEX",Vac_Energy, Vac_RIND, Vac_NUMENTRIES);
  tmpMaterial->SetMaterialPropertiesTable(vacMaterialPropertiesTable);

  tmpMaterial = new G4Material
    (name="laservac"      , density, 1, kStateGas, temperature, pressure);
  tmpMaterial->AddMaterial(materials["vacuum"], fractionmass=1.);
  materials[name] = tmpMaterial; 

  //High density carbon monoxide, density chosen such that 1mm length gives ~ one interaction
  density=37.403/10.*CLHEP::g/CLHEP::cm3;  
  tmpMaterial = new G4Material
    (name="beamgasplugmat", density, 2, kStateGas);
   tmpMaterial->AddElement(elements["C"], 1);
  tmpMaterial->AddElement(elements["O"], 1);
  materials[name] = tmpMaterial; 
}

void BDSMaterials::AddMaterial(G4Material* aMaterial, G4String aName)
{
  aName.toLower();
  if(materials.insert(make_pair(aName,aMaterial)).second){
#ifdef BDSDEBUG
    G4cout << "New material : " << aName << " added to material table" << G4endl;
#endif

  }else{
    G4String exceptionString = "Material "+aName+" already exists\n";
    G4Exception(exceptionString.c_str(), "-1", FatalException, "");
  }
}

// add material with default state (kSolidState), temperature (273.15K)
// and pressure (1atm)
void BDSMaterials::AddMaterial(G4String aName, G4double itsZ, G4double itsA, G4double itsDensity)
{
  aName.toLower();
  G4Material* tmpMaterial = new G4Material(aName, itsZ, itsA*CLHEP::g/CLHEP::mole, itsDensity*CLHEP::g/CLHEP::cm3);
  if(materials.insert(make_pair(aName,tmpMaterial)).second){
#ifdef BDSDEBUG
    G4cout << "New material : " << aName << " added to material table" << G4endl;
#endif
  }else{
    G4String exceptionString = "Material "+aName+" already exists\n";
    G4Exception(exceptionString.c_str(), "-1", FatalException, "");
  }
}

template <typename Type> void BDSMaterials::AddMaterial(
G4String aName, G4double itsDensity, G4State itsState,
G4double itsTemp, G4double itsPressure,
std::list<const char*> itsComponents, std::list<Type> itsComponentsFractions)
{
  aName.toLower();
  G4Material* tmpMaterial = new G4Material(aName, itsDensity*CLHEP::g/CLHEP::cm3, 
		(G4int)itsComponents.size(),itsState, itsTemp*CLHEP::kelvin, itsPressure*CLHEP::atmosphere);
  std::list<const char*>::iterator sIter;
  typename std::list<Type>::iterator dIter;
  for(sIter = itsComponents.begin(), dIter = itsComponentsFractions.begin();
      sIter != itsComponents.end();
      sIter++, dIter++)
  {
#ifdef BDSDEBUG
    G4cout << "BDSMaterials::AddMaterial - Adding element: " << (G4String)*sIter << G4endl;
#endif
    if(CheckElement((G4String)*sIter)){
      tmpMaterial->AddElement(GetElement((G4String)*sIter),(*dIter));
    } else tmpMaterial->AddMaterial(GetMaterial((G4String)*sIter),(*dIter));
  }
  if(materials.insert(make_pair(aName,tmpMaterial)).second){
#ifdef BDSDEBUG
    G4cout << "New material : " << aName << " added to material table" << G4endl;
#endif
  }else{
    G4String exceptionString = "Material "+aName+" already exists\n";
    G4Exception(exceptionString.c_str(), "-1", FatalException, "");  
  }
}

void BDSMaterials::AddElement(G4Element* aElement, G4String aSymbol)
{
  if(elements.insert(make_pair(aSymbol,aElement)).second){
#ifdef BDSDEBUG
    G4cout << "New atom : " << aSymbol << G4endl;
#endif
  }else{
    G4String exceptionString = "Atom "+aSymbol+" already exists\n";
    G4Exception(exceptionString.c_str(), "-1", FatalException, "");
  }
}

void BDSMaterials::AddElement(G4String aName, G4String aSymbol, G4double itsZ, G4double itsA)
{
  G4Element* tmpElement = new G4Element(aName, aSymbol, itsZ, itsA*CLHEP::g/CLHEP::mole);
  if(elements.insert(make_pair(aSymbol,tmpElement)).second){
#ifdef BDSDEBUG
    G4cout << "New atom : " << aSymbol << G4endl;
#endif
  }else{
    G4String exceptionString = "Atom "+aSymbol+" already exists\n";
    G4Exception(exceptionString.c_str(), "-1", FatalException, "");
  }
}

G4Material* BDSMaterials::GetMaterial(G4String aMaterial)
{
  G4String cmpStr1 ("G4_");
  G4String cmpStr2 (aMaterial, 3);
#ifdef BDSDEBUG
  G4cout << "BDSMaterials::GetMaterial() - " << cmpStr1 << " " << cmpStr2 << " " << cmpStr1.compareTo(cmpStr2) << G4endl;
#endif
  if (!cmpStr1.compareTo(cmpStr2)){
#ifdef BDSDEBUG
    G4cout << "Using NIST material " << aMaterial << G4endl;
#endif
    return G4NistManager::Instance()->FindOrBuildMaterial(aMaterial, true, true);
  } else {
    aMaterial.toLower();
    std::map<G4String,G4Material*>::iterator iter = materials.find(aMaterial);
    if(iter != materials.end()) return (*iter).second;
    else{
      G4String exceptionString = "BDSMaterials::GetMaterial - Material "+aMaterial+" not known. Aborting.";
      G4Exception(exceptionString.c_str(), "-1", FatalException, "");
      exit(1);
    }
  }
}

G4Element* BDSMaterials::GetElement(G4String aSymbol)
{
  G4String cmpStr1 ("G4_");
  G4String cmpStr2 (aSymbol, 3);
#ifdef BDSDEBUG
  G4cout << cmpStr1 << " " << cmpStr2 << " " << cmpStr1.compareTo(cmpStr2) << G4endl;
#endif
  if (!cmpStr1.compareTo(cmpStr2)){
#ifdef BDSDEBUG
    G4cout << "Using NIST material " << aSymbol << G4endl;
#endif
    return G4NistManager::Instance()->FindOrBuildElement(aSymbol, true);
  } else {
    std::map<G4String,G4Element*>::iterator iter = elements.find(aSymbol);
    if(iter != elements.end()) return (*iter).second;
    else{
      G4String exceptionString="BDSMaterials::GetElement - Element "+aSymbol+" not known. Aborting.";
      G4Exception(exceptionString.c_str(), "-1", FatalException, "");
      exit(1);
    }
  }
}

G4Element* BDSMaterials::GetElement(const char* aSymbol)
{
  return GetElement((G4String)aSymbol);
}

G4bool BDSMaterials::CheckMaterial(G4String aMaterial)
{
  aMaterial.toLower();
  std::map<G4String,G4Material*>::iterator iter = materials.find(aMaterial);
  if(iter != materials.end()) return true;
  else return false;
}

G4bool BDSMaterials::CheckElement(G4String aSymbol)
{
  std::map<G4String,G4Element*>::iterator iter = elements.find(aSymbol);
  if(iter != elements.end()) return true;
  else return false;
}

void BDSMaterials::ListMaterials(){
  G4cout << "Available elements are:" << G4endl;
  G4cout << "Boron  - B" << G4endl;
  G4cout << "Aluminium  - Al" << G4endl;
  G4cout << "Calcium    - Ca" << G4endl;
  G4cout << "Carbon     - C" << G4endl;
  G4cout << "Cobalt     - Co" << G4endl;
  G4cout << "Helium     - He" << G4endl;
  G4cout << "Hydrogen   - H" << G4endl;
  G4cout << "Iron       - Fe" << G4endl;
  G4cout << "Lead       - Pb" << G4endl;
  G4cout << "Manganese  - Mn" << G4endl;
  G4cout << "Niobium    - Nb" << G4endl;
  G4cout << "Nitrogen   - N" << G4endl;
  G4cout << "Oxygen     - O" << G4endl;
  G4cout << "Phosphorus - P" << G4endl;
  G4cout << "Samarium   - Sm" << G4endl;
  G4cout << "Silicon    - Si" << G4endl;
  G4cout << "Sulphur    - S" << G4endl;
  G4cout << "Titanium   - Ti" << G4endl;
  G4cout << "Zirkonium   - Zr" << G4endl;
  G4cout << "Tungsten   - W" << G4endl;
  G4cout << "Vanadium   - V" << G4endl;
  G4cout << "Molybdenum   - Mo" << G4endl;
  G4cout << "Chromium   - Cr" << G4endl;
  G4cout << "Sodium   - Na" << G4endl;
  G4cout << "Barium   - Ba" << G4endl;
  G4cout << "Zinc   - Zn" << G4endl;
  G4cout << "Europium   - Eu" << G4endl;
  G4cout << "***************" << G4endl;
  G4cout << "Available materials are:" << G4endl;
  G4cout << "Air" << G4endl;
  G4cout << "Aluminium" << G4endl;
  G4cout << "AralditeF" << G4endl;
  G4cout << "Beryllium" << G4endl;
  G4cout << "CarbonMonoxide" << G4endl;
  G4cout << "CarbonSteel" << G4endl;
  G4cout << "Concrete" << G4endl;
  G4cout << "LHCConcrete" << G4endl;
  G4cout << "Copper" << G4endl;
  G4cout << "Graphite" << G4endl;
  G4cout << "GraphiteFoam" << G4endl;
  G4cout << "BN5000" << G4endl;
  G4cout << "Invar" << G4endl;
  G4cout << "Iron" << G4endl;
  G4cout << "LaserVac" << G4endl;
  G4cout << "Lead" << G4endl;
  G4cout << "LeadTungstate" << G4endl;
  G4cout << "LiquidHelium" << G4endl;
  G4cout << "NbTi" << G4endl;
  G4cout << "Niobium" << G4endl;
  G4cout << "Silicon" << G4endl;
  G4cout << "SmCo" << G4endl;
  G4cout << "Soil" << G4endl;
  G4cout << "StainlessSteel" << G4endl;
  G4cout << "Titanium" << G4endl;
  G4cout << "TitaniumAlloy" << G4endl;
  G4cout << "Tungsten" << G4endl;
  G4cout << "Vacuum" << G4endl;
  G4cout << "Vanadium" << G4endl;
  G4cout << "Water" << G4endl;
  G4cout << "WeightIron" << G4endl;
  G4cout << "AwakePlasma" << G4endl;
  G4cout << "****************************" << G4endl;
  G4cout << "Available nist materials are:" << G4endl;
  G4String list="all";
  G4NistManager::Instance()->ListMaterials(list);
}

BDSMaterials::~BDSMaterials(){
  std::map<G4String,G4Material*>::iterator mIter;
  for(mIter = materials.begin(); mIter!=materials.end(); mIter++)
    delete (*mIter).second;
  materials.clear();

  std::map<G4String,G4Element*>::iterator eIter;
  for(eIter = elements.begin(); eIter!=elements.end(); eIter++)
    delete (*eIter).second;
  elements.clear();

  delete airMaterialPropertiesTable;
  delete celluloseMaterialPropertiesTable;
  delete fsMaterialPropertiesTable;
  delete petMaterialPropertiesTable;
  delete vacMaterialPropertiesTable;  

  _instance = 0;
}

void PrepareRequiredMaterials()
{
  // This function uses the list from the parser and prepares
  // the necessary materials for this run.
  // Put in a function instead of in full in BDSDetectorConstruction.cc
  
  G4bool verbose = BDSExecOptions::Instance()->GetVerbose();
#ifdef BDSDEBUG
  G4bool debug = true;
#else
  G4bool debug = false;
#endif

  // convert the parsed atom list to list of Geant4 G4Elements
  
  std::list<struct Element>::iterator it;

  if (verbose || debug) G4cout << "parsing the atom list..."<< G4endl;
  for(it = atom_list.begin();it!=atom_list.end();it++)
  {
#ifdef BDSDEBUG
    G4cout << "---->adding Atom, "
           << "name= " << (*it).name << " "
           << "symbol= " << (*it).symbol << " "
           << "Z= " << (*it).Z << " "
           << "A= " << (*it).A << "g/mole "
           << G4endl;
#endif

    BDSMaterials::Instance()->AddElement((*it).name,(*it).symbol,(*it).Z,(*it).A);
  }
  if (verbose || debug) G4cout << "size of atom list: "<< atom_list.size() << G4endl;
  
  // convert the parsed material list to list of Geant4 G4Materials
  if (verbose || debug) G4cout << "parsing the material list..."<< G4endl;
  for(it = material_list.begin();it!=material_list.end();it++)
  {
    if((*it).Z != 0) {
#ifdef BDSDEBUG  
      G4cout << "---->adding Material, "
             << "name= "<< (*it).name << " "
             << "Z= " << (*it).Z << " "
             << "A= " << (*it).A << "g/mole "
             << "density= "<< (*it).density << "g/cm3 "
             << G4endl;
#endif
      BDSMaterials::Instance()->AddMaterial((*it).name,(*it).Z,(*it).A,(*it).density);
    }
    else if((*it).components.size() != 0){

      G4State itsState;
      if ((*it).state=="solid") itsState = kStateSolid;
      else
      if ((*it).state=="liquid") itsState = kStateLiquid;
      else
      if ((*it).state=="gas") itsState = kStateGas;
      else {
	G4cout << "Unknown material state "<< (*it).state 
	       << ", setting it to default (solid)"
	       << G4endl;
	(*it).state="solid";
	itsState = kStateSolid;
      }

      if((*it).componentsWeights.size()==(*it).components.size()) {

#ifdef BDSDEBUG 
        G4cout << "---->adding Material, "
               << "name= "<< (*it).name << " "
               << "density= "<< (*it).density << "g/cm3 "
               << "state= " << (*it).state << " "
               << "T= " << (*it).temper << "K "
               << "P= " << (*it).pressure << "atm "
               << "ncomponents= " << (*it).components.size() << " "
               << G4endl;
#endif

	BDSMaterials::Instance()->AddMaterial((G4String)(*it).name,
				  (G4double)(*it).density,
				  (G4State)itsState,
				  (G4double)(*it).temper,
				  (G4double)(*it).pressure,
				  (std::list<const char*>)(*it).components,
				  (std::list<G4int>)(*it).componentsWeights);
      }
      else if((*it).componentsFractions.size()==(*it).components.size()) {

#ifdef BDSDEBUG 
        G4cout << "---->adding Material, "
        << "name= "<< (*it).name << " "
        << "density= "<< (*it).density << "g/cm3 "
        << "state= " << (*it).state << " "
        << "T= " << (*it).temper << "K "
        << "P= " << (*it).pressure << "atm "
        << "ncomponents= " << (*it).components.size() << " "
        << G4endl;
#endif
        BDSMaterials::Instance()->AddMaterial((*it).name,
				  (*it).density,
				  itsState,
				  (*it).temper,
				  (*it).pressure,
				  (*it).components,
				  (*it).componentsFractions);
      }
      else {
	G4Exception("Badly defined material - number of components is not equal to number of weights or mass fractions!", "-1", FatalErrorInArgument, "");
	exit(1);
      }
    }
    else {
      G4Exception("Badly defined material - need more information!", "-1", FatalErrorInArgument, "");
      exit(1);
    }
  }
  if (verbose || debug) G4cout << "size of material list: "<< material_list.size() << G4endl;

}
