/*
   bison grammar for the gmad parser
   Ilya Agapov, Steve Malton 2005-2007
   bdsim v.0.4
*/

%{

  extern int line_num;
  extern char* yyfilename;

  const int ECHO_GRAMMAR = 0; // print grammar rule expansion (for debugging)
  const int VERBOSE = 0; // print warnings and errors
  const int VERBOSE_EXPAND = 0; // print the process of line expansion 
  const int INTERACTIVE = 0; // print output of commands (like in interactive mode)

#include "parser.h"

  int execute = 1;
  int element_count = 1; // for samplers , ranges etc.
#ifdef __cplusplus
  using namespace std;
#endif

%}


%union{
  double dval;
  int ival;
  struct symtab *symp;
  char *str;
  struct Array *array;
}


%left '+' '-'
%left '*' '/'
%left '^' '<' '>' NE LE GE EQ
%nonassoc UMINUS
%nonassoc UPLUS

%token <dval> NUMBER
%token <symp> VARIABLE VECVAR FUNC 
%token <str> STR
%token MARKER ELEMENT DRIFT PCLDRIFT RF DIPOLE RBEND SBEND QUADRUPOLE SEXTUPOLE OCTUPOLE MULTIPOLE SCREEN
%token SOLENOID COLLIMATOR RCOL ECOL LINE SEQUENCE SPOILER ABSORBER LASER TRANSFORM3D MUSPOILER
%token VKICK HKICK KICK
%token PERIOD APERTURE FILENAME GAS PIPE TUNNEL MATERIAL ATOM
%token BEAM OPTION PRINT RANGE STOP USE VALUE ECHO PRINTF SAMPLE CSAMPLE BETA0 TWISS DUMP
%token IF ELSE BEGN END LE GE NE EQ FOR
%token CUT

%type <dval> aexpr
%type <dval> expr
%type <symp> assignment
%type <array> vecexpr;
%type <array> vectnum vectstr;
%type <str> use_parameters;
%type <ival> extension;
%type <ival> newinstance;
%type <symp> sample_options
%type <symp> csample_options
%type <symp> gas_options
%%

input : 
      | input stmt ';'
       { 
	 if(ECHO_GRAMMAR) printf("input -> input stmt ';' \n");
       }
/*       | error input';' */
/* { */
/*   yyerrok; */
/* } */
;


stmt :          if_clause '{' stmt '}' { if(ECHO_GRAMMAR) printf("stmt -> IF '(' aexpr ')' stmt\n" ); execute = 1;}
              | if_clause '{' stmt '}' ELSE '{' stmt '}' 
                { if(ECHO_GRAMMAR) printf("stmt -> IF '(' bool_expr ')' ELSE stmt \n" ); }
              | atomic_stmt  { if(ECHO_GRAMMAR) printf("stmt -> atomic_stmt \n"); }
              | BEGN input END { if(ECHO_GRAMMAR) printf("stmt -> '{' stmt ';' atomic_stmt '}' \n"); }
;

if_clause: IF '(' aexpr ')' {if( ($3 > 0) && (execute > 0) ) execute = 1; else execute = 0;}
;

atomic_stmt : 
            | expr { if(ECHO_GRAMMAR) printf("atomic_stmt -> expr\n"); }
            | command  { if(ECHO_GRAMMAR) printf("atomic_stmt -> command\n"); }
            | decl  { if(ECHO_GRAMMAR) printf("atomic_stmt -> decl\n"); }
            | error
              {
		//yyerror(" : some error message\n"); 
		if(ECHO_GRAMMAR) printf("\natomic_stmt -> error\n");
	      }
;



