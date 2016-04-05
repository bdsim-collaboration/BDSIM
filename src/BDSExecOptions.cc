#include "BDSExecOptions.hh"

#include <cstdlib>
#include <iomanip>
#include <unistd.h>

#include "globals.hh"

#include "BDSDebug.hh"
#include "BDSMaterials.hh"
#include "BDSOutputFormat.hh"
#include "BDSUtilities.hh"

#include "parser/getEnv.h"

BDSExecOptions* BDSExecOptions::_instance=nullptr;

ClassImp(BDSExecOptions);

const BDSExecOptions* BDSExecOptions::Instance(int argc, char **argv){
  if(_instance==nullptr) {
    _instance = new BDSExecOptions(argc, argv);
    return _instance;
  } else {
    G4Exception("BDSExecOptions::Instance is already initialized. Return pointer to singleton with BDSExecOptions::Instance()", "-1", FatalException, "");
    return nullptr;
  }
}

const BDSExecOptions* BDSExecOptions::Instance(){
  if(_instance==nullptr) {
    G4Exception("BDSExecOptions::Instance was not initialised. Initialize first with BDSExecOptions::Instance(int argc, char **argv).", "-1", FatalException, "");
    return nullptr;
  } else 
    return _instance;
}

BDSExecOptions::BDSExecOptions() {
}

BDSExecOptions::BDSExecOptions(int argc, char **argv)
{
  inputFilename       = "optics.mad";
  visMacroFilename    = "";
  visDebug            = false;
  outputFilename      = "output";
  outputFilenameSet   = false;
  outputFormat        = BDSOutputFormat::ascii;
  survey              = false;
  surveyFilename      = "survey.dat";

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

  generatePrimariesOnly = false;

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

BDSExecOptions::~BDSExecOptions()
{
  _instance = nullptr;
}

void BDSExecOptions::Parse(int argc, char **argv)
{
  static struct option LongOptions[] = {{ "help" , 0, 0, 0 },
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
					{ "circular", 0, 0, 0 },
					{ "seed", 1, 0, 0 },
					{ "seedstate",1,0,0 },
					{ "survey", 1, 0, 0 },
					{ "ngenerate", 1, 0, 0 },
					{ "exportgeometryto", 1, 0, 0 },
					{ "generatePrimariesOnly", 0, 0, 0 },
					{ 0, 0, 0, 0 }};
  
  int OptionIndex  = 0;
  int OptionNumber = 0;
  const char* optionName;
  // return code
  int c;
  // number conversion check
  bool conversion = true;
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
      OptionNumber++;
      optionName = LongOptions[OptionIndex].name;
      if( !strcmp(optionName , "help") ) {
	Usage();
	exit(0);
      }
      else if( !strcmp(optionName , "batch") ) {
	batch = true;
      }
      else if( !strcmp(optionName , "verbose") ) {
	verbose = true; 
      }
      else if( !strcmp(optionName , "verbose_step") ) {
	verboseStep = true; 
	// we shouldn't have verbose steps without verbose events etc.
	verboseEvent = true;
      }
      else if( !strcmp(optionName , "verbose_event") ) {
	verboseEvent = true; 
      }
      else if( !strcmp(optionName , "verbose_event_num") ){
	conversion = BDS::IsInteger(optarg,verboseEventNumber);
      }
      else if( !strcmp(optionName , "verbose_G4run") ) {
	conversion = BDS::IsInteger(optarg,verboseRunLevel);
      }
      else if( !strcmp(optionName , "verbose_G4event") ) {
	conversion = BDS::IsInteger(optarg,verboseEventLevel);
      }
      else if( !strcmp(optionName , "verbose_G4tracking") )  {
	conversion = BDS::IsInteger(optarg,verboseTrackingLevel);
      }
      else if( !strcmp(optionName , "verbose_G4stepping") ) {
	conversion = BDS::IsInteger(optarg,verboseSteppingLevel);
      }
      else if( !strcmp(optionName , "output") ) {
	outputFormat = BDS::DetermineOutputFormat(optarg);
      }
      else if( !strcmp(optionName , "outfile") ) {
	outputFilename=optarg;
	outputFilenameSet=true;
      }
      else if( !strcmp(optionName , "survey") ) {
	surveyFilename = optarg; 
	survey=true;
      }
      else if( !strcmp(optionName , "file") ) {
	inputFilename=optarg;
	G4cout << optarg << " " << inputFilename << G4endl;
      }
      else if( !strcmp(optionName , "vis_debug") ) {
	visDebug = true;
      }
      else if( !strcmp(optionName , "vis_mac") ) {
	visMacroFilename=optarg;
      }
      else if( !strcmp(optionName , "gflash") ) {
	gflash = true;
      }
      else if( !strcmp(optionName , "gflashemax") ) {
	conversion = BDS::IsNumber(optarg,gflashemax);
      }
      else if( !strcmp(optionName , "gflashemin") ) {
	conversion = BDS::IsNumber(optarg,gflashemin);
      }
      else if( !strcmp(optionName, "materials") ) {
	BDSMaterials::ListMaterials();
	// return after printing material list
	exit(0);
      }
      else if( !strcmp(optionName, "circular")  ) {
	circular = true;
      }
      else if( !strcmp(optionName, "seed")  ){
	conversion = BDS::IsInteger(optarg,seed);
	setSeed = true;
      }
      else if( !strcmp(optionName, "seedstate") ){
	seedStateFilename = optarg;
	setSeedState = true;
      }
      else if( !strcmp(optionName, "ngenerate") ){
	conversion = BDS::IsInteger(optarg,nGenerate);
      }
      else if( !strcmp(optionName, "generatePrimariesOnly") )
	{generatePrimariesOnly = true;}
      else if( !strcmp(optionName, "exportgeometryto") ){
	std::string fn = optarg;
	if (fn.substr(fn.find_last_of(".") + 1) == "gdml")
	  {
	    exportType    = "gdml";
	    exportFileName = fn;
	  }
	else
	  {
	    // remember if you extend this to do it also in the usage print out
	    G4cerr << __METHOD_NAME__ << "Unknown geometry format \""
		   << fn.substr(fn.find_last_of(".") + 1) << "\"\n"
		   << "Please specify a valid filename extension - options are: \"gdml\"" << G4endl;
	    exit(1);
	  }
	exportGeometry = true;
      }

      if (conversion == false) {
	// conversion from character string to number went wrong, exit
	G4cerr << __METHOD_NAME__ << "Conversion to number (or integer) went wrong for \"" << optionName << "\" with value: \"" << optarg << "\"" << G4endl;
	exit(1);
      }

      break;
      
    default:
      G4cout << "Warning unknown returned character code " <<  c << G4endl;
      break;
    }
  }
  // there should be no remaining options
  if (OptionNumber < argc - 1) {
    G4cout << "ERROR there are remaining options: " << G4endl;
    for (int i=1; i<argc; i++) {
      // options with '-' are ignored by getopt_long, other unknown options are covered
      if (strncmp(argv[i], "-", 1)) {
	G4cout << argv[i] << G4endl;
      }
    }
    
    G4cout << "Please check your input" << G4endl;
    exit(1);
  }
}

