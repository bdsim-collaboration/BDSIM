#include "BDSExecOptions.hh"

#include <iomanip>
#include <unistd.h>

#include "globals.hh"

#include "BDSDebug.hh"
#include "BDSMaterials.hh"
#include "BDSOutputFormat.hh"
#include "BDSUtilities.hh"

#include "parser/getEnv.h"

BDSExecOptions* BDSExecOptions::_instance=0;

const BDSExecOptions* BDSExecOptions::Instance(int argc, char **argv){
  if(_instance==0) {
    _instance = new BDSExecOptions(argc, argv);
    return _instance;
  } else {
    G4Exception("BDSExecOptions::Instance is already initialized. Return pointer to singleton with BDSExecOptions::Instance()", "-1", FatalException, "");
    return NULL;
  }
}

const BDSExecOptions* BDSExecOptions::Instance(){
  if(_instance==0) {
    G4Exception("BDSExecOptions::Instance was not initialised. Initialize first with BDSExecOptions::Instance(int argc, char **argv).", "-1", FatalException, "");
    return NULL;
  } else 
    return _instance;
}

BDSExecOptions::BDSExecOptions(int argc, char **argv){
  inputFilename       = "optics.mad";
  visMacroFilename    = "";
  visDebug            = false;
  outputFilename      = "output";
  outputFormat        = BDSOutputFormat::_ASCII;
  outline             = false;
  outlineFilename     = "outline.dat";
  outlineFormat       = "";

  gflash      = false;
  gflashemax  = 10000;
  gflashemin  = 0.1;

  verbose       = false;
  verboseEvent  = false;
  verboseStep   = false;
  verboseEventNumber   = -1;
  batch         = false; 
  
  verboseRunLevel      = 0;
  verboseEventLevel    = 0;
  verboseTrackingLevel = 0;
  verboseSteppingLevel = 0;
  
  circular      = false;
  
  seed              = -1;
  setSeed           = false;
  seedStateFilename = "";
  setSeedState      = false;

  // default is -1 so easy to test
  nGenerate         = -1;

  exportGeometry    = false;
  exportType        = "";
  exportFileName    = "none";

  Parse(argc, argv);
  /// after parsing the absolute path can be reconstructed
  itsBDSIMPATH = GetPath(inputFilename);
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "BDSIMPATH set to: " << itsBDSIMPATH << G4endl;
#endif
}

BDSExecOptions::~BDSExecOptions() {
  _instance = 0;
}

/** <Parse the command line options>
 * @param command line input number of variables
 * @param array of char* of the input parameters
 */