decl : VARIABLE ':' marker
       {
	 if(execute)  {
	   if(ECHO_GRAMMAR) printf("decl -> VARIABLE (%s) : marker\n",$1->name);
	   // check parameters and write into element table
	   write_table(params,$1->name,_MARKER);
	   params.flush();
	 }
       }
     | VARIABLE ':' drift
       {
	 if(execute) {
	   if(ECHO_GRAMMAR) printf("decl -> VARIABLE (%s) : drift\n",$1->name);
	   // check parameters and write into element table
	   write_table(params,$1->name,_DRIFT);
	   params.flush();
	 }
       }
     | VARIABLE ':' pcldrift
       {
	 if(execute) {
	   if(ECHO_GRAMMAR) printf("decl -> VARIABLE (%s) : pcldrift\n",$1->name);
	   // check parameters and write into element table
	   write_table(params,$1->name,_PCLDRIFT);
	   params.flush();
	 }
       } 
     | VARIABLE ':' rf
       {
	 if(execute) {
	   if(ECHO_GRAMMAR) printf("decl -> VARIABLE (%s) : rf\n",$1->name);
	   // check parameters and write into element table
	   write_table(params,$1->name,_RF);
	   params.flush();
	 }
       } 
     | VARIABLE ':' sbend
       {  
	 if(execute) {
	   if(ECHO_GRAMMAR) printf("decl -> VARIABLE (%s) : sbend\n",$1->name);
	   // check parameters and write into element table
	   write_table(params,$1->name,_SBEND);
	   params.flush();
	 }
       }
     | VARIABLE ':' rbend
       {
         if(execute) {
           if(ECHO_GRAMMAR) printf("decl -> VARIABLE (%s) : rbend\n",$1->name);
           // check parameters and write into element table
           write_table(params,$1->name,_RBEND);
           params.flush();
         }
       }

    | VARIABLE ':' vkick
       {  
	 if(execute) {
	   if(ECHO_GRAMMAR) printf("decl -> VARIABLE (%s) : vkick\n",$1->name);
	   // check parameters and write into element table
	   write_table(params,$1->name,_VKICK);
	   params.flush();
	 }
       }
    | VARIABLE ':' hkick
       {  
	 if(execute) {
	   if(ECHO_GRAMMAR) printf("decl -> VARIABLE (%s) : hkick\n",$1->name);
	   // check parameters and write into element table
	   write_table(params,$1->name,_HKICK);
	   params.flush();
	 }
       }
     | VARIABLE ':' quad
       {
	 if(execute)       
	   {
	     if(ECHO_GRAMMAR) printf("decl -> VARIABLE : quad %s \n",$1->name);
	     // check parameters and write into element table
	     write_table(params,$1->name,_QUAD);
	     params.flush();
	   }
       }
     | VARIABLE ':' sextupole
       {
	 if(execute)
	   {
	     if(ECHO_GRAMMAR) printf("decl -> VARIABLE : sext %s \n",$1->name);
	     // check parameters and write into element table
	     write_table(params,$1->name,_SEXTUPOLE);
	     params.flush();
	   }
       }
     | VARIABLE ':' octupole
       {
	 if(execute)
	   {
	     if(ECHO_GRAMMAR) printf("VARIABLE : octupole %s \n",$1->name);
	     // check parameters and write into element table
	     write_table(params,$1->name,_OCTUPOLE);
	     params.flush();
	   }
       }
     | VARIABLE ':' multipole
       {
	 if(execute)
	   {	 
	     if(ECHO_GRAMMAR) printf("VARIABLE : multipole %s \n",$1->name);
	     // check parameters and write into element table
	     write_table(params,$1->name,_MULT);
	     params.flush();	 
	   }
       }
     | VARIABLE ':' solenoid
       {
	 if(execute)       
	   {
	     if(ECHO_GRAMMAR) printf("decl -> VARIABLE : solenoid %s \n",$1->name);
	     // check parameters and write into element table
	     write_table(params,$1->name,_SOLENOID);
	     params.flush();
	   }
       }
     | VARIABLE ':' rcol
       {
	 if(execute)
	   {
	     if(ECHO_GRAMMAR) printf("VARIABLE : rcol %s \n",$1->name);
	     // check parameters and write into element table
	     write_table(params,$1->name,_RCOL);
	     params.flush();
	   }
       }
     | VARIABLE ':' ecol
       {
	 if(execute)
	   {
	     if(ECHO_GRAMMAR) printf("VARIABLE : ecol %s \n",$1->name);
	     // check parameters and write into element table
	     write_table(params,$1->name,_ECOL);
	     params.flush();
	   }
       }
     | VARIABLE ':' muspoiler
       {
	 if(execute)
	   {
	     if(ECHO_GRAMMAR) printf("VARIABLE : muspoiler %s \n",$1->name);
	     // check parameters and write into element table
	     write_table(params,$1->name,_MUSPOILER);
	     params.flush();
	   }
       }
     | VARIABLE ':' element
       {
	 if(execute)
	   {	 
	     if(ECHO_GRAMMAR) printf("VARIABLE : element %s \n",$1->name);
	     // check parameters and write into element table
	     write_table(params,$1->name,_ELEMENT);
	     params.flush();	 
	   }
       }
     | VARIABLE ':' laser
       {
	 if(execute)
	   {	 
	     if(ECHO_GRAMMAR) printf("VARIABLE : laser %s \n",$1->name);
	     // check parameters and write into element table
	     write_table(params,$1->name,_LASER);
	     params.flush();	 
	   }
       }
     | VARIABLE ':' screen
       {
	 if(execute) {
	   if(ECHO_GRAMMAR) printf("decl -> VARIABLE (%s) : screen\n",$1->name);
	   // check parameters and write into element table
	   write_table(params,$1->name,_SCREEN);
	   params.flush();
	 }
       }
     | VARIABLE ':' transform3d
       {
	 if(execute)
	   {	 
	     if(ECHO_GRAMMAR) printf("VARIABLE : transform3d %s \n",$1->name);
	     // check parameters and write into element table
	     write_table(params,$1->name,_TRANSFORM3D);
	     params.flush();	 
	   }
       }
     | VARIABLE ':' line 
       {
	 if(execute)
	   {
	     // create entry in the main table and add pointer to the parsed sequence
	     if(ECHO_GRAMMAR) printf("VARIABLE : LINE %s\n",$1->name);
	     //  list<struct Element>* tmp_list = new list<struct Element>;
	     write_table(params,$1->name,_LINE,new list<struct Element>(tmp_list));
	     // write_table(params,$1->name,_LINE,tmp_list);
	      tmp_list.erase(tmp_list.begin(), tmp_list.end());
	      tmp_list.~list<struct Element>();
	   }
       }     
     | VARIABLE ':' sequence
       {
	 if(execute)
	   {
             // create entry in the main table and add pointer to the parsed sequence
	     if(ECHO_GRAMMAR) printf("VARIABLE : SEQUENCE %s\n",$1->name);
	     write_table(params,$1->name,_SEQUENCE,new list<struct Element>(tmp_list));
	     tmp_list.erase(tmp_list.begin(), tmp_list.end());
	   }
       }
     | VARIABLE ':' extension
       {
	 if(execute)
	   {
	     if(ECHO_GRAMMAR) printf("decl -> VARIABLE : VARIABLE, %s  :  %s\n",$1->name, typestr($3));
	     if($3 != _NONE)
	       {
		 write_table(params,$1->name,$3);
	       }
	     params.flush();
	   }
       }
     | VARIABLE ':' newinstance
       {
         if(execute)
	   {
	     if(ECHO_GRAMMAR) printf("decl -> VARIABLE : VARIABLE, %s  :  %s\n",$1->name, typestr($3));
	     if($3 != _NONE)
	       {
		 write_table(params,$1->name,$3);
	       }
	     params.flush();
	   }
       }
       | VARIABLE ',' parameters
       {
	 if(execute)
	   {
	     if(ECHO_GRAMMAR) printf("edit : VARIABLE parameters   -- %s \n",$1->name);
	     list<struct Element>::iterator it = element_lookup($1->name);
	     list<struct Element>::iterator iterNULL = element_list.end();
	     if(it == iterNULL)
	       {
		 if(VERBOSE) printf("type %s has not been defined\n",$1->name);
	       }
	     else
	       {
		 // inherit properties from the base type
		 inherit_properties(*it);
	       }
		
	     if(ECHO_GRAMMAR) printf("decl -> VARIABLE : VARIABLE, %s  :  %s\n",$1->name, typestr((*it).type));
	     if((*it).type != _NONE)
	       {
		 write_table(params,$1->name,(*it).type);
	       }
	     params.flush();
	   }
       }
     | VARIABLE ':' matdef
       {
	 if(execute)
	   {
	     if(ECHO_GRAMMAR) printf("decl -> VARIABLE : Material, %s \n",$1->name);
	     write_table(params,$1->name,_MATERIAL);
	     params.flush();
	   }
       }
     | VARIABLE ':' atom
       {
         if(execute)
           {
             if(ECHO_GRAMMAR) printf("decl -> VARIABLE : Atom, %s \n",$1->name);
             write_table(params,$1->name,_ATOM);
             params.flush();
           }
       }
;

marker : MARKER ;

drift : DRIFT ',' parameters
;

pcldrift : PCLDRIFT ',' parameters
;

rf : RF ',' parameters
;

sbend : SBEND ',' parameters
;

rbend : RBEND ',' parameters
;

vkick : VKICK ',' parameters
;

hkick : HKICK ',' parameters
;

quad : QUADRUPOLE ',' parameters
;

sextupole : SEXTUPOLE ',' parameters
;

octupole : OCTUPOLE ',' parameters
;

multipole : MULTIPOLE ',' parameters
;

solenoid : SOLENOID ',' parameters
;

ecol : ECOL ',' parameters
;

muspoiler : MUSPOILER ',' parameters
;

rcol : RCOL ',' parameters
;

laser : LASER ',' parameters
;

screen : SCREEN ',' parameters
;

transform3d : TRANSFORM3D ',' parameters
;

element : ELEMENT ',' parameters
;

matdef : MATERIAL ',' parameters
;

atom : ATOM ',' parameters
;

extension : VARIABLE ',' parameters
            {
	      if(execute)
		{	 
		  if(ECHO_GRAMMAR) printf("extension : VARIABLE parameters   -- %s \n",$1->name);
		  list<struct Element>::iterator it = element_lookup($1->name);
		  list<struct Element>::iterator iterNULL = element_list.end();
		  if(it == iterNULL)
		    {
		      if(VERBOSE) printf("type %s has not been defined\n",$1->name);
		      $$ = _NONE;
		    }
		  else
		    {
		      // inherit properties from the base type
		      $$ = (*it).type;
		      inherit_properties(*it);
		    }
		  
		}
	    }
;

newinstance : VARIABLE 
            {
	      if(execute)
		{	 
		  if(ECHO_GRAMMAR) printf("newinstance : VARIABLE -- %s \n",$1->name);
		  list<struct Element>::iterator it = element_lookup($1->name);
		  list<struct Element>::iterator iterNULL = element_list.end();
		  if(it == iterNULL)
		    {
		      if(VERBOSE) printf("type %s has not been defined\n",$1->name);
		      $$ = _NONE;
		    }
		  else
		    {
		      // inherit properties from the base type
		      $$ = (*it).type;
		      inherit_properties(*it);
		    }
		  
		}
	    }
;

