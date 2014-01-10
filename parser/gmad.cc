 /*
 * GMAD interface
 *
 */

#include "gmad.h"
#include "sym_table.h"

#include <cmath>
#include <map>
#include <string>
#include <cstring>

extern struct Parameters params;
extern int yyparse();

extern FILE *yyin;
extern char* yyfilename;

extern int add_func(const char *name, double (*func)(double));
extern int add_var(const char *name, double val,int is_reserved = 0);

// aux. parser lists - to clear
extern ElementList element_list;
extern std::list<struct Element> tmp_list;
extern std::map<std::string, struct symtab*> symtab_map;

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

  // Default Values for Options (the rest are set to 0)

  options.maximumTrackingTime = 0.1;
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
  options.prodCutPositronsA=1;
  options.prodCutElectronsA=1;
  options.prodCutPhotonsA=1;

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

void ElementList::push_back(Element& el) {
  // insert at back of list (insert() instead of push_back() to get iterator for map):
  ElementListIterator it = itsList.insert(end(),el);
  itsMap.insert(std::pair<std::string,ElementListIterator>(el.name,it));
}

int ElementList::size()const {
  return itsList.size();
}

void ElementList::clear() {
  itsList.clear();
  itsMap.clear();
}

ElementList::ElementListIterator ElementList::erase(ElementListIterator it) {

  // find entry in map to erase:
  std::string name = (*it).name;
  if (itsMap.count(name) == 1) {
    itsMap.erase(name);
  }
  else { // more than one entry with same name 
    std::pair<ElementMapIterator,ElementMapIterator> ret = itsMap.equal_range(name);
    for (ElementMapIterator emit = ret.first; emit!=ret.second; ++emit) {
      if ((*emit).second == it) // valid comparison? if not, how to find correct element?
      {
	itsMap.erase(emit);
	break;
      }
    }
  }
  return itsList.erase(it);
}

ElementList::ElementListIterator ElementList::erase(ElementListIterator first, ElementListIterator last) {
  ElementListIterator it=first;
  while (it!=last) {
    // erase one by one
    it = erase(it);
  }
  return it;
}

ElementList::ElementListIterator ElementList::begin() {
  return itsList.begin();
}

ElementList::ElementListIterator ElementList::end() {
  return itsList.end();
}

ElementList::ElementListIterator ElementList::find(std::string name,unsigned int count) {
  if (count==1) {
    ElementMapIterator emit = itsMap.find(name);
    if (emit==itsMap.end()) return itsList.end();
    return (*emit).second;
  } else {
    // if count > 1
    std::pair<ElementMapIterator,ElementMapIterator> ret = itsMap.equal_range(name);
    int i=1;
    for (ElementMapIterator emit = ret.first; emit!=ret.second; ++emit, i++) {
      if (i==count) {
	return (*emit).second;
      }
    }
    return itsList.end();
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