void BDSExecOptions::Parse(int argc, char **argv) {
  static struct option LongOptions[] = {{ "help" , 0, 0, 0 },
					{ "outline", 1, 0, 0 },
					{ "outline_type", 1, 0, 0 },
					{ "verbose", 0, 0, 0 },
					{ "verbose_step", 0, 0, 0 },
					{ "verbose_event", 0, 0, 0 },
					{ "verbose_event_num", 1, 0, 0 },
					{ "verbose_G4run", 1, 0, 0 },
					{ "verbose_G4event", 1, 0, 0 },
					{ "verbose_G4tracking", 1, 0, 0 },
					{ "verbose_G4stepping", 1, 0, 0 },
					{ "file", 1, 0, 0 },
					{ "vis_debug", 0, 0, 0 },
					{ "vis_mac", 1, 0, 0 },
					{ "gflash", 0, 0, 0 },
					{ "gflashemax", 1, 0, 0 },
					{ "gflashemin", 1, 0, 0 },
					{ "output", 1, 0, 0 },
					{ "outfile", 1, 0, 0 },
					{ "batch", 0, 0, 0 },
					{ "materials", 0, 0, 0 },
					{ "circular", 0, 0, 0},
					{ "seed", 1, 0, 0},
					{ "seedstate",1,0,0},
					{ "ngenerate", 1, 0, 0},
					{ "exportgeometryto", 1, 0, 0},
					{ 0, 0, 0, 0 }};
  
  int OptionIndex = 0;
  int c;
 
  for(;;) {
    OptionIndex = 0;
  
    // see e.g. http://linux.die.net/man/3/getopt
    c = getopt_long(argc, argv, "Vv",
		    LongOptions, &OptionIndex );
    
    if ( c == -1 ) // end of options list
      break;
    
    switch (c) {
    case '?': // unrecognised option
      G4cout << "invalid option for command " << argv[0] << G4endl << G4endl << G4endl;
      Usage();
      exit(1);
      break;
    case 0:
      if( !strcmp(LongOptions[OptionIndex].name , "help") ) {
	Usage();
	exit(0);
      }
      if( !strcmp(LongOptions[OptionIndex].name , "batch") ) {
	batch = true;
      }
      if( !strcmp(LongOptions[OptionIndex].name , "verbose") ) {
	verbose = true; 
      }
      if( !strcmp(LongOptions[OptionIndex].name , "verbose_step") ) {
	verboseStep = true; 
	// we shouldn't have verbose steps without verbose events etc.
	verboseEvent = true;
      }
      if( !strcmp(LongOptions[OptionIndex].name , "verbose_event") ) {
	verboseEvent = true; 
      }
      if( !strcmp(LongOptions[OptionIndex].name , "verbose_event_num") ){
	verboseEventNumber = atoi(optarg);
      }
      if( !strcmp(LongOptions[OptionIndex].name , "verbose_G4run") ) {
	verboseRunLevel = atoi(optarg);
      }
      if( !strcmp(LongOptions[OptionIndex].name , "verbose_G4event") ) {
	verboseEventLevel = atoi(optarg);
      }
      if( !strcmp(LongOptions[OptionIndex].name , "verbose_G4tracking") )  {
	verboseTrackingLevel = atoi(optarg);
      }
      if( !strcmp(LongOptions[OptionIndex].name , "verbose_G4stepping") ) {
	verboseSteppingLevel = atoi(optarg);
      }
      if( !strcmp(LongOptions[OptionIndex].name , "output") ) {
	if(!strcmp(optarg,"none") || !strcmp(optarg,"NONE")) outputFormat=BDSOutputFormat::_NONE;
	else if(!strcmp(optarg,"ascii") || !strcmp(optarg,"ASCII")) outputFormat=BDSOutputFormat::_ASCII;
	else if (!strcmp(optarg,"root") || !strcmp(optarg,"ROOT")) outputFormat=BDSOutputFormat::_ROOT;
	else if (!strcmp(optarg,"combined") || !strcmp(optarg,"COMBINED")) outputFormat=BDSOutputFormat::_COMBINED;
	else {
	  G4cerr<<"unknown output format "<<optarg<<G4endl;
	  exit(1);
	}
#ifndef USE_ROOT
	if (outputFormat == BDSOutputFormat::_ROOT || outputFormat == BDSOutputFormat::_COMBINED) {
	  G4cerr << "ERROR outputFormat root, but BDSIM not configured with ROOT support!" << G4endl;
	  G4cerr << "Use ascii instead, or recompile with ROOT!" << G4endl;
	  exit(1);
	}
#endif
      }
      if( !strcmp(LongOptions[OptionIndex].name , "outfile") ) {
	outputFilename=optarg;
      }
      if( !strcmp(LongOptions[OptionIndex].name , "outline") ) {
	outlineFilename = optarg; 
	outline=true;
      }
      if( !strcmp(LongOptions[OptionIndex].name , "outline_type") ) {
	outlineFormat = optarg; 
	outline=true;  // can't have outline type without turning on outline!
      }
      if( !strcmp(LongOptions[OptionIndex].name , "file") ) {
	inputFilename=optarg;
      }
      if( !strcmp(LongOptions[OptionIndex].name , "vis_debug") ) {
	visDebug = true;
      }
      if( !strcmp(LongOptions[OptionIndex].name , "vis_mac") ) {
	visMacroFilename=optarg;
      }
      if( !strcmp(LongOptions[OptionIndex].name , "gflash") ) {
	gflash = true;
      }
      if( !strcmp(LongOptions[OptionIndex].name , "gflashemax") ) {
	gflashemax = atof(optarg);
      }
      if( !strcmp(LongOptions[OptionIndex].name , "gflashemin") ) {
	gflashemin = atof(optarg);
      }
      if( !strcmp(LongOptions[OptionIndex].name, "materials") ) {
	BDSMaterials::ListMaterials();
	exit(0);
      }
      if( !strcmp(LongOptions[OptionIndex].name, "circular")  ) {
	circular = true;
      }
      if( !strcmp(LongOptions[OptionIndex].name, "seed")  ){
	seed = atoi(optarg);
	setSeed = true;
      }
      if( !strcmp(LongOptions[OptionIndex].name, "seedstate") ){
	seedStateFilename = optarg;
	setSeedState = true;
      }
      if( !strcmp(LongOptions[OptionIndex].name, "ngenerate") ){
	nGenerate = atof(optarg);
      }
      if( !strcmp(LongOptions[OptionIndex].name, "exportgeometryto") ){
	std::string fn = optarg;
	if (fn.substr(fn.find_last_of(".") + 1) == "gdml")
	  {
	    exportType    = "gdml";
	    exportFileName = fn;
	  }
	else
	  {
	    // remember if you extend this to do it also in the usage print out
	    G4cerr << __METHOD_NAME__ << "unkonwn geometry format \""
		   << fn.substr(fn.find_last_of(".") + 1) << "\"\n"
		   << "Please specify a valid filename extension - options are: \"gdml\"" << G4endl;
	    exit(1);
	  }
	exportGeometry = true;
      }
      break;
      
    default:
      break;
    }
  } 
}

