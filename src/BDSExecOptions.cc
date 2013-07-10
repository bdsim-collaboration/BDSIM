#include "BDSExecOptions.hh"

BDSExecOptions* BDSExecOptions::_instance=0;

BDSExecOptions* BDSExecOptions::Instance(){
  if(_instance==0) {
    _instance = new BDSExecOptions();
  }
  return _instance;
}

BDSExecOptions::BDSExecOptions() {  
  inputFilename       = "optics.mad";
  visMacroFilename    = "vis.mac";
  outputFilename      = "output";
  outputFormat        = _ASCII;
  outline             = false;
  outlineFilename     = "outline.dat";
  outlineFormat       = "";
  fifoFilename        = "";

  gflash      = 0;
  gflashemax  = 10000;
  gflashemin  = 0.1;

  nptwiss     = 200;

  verbose       = false;
  verboseEvent  = false;
  verboseStep   = false;
  verboseEventNumber   = -1;
  batch         = false; 
  listMaterials = false;
  
  verboseRunLevel      = 0;
  verboseEventLevel    = 0;
  verboseTrackingLevel = 0;
  verboseSteppingLevel = 0;
}

BDSExecOptions::~BDSExecOptions() {
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
					{ "vis_mac", 1, 0, 0 },
					{ "gflash", 1, 0, 0 },
					{ "gflashemax", 1, 0, 0 },
					{ "gflashemin", 1, 0, 0 },
					{ "output", 1, 0, 0 },
					{ "outfile", 1, 0, 0 },
					{ "fifo", 1, 0, 0 },
					{ "batch", 0, 0, 0 },
					{ "materials", 0, 0, 0 },
					{ 0, 0, 0, 0 }};
  
  int OptionIndex = 0;
  int c;
 
  for(;;) {      
    OptionIndex = 0;
  
    c = getopt_long(argc, argv, "Vv",
		    LongOptions, &OptionIndex );
      
    if ( c == -1 ) // end of options list
      break;
      
    switch (c) {
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
	if(optarg)
	  verboseEventNumber = atoi(optarg); 
      }
      if( !strcmp(LongOptions[OptionIndex].name , "verbose_G4run") ) {
	if(optarg)
	  verboseRunLevel = atoi(optarg);
      }
      if( !strcmp(LongOptions[OptionIndex].name , "verbose_G4event") ) {
	if(optarg)
	  verboseEventLevel = atoi(optarg);
      }
      if( !strcmp(LongOptions[OptionIndex].name , "verbose_G4tracking") )  {
	if(optarg)
	  verboseTrackingLevel = atoi(optarg);
      }
      if( !strcmp(LongOptions[OptionIndex].name , "verbose_G4stepping") ) {
	if(optarg)
	  verboseSteppingLevel = atoi(optarg);
      }
      if( !strcmp(LongOptions[OptionIndex].name , "output") ) {
	if(optarg) {
	  if(!strcmp(optarg,"ascii") || !strcmp(optarg,"ASCII")) outputFormat=_ASCII;
	  else if (!strcmp(optarg,"root") || !strcmp(optarg,"ROOT")) outputFormat=_ROOT;
	  else {
	    G4cerr<<"unknown output format "<<optarg<<G4endl;
	    exit(1);
	  }
#ifndef USE_ROOT
	  if (outputFormat == _ROOT) {
	    G4cerr << "ERROR outputFormat root, but BDSIM not configured with ROOT support!" << G4endl;
	    G4cerr << "Use ascii instead, or recompile with ROOT!" << G4endl;
	    exit(1);
	  }
#endif
	}
      }
      if( !strcmp(LongOptions[OptionIndex].name , "outfile") ) {
	if(optarg) {
	  outputFilename=optarg;
	}
      }
      if( !strcmp(LongOptions[OptionIndex].name , "fifo") ) {
	if(optarg) {
	  outputFilename=optarg;
	}
      }
      if( !strcmp(LongOptions[OptionIndex].name , "outline") ) {
	if(optarg) outlineFilename = optarg; 
	outline=true;
      }
      if( !strcmp(LongOptions[OptionIndex].name , "outline_type") ) {
	if(optarg) outlineFormat = optarg; 
	outline=true;  // can't have outline type without turning on outline!
      }
      if( !strcmp(LongOptions[OptionIndex].name , "file") ) {
	if(optarg) {
	  inputFilename=optarg;
	}
	else {
	  G4cout<<"please specify the lattice filename"<<G4endl;
	}
      }
      if( !strcmp(LongOptions[OptionIndex].name , "vis_mac") ) {
	if(optarg) {
	  visMacroFilename=optarg;
	}
	else {
	  G4cout<<"please specify the visualization macro file"<<G4endl;
	}
      }
      if( !strcmp(LongOptions[OptionIndex].name , "gflash") ) {
	if(optarg)
	  gflash = atoi(optarg); 
      }
      if( !strcmp(LongOptions[OptionIndex].name , "gflashemax") ) {
	if(optarg)
	  gflashemax = atof(optarg); 
      }
      if( !strcmp(LongOptions[OptionIndex].name , "gflashemin") ) {
	if(optarg)
	  gflashemin = atof(optarg); 
      }
      if( !strcmp(LongOptions[OptionIndex].name, "materials") ) {
	BDSMaterials::ListMaterials();
      }
      break;
      
    default:
      break;
    }      
  } 
}