parameters: 
          | VARIABLE '=' aexpr ',' parameters
            {
	      if(execute)
		{
#ifdef DEBUG 
                  printf("parameters, VARIABLE(%s) = aexpr(%.10g)\n",$1->name,$3);
#endif
		  if(!strcmp($1->name,"l")) { params.l = $3; params.lset = 1;} // length
		    else
	          if(!strcmp($1->name,"B")) { params.B = $3; params.Bset = 1;} // dipole field
		    else 
		  if(!strcmp($1->name,"ks")) { params.ks = $3; params.ksset = 1;} // solenoid strength
		    else
		  if(!strcmp($1->name,"k0")) { params.k0 = $3; params.k0set = 1;} // dipole coef.
		    else 
		  if(!strcmp($1->name,"k1")) { params.k1 = $3; params.k1set = 1;} // quadrupole coef. 
		    else
		  if(!strcmp($1->name,"k2")) { params.k2 = $3; params.k2set = 1;} // sextupole coef.
		    else 
		  if(!strcmp($1->name,"k3")) { params.k3 = $3; params.k3set = 1;} // octupole coef.
		    else 
		  if(!strcmp($1->name,"angle")) { params.angle = $3; params.angleset = 1;} // dipole bending angle
		  else
		  if(!strcmp($1->name,"phiAngleIn")) { params.phiAngleIn = $3; params.phiAngleInset = 1;} // element incoming angle
		  else
		  if(!strcmp($1->name,"phiAngleOut")) { params.phiAngleOut = $3; params.phiAngleOutset = 1;} // element outgoing angle
		  else
		   if(!strcmp($1->name,"beampipeThickness") ||!strcmp($1->name,"beampipeThickness") ) 
		      { params.beampipeThickness = $3; params.beampipeThicknessset = 1;}
		    else
		  if(!strcmp($1->name,"aper") ||!strcmp($1->name,"aperture") ) 
		      { params.aper = $3; params.aperset = 1;}
		    else
		  if(!strcmp($1->name,"aperX") ||!strcmp($1->name,"apertureX") ) 
		      { params.aperX = $3; params.aperXset = 1;}
		    else
		  if(!strcmp($1->name,"aperY") ||!strcmp($1->name,"apertureY") ) 
		      { params.aperY = $3; params.aperYset = 1;}
		    else
		  if(!strcmp($1->name,"aperYUp") ||!strcmp($1->name,"apertureYUp") ) 
		      { params.aperYUp = $3; params.aperYUpset = 1;}
		    else
		  if(!strcmp($1->name,"aperYDown") ||!strcmp($1->name,"apertureYDown") ) 
		      { params.aperYDown = $3; params.aperYDownset = 1;}
		    else
		  if(!strcmp($1->name,"aperDy") ||!strcmp($1->name,"apertureDy") ) 
		    { params.aperDy = $3; params.aperDyset = 1;}
		    else
		  if(!strcmp($1->name,"outR") ) { params.outR = $3; params.outRset = 1;}
		    else
                  if(!strcmp($1->name,"inR") ) { params.inR = $3; params.inRset = 1;}
		    else
		  if(!strcmp($1->name,"xsize") ) { params.xsize = $3; params.xsizeset = 1;}
		    else
		  if(!strcmp($1->name,"ysize") ) { params.ysize = $3; params.ysizeset = 1;}
		    else
		  if(!strcmp($1->name,"tilt")) { params.tilt = $3; params.tiltset = 1;}
		    else
		  if(!strcmp($1->name,"x")) {params.xdir = $3; params.xdirset = 1;} // x direction
		    else
		  if(!strcmp($1->name,"y")) {params.ydir = $3; params.ydirset = 1;} // y direction 
		    else
		  if(!strcmp($1->name,"z")) {params.zdir = $3; params.zdirset = 1;} // z direction 
		    else
		  if(!strcmp($1->name,"phi")) {params.phi = $3; params.phiset = 1;}  // polar angle
		    else
		  if(!strcmp($1->name,"theta"))  {params.theta = $3; params.thetaset = 1;} 
		  // azimuthal angle
		    else
		  if(!strcmp($1->name,"psi"))  {params.psi = $3; params.psiset = 1;} // 3rd  angle
		  else
		  if(!strcmp($1->name,"gradient"))  {params.gradient = $3; params.gradientset = 1;} // rf voltage
		  else
		  if(!strcmp($1->name,"fint")) {;} // fringe field parameters
		  else
		  if(!strcmp($1->name,"fintx")) {;}  //
		  else
		  if(!strcmp($1->name,"tunnelRadius")) { params.tunnelRadius = $3; params.tunnelRadiusset = 1;} // tunnel radius
		  else
		  if(!strcmp($1->name,"tunnelOffsetX")) { params.tunnelOffsetX = $3; params.tunnelOffsetXset = 1;} // tunnel offset
		  else
		  if(!strcmp($1->name,"precisionRegion")) { params.precisionRegion = $3; params.precisionRegionset = 1;} // tunnel offset
		    else
		  if(!strcmp($1->name,"e1")) {;}  //
                    else
		  if(!strcmp($1->name,"e2")) {;}  //
                    else
		  if(!strcmp($1->name,"hgap")) {params.hgap = $3; params.hgapset=1;}  //
		    else
		  if(!strcmp($1->name,"A")) {params.A = $3; params.Aset = 1;}  // mass number
		    else
		  if(!strcmp($1->name,"Z")) {params.Z = $3; params.Zset = 1;}  // atomic number
		    else
		  if(!strcmp($1->name,"density")) {params.density = $3; params.densityset = 1;}  // density
                    else
		  if(!strcmp($1->name,"T")) {params.temper = $3; params.temperset = 1;}  // temperature
		    else
		  if(!strcmp($1->name,"P")) {params.pressure = $3; params.pressureset = 1;}  // pressure
		    else
		  if(!strcmp($1->name,"waveLength")) {params.waveLength = $3; params.waveLengthset = 1;}
		    else
		  if(!strcmp($1->name,"taperlength")) {params.taperlength = $3; params.taperlengthset = 1;}
		    else
		  if(!strcmp($1->name,"flatlength")) {params.flatlength = $3; params.flatlengthset = 1;}
                    else
		  if(!strcmp($1->name,"at")) {params.at = $3; params.atset = 1;}  //position of an element within a sequence
		    else
                  if(!strcmp($1->name,"tscint")) { params.tscint = $3; params.tscintset = 1;} // thickness for a scintillator screen 
		    else
                  if(VERBOSE) printf("Warning : unknown parameter %s\n",$1->name);
		  
		}
	    }
           | VARIABLE '=' vecexpr ',' parameters
             {
	       if(execute) 
		 {
#ifdef DEBUG 
                   printf("params,VARIABLE (%s) = vecexpr (%d)\n",$1->name,$3->size);
#endif
                   if(!strcmp($1->name,"knl")) 
		     {
		       params.knlset = 1;
		       set_vector(params.knl,$3);
		       delete[] $3->data;
		     } 
		   else
		     if(!strcmp($1->name,"ksl")) 
		       {
			 params.kslset = 1;
			 set_vector(params.ksl,$3);
			 delete[] $3->data;
		       }
                     else
		     if(!strcmp($1->name,"blmLocZ")) 
		       {
			 params.blmLocZset = 1;
			 set_vector(params.blmLocZ,$3);
			 delete[] $3->data;
		       }
		   else
		     if(!strcmp($1->name,"blmLocTheta")) 
		       {
			 params.blmLocThetaset = 1;
			 set_vector(params.blmLocTheta,$3);
			 delete[] $3->data;
		       }
                   else
                     if(!strcmp($1->name,"components"))
                       {
                         params.componentsset = 1;
                         set_vector(params.components,$3);
                         $3->symbols.clear();
                       } 
                   else
                     if(!strcmp($1->name,"componentsWeights"))
                       {
                         params.componentsWeightsset = 1;
                         set_vector(params.componentsWeights,$3);
                         delete[] $3->data;
                       }
                   else
                     if(!strcmp($1->name,"componentsFractions"))
                       {
                         params.componentsFractionsset = 1;
                         set_vector(params.componentsFractions,$3);
                         delete[] $3->data;
                       }
		     else 	  
		       if(VERBOSE) printf("unknown parameter %s\n",$1->name);
		 }
	     }         
           | VARIABLE '=' vecexpr
             {
	       if(execute) 
		 {
#ifdef DEBUG 
                   printf("VARIABLE (%s) = vecexpr\n",$1->name);
#endif
		   if(!strcmp($1->name,"knl")) 
		     {
		       params.knlset = 1;
		       set_vector(params.knl,$3);
		       delete[] $3->data;
		     } 
		   else
		     if(!strcmp($1->name,"ksl")) 
		       {
			 params.kslset = 1;
			 set_vector(params.ksl,$3);
			 delete[] $3->data;
		       }
                     else
		       if(!strcmp($1->name,"blmLocZ")) 
			 {
			   params.blmLocZset = 1;
			   set_vector(params.blmLocZ,$3);
			   delete[] $3->data;
			 }
		       else
			 if(!strcmp($1->name,"blmLocTheta")) 
			   {
			     params.blmLocThetaset = 1;
			     set_vector(params.blmLocTheta,$3);
			     delete[] $3->data;
			   }
			 else
                     if(!strcmp($1->name,"components"))
                       {
                         params.componentsset = 1;
                         set_vector(params.components,$3);
                         delete[] $3->data;
                       }
                   else
                     if(!strcmp($1->name,"componentsWeights"))
                       {
                         params.componentsWeightsset = 1;
                         set_vector(params.componentsWeights,$3);
                         delete[] $3->data;
                       }
                   else
                     if(!strcmp($1->name,"componentsFractions"))
                       {
                         params.componentsFractionsset = 1;
                         set_vector(params.componentsFractions,$3);
                         delete[] $3->data;
                       }
		   else 	  
		     if(VERBOSE) printf("unknown parameter %s\n",$1->name);
		 }         
	     }
          | VARIABLE '=' aexpr
            {
	      if(execute)
		{
#ifdef DEBUG 
                  printf("VARIABLE (%s) = aexpr(%.10g)\n",$1->name,$3);
#endif
		  if(!strcmp($1->name,"l")) { params.l = $3; params.lset = 1;} // length
		    else
		  if(!strcmp($1->name,"B")) { params.B = $3; params.Bset = 1;} // dipole field 
		    else 
		  if(!strcmp($1->name,"ks")) { params.ks = $3; params.ksset = 1;} // solenoid strength
		    else
		  if(!strcmp($1->name,"k0")) { params.k0 = $3; params.k0set = 1;} // dipole coef.
		    else 
		  if(!strcmp($1->name,"k1")) { params.k1 = $3; params.k1set = 1;} // quadrupole coef.
		    else
		  if(!strcmp($1->name,"k2")) { params.k2 = $3; params.k2set = 1;} // sextupole coef.
		    else 
		  if(!strcmp($1->name,"k3")) { params.k3 = $3; params.k3set = 1;} // octupole coef.
		    else 
		      if(!strcmp($1->name,"angle")) { params.angle = $3; params.angleset = 1;} // dipole bending angle
		      else
			if(!strcmp($1->name,"phiAngleIn")) { params.phiAngleIn = $3; params.phiAngleInset = 1;} // element incoming angle
			else
		      if(!strcmp($1->name,"phiAngleOut")) { params.phiAngleOut = $3; params.phiAngleOutset = 1;} // element outgoing angle
		      else
		  if(!strcmp($1->name,"aper") ||!strcmp($1->name,"aperture") ) 
			      { params.aper = $3; params.aperset = 1;}
		  if(!strcmp($1->name,"aperX") ||!strcmp($1->name,"apertureX") ) 
			      { params.aperX = $3; params.aperXset = 1;}
		  if(!strcmp($1->name,"aperY") ||!strcmp($1->name,"apertureY") ) 
			      { params.aperY = $3; params.aperYset = 1;}
		  else
		 
		  if(!strcmp($1->name,"beampipeThickness") ||!strcmp($1->name,"beampipeThickness") ) 
			      { params.beampipeThickness = $3; params.beampipeThicknessset = 1;}
		  else
		  if(!strcmp($1->name,"outR") ) { params.outR = $3; params.outRset = 1;}
		    else
		  if(!strcmp($1->name,"xsize") ) { params.xsize = $3; params.xsizeset = 1;}
		    else
		  if(!strcmp($1->name,"ysize") ) { params.ysize = $3; params.ysizeset = 1;}
		    else
		  if(!strcmp($1->name,"tilt")) { params.tilt = $3; params.tiltset = 1;}
		    else
		  if(!strcmp($1->name,"x")) {params.xdir = $3; params.xdirset = 1;} // x direction
		    else
		  if(!strcmp($1->name,"y")) {params.ydir = $3; params.ydirset = 1;} // y direction 
		    else
		  if(!strcmp($1->name,"z")) {params.zdir = $3; params.zdirset = 1;} // z direction 
		    else
		  if(!strcmp($1->name,"phi")) {params.phi = $3; params.phiset = 1;}  // polar angle
		    else
		  if(!strcmp($1->name,"theta"))  {params.theta = $3; params.thetaset = 1;} // azimuthal angle
		    else
		  if(!strcmp($1->name,"psi"))  {params.psi = $3; params.psiset = 1;} // 3rd angle
		    else
		  if(!strcmp($1->name,"gradient"))  {params.gradient = $3; params.gradientset = 1;} // rf voltage
		    else
		  if(!strcmp($1->name,"fint")) {;} // fringe field parameters
		  else
		  if(!strcmp($1->name,"fintx")) {;}  //
		  else
		  if(!strcmp($1->name,"tunnelRadius")) { params.tunnelRadius = $3; params.tunnelRadiusset = 1;} // tunnel radius
		  else
		  if(!strcmp($1->name,"tunnelOffsetX")) { params.tunnelOffsetX = $3; params.tunnelOffsetXset = 1;} // tunnel offset
		    else
		  if(!strcmp($1->name,"precisionRegion")) { params.precisionRegion = $3; params.precisionRegionset = 1;} // tunnel offset
		    else
		  if(!strcmp($1->name,"e1")) {;}  //
                    else
		  if(!strcmp($1->name,"e2")) {;}  //
		    else
		  if(!strcmp($1->name,"hgap")) {params.hgap = $3; params.hgapset=1;}  //
		    else
		  if(!strcmp($1->name,"A")) {params.A = $3; params.Aset = 1;}  // mass number
		    else
		  if(!strcmp($1->name,"Z")) {params.Z = $3; params.Zset = 1;}  // atomic number
		    else
		  if(!strcmp($1->name,"density")) {params.density = $3; params.densityset = 1;}  // density
                    else
		  if(!strcmp($1->name,"T")) {params.temper = $3; params.temperset = 1;}  // temperature
		    else
		  if(!strcmp($1->name,"P")) {params.pressure = $3; params.pressureset = 1;}  // pressure
		  //else
		  //  if(!strcmp($1->name,"state")) {params.state = $3; params.stateset = 1;}  // state
		    else
		  if(!strcmp($1->name,"waveLength")) {params.waveLength = $3; params.waveLengthset = 1;}
		    else
		  if(VERBOSE) printf("Warning : unknown parameter %s\n",$1->name);
		  
		}
	    }
          | VARIABLE '=' STR ',' parameters
             {
	       if(execute) 
		 {
#ifdef DEBUG 
                   printf("params,VARIABLE (%s) = str (%s)\n",$1->name,$3);
#endif
		   if(!strcmp($1->name,"geometry")) 
		     {
		       params.geomset = 1;
		       strcpy(params.geometry, $3);
		     } 
		   else
		     if(!strcmp($1->name,"bmap")) 
		       {
			 params.geomset = 1;
			 strcpy(params.bmap, $3);
		       }
		   else 
		     if(!strcmp($1->name,"type")) 
		       {
			 //ignore the "type attribute for the moment"
		       }
		   else
		   if(!strcmp($1->name,"material")) 
		       {
			 params.materialset = 1;
			 strcpy(params.material, $3);
		       }
		   else
		   if(!strcmp($1->name,"tunnelMaterial")) 
		       {
			 params.tunnelmaterialset = 1;
			 strcpy(params.tunnelMaterial, $3);
		       }
		   else 
		   if(!strcmp($1->name,"tunnelCavityMaterial")) 
		       {
			 params.tunnelcavitymaterialset = 1;
			 strcpy(params.tunnelCavityMaterial, $3);
		       }
		   else 
		   if(!strcmp($1->name,"scintmaterial")) 
		     { strcpy(params.scintmaterial, $3); 
		       params.scintmaterialset = 1;
		     } // material for a scintillator screen 
		   else
		   if(!strcmp($1->name,"airmaterial")) 
		     { strcpy(params.airmaterial, $3); 
		       params.airmaterialset = 1;
		     } // material for air around scintillator screen 
		    else
		   if(!strcmp($1->name,"spec")) 
		       {
			 params.specset = 1;
			 strcpy(params.spec, $3);
		       }
                   else 
                   if(!strcmp($1->name,"symbol"))
                       {
                         params.symbolset = 1;
                         strcpy(params.symbol, $3);
                       }
		   else 
                   if(!strcmp($1->name,"state"))
                       {
                         params.stateset = 1;
                         strcpy(params.state, $3);
                       }
		   else 
		   if(VERBOSE) printf("unknown parameter %s\n",$1->name);
		 }
	     }         
           | VARIABLE '=' STR
             {
	       if(execute) 
		 {
#ifdef DEBUG 
                   printf("VARIABLE (%s) = str\n",$1->name);
#endif
		   if(!strcmp($1->name,"geometry")) 
		     {
		       params.geomset = 1;
		       strcpy(params.geometry, $3);
		     } 
		   else
		     if(!strcmp($1->name,"bmap")) 
		       {
			 params.geomset = 1;
			 strcpy(params.bmap, $3);
		       }
		     else 
		     if(!strcmp($1->name,"type")) 
		       {
			 //ignore the "type attribute for the moment"
		       }
                     else
                       if(!strcmp($1->name,"material")) 
                         {	 
                           params.materialset = 1;
                           strcpy(params.material, $3);
                         }
                       else
                         if(!strcmp($1->name,"tunnelMaterial")) 
		       {	 
			 params.tunnelmaterialset = 1;
			 strcpy(params.tunnelMaterial, $3);
		       }
			 else
                         if(!strcmp($1->name,"tunnelCavityMaterial")) 
		       {	 
			 params.tunnelcavitymaterialset = 1;
			 strcpy(params.tunnelCavityMaterial, $3);
		       }
                     else
                   if(!strcmp($1->name,"spec")) 
		       {
			 params.specset = 1;
			 strcpy(params.spec, $3);
		       }
		   else 
                   if(!strcmp($1->name,"symbol"))
                       {
                         params.symbolset = 1;
                         strcpy(params.symbol, $3);
                       }
		   else 
                   if(!strcmp($1->name,"state"))
                       {
                         params.stateset = 1;
                         strcpy(params.state, $3);
                       }
		   else 
		   if(VERBOSE) printf("unknown parameter %s\n",$1->name);
		 }         
	     }