void BDSExecOptions::Usage()const {
  G4cout<<"bdsim : version 0.6.develop"<<G4endl;
  G4cout<<"        (C) 2001-2015 Royal Holloway University London"<<G4endl;
  G4cout<<"        http://www.ph.rhul.ac.uk/twiki/bin/view/PP/JAI/BdSim"<<G4endl;
  G4cout<<G4endl;

  G4cout<<"Usage: bdsim [options]"<<G4endl;
  G4cout<<"Options:"<<G4endl;
  G4cout<<"--file=<filename>         : specify the lattice and options file "<<G4endl
	<<"--batch                   : batch mode - no graphics"<<G4endl
	<<"--circular                : assume circular machine - turn control"<<G4endl
	<<"--exportgeometryto=<file> : export the geometry to a file - extension determines format"<<G4endl
	<<"                            where possible extensions are (\"gdml\")"<<G4endl
	<<"--gflash                  : turn on gFlash fast shower parameterisation. Default false."<<G4endl
	<<"--gflashemax=N            : maximum energy for gflash shower parameterisation in GeV. Default 10000."<<G4endl
	<<"--gflashemin=N            : minimum energy for gflash shower parameterisation in GeV. Default 0.1."<<G4endl
	<<"--help                    : display this message"<<G4endl
	<<"--materials               : list materials included in bdsim by default"<<G4endl
	<<"--outline=<file>          : print geometry info to <file>"<<G4endl
	<<"--outline_type=<fmt>      : type of outline format"<<G4endl
	<<"                            where fmt = optics | survey"<<G4endl
	<<"--output=<fmt>            : output format (root|ascii|combined), default ascii"<<G4endl
	<<"--outfile=<file>          : output file name. Will be appended with _N"<<G4endl
        <<"                            where N = 0, 1, 2, 3... etc."<<G4endl
	<<"--ngenerate=N             : the number of primary events to simulate - overrides the ngenerate " << G4endl
	<<"                            option in the input gmad file" << G4endl
        <<"--seed=N                  : the seed to use for the random number generator" <<G4endl
	<<"--seedstate=<file>        : file containing CLHEP::Random seed state - overrides other seed options"<<G4endl
	<<"--verbose                 : display general parameters before run"<<G4endl
	<<"--verbose_event           : display information for every event "<<G4endl
	<<"--verbose_event_num=N     : display tracking information for event number N"<<G4endl
	<<"--verbose_step            : display tracking information after each step"<<G4endl
	<<"--verbose_G4event=N       : set Geant4 Event manager verbosity level"<<G4endl
	<<"--verbose_G4run=N         : set Geant4 verbosity level (see Geant4 manual for details)"<<G4endl
	<<"--verbose_G4stepping=N    : set Geant4 Stepping manager verbosity level"<<G4endl
	<<"--verbose_G4tracking=N    : set Geant4 Tracking manager verbosity level [-1:5]"<<G4endl
	<<"--vis_debug               : display all volumes in visualiser"<<G4endl
	<<"--vis_mac=<file>          : file with the visualisation macro script, default provided by BDSIM openGL (OGLSQt))"<<G4endl;
}