void BDSExecOptions::Usage() {
  G4cout<<"bdsim : version 0.6.0"<<G4endl;
  G4cout<<"        (C) 2001-2013 Royal Holloway University London"<<G4endl;
  G4cout<<"        http://www.ph.rhul.ac.uk/twiki/bin/view/PP/JAI/BdSim"<<G4endl;
  G4cout<<G4endl;

  G4cout<<"Usage: bdsim [options]"<<G4endl;
  G4cout<<"Options:"<<G4endl;
  G4cout<<"--file=<filename>     : specify the lattice file "<<G4endl
	<<"--output=<fmt>        : output format (root|ascii), default ascii"<<G4endl
	<<"--outfile=<file>      : output file name. Will be appended with _N"<<G4endl
        <<"                        where N = 0, 1, 2, 3... etc."<<G4endl
	<<"--vis_mac=<file>      : file with the visualization macro script, default vis.mac"<<G4endl
	<<"--gflash=N            : whether or not to turn on gFlash fast shower parameterisation. Default 0."<<G4endl
	<<"--gflashemax=N        : maximum energy for gflash shower parameterisation in GeV. Default 10000."<<G4endl
	<<"--gflashemin=N        : minimum energy for gflash shower parameterisation in GeV. Default 0.1."<<G4endl
	<<"--help                : display this message"<<G4endl
	<<"--verbose             : display general parameters before run"<<G4endl
    	<<"--verbose_event       : display information for every event "<<G4endl
    	<<"--verbose_step        : display tracking information after each step"<<G4endl
	<<"--verbose_event_num=N : display tracking information for event number N"<<G4endl
	<<"--batch               : batch mode - no graphics"<<G4endl
	<<"--outline=<file>      : print geometry info to <file>"<<G4endl
	<<"--outline_type=<fmt>  : type of outline format"<<G4endl
	<<"                        where fmt = optics | survey"<<G4endl
	<<"--materials           : list materials included in bdsim by default"<<G4endl;
}

void BDSExecOptions::Print() {
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << __METHOD_NAME__ << inputFilename       << G4endl;
  G4cout << __METHOD_NAME__ << visMacroFilename    << G4endl;
  G4cout << __METHOD_NAME__ << outputFilename      << G4endl;
  G4cout << __METHOD_NAME__ << outputFormat        << G4endl;
  G4cout << __METHOD_NAME__ << outlineFilename     << G4endl;
  G4cout << __METHOD_NAME__ << gflash              << G4endl;
  G4cout << __METHOD_NAME__ << gflashemin          << G4endl;  
  G4cout << __METHOD_NAME__ << gflashemax          << G4endl;
  G4cout << __METHOD_NAME__ << verbose             << G4endl;
  G4cout << __METHOD_NAME__ << verboseEvent        << G4endl;  
  G4cout << __METHOD_NAME__ << verboseStep         << G4endl;  
  G4cout << __METHOD_NAME__ << batch               << G4endl;
  G4cout << __METHOD_NAME__ << outline             << G4endl;
  G4cout << __METHOD_NAME__ << listMaterials       << G4endl;
  G4cout << __METHOD_NAME__ << verboseRunLevel     << G4endl;  
  G4cout << __METHOD_NAME__ << verboseEventLevel   << G4endl;
  G4cout << __METHOD_NAME__ << verboseTrackingLevel<< G4endl;  
  G4cout << __METHOD_NAME__ << verboseSteppingLevel<< G4endl;

  return;
}