line : LINE '=' '(' element_seq ')'
;

line : LINE '=' '-' '(' rev_element_seq ')'
;

//sequence : SEQUENCE ',' params ',' '(' element_seq ')'
//;

//sequence : SEQUENCE ',' params ',' '-' '(' rev_element_seq ')'
//;

sequence : SEQUENCE '=' '(' seq_element_seq ')'
;


element_seq : 
            | VARIABLE ',' element_seq
              {
		if(execute)
		  {
#ifdef DEBUG 
                    printf("matched sequence element, %s\n",$1->name);
#endif
		    // add to temporary element sequence
		    {
		      struct Element e;
		      e.name = $1->name;
		      e.type = _LINE;
		      e.lst = NULL;
		      tmp_list.push_front(e);
		    }
		  }
	      }
            | VARIABLE '*' NUMBER ',' element_seq
              {
		if(execute)
		  {
#ifdef DEBUG 
                    printf("matched sequence element, %s * %d \n",$1->name,(int)$3);
#endif
		    // add to temporary element sequence
		    {
		      struct Element e;
		      e.name = $1->name;
		      e.type = _LINE;
		      e.lst = NULL;
		      for(int i=0;i<(int)$3;i++)
			tmp_list.push_front(e);
		    }
		  }
	      }
            | NUMBER '*' VARIABLE ',' element_seq
              {
		if(execute)
		  {
#ifdef DEBUG 
                    printf("matched sequence element, %s * %d \n",$3->name,(int)$1);
#endif
                    // add to temporary element sequence
		    {
		      struct Element e;
		      e.name = $3->name;
		      e.type = _LINE;
		      e.lst = NULL;
		      for(int i=0;i<(int)$1;i++)
			tmp_list.push_front(e);
		    }
		  }
	      }
            | VARIABLE
              {
		if(execute)
		  {
#ifdef DEBUG 
                    printf("matched last sequence element, %s\n",$1->name);
#endif
                    // add to temporary element sequence
		    {
		      struct Element e;
		      e.name = $1->name;
		      e.type = _LINE;
		      e.lst = NULL;
		      tmp_list.push_front(e);
		    }
		  }
	      }
           | VARIABLE '*' NUMBER
              {
		if(execute)
		  {
#ifdef DEBUG 
                    printf("matched last sequence element, %s * %d\n",$1->name,(int)$3);
#endif
                    // add to temporary element sequence
		    {
		      struct Element e;
		      e.name = $1->name;
		      e.type = _LINE;
		      e.lst = NULL;
		      for(int i=0;i<(int)$3;i++)
			tmp_list.push_front(e);
		    }
		  }
	      }
            | NUMBER '*' VARIABLE
              {
		if(execute)
		  {
#ifdef DEBUG 
                    printf("matched last sequence element, %s * %d\n",$3->name,(int)$1);
#endif
                    // add to temporary element sequence
		    {
		      struct Element e;
		      e.name = $3->name;
		      e.type = _LINE;
		      e.lst = NULL;
		      for(int i=0;i<(int)$1;i++)
			tmp_list.push_front(e);
		    }
		  }
	      }
            | '-' VARIABLE ',' element_seq
              {
		if(execute)
		  {
#ifdef DEBUG 
                    printf("matched last sequence element, %s\n",$2->name);
#endif
                    // add to temporary element sequence
		    {
		      struct Element e;
		      e.name = $2->name;
		      e.type = _REV_LINE;
		      e.lst = NULL;
	    	      tmp_list.push_front(e);
		    }
		  }
	      }
            | '-' VARIABLE
              {
		if(execute)
		  {
#ifdef DEBUG 
                    printf("matched last sequence element, %s\n",$2->name);
#endif
                    // add to temporary element sequence
		    {
		      struct Element e;
		      e.name = $2->name;
		      e.type = _REV_LINE;
		      e.lst = NULL;
	    	      tmp_list.push_front(e);
		    }
		  }
	      }
