 /*
 * GMAD interface
 *
 */

#include "gmad.h"
#include "sym_table.h"

#include <cmath>
#include <string>
#include <cstring>

using namespace std;

extern struct Parameters params;
extern struct symtab *symtab;
extern int yyparse();

extern FILE *yyin;
extern char* yyfilename;

extern int add_func(const char *name, double (*func)(double));
extern int add_var(const char *name, double val,int is_rserved = 0);

// aux. parser lists - to clear
extern list<struct Element> element_list;
extern list<struct Element> tmp_list;

void init()
{

  symtab = new struct symtab[NSYMS];

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
 

  add_var("pi",4.0*atan(1),_RESERVED);

  add_var("TeV",1e+3,_RESERVED);
  add_var("GeV",1.0 ,_RESERVED);
  add_var("MeV",1e-3,_RESERVED);
  add_var("KeV",1e-6,_RESERVED);
  add_var("eV" ,1e-9,_RESERVED);

  add_var("MV",1.0,_RESERVED);

  add_var("Tesla",1.0,_RESERVED);

  add_var("m"  ,1.0 ,_RESERVED);
  add_var("cm" ,1e-2,_RESERVED);
  add_var("mm" ,1e-3,_RESERVED);
  add_var("um",1e-6,_RESERVED);
  add_var("mum",1e-6,_RESERVED);
  add_var("nm" ,1e-9,_RESERVED);

  add_var("s"  ,1.0  ,_RESERVED);
  add_var("ms" ,1.e-3,_RESERVED);
  add_var("us",1.e-6,_RESERVED);
  add_var("ns" ,1.e-9,_RESERVED);

  add_var("rad" ,1.0  ,_RESERVED);
  add_var("mrad",1.e-3,_RESERVED);


  add_var("clight",2.99792458e+8,_RESERVED);

  params.flush();

  // Default Values for Options (the rest are set to 0)

  options.maximumTrackingTime = 1.0;
  options.vacuumPressure = 1e-12;
  options.planckScatterFe = 1.0;
  options.doPlanckScattering=0;
  options.checkOverlaps=0;
  options.synchPhotonMultiplicity = 1;
  options.synchMeanFreeFactor = 1;
  options.gammaToMuFe = 1;
  options.annihiToMuFe = 1;
  options.eeToHadronsFe = 1;
  options.useEMLPB = 0;
  options.useHadLPB = 0;
  options.LPBFraction = 0;
  options.includeIronMagFields = 0;
  options.buildTunnel = 0;
  options.buildTunnelFloor = 0;
  options.showTunnel = 0;
  options.tunnelOffsetX = 0;
  options.tunnelOffsetY = 0;
  options.tunnelSoilThickness = 0;
  options.tunnelThickness = 0;
  options.geometryBias = false;
  options.ffact = 1.0;
  options.elossHistoBinWidth = 1.0;
  options.elossHistoTransBinWidth = 0.1;
  options.defaultRangeCut = 7e-4;
  options.prodCutPositrons=7e-4;
  options.prodCutElectrons=7e-4;
  options.prodCutPhotons=7e-4;
  options.prodCutPositronsP=7e-4;
  options.prodCutElectronsP=7e-4;
  options.prodCutPhotonsP=7e-4;

  //Beam loss monitors geometry
  options.blmRad = 0.05;
  options.blmLength = 0.18;
  options.sensitiveBeamlineComponents = 1;
  options.sensitiveBeamPipe = 1;
  options.sensitiveBLMs = 1;
  options.turnOnCerenkov = 1;
  options.decayOn = 1;
  //  options.synchRadOn = 0;
  //tracking options
  options.chordStepMinimum = 0.000001;
  options.deltaIntersection = 0.00001;
  options.deltaChord = 0.00001;
  options.deltaOneStep = 0.00001;
  options.minimumEpsilonStep=0;
  options.maximumEpsilonStep=1e-7;
  options.lengthSafety = 0.000000001;

}

int gmad_parser(FILE *f)
{

  init();

  yyin=f; 

#ifdef DEBUG
  cout << "gmad_parser> beginning to parse file" << endl;
#endif

  while(!feof(yyin))
    {
      yyparse();
    }

#ifdef DEBUG
  cout << "gmad_parser> finished to parsing file" << endl;
#endif

  // clear temporary stuff

#ifdef DEBUG
  cout << "gmad_parser> clearing temporary lists" << endl;
#endif
  element_list.clear();
  tmp_list.clear();
  
  delete[] symtab;
  symtab = 0;

#ifdef DEBUG
  cout << "gmad_parser> finished" << endl;
#endif

  fclose(f);

  return 0;
}

int gmad_parser(string name)
{
#ifdef DEBUG
  cout << "gmad_parser> opening file" << endl;
#endif
  FILE *f = fopen(name.c_str(),"r");

  if(f==NULL) return -1;

  yyfilename = new char[MAXFILENAMELENGTH];
  strncpy(yyfilename,name.c_str(),MAXFILENAMELENGTH);

  gmad_parser(f);

  return 0;
}


/** Python interface **/ 
int gmad_parser_c(char *name) 
{
  gmad_parser(name);
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
  return it->name;
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