void BDSExecOptions::Usage()const {
  G4cout<<"Usage: bdsim [options]"<<G4endl;
  G4cout<<"Options:"<<G4endl;
  G4cout<<"--file=<filename>         : specify the lattice and options file "<<G4endl
	<<"--batch                   : batch mode - no graphics"<<G4endl
	<<"--circular                : assume circular machine - turn control"<<G4endl
	<<"--exportgeometryto=<file> : export the geometry to a file - extension determines format"<<G4endl
	<<"                            where possible extensions are (\"gdml\")"<<G4endl
	<<"--generatePrimariesOnly   : generate N primary particle coordinates without simulation then quit"<<G4endl
	<<"--gflash                  : turn on gFlash fast shower parameterisation. Default false."<<G4endl
	<<"--gflashemax=N            : maximum energy for gflash shower parameterisation in GeV. Default 10000."<<G4endl
	<<"--gflashemin=N            : minimum energy for gflash shower parameterisation in GeV. Default 0.1."<<G4endl
	<<"--help                    : display this message"<<G4endl
	<<"--materials               : list materials included in bdsim by default"<<G4endl
	<<"--output=<fmt>            : output format (root|ascii|combined|none), default ascii"<<G4endl
	<<"--outfile=<file>          : output file name. Will be appended with _N"<<G4endl
        <<"                            where N = 0, 1, 2, 3... etc."<<G4endl
	<<"--ngenerate=N             : the number of primary events to simulate - overrides the ngenerate " << G4endl
	<<"                            option in the input gmad file" << G4endl
        <<"--seed=N                  : the seed to use for the random number generator" <<G4endl
	<<"--seedstate=<file>        : file containing CLHEP::Random seed state - overrides other seed options"<<G4endl
	<<"--survey=<file>           : print survey info to <file>"<<G4endl
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

void BDSExecOptions::Print()const
{
  G4cout << __METHOD_NAME__ << std::setw(23) << " inputFilename: "       << std::setw(15) << inputFilename       << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " batch: "               << std::setw(15) << batch               << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " circular: "            << std::setw(15) << circular            << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " exportgeometryto "     << std::setw(15) << exportFileName      << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " generatePrimariesOnly "<< std::setw(15) << generatePrimariesOnly << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " gflash: "              << std::setw(15) << gflash              << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " gflashemin: "          << std::setw(15) << gflashemin          << G4endl;  
  G4cout << __METHOD_NAME__ << std::setw(23) << " gflashemax: "          << std::setw(15) << gflashemax          << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " ngnerate: "            << std::setw(15) << nGenerate           << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " outputFilename: "      << std::setw(15) << outputFilename      << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " outputFormat: "        << std::setw(15) << outputFormat        << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " seed: "                << std::setw(15) << seed                << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " seedStateFilename: "   << std::setw(15) << seedStateFilename   << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " survey: "              << std::setw(15) << survey              << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " surveyFilename: "      << std::setw(15) << surveyFilename      << G4endl;
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
  G4cout << fileName << G4endl;
  //Set fullPath to mirror what is done in parser.l (i.e. if no environment varible set, assume base filename path is that of the gmad file).
  G4String fullPath = getEnv("BDSIMPATH");
  G4cout << fullPath << G4endl;
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
  G4cout << "fullpath :" << fullPath << ":" << G4endl;
  return fullPath;
}