;

rev_element_seq : 
            | VARIABLE ',' rev_element_seq 
              {
		if(execute)
		  {
#ifdef DEBUG 
                    printf("matched sequence element, %s\n",$1->name);
#endif
                    // add to temporary element sequence
		    {
		      struct Element e;
		      e.name = $1->name;
		      e.type = _REV_LINE;
		      e.lst = NULL;
		      tmp_list.push_back(e);
		    }
		  }
	      }
            | VARIABLE '*' NUMBER ',' rev_element_seq
              {
		if(execute)
		  {
#ifdef DEBUG
                    printf("matched sequence element, %s * %d \n",$1->name,(int)$3);
#endif
                    // add to temporary element sequence
		    {
		      struct Element e;
		      e.name = $1->name;
		      e.type = _REV_LINE;
		      e.lst = NULL;
		      for(int i=0;i<(int)$3;i++)
			tmp_list.push_back(e);
		    }
		  }
	      }
            | NUMBER '*' VARIABLE ',' rev_element_seq
              {
		if(execute)
		  {
#ifdef DEBUG
                    printf("matched sequence element, %s * %d \n",$3->name,(int)$1);
#endif
		    // add to temporary element sequence
		    {
		      struct Element e;
		      e.name = $3->name;
		      e.type = _REV_LINE;
		      e.lst = NULL;
		      for(int i=0;i<(int)$1;i++)
			tmp_list.push_back(e);
		    }
		  }
	      }
            | VARIABLE
              {
		if(execute)
		  {
#ifdef DEBUG
                    printf("matched last sequence element, %s\n",$1->name);
#endif
                    // add to temporary element sequence
		    {
		      struct Element e;
		      e.name = $1->name;
		      e.type = _REV_LINE;
		      e.lst = NULL;
		      tmp_list.push_back(e);
		    }
		  }
	      }
           | VARIABLE '*' NUMBER
              {
		if(execute)
		  {
#ifdef DEBUG 
                    printf("matched last sequence element, %s * %d\n",$1->name,(int)$3);
#endif
		    // add to temporary element sequence
		    {
		      struct Element e;
		      e.name = $1->name;
		      e.type = _REV_LINE;
		      e.lst = NULL;
		      for(int i=0;i<(int)$3;i++)
			tmp_list.push_back(e);
		    }
		  }
	      }
            | NUMBER '*' VARIABLE
              {
		if(execute)
		  {
#ifdef DEBUG
                    printf("matched last sequence element, %s * %d\n",$3->name,(int)$1);
#endif
                    // add to temporary element sequence
		    {
		      struct Element e;
		      e.name = $3->name;
		      e.type = _REV_LINE;
		      e.lst = NULL;
		      for(int i=0;i<(int)$1;i++)
			tmp_list.push_back(e);
		    }
		  }
	      }
            | '-' VARIABLE ',' element_seq
              {
		if(execute)
		  {
#ifdef DEBUG
                    printf("matched last sequence element, %s\n",$2->name);
#endif
                    // add to temporary element sequence
		    {
		      struct Element e;
		      e.name = $2->name;
		      e.type = _LINE;
		      e.lst = NULL;
	    	      tmp_list.push_back(e);
		    }
		  }
	      }
            | '-' VARIABLE
              {
		if(execute)
		  {
#ifdef DEBUG
                    printf("matched last sequence element, %s\n",$2->name);
#endif
                    // add to temporary element sequence
		    {
		      struct Element e;
		      e.name = $2->name;
		      e.type = _LINE;
		      e.lst = NULL;
	    	      tmp_list.push_back(e);
		    }
		  }
	      }
