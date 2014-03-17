 /*
 * GMAD interface
 *
 */

#include "gmad.h"
#include "elementlist.h"
#include "options.h"
#include "parameters.h"
#include "sym_table.h"

#include <cmath>
#include <map>
#include <string>
#include <cstring>

extern struct Parameters params;
extern Options options;

extern int yyparse();

extern FILE *yyin;
extern char* yyfilename;

extern int add_func(const char *name, double (*func)(double));
extern int add_var(const char *name, double val,int is_reserved = 0);

// aux. parser lists - to clear
extern ElementList element_list;
extern std::list<struct Element> tmp_list;
extern std::map<std::string, struct symtab*> symtab_map;

extern void print(std::list<Element> l, int ident);

namespace {
void init()
{
  const int reserved = 1;
  // embedded arithmetical functions
  add_func("sqrt",sqrt);
  add_func("cos",cos);
  add_func("sin",sin);
  add_func("exp",exp);
  add_func("log",log); 
  add_func("tan",tan);
  add_func("asin",asin); 
  add_func("acos",acos);
  add_func("atan",atan);
  add_func("abs",fabs);
 

  add_var("pi",4.0*atan(1),reserved);

  add_var("TeV",1e+3,reserved);
  add_var("GeV",1.0 ,reserved);
  add_var("MeV",1e-3,reserved);
  add_var("KeV",1e-6,reserved);
  add_var("eV" ,1e-9,reserved);

  add_var("MV",1.0,reserved);

  add_var("Tesla",1.0,reserved);

  add_var("m"  ,1.0 ,reserved);
  add_var("cm" ,1e-2,reserved);
  add_var("mm" ,1e-3,reserved);
  add_var("um",1e-6,reserved);
  add_var("mum",1e-6,reserved);
  add_var("nm" ,1e-9,reserved);

  add_var("s"  ,1.0  ,reserved);
  add_var("ms" ,1.e-3,reserved);
  add_var("us",1.e-6,reserved);
  add_var("ns" ,1.e-9,reserved);

  add_var("rad" ,1.0  ,reserved);
  add_var("mrad",1.e-3,reserved);


  add_var("clight",2.99792458e+8,reserved);

  params.flush();

  // Default Values for Options

  options.physicsList = "standard";
  options.particleName = "";
  options.distribType = "";
  options.distribFile = "";

  options.numberToGenerate = 1;
  options.nlinesIgnore = 0;
  options.elossHistoBinWidth = 1.0;
  options.elossHistoTransBinWidth = 0.1;
  options.defaultRangeCut = 7e-4;
  options.ffact = 1.0;
  options.beamEnergy = 0.0;

  options.X0 = 0.0, options.Y0 = 0.0, options.Z0 = 0.0;
  options.Xp0 = 0.0, options.Yp0 = 0.0, options.Zp0 = 0.0;
  options.T0 = 0.0;
  options.sigmaT = 0.0;
  options.betx = 0.0, options.bety = 0.0, options.alfx = 0.0, options.alfy = 0.0, options.emitx = 0.0, options.emity = 0.0;
  options.sigmaX = 0.0, options.sigmaXp = 0.0, options.sigmaY = 0.0, options.sigmaYp = 0.0;
  options.envelopeX = 0.0, options.envelopeXp = 0.0, options.envelopeY = 0.0, options.envelopeYp = 0.0, options.envelopeT = 0.0, options.envelopeE = 0.0;
  options.sigma11 = 0.0,options.sigma12 = 0.0,options.sigma13 = 0.0,options.sigma14 = 0.0,options.sigma15 = 0.0,options.sigma16 = 0.0;
  options.sigma22 = 0.0,options.sigma23 = 0.0,options.sigma24 = 0.0,options.sigma25 = 0.0,options.sigma26 = 0.0;
  options.sigma33 = 0.0,options.sigma34 = 0.0,options.sigma35 = 0.0,options.sigma36 = 0.0;
  options.sigma44 = 0.0,options.sigma45 = 0.0,options.sigma46 = 0.0;
  options.sigma55 = 0.0,options.sigma56 = 0.0;
  options.sigma66 = 0.0;
  options.shellX=0.0, options.shellXp=0.0, options.shellY=0.0, options.shellYp=0.0;
  options.Rmin=0.0, options.Rmax=0.0;
  options.sigmaE=0.0;

  options.doTwiss = 0;
  options.doPlanckScattering=0;
  options.checkOverlaps=0;
  options.numberOfEventsPerNtuple=0;
  options.eventNumberOffset=0;

  options.vacuumPressure = 1e-12;
  options.planckScatterFe = 1.0;

  options.xsize=0.0, options.ysize=0.0;

  options.componentBoxSize = 0.0;
  options.tunnelRadius = 0.0;
  options.beampipeRadius = 0.0;
  options.beampipeThickness = 0.0;

  options.pipeMaterial = "StainlessSteel";
  options.vacMaterial = "Vacuum";
  options.tunnelMaterial = "concrete";
  options.tunnelCavityMaterial = "Air";
  options.soilMaterial = "soil";

  options.includeIronMagFields = 0;

  options.buildTunnel = 0;
  options.buildTunnelFloor = 0;
  options.showTunnel = 0;
  options.tunnelOffsetX = 0;
  options.tunnelOffsetY = 0;
  options.samplerDiameter = 0.0;
  options.tunnelThickness = 0.0;
  options.tunnelSoilThickness = 0.0;
  options.tunnelFloorOffset = 0.0;

  options.geometryBias = 0;
  //Beam loss monitors geometry
  options.blmRad = 0.05;
  options.blmLength = 0.18;

  options.gammaToMuFe = 1;
  options.annihiToMuFe = 1;
  options.eeToHadronsFe = 1;
  options.useEMLPB = 0;
  options.useHadLPB = 0;

  options.sensitiveBeamlineComponents = 1;
  options.sensitiveBeamPipe = 1;
  options.sensitiveBLMs = 1;

  options.LPBFraction = 0.0;

  options.thresholdCutCharged = 0.0;
  options.thresholdCutPhotons = 0.0;

  options.prodCutPhotons=7e-4;
  options.prodCutPhotonsP=7e-4;
  options.prodCutPhotonsA=1;
  options.prodCutElectrons=7e-4;
  options.prodCutElectronsP=7e-4;
  options.prodCutElectronsA=1;
  options.prodCutPositrons=7e-4;
  options.prodCutPositronsP=7e-4;
  options.prodCutPositronsA=1;

  //tracking options
  options.maximumTrackingTime = 0.1;
  options.deltaChord = 0.00001;
  options.chordStepMinimum = 0.000001;
  options.deltaIntersection = 0.00001;
  options.minimumEpsilonStep=0;
  options.maximumEpsilonStep=1e-7;
  options.deltaOneStep = 0.00001;
  options.turnOnCerenkov = 1;
  options.synchRadOn = 0;
  options.decayOn = 1;
  options.synchRescale = 0;
  options.synchTrackPhotons = 0;
  options.synchLowX = 0.0;
  options.synchLowGamE = 0.0;
  options.synchPhotonMultiplicity = 1;
  options.synchMeanFreeFactor = 1;
  options.lengthSafety = 0.000000001;
  options.randomSeed = 0;
  
  options.useTimer = 0;
  options.storeMuonTrajectories = 0;
  options.trajCutGTZ = 0.0;
  options.trajCutLTR = 0.0;
  options.storeNeutronTrajectories = 0;
  options.storeTrajectory = 0;
  options.stopTracks = 0;

  options.fifo = "";
  options.refvolume = "";
  options.refcopyno = 0;
}
}