void BDSExecOptions::Print()const {
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " inputFilename: "       << std::setw(15) << inputFilename       << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " batch: "               << std::setw(15) << batch               << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " circular: "            << std::setw(15) << circular            << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " exportgeometryto "     << std::setw(15) << exportFileName      << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " gflash: "              << std::setw(15) << gflash              << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " gflashemin: "          << std::setw(15) << gflashemin          << G4endl;  
  G4cout << __METHOD_NAME__ << std::setw(23) << " gflashemax: "          << std::setw(15) << gflashemax          << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " ngnerate: "            << std::setw(15) << nGenerate           << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " outline: "             << std::setw(15) << outline             << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " outputFilename: "      << std::setw(15) << outputFilename      << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " outputFormat: "        << std::setw(15) << outputFormat.underlying() << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " outlineFilename: "     << std::setw(15) << outlineFilename     << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " seed: "                << std::setw(15) << seed                << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " seedStateFilename: "   << std::setw(15) << seedStateFilename   << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " verbose: "             << std::setw(15) << verbose             << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " verboseEvent: "        << std::setw(15) << verboseEvent        << G4endl;  
  G4cout << __METHOD_NAME__ << std::setw(23) << " verboseStep: "         << std::setw(15) << verboseStep         << G4endl;  
  G4cout << __METHOD_NAME__ << std::setw(23) << " verboseRunLevel: "     << std::setw(15) << verboseRunLevel     << G4endl;  
  G4cout << __METHOD_NAME__ << std::setw(23) << " verboseEventLevel: "   << std::setw(15) << verboseEventLevel   << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " verboseTrackingLevel: "<< std::setw(15) << verboseTrackingLevel<< G4endl;  
  G4cout << __METHOD_NAME__ << std::setw(23) << " verboseSteppingLevel: "<< std::setw(15) << verboseSteppingLevel<< G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " visMacroFilename: "    << std::setw(15) << visMacroFilename    << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " visDebug: "            << std::setw(15) << visDebug            << G4endl;
  
  return;
}

G4String BDSExecOptions::GetPath(G4String fileName)
{
  //Set fullPath to mirror what is done in parser.l (i.e. if no environment varible set, assume base filename path is that of the gmad file).
  G4String fullPath = getEnv("BDSIMPATH");
  if(fullPath.length()<=0){
    G4String inputFilepath = "";
    // get the path part of the supplied path to the main input file
    G4String::size_type found = fileName.rfind("/"); // find the last '/'
    if (found != G4String::npos){
      inputFilepath = fileName.substr(0,found); // the path is the bit before that
    } // else remains empty string
    // need to know whether it's an absolute or relative path
    if ((fileName.substr(0,1)) == "/"){
      // the main file has an absolute path
      fullPath = inputFilepath;
    } else {
      // the main file has a relative path or just the file name
      char cwdchars[200]; //filepath up to 200 characters
      // get current working directory
      G4String cwd = (G4String)getcwd(cwdchars, sizeof(cwdchars)) + "/";
      fullPath = cwd + inputFilepath;
    }
  }
  // add additional slash just to be safe
  fullPath += "/";
  return fullPath;
}