;

seq_element_seq : 
            | VARIABLE ',' seq_element_seq
              {
		if(execute)
		  {
#ifdef DEBUG
                    printf("matched sequence element, %s\n",$1->name);
#endif
		    // add to temporary element sequence
		    {
		      struct Element e;
		      e.name = $1->name;
		      e.type = _SEQUENCE;
		      e.lst = NULL;
		      tmp_list.push_front(e);
		    }
		  }
	      }
            | VARIABLE '*' NUMBER ',' seq_element_seq
              {
		if(execute)
		  {
#ifdef DEBUG 
                    printf("matched sequence element, %s * %d \n",$1->name,(int)$3);
#endif
		    // add to temporary element sequence
		    {
		      struct Element e;
		      e.name = $1->name;
		      e.type = _SEQUENCE;
		      e.lst = NULL;
		      for(int i=0;i<(int)$3;i++)
			tmp_list.push_front(e);
		    }
		  }
	      }
            | NUMBER '*' VARIABLE ',' seq_element_seq
              {
		if(execute)
		  {
#ifdef DEBUG
                    printf("matched sequence element, %s * %d \n",$3->name,(int)$1);
#endif
                    // add to temporary element sequence
		    {
		      struct Element e;
		      e.name = $3->name;
		      e.type = _SEQUENCE;
		      e.lst = NULL;
		      for(int i=0;i<(int)$1;i++)
			tmp_list.push_front(e);
		    }
		  }
	      }
            | VARIABLE 
              {
		if(execute)
		  {
#ifdef DEBUG
                    printf("matched last sequence element, %s\n",$1->name);
#endif
                    // add to temporary element sequence
		    {
		      struct Element e;
		      e.name = $1->name;
		      e.type = _SEQUENCE;
		      e.lst = NULL;
		      tmp_list.push_front(e);
		    }
		  }
	      }
           | VARIABLE '*' NUMBER 
              {
		if(execute)
		  {
#ifdef DEBUG
                    printf("matched last sequence element, %s * %d\n",$1->name,(int)$3);
#endif
		    // add to temporary element sequence
		    {
		      struct Element e;
		      e.name = $1->name;
		      e.type = _SEQUENCE;
		      e.lst = NULL;
		      for(int i=0;i<(int)$3;i++)
			tmp_list.push_front(e);
		    }
		  }
	      }
            | NUMBER '*' VARIABLE 
              {
		if(execute)
		  {
#ifdef DEBUG
                    printf("matched last sequence element, %s * %d\n",$3->name,(int)$1);
#endif
                    // add to temporary element sequence
		    {
		      struct Element e;
		      e.name = $3->name;
		      e.type = _SEQUENCE;
		      e.lst = NULL;
		      for(int i=0;i<(int)$1;i++)
			tmp_list.push_front(e);
		    }
		  }
	      }
;

expr : aexpr 
       { // check type ??
	 if(ECHO_GRAMMAR) printf("expr -> aexpr\n");
	 if(execute) 
	   {
	     if(INTERACTIVE) printf ("\t%.10g\n", $1); $$=$1;
	   }
       }
     | vecexpr 
       {
	 if(ECHO_GRAMMAR) printf("expr -> vecexpr\n");
	 if(execute)
	   {
	     if(INTERACTIVE)
	       for(int i=0;i<$1->size;i++)
		 {
		   printf(" %.10g ",$1->data[i]);
		 }
	     $$ = 0;
	   } 
       }
     | assignment 
       { // check type
	 if(ECHO_GRAMMAR) printf("expr -> assignment\n");
	 if(execute)
	   {
	     if(INTERACTIVE) {
	       if($1->type == _ARRAY)
		 {
		   for(list<double>::iterator it = $1->array.begin();
		       it!=$1->array.end();it++)
		     printf ("\t%.10g", (*it));
		   printf("\n");
		 }
	       else
		 printf ("\t%.10g\n", $1->value);
	     } 
	     $$=0;
	   }
       }
;

aexpr :  NUMBER               { $$ = $1;                         }
       | VARIABLE             
         { 
	   //check type ??
	   $$ = $1->value;        
          } 
       | FUNC '(' aexpr ')'   { $$ = (*($1->funcptr))($3);       } 
       | aexpr '+' aexpr      { $$ = $1 + $3;                    }
       | aexpr '-' aexpr      { $$ = $1 - $3;                    }  
       | aexpr '*' aexpr      { $$ = $1 * $3;                    }
       | aexpr '/' aexpr      { $$ = $1 / $3;                    }
       | aexpr '^' aexpr      { $$ = pow($1,$3);                 }
       | '-' aexpr  %prec UMINUS { $$ = -$2; }
       | '+' aexpr  %prec UPLUS { $$ = $2; }
       | '(' aexpr ')'         { $$ = $2;                         }
       | '<' vecexpr ',' vecexpr '>' // scalar product
         {
	   if($2->size == $4->size)
	     {
	       $$ = 0;
	       for(int i=0;i<$2->size;i++)
		 $$ += $2->data[i] * $4->data[i];
	     }
	   else
	     {
	       if(VERBOSE) printf("vector dimensions do not match");
	       $$ = _undefined;
	     }
         } 
       // boolean stuff
        | aexpr '<' aexpr { $$ = ($1 < $3 )? 1 : 0; } 
        | aexpr LE aexpr { $$ = ($1 <= $3 )? 1 : 0; } 
        | aexpr '>' aexpr { $$ = ($1 > $3 )? 1 : 0; } 
        | aexpr GE aexpr { $$ = ($1 >= $3 )? 1 : 0; } 
        | aexpr NE aexpr { $$ = ($1 != $3 )? 1 : 0; } 
	| aexpr EQ aexpr { $$ = ($1 == $3 )? 1 : 0; }
        | VARIABLE '[' VARIABLE ']' 
          { 
	    if(ECHO_GRAMMAR) printf("aexpr-> %s [ %s ]\n ",$1->name, $3->name); 
	    $$ = property_lookup($1->name,$3->name);
	  }// element attributes
 ; 

assignment :  VARIABLE '=' aexpr  
              {
		if(ECHO_GRAMMAR) printf("%s \n",$1->name);
		if(execute)
		  {
		    if($1->is_reserved)
		      printf("%s is reserved",$1->name);
		    else
		      {
			$1->value = $3; $$=$1;       
		      }
		  }
	      }
           |  VARIABLE '=' vecexpr
              {
		if(execute)
		  {
		    $1->array.erase($1->array.begin(),$1->array.end());
		    for(int i=0;i<$3->size;i++)
		      $1->array.push_back($3->data[i]);
		    $1->type = _ARRAY;
		    $$ = $1;
		    delete[] $3->data;
		    $3->size = 0;
		  }
              }

           |  VECVAR '=' vecexpr
              {
		if(execute)
		  {
		    $1->array.erase($1->array.begin(),$1->array.end());
		    for(int i=0;i<$3->size;i++)
		      $1->array.push_back($3->data[i]);
		    $$ = $1;
		    delete[] $3->data;
		    $3->size = 0;
		  }
              }
;