int gmad_parser(FILE *f)
{
  init();

  yyin=f; 

#ifdef DEBUG
  std::cout << "gmad_parser> beginning to parse file" << std::endl;
#endif

  while(!feof(yyin))
    {
      yyparse();
    }

#ifdef DEBUG
  std::cout << "gmad_parser> finished to parsing file" << std::endl;
#endif

  // clear temporary stuff

#ifdef DEBUG
  std::cout << "gmad_parser> clearing temporary lists" << std::endl;
#endif
  element_list.clear();
  tmp_list.clear();
  std::map<std::string,symtab*>::iterator it;
  for(it=symtab_map.begin();it!=symtab_map.end();++it) {
    delete (*it).second;
  }
  symtab_map.clear();

#ifdef DEBUG
  std::cout << "gmad_parser> finished" << std::endl;
#endif

  fclose(f);

  return 0;
}

int gmad_parser(std::string name)
{
  const int maxfilenamelength = 200;
#ifdef DEBUG
  std::cout << "gmad_parser> opening file" << std::endl;
#endif
  FILE *f = fopen(name.c_str(),"r");

  if(f==NULL) return -1;

  yyfilename = new char[maxfilenamelength];
  strncpy(yyfilename,name.c_str(),maxfilenamelength);

  gmad_parser(f);

  return 0;
}


/** Python interface **/ 
int gmad_parser_c(char *name) 
{
  gmad_parser(std::string(name));
  return 0;
}

int get_nelements() 
{
  return beamline_list.size();
}  

const char* get_name(int i) 
{
  std::list<Element>::iterator it = beamline_list.begin();
  std::advance(it, i);
  return (it->name).c_str();
}

short get_type(int i) 
{
  std::list<Element>::iterator it = beamline_list.begin();
  std::advance(it, i);
  return it->type;
}

double get_length(int i) 
{
  std::list<Element>::iterator it = beamline_list.begin();
  std::advance(it, i);
  return it->l;
}

double get_angle(int i) 
{
  std::list<Element>::iterator it = beamline_list.begin();
  std::advance(it, i);
  return it->angle;  
}