vecexpr :   VECVAR  
        {
	  if(execute)
	    {
	      $$ = new struct Array;
	      $$->data = new double[$1->array.size()];
	      $$->size = $1->array.size();
	      //array_list.push_back($$);
	      list<double>::iterator it;
	      int i = 0;
	      for(it=$1->array.begin();it!=$1->array.end();it++)
		{
		  $$->data[i++] = (*it);
		}
	    }
        } 
        | vectnum
        {
	  if(execute)
	    {
	      $$ = new struct Array;
	      $$->data = new double[$1->size];
	      $$->size = $1->size;
	      //array_list.push_back($$);
	      
	      for(int i=0;i<$1->size;i++)
		{
		  $$->data[i] = $1->data[i];
		}
	      
	      // erase data in vect
	      
	      delete[] $1->data;
	      $1->size = 0;
	    }
	}
       | vectstr
	{
	  if(execute)
	  {
	    $$ = new struct Array;
	    $$->size = $1->size;
	    $$->symbols = $1->symbols;

	    $1->symbols.clear();
	    $1->size = 0;
	  }
	}

       | vecexpr '+' vecexpr
        {
	  if(execute)
	    {
	      $$ = new struct Array;
	      $$->size = ($1->size < $3->size )? $1->size : $3->size;
	      $$->data = new double[$$->size];
	      //array_list.push_back($$);
	      
	      for(int i=0;i<$$->size;i++)
		{
		  $$->data[i] = $1->data[i] + $3->data[i];
		}
	      
	      
	      // erase data in vect
	      
	      delete[] $1->data;
	      delete[] $3->data;
	      $1->size = 0;
	      $3->size = 0;
	    }
        }
      | vecexpr '-' vecexpr
        {
	  if(execute)
	    {
	      $$ = new struct Array;
	      $$->size = ($1->size < $3->size )? $1->size : $3->size;
	      $$->data = new double[$$->size];
	      //array_list.push_back($$);
	      
	      for(int i=0;i<$$->size;i++)
		{
		  $$->data[i] = $1->data[i] - $3->data[i];
		}
	      
	      
	      // erase data in vect
	      
	      delete[] $1->data;
	      delete[] $3->data;
	      $1->size = 0;
	      $3->size = 0;
	    }
	}
       | vecexpr '+' aexpr
        {
	  if(execute)
	    {
	      $$ = new struct Array;
	      $$->size = $1->size;
	      $$->data = new double[$$->size];
	      //array_list.push_back($$);
	      
	      for(int i=0;i<$$->size;i++)
		{
		  $$->data[i] = $1->data[i] + $3;
		}
	      
	      // erase data in vect
	      
	      delete[] $1->data;
	      $1->size = 0;
	    }
	}

      | vecexpr '*' aexpr
        {
	  if(execute)
	    {
	      $$ = new struct Array;
	      $$->size = $1->size;
	      $$->data = new double[$$->size];
	      //array_list.push_back($$);
	      
	      for(int i=0;i<$$->size;i++)
		{
		  $$->data[i] = $1->data[i] * $3;
		}
	      
	      // erase data in vect
	      
	      delete[] $1->data;
	      $1->size = 0;
	    }
	}
      | vecexpr '/' aexpr
        {
	  if(execute)
	    {
	      $$ = new struct Array;
	      $$->size = $1->size;
	      $$->data = new double[$$->size];
	      //array_list.push_back($$);
	      
	      for(int i=0;i<$$->size;i++)
		{
		  $$->data[i] = $1->data[i] / $3;
		}
	      
	      // erase data in vect
	      
	      delete[] $1->data;
	      $1->size = 0;
	    }
	}
       | aexpr '+' vecexpr
        {
	  if(execute)
	    {
	      $$ = new struct Array;
	      $$->size = $3->size;
	      $$->data = new double[$$->size];
	      //array_list.push_back($$);
	      
	      for(int i=0;i<$$->size;i++)
		{
		  $$->data[i] = $3->data[i] + $1;
		}
	      
	      // erase data in vect
	      
	      delete[] $3->data;
	      $3->size = 0;
	    }
	}
       | aexpr '-' vecexpr
        {
	  if(execute)
	    {
	      $$ = new struct Array;
	      $$->size = $3->size;
	      $$->data = new double[$$->size];
	      //array_list.push_back($$);
	      
	      for(int i=0;i<$$->size;i++)
		{
		  $$->data[i] = $1 - $3->data[i];
		}
	      
	      // erase data in vect
	      
	      delete[] $3->data;
	      $3->size = 0;
	    }
	}
      | aexpr '*' vecexpr
        {
	  if(execute)
	    {
	      $$ = new struct Array;
	      $$->size = $3->size;
	      $$->data = new double[$$->size];
	      //array_list.push_back($$);
	      
	      for(int i=0;i<$$->size;i++)
		{
		  $$->data[i] = $1 * $3->data[i];
		}
	      
	      // erase data in vect
	      
	      delete[] $3->data;
	      $3->size = 0;
	    }
	}

;

vectnum : '{' numbers '}' 
	  {
	    if(execute)
	      {
	        //printf("matched vector of size %d\n",_tmparray.size());
	        $$ = new struct Array;
	        $$->data = new double[_tmparray.size()];
	        $$->size = _tmparray.size();
      
	        //array_list.push_back(a);
      
	        list<double>::iterator it;
		int i=0;      
	        for(it=_tmparray.begin();it!=_tmparray.end();it++)
	  	{
	 	 $$->data[i++] = (*it);
		}
    	        _tmparray.erase(_tmparray.begin(),_tmparray.end());

	        list<char*>::iterator lIter;
	        for(lIter = _tmpstring.begin(); lIter != _tmpstring.end(); lIter++)
	          $$->symbols.push_back(*lIter);

	        _tmpstring.clear();
	      }
	}
;

vectstr : '[' letters ']'
	{
	  if(execute)
	  {
	    $$ = new struct Array;
	    $$->size = _tmpstring.size();

	    list<char*>::iterator iter;
	    for(iter = _tmpstring.begin(); iter != _tmpstring.end(); iter++)
	      $$->symbols.push_back(*iter);

	    _tmpstring.clear();
	  }
	}
;

numbers : 
        | aexpr ',' numbers 
          {
	    if(execute)
	      _tmparray.push_front($1);
          } 
       | aexpr
         {
	   if(execute)
	     _tmparray.push_front($1);
        }
;

letters :
	| STR ',' letters
          {
            if(execute)
              _tmpstring.push_front($1);
          }
	| STR
         {
           if(execute)
             _tmpstring.push_front($1);
         }
;

command : STOP             { if(execute) quit(); }
        | BEAM ',' beam_parameters
        | PRINT            { if(execute) print( element_list ); }
        | PRINT ',' LINE   { if(execute) print( beamline_list); }
        | PRINT ',' OPTION { if(execute) print(options); }
        | PRINT ',' VARIABLE 
          {
	    if(execute)
	      {
		printf("\t");
		printf("\t%.10g\n",$3->value);
	      }
	  } 
        | PRINT ',' VECVAR 
          {
	    if(execute)
	      {
		printf("\t");
		
		list<double>::iterator it;
		for(it=$3->array.begin();it!=$3->array.end();it++)
		  {
		    printf("  %.10g ",(*it));
		  }
		
		printf("\n");
	      } 
	  }
        | USE ',' use_parameters { if(execute) expand_line(current_line,current_start, current_end);}
        | OPTION  ',' option_parameters
	| ECHO STR { if(execute) printf("%s\n",$2); }
        | SAMPLE ',' sample_options 
          {
	    if(execute)
	      {  
		if(ECHO_GRAMMAR) printf("command -> SAMPLE\n");
		add_sampler($3->name,$3->name, element_count);
		element_count = 1;
		params.flush();
	      }
          }
        | CSAMPLE ',' csample_options // cylindrical sampler
          {
	    if(execute)
	      {  
		if(ECHO_GRAMMAR) printf("command -> CSAMPLE\n");
//SPM		add_csampler("sampler",$3->name, element_count,params.l, params.r);
		add_csampler($3->name,$3->name, element_count,params.l, params.r);
		element_count = 1;
		params.flush();
	      }
          }
        | GAS ',' gas_options // beampipe gas
          {
	    if(execute)
	      {  
		if(ECHO_GRAMMAR) printf("command -> GAS\n");
		add_gas("gas",$3->name, element_count, params.material);
		element_count = 1;
		params.flush();
	      }
          }
        | TUNNEL ',' parameters // tunnel
          {
	    if(execute)
	      {  
		if(ECHO_GRAMMAR) printf("command -> TUNNEL\n");
		write_table(params,"tunnel",_TUNNEL);
		params.flush();
	      }
          }
        | BETA0 ',' option_parameters // beta 0 (is a synonim of option, for clarity)
          {
	    if(execute)
	      {  
		if(ECHO_GRAMMAR) printf("command -> BETA0\n");
	      }
          }
        | TWISS ',' option_parameters // twiss (again, is a synonim of option, for clarity)
          {
	    if(execute)
	      {
		set_value("doTwiss",1);
		if(ECHO_GRAMMAR) printf("command -> TWISS\n");
	      }
          }
        | DUMP ',' sample_options //  options for beam dump 
          {                                                   
            if(execute)                                       
              {                                               
                if(ECHO_GRAMMAR) printf("command -> DUMP\n"); 
                add_dump($3->name,$3->name, element_count);     
                element_count = 1;                            
                params.flush();                               
              }                                               
          }                                                   

//| PRINTF '(' fmt ')' { if(execute) printf($3,$5); }
;

use_parameters :  VARIABLE
                  {
		    if(execute)
		      {
			$$ = $1->name;
			current_line = $1->name;
			current_start = NULL;
			current_end = NULL;
		      }
                  }
		| PERIOD '=' VARIABLE
                  {
		    if(execute)
		      {
			$$ = $3->name;
			current_line = $3->name;
			current_start = NULL;
			current_end = NULL;
		      }
                  }
                | PERIOD '=' VARIABLE ',' RANGE '=' VARIABLE '/' VARIABLE
                  {
		    if(execute)
		      {
			$$ = $3->name;
			current_line = $3->name;
			current_start = $7->name;
			current_end = $9->name;
		      }
		  }
;



sample_options: RANGE '=' VARIABLE
                { if(ECHO_GRAMMAR)  printf("sample_opt : RANGE =  %s \n",$3->name);
		  {
		    if(execute) $$ = $3;
		  }
                }
              | RANGE '=' VARIABLE '[' NUMBER ']'
                {
                  if(ECHO_GRAMMAR) printf("sample_opt : RANGE =  %s [%.10g] \n",$3->name,$5);
		    {
		      if(execute) { $$ = $3; element_count = (int)$5; }
		    }
                }
;



csample_options : VARIABLE '=' aexpr
                  {
		    if(ECHO_GRAMMAR) printf("csample_opt ->csopt , %s =  %.10g \n",$1->name,$3);
		    
		    if(execute)
		      {
			if( !strcmp($1->name,"r") ) params.r = $3;
			else if (!strcmp($1->name,"l") ) params.l = $3;
			else if(VERBOSE) 
			  printf("Warning : CSAMPLER: unknown parameter %s \n",$1->name);
		      }
		  }   
                | VARIABLE '=' STR
                  {
		    if(ECHO_GRAMMAR) printf("csample_opt -> %s =  %s \n",$1->name,$3);
		    /* if(execute) */
		    /*   { */
		    /* 	;//set_value($1->name,string($3)); */
		    /*   } */
		  }   
                | VARIABLE '=' aexpr ',' csample_options
                  {
		    if(ECHO_GRAMMAR) printf("csample_opt ->csopt , %s =  %.10g \n",$1->name,$3);
		    
		    if(execute)
		      {
			if( !strcmp($1->name,"r") ) params.r = $3;
			else if (!strcmp($1->name,"l") ) params.l = $3;
			else if(VERBOSE) 
			  printf("Warning : CSAMPLER: unknown parameter %s at line\n",$1->name);
		      }

		  }   
                | VARIABLE '=' STR ',' csample_options
                  {
		    if(ECHO_GRAMMAR) printf("csample_opt -> %s =  %s \n",$1->name,$3);
		    // if(execute) //set_value($1->name,string($3));
		  }   
                | sample_options ',' csample_options
                  {
		    if(ECHO_GRAMMAR) printf("csample_opt -> sopt, csopt\n");
		    $$ = $1;
		  }
                | sample_options
                  {
		    if(ECHO_GRAMMAR) printf("csample_opt -> sopt\n");
		    $$ = $1;
                  }
;

gas_options : VARIABLE '=' aexpr
                  {
		    if(ECHO_GRAMMAR) printf("gas_opt -> , %s =  %.10g \n",$1->name,$3);
		    
		    if(execute)
		      {
			if( !strcmp($1->name,"r") ) params.r = $3;
			else if (!strcmp($1->name,"l") ) params.l = $3;
			else if(VERBOSE) 
			  printf("Warning : GAS: unknown parameter %s \n",$1->name);
		      }
		  }   
                | VARIABLE '=' STR
                  {
		    if(ECHO_GRAMMAR) printf("gas_opt -> %s =  %s \n",$1->name,$3);
		    if(execute)
		      {
			if( !strcmp($1->name,"material") ) 
			  {
			    strcpy(params.material ,$3);
			    params.materialset = 1;
			  }
			//set_value($1->name,string($3));
		      }
		  }   
                | VARIABLE '=' aexpr ',' gas_options
                  {
		    if(ECHO_GRAMMAR) printf("gas_opt -> , %s =  %.10g \n",$1->name,$3);
		    
		    if(execute)
		      {
			if( !strcmp($1->name,"r") ) params.r = $3;
			else if (!strcmp($1->name,"l") ) params.l = $3;
			else if(VERBOSE) 
			  printf("Warning : GAS: unknown parameter %s at line\n",$1->name);
		      }

		  }   
                | VARIABLE '=' STR ',' gas_options
                  {
		    if(ECHO_GRAMMAR) printf("csample_opt -> %s =  %s \n",$1->name,$3);
		    if(execute)
		      {
			  if( !strcmp($1->name,"material") ) 
			    {
			      strcpy(params.material ,$3);
			      params.materialset = 1;
			    }
		      }
		  }   
                | RANGE '='  VARIABLE '/' VARIABLE ',' gas_options
                  {
		    if(ECHO_GRAMMAR) printf("gas_opt -> range, csopt\n");

		  }
                | RANGE '='  VARIABLE '/' VARIABLE
                  {
		    if(ECHO_GRAMMAR) printf("gas_opt -> range\n");

                  }
                | RANGE '='  VARIABLE ',' gas_options
                  {
		    if(ECHO_GRAMMAR) printf("gas_opt -> range\n");
		    $$ = $3;
		  }
                | RANGE '='  VARIABLE
                  {
		    if(ECHO_GRAMMAR) printf("gas_opt -> range\n");
		    $$ = $3;
                  }
;

option_parameters : 
                  | VARIABLE '=' aexpr ',' option_parameters
                    {
		      if(execute)
			set_value($1->name,$3);
		    }   
                  | VARIABLE '=' aexpr
                    {
		      if(execute)
			set_value($1->name,$3);
		    } 
                  | VARIABLE '=' STR ',' option_parameters
                    {
		      if(execute)
			set_value($1->name,std::string($3));
		    }   
                  | VARIABLE '=' STR
                    {
		      if(execute)
			set_value($1->name,std::string($3));
		    }   
;

beam_parameters :
                | VARIABLE '=' aexpr ',' beam_parameters
                  {
		    if(execute)
		      set_value($1->name,$3);
		  }   
                | VARIABLE '=' aexpr
                  {
		    if(execute)
		      set_value($1->name,$3);
		  }   
                | VARIABLE '=' STR ',' beam_parameters
                  {
		    if(execute)
		      set_value($1->name,string($3));
		  }   
                | VARIABLE '=' STR
                  {
		    if(execute)
		      set_value($1->name,string($3));
		  }   
;

%%



int yyerror(const char *s)
{
  printf("%s at line %d , file %s\n",s, line_num, yyfilename);
  exit(1);
}

#ifdef __cplusplus
extern "C" {
#endif
int yywrap()
{
	return 1;
}
#ifdef __cplusplus
}
#endif
