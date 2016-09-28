/*
   bison grammar for the gmad parser
*/

%{

#include "array.h"
#include "parser.h"
#include "sym_table.h"
#include "elementtype.h"
#include <cmath>
#include <cstring>
#include <iostream>
#include <string>
  
  using namespace GMAD;

  extern char* yytext;

  namespace GMAD {
    extern int line_num;
    extern std::string yyfilename;
  
    const int ECHO_GRAMMAR = 0; ///< print grammar rule expansion (for debugging)
    const int INTERACTIVE = 0; ///< print output of commands (like in interactive mode)
    /* for more debug with parser:
       1) set yydebug to 1 in parser.tab.cc (needs to be reset as this file gets overwritten from time to time!) 
       2) add %debug below
       3) compile bison with "-t" flag. This is automatically done when CMAKE_BUILD_TYPE equals Debug
    */

    bool execute = true;
    int element_count = -1; // for samplers , ranges etc. -1 means add to all
    ElementType element_type = ElementType::_NONE; // for samplers, ranges etc.
  }
%}

/* define stack type */

%union{
  double dval;
  int ival; // ElementType, but underlying type as it is not possible to have enum class in union, rely on static_casts
  GMAD::Symtab *symp;
  std::string *str;
  GMAD::Array *array;
}

/* more debug output can be added with %debug" */
//%debug

// change associativity of operators
%left '+' '-'
%left '*' '/'
%left '^' '<' '>' NE LE GE EQ
%nonassoc UMINUS
%nonassoc UPLUS

// declare union value of tokens and types
%token <dval> NUMBER
%token <symp> NUMVAR STRVAR VECVAR FUNC
%token <str> STR VARIABLE
%token <ival> MARKER ELEMENT DRIFT RF RBEND SBEND QUADRUPOLE SEXTUPOLE OCTUPOLE DECAPOLE MULTIPOLE SCREEN AWAKESCREEN AWAKESPECTROMETER THINMULT
%token <ival> SOLENOID RCOL ECOL LINE LASER TRANSFORM3D MUSPOILER DEGRADER
%token <ival> VKICK HKICK
%token ALL ATOM MATERIAL PERIOD XSECBIAS REGION FIELD CAVITYMODEL TUNNEL
%token BEAM OPTION PRINT RANGE STOP USE SAMPLE CSAMPLE
%token IF ELSE BEGN END LE GE NE EQ FOR

%type <dval> aexpr
%type <dval> expr
%type <symp> assignment symdecl
%type <array> vecexpr
%type <array> vectnum vectstr
%type <str> use_parameters
%type <ival> component component_with_params newinstance
%type <str> sample_options
%type <str> csample_options
%type <str> paramassign string

/* printout format for debug output */
/*
%printer { fprintf (yyoutput, "%.10g", $$); } <dval>
%printer { fprintf (yyoutput, "%d", $$); } <ival>
%printer { fprintf (yyoutput, "\"%s\"", $$); } <str>
%printer { fprintf (yyoutput, "\"%s\"", $$->name); } <symp>
%printer { fprintf (yyoutput, "size %d, &%p", $$->size, (void*)$$->data); } <array>
%printer { fprintf (yyoutput, "<>"); } <>
*/

%%

// every statement ends in a semicolon 
input : 
      | input stmt ';'
       { 
	 if(ECHO_GRAMMAR) printf("input -> input stmt ';' \n");
       }

// deconstruct statements into atomic statements
stmt :        if_clause '{' stmt '}'
              {
		if(ECHO_GRAMMAR) printf("stmt -> IF '(' aexpr ')' stmt\n" );
		execute = true;
	      }
              | if_clause '{' stmt '}' else_clause '{' stmt '}' 
	      {
		if(ECHO_GRAMMAR) printf("stmt -> IF '(' bool_expr ')' ELSE stmt \n" );
		execute = true;
	      }
              | atomic_stmt { if(ECHO_GRAMMAR) printf("stmt -> atomic_stmt \n"); }
              | BEGN input END { if(ECHO_GRAMMAR) printf("stmt -> '{' stmt ';' atomic_stmt '}' \n"); }

if_clause: IF '(' aexpr ')' {if( ($3 > 0) && execute ) execute = true; else execute = false;}

else_clause: ELSE
             {
	       if (execute) {execute = false;}
	       else {execute = true;}
             }

// atomic statements can be an mathematical expression, a declaration or a command
atomic_stmt : 
            | expr { if(ECHO_GRAMMAR) printf("atomic_stmt -> expr\n"); }
            | command  { if(ECHO_GRAMMAR) printf("atomic_stmt -> command\n"); }
            | decl  { if(ECHO_GRAMMAR) printf("atomic_stmt -> decl\n"); }

// instantiate an object
decl : VARIABLE ':' component_with_params
       {
	 if(execute) {
	   if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *$1 << " : " << $3 << std::endl;
	   // check parameters and write into element table
	   Parser::Instance()->write_table($1,static_cast<ElementType>($3));
	   Parser::Instance()->ClearParams();
	 }
       }
      | VARIABLE ':' MARKER
       {
	 if(execute) {
	   if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *$1 << " : " << ElementType::_MARKER << std::endl;
	   // check parameters and write into element table
	   Parser::Instance()->write_table($1,ElementType::_MARKER);
	   Parser::Instance()->ClearParams();
	 }
       }

     | VARIABLE ':' line 
       {
	 if(execute)
	   {
	     // copy tmp_list to params
	     Parser::Instance()->write_table($1,ElementType::_LINE,true);
	   }
       }
     | VARIABLE ':' newinstance
       {
         if(execute)
	   {
	     ElementType type = static_cast<ElementType>($3);
	     if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE : VARIABLE, " << *($1) << " : " << type << std::endl;
	     if(type != ElementType::_NONE)
	       {
		 Parser::Instance()->write_table($1,type);
	       }
	     Parser::Instance()->ClearParams();
	   }
       }
       | VARIABLE ':' parameters
       {
	 if(execute)
	   {
	     if(ECHO_GRAMMAR) std::cout << "edit : VARIABLE parameters   -- " << *($1) << std::endl;
	     Parser::Instance()->OverwriteElement(*$1);
	   }
       }
     | VARIABLE ':' atom
       {
         if(execute)
           {
	     if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : atom" << std::endl;
	     Parser::Instance()->SetValue<Atom>("name",*($1));
	     Parser::Instance()->Add<Atom>();
           }
       }
     | VARIABLE ':' material
       {
         if(execute)
           {
	     if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : material" << std::endl;
	     Parser::Instance()->SetValue<Material>("name",*($1));
	     Parser::Instance()->Add<Material>();
           }
       }
     | VARIABLE ':' tunnel
       {
         if(execute)
           {
	     if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : tunnel" << std::endl;
	     Parser::Instance()->SetValue<Tunnel>("name",*($1));
	     Parser::Instance()->Add<Tunnel>();
           }
       }
     | VARIABLE ':' region
       {
         if(execute)
           {
	     if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : region" << std::endl;
	     Parser::Instance()->SetValue<Region>("name",*($1));
	     Parser::Instance()->Add<Region>();
           }
       }
     | VARIABLE ':' field
       {
	 if(execute)
	   {
	     if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : field" << std::endl;
	     Parser::Instance()->SetValue<Field>("name", *($1));
	     Parser::Instance()->Add<Field>();
	   }
       }
     | VARIABLE ':' cavitymodel
       {
         if(execute)
           {
	     if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : cavitymodel" << std::endl;
	     Parser::Instance()->SetValue<CavityModel>("name",*($1));
	     Parser::Instance()->Add<CavityModel>();
           }
       }
     | VARIABLE ':' xsecbias
       {
         if(execute)
           {
	     if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : xsecbias" << std::endl;
	     Parser::Instance()->SetValue<PhysicsBiasing>("name",*($1));
	     Parser::Instance()->Add<PhysicsBiasing,FastList<PhysicsBiasing>>();
           }
       }
      | VARIABLE ':' error_noparams
      {
	if(execute)
	  {
	    yyerror("ERROR: Element needs parameters");
	  }
      }

component_with_params : component ',' parameters

component : DRIFT       {$$=static_cast<int>(ElementType::_DRIFT);}
          | RF          {$$=static_cast<int>(ElementType::_RF);}
          | SBEND       {$$=static_cast<int>(ElementType::_SBEND);}
          | RBEND       {$$=static_cast<int>(ElementType::_RBEND);}
          | VKICK       {$$=static_cast<int>(ElementType::_VKICK);}
          | HKICK       {$$=static_cast<int>(ElementType::_HKICK);}
          | QUADRUPOLE  {$$=static_cast<int>(ElementType::_QUAD);}
          | SEXTUPOLE   {$$=static_cast<int>(ElementType::_SEXTUPOLE);}
          | OCTUPOLE    {$$=static_cast<int>(ElementType::_OCTUPOLE);}
          | DECAPOLE    {$$=static_cast<int>(ElementType::_DECAPOLE);}
          | MULTIPOLE   {$$=static_cast<int>(ElementType::_MULT);}
          | THINMULT    {$$=static_cast<int>(ElementType::_THINMULT);}
          | SOLENOID    {$$=static_cast<int>(ElementType::_SOLENOID);}
          | ECOL        {$$=static_cast<int>(ElementType::_ECOL);}
          | RCOL        {$$=static_cast<int>(ElementType::_RCOL);}
          | MUSPOILER   {$$=static_cast<int>(ElementType::_MUSPOILER);}
          | DEGRADER    {$$=static_cast<int>(ElementType::_DEGRADER);}
          | LASER       {$$=static_cast<int>(ElementType::_LASER);}
          | SCREEN      {$$=static_cast<int>(ElementType::_SCREEN);}
          | AWAKESCREEN {$$=static_cast<int>(ElementType::_AWAKESCREEN);}
          | AWAKESPECTROMETER {$$=static_cast<int>(ElementType::_AWAKESPECTROMETER);}
          | TRANSFORM3D {$$=static_cast<int>(ElementType::_TRANSFORM3D);}
          | ELEMENT     {$$=static_cast<int>(ElementType::_ELEMENT);}

atom : ATOM ',' atom_options
material : MATERIAL ',' material_options
region : REGION ',' region_options
field : FIELD ',' field_options
cavitymodel : CAVITYMODEL ',' cavitymodel_options
tunnel : TUNNEL ',' tunnel_options
xsecbias : XSECBIAS ',' xsecbias_options

// every element needs parameters
error_noparams : DRIFT
               | RF
               | SBEND
               | RBEND
               | VKICK
               | HKICK
               | QUADRUPOLE
               | SEXTUPOLE
               | OCTUPOLE
               | DECAPOLE
               | MULTIPOLE
               | THINMULT
               | SOLENOID
               | ECOL
               | MUSPOILER
               | RCOL
               | LASER
               | SCREEN
               | AWAKESCREEN
               | AWAKESPECTROMETER
               | TRANSFORM3D
               | ELEMENT
               | MATERIAL
               | ATOM
               | REGION
               | FIELD
               | CAVITYMODEL
               | TUNNEL
               | XSECBIAS

newinstance : VARIABLE ',' parameters
            {
	      if(execute) {
		$$ = Parser::Instance()->copy_element_to_params(*$1);
	      }
	    }
            | VARIABLE
	    {
	      if(execute) {
		$$ = Parser::Instance()->copy_element_to_params(*$1);
	      }
	    }

paramassign: VARIABLE
             {
               $$=$1;
             }
           // allow defined variables to have the same name as parameters
           | NUMVAR
             {
               $$ = new std::string($1->GetName());
	       Parser::Instance()->AddVariable($$);
             }
           | STRVAR
             {
               $$ = new std::string($1->GetName());
	       // store to prevent leak
	       Parser::Instance()->AddVariable($$);
             }

// reduce STR and STRVAR
string: STR
        {
	  $$ = $1;
	}
        | STRVAR
	{
	  $$ = new std::string($1->GetString());
	  // store to prevent leak
	  Parser::Instance()->AddVariable($$);
	}

parameters_extend : /* nothing */
                  | ',' parameters

parameters: paramassign '=' aexpr parameters_extend
            {
	      if(execute)
		Parser::Instance()->SetValue<Parameters>(*($1),$3);
	    }
          | paramassign '=' vecexpr parameters_extend
            {
	      if(execute) 
		Parser::Instance()->SetValue<Parameters>(*($1),$3);
	    }
          | paramassign '=' string parameters_extend
            {
	      if(execute) {
		Parser::Instance()->SetValue<Parameters>(*($1),*$3);
	      }
	    }

line : LINE '=' '(' element_seq ')'

line : LINE '=' '-' '(' rev_element_seq ')'

element_seq_extend : /* nothing */
                   | ',' element_seq

element_seq : 
            | VARIABLE element_seq_extend
              {
		if(execute) Parser::Instance()->add_element_temp(*($1), 1, true, ElementType::_LINE);
	      }
            | VARIABLE '*' NUMBER element_seq_extend
              {
		if(execute) Parser::Instance()->add_element_temp(*($1), (int)$3, true, ElementType::_LINE);
	      }
            | NUMBER '*' VARIABLE element_seq_extend
              {
		if(execute) Parser::Instance()->add_element_temp(*($3), (int)$1, true, ElementType::_LINE);
	      }
            | '-' VARIABLE element_seq_extend
              {
		if(execute) Parser::Instance()->add_element_temp(*($2), 1, true, ElementType::_REV_LINE);
	      }

rev_element_seq_extend : /* nothing */
                       | ',' rev_element_seq

rev_element_seq : 
            | VARIABLE rev_element_seq_extend
              {
		if(execute) Parser::Instance()->add_element_temp(*($1), 1, false, ElementType::_REV_LINE);
	      }
            | VARIABLE '*' NUMBER rev_element_seq_extend
              {
		if(execute) Parser::Instance()->add_element_temp(*($1), int($3), false, ElementType::_REV_LINE);
	      }
            | NUMBER '*' VARIABLE rev_element_seq_extend
              {
		if(execute) Parser::Instance()->add_element_temp(*($3), int($1), false, ElementType::_REV_LINE);
	      }
            | '-' VARIABLE rev_element_seq_extend
              {
		if(execute) Parser::Instance()->add_element_temp((*$2), 1, false, ElementType::_LINE);
	      }

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
	       {$1->Print();}
	     $$ = 0;
	   } 
       }
     | assignment 
       { // check type
	 if(ECHO_GRAMMAR) printf("expr -> assignment\n");
	 if(execute)
	   {
	     if(INTERACTIVE) {
	       $1->Print();
	     } 
	     $$=0;
	   }
       }

aexpr  : NUMBER               { $$ = $1;                         }
       | NUMVAR               { $$ = $1->GetNumber();            }
       | FUNC '(' aexpr ')'   { $$ = $1->GetFunction()($3);      }
       | aexpr '+' aexpr      { $$ = $1 + $3;                    }
       | aexpr '-' aexpr      { $$ = $1 - $3;                    }  
       | aexpr '*' aexpr      { $$ = $1 * $3;                    }
       | aexpr '/' aexpr      { $$ = $1 / $3;                    }
       | aexpr '^' aexpr      { $$ = std::pow($1,$3);            }
       | '-' aexpr  %prec UMINUS { $$ = -$2; }
       | '+' aexpr  %prec UPLUS { $$ = $2; }
       | '(' aexpr ')'         { $$ = $2;                        }
       | '<' vecexpr ',' vecexpr '>' // scalar product
         {
	   $$ = $2->Product($4);
         } 
       // boolean stuff
        | aexpr '<' aexpr { $$ = ($1 < $3 )? 1 : 0; } 
        | aexpr LE aexpr { $$ = ($1 <= $3 )? 1 : 0; } 
        | aexpr '>' aexpr { $$ = ($1 > $3 )? 1 : 0; } 
        | aexpr GE aexpr { $$ = ($1 >= $3 )? 1 : 0; } 
        | aexpr NE aexpr { $$ = ($1 != $3 )? 1 : 0; } 
	| aexpr EQ aexpr { $$ = ($1 == $3 )? 1 : 0; }
        | VARIABLE '[' string ']' 
          { 
	    if(ECHO_GRAMMAR) std::cout << "aexpr-> " << *($1) << " [ " << *($3) << " ]" << std::endl; 
	    $$ = Parser::Instance()->property_lookup(*($1),*($3));
	  }// element attributes

symdecl : VARIABLE '='
        {
	  if(execute)
	    {
	      Symtab *sp = Parser::Instance()->symcreate(*($1));
	      $$ = sp;
	    }
	}
        | NUMVAR '='
	{
	  if(execute)
	    {
	      std::cout << "WARNING redefinition of variable " << $1->GetName() << " with old value: " << $1->GetNumber() << std::endl;
	      $$ = $1;
	    }
	}
        | STRVAR '='
	{
	  if(execute)
	    {
	      std::cout << "WARNING redefinition of variable " << $1->GetName() << " with old value: " << $1->GetString() << std::endl;
	      $$ = $1;
	    }
	}
        | VECVAR '='
        {
           if(execute)
	     {
	       std::cout << "WARNING redefinition of array variable " << $1->GetName() << std::endl;
	       $$=$1;
	     }
	}

assignment :  symdecl aexpr  
              {
		if(ECHO_GRAMMAR) std::cout << $1->GetName() << std::endl;
		if(execute)
		  {
		    if($1->IsReserved()) {
		      std::string errorstring = "ERROR: " + $1->GetName() + " is reserved\n";
		      yyerror(errorstring.c_str());
		    }
		    else
		      {
			$1->Set($2);
			$$=$1;       
		      }
		  }
	      }
           |  symdecl STR
	      {
		if (execute)
		  {
		    if($1->IsReserved()) {
		      std::string errorstring = "ERROR: " + $1->GetName() + " is reserved\n";
		      yyerror(errorstring.c_str());
		    }
		    else
		      {
			$1->Set(*$2);
			$$=$1;
		      }
		  }
	      }
           |  symdecl vecexpr
              {
		if(execute)
		  {
		    $1->Set($2);
		    $$=$1;
		  }
              }

vecexpr : VECVAR              {if(execute) $$ = new Array($1);} 
        | vectnum             {if(execute) $$ = $1;}
        | vectstr             {if(execute) $$ = $1;}
        | vecexpr '+' vecexpr {if(execute) $$ = Array::Add($1,$3);}
        | vecexpr '-' vecexpr {if(execute) $$ = Array::Subtract($1,$3);}
        | vecexpr '+' aexpr   {if(execute) $$ = Array::Add($1,$3);}
        | aexpr   '+' vecexpr {if(execute) $$ = Array::Add($3,$1);}
        | vecexpr '*' aexpr   {if(execute) $$ = Array::Multiply($1,$3);}
        | aexpr   '*' vecexpr {if(execute) $$ = Array::Multiply($3,$1);}
        | vecexpr '/' aexpr   {if(execute) $$ = Array::Divide($1,$3);}
        | vecexpr '-' aexpr   {if(execute) $$ = Array::Subtract($1,$3);}
        | aexpr '-' vecexpr
        {
	  if(execute)
	    {
	      Array* a = Array::Multiply($3,-1);
	      $$ = Array::Add(a,$1);
	    }
	}

vectnumexec : '{' numbers '}'
            | '[' numbers ']'

vectnum : vectnumexec
	  {
	    if(execute)
	      {
	        $$ = new Array;
	        Parser::Instance()->FillArray($$);
	        Parser::Instance()->FillString($$);		
	      }
	  }

vectstrexec : '[' letters ']'
            | '{' letters '}'

vectstr : vectstrexec
	{
	  if(execute)
	  {
	    $$ = new Array;
	    Parser::Instance()->FillString($$);
	  }
	}

numbers : aexpr ',' numbers { if(execute) Parser::Instance()->Store($1);} 
        | aexpr             { if(execute) Parser::Instance()->Store($1);}

letters : string ',' letters { if(execute) Parser::Instance()->Store(*$1);}
        | string             { if(execute) Parser::Instance()->Store(*$1);}

// accept print with and without comma
print   : PRINT
        | PRINT ','

command : STOP             { if(execute) Parser::Instance()->quit(); }
        | BEAM ',' beam_parameters
        | print        { if(execute) Parser::Instance()->PrintElements(); }
        | print LINE   { if(execute) Parser::Instance()->PrintBeamline(); }
        | print OPTION { if(execute) Parser::Instance()->PrintOptions(); }
        | print VARIABLE
          {
	    if(execute) {
	      Symtab *sp = Parser::Instance()->symlook(*($2));
	      if (!sp) {
		std::cout << "Variable " << *($2) << " not defined!" << std::endl;
	      }
	      else {
		sp->Print();
	      }
	    }
	  }
        | print NUMVAR { if(execute) $2->Print();}
        | print STRVAR { if(execute) $2->Print();}
        | print VECVAR { if(execute) $2->Print();}
        | USE ',' use_parameters { if(execute) Parser::Instance()->expand_line(Parser::Instance()->current_line,Parser::Instance()->current_start, Parser::Instance()->current_end);}
        | OPTION  ',' option_parameters
        | SAMPLE ',' sample_options 
          {
	    if(execute)
	      {  
		if(ECHO_GRAMMAR) printf("command -> SAMPLE\n");
		Parser::Instance()->add_sampler(*($3), element_count, element_type);
		element_count = -1;
		Parser::Instance()->ClearParams();
	      }
          }
        | CSAMPLE ',' csample_options // cylindrical sampler
          {
	    if(execute)
	      {  
		if(ECHO_GRAMMAR) printf("command -> CSAMPLE\n");
		Parser::Instance()->add_csampler(*($3), element_count, element_type);
		element_count = -1;
		Parser::Instance()->ClearParams();
	      }
          }
        | ATOM ',' atom_options // atom
          {
	    if(execute)
	      {  
		if(ECHO_GRAMMAR) printf("command -> ATOM\n");
		Parser::Instance()->Add<Atom>();
	      }
          }
        | MATERIAL ',' material_options // material
          {
	    if(execute)
	      {  
		if(ECHO_GRAMMAR) printf("command -> MATERIAL\n");
		Parser::Instance()->Add<Material>();
	      }
          }
        | TUNNEL ',' tunnel_options // tunnel
          {
	    if(execute)
	      {  
		if(ECHO_GRAMMAR) printf("command -> TUNNEL\n");
		Parser::Instance()->Add<Tunnel>();
	      }
          }
        | REGION ',' region_options // region
          {
	    if(execute)
	      {  
		if(ECHO_GRAMMAR) printf("command -> REGION\n");
		Parser::Instance()->Add<Region>();
	      }
          }
        | FIELD ',' field_options // field
	  {
	    if(execute)
	      {
		if(ECHO_GRAMMAR) printf("command -> FIELD\n");
		Parser::Instance()->Add<Field>();
	      }
	  }
        | CAVITYMODEL ',' cavitymodel_options // cavitymodel
          {
	    if(execute)
	      {  
		if(ECHO_GRAMMAR) printf("command -> CAVITYMODEL\n");
		Parser::Instance()->Add<CavityModel>();
	      }
          }
        | XSECBIAS ',' xsecbias_options // xsecbias
          {
	    if(execute)
	      {  
		if(ECHO_GRAMMAR) printf("command -> XSECBIAS\n");
		Parser::Instance()->Add<PhysicsBiasing,FastList<PhysicsBiasing>>();
	      }
          }

use_parameters :  VARIABLE
                  {
		    if(execute)
		      {
			$$ = $1;
			Parser::Instance()->current_line = (*$1);
			Parser::Instance()->current_start = "";
			Parser::Instance()->current_end = "";
		      }
                  }
		| PERIOD '=' VARIABLE
                  {
		    if(execute)
		      {
			$$ = $3;
			Parser::Instance()->current_line = *($3);
			Parser::Instance()->current_start = "";
			Parser::Instance()->current_end = "";
		      }
                  }
                | PERIOD '=' VARIABLE ',' RANGE '=' VARIABLE '/' VARIABLE
                  {
		    if(execute)
		      {
			$$ = $3;
			Parser::Instance()->current_line = *($3);
			Parser::Instance()->current_start = *($7);
			Parser::Instance()->current_end = *($9);
		      }
		  }

sample_options: RANGE '=' VARIABLE
                {
		  if(ECHO_GRAMMAR) std::cout << "sample_opt : RANGE =  " << *($3) << std::endl;
		  if(execute) $$ = $3;
                }
              | RANGE '=' VARIABLE '[' NUMBER ']'
                {
                  if(ECHO_GRAMMAR) std::cout << "sample_opt : RANGE =  " << *($3) << " [" << $5 << "]" << std::endl;
		  if(execute) { $$ = $3; element_count = (int)$5; }
                }
              | ALL
	        {
		  if(ECHO_GRAMMAR) std::cout << "sample_opt, all" << std::endl;
		  // -2: convention to add to all elements
		  // empty name so that element name can be attached
		  if(execute)
                  {
                    $$ = new std::string("");
		    element_count = -2;
		    element_type=ElementType::_NONE;
	          }
	        }
	      | component
		{
	          if(ECHO_GRAMMAR) std::cout << "sample_opt, all " << typestr(static_cast<ElementType>($1)) << std::endl;
	          if(execute) {
	             element_type = static_cast<ElementType>($1);
		     element_count = -2;
		     $$ = new std::string("");
	          }
	        }

csample_options_extend : /* nothing */
                       | ',' csample_options

csample_options : paramassign '=' aexpr csample_options_extend
                  {
		    if(ECHO_GRAMMAR) std::cout << "csample_opt ->csopt " << (*$1) << " = " << $3 << std::endl;
		    if(execute)
		      Parser::Instance()->SetValue<Parameters>(*($1),$3);
		  }
                | sample_options csample_options_extend
                  {
		    if(ECHO_GRAMMAR) printf("csample_opt -> sopt, csopt\n");
		    $$ = $1;
		  }

cavitymodel_options_extend : /* nothing */
                           | ',' cavitymodel_options

cavitymodel_options : paramassign '=' aexpr cavitymodel_options_extend
                    {
		      if(execute)
			Parser::Instance()->SetValue<CavityModel>((*$1),$3);
		    }
                 | paramassign '=' string cavitymodel_options_extend
                    {
		      if(execute)
			Parser::Instance()->SetValue<CavityModel>(*$1,*$3);
		    }

material_options_extend : /* nothing */
                        | ',' material_options

material_options : paramassign '=' aexpr material_options_extend
                    {
		      if(execute)
			Parser::Instance()->SetValue<Material>((*$1),$3);
		    }
                 | paramassign '=' string material_options_extend
                    {
		      if(execute)
			Parser::Instance()->SetValue<Material>(*$1,*$3);
		    }
                 | paramassign '=' vecexpr material_options_extend
                    {
		      if(execute) 
			Parser::Instance()->SetValue<Material>(*($1),$3);
		    }

atom_options_extend : /* nothing */
                      | ',' atom_options

atom_options : paramassign '=' aexpr atom_options_extend
                    {
		      if(execute)
			Parser::Instance()->SetValue<Atom>((*$1),$3);
		    }
                 | paramassign '=' string atom_options_extend
                    {
		      if(execute)
			Parser::Instance()->SetValue<Atom>(*$1,*$3);
		    }

region_options_extend : /* nothing */
                      | ',' region_options

region_options : paramassign '=' aexpr region_options_extend
                    {
		      if(execute)
			Parser::Instance()->SetValue<Region>((*$1),$3);
		    }
                 | paramassign '=' string region_options_extend
                    {
		      if(execute)
			Parser::Instance()->SetValue<Region>(*$1,*$3);
		    }

field_options_extend : /* nothing */
                      | ',' field_options

field_options : paramassign '=' aexpr field_options_extend
                    {
		      if(execute)
			Parser::Instance()->SetValue<Field>((*$1),$3);
		    }
                 | paramassign '=' string field_options_extend
                   {
		     if(execute)
		       Parser::Instance()->SetValue<Field>((*$1),*$3);
		   }

tunnel_options_extend : /* nothing */
                      | ',' tunnel_options

tunnel_options : paramassign '=' aexpr tunnel_options_extend
                    {
		      if(execute)
			Parser::Instance()->SetValue<Tunnel>((*$1),$3);
		    }
                 | paramassign '=' string tunnel_options_extend
                    {
		      if(execute)
			Parser::Instance()->SetValue<Tunnel>(*$1,*$3);
		    }

xsecbias_options_extend : /* nothing */
                        | ',' xsecbias_options

xsecbias_options : paramassign '=' aexpr xsecbias_options_extend
                    {
		      if(execute)
			Parser::Instance()->SetValue<PhysicsBiasing>(*$1,$3);
		    }
                 | paramassign '=' string xsecbias_options_extend
                    {
		      if(execute)
			Parser::Instance()->SetValue<PhysicsBiasing>(*$1,*$3);
		    }
                 | paramassign '=' vecexpr xsecbias_options_extend
		    {
		      if(execute)
			Parser::Instance()->SetValue<PhysicsBiasing>(*$1,$3);
		    }

option_parameters_extend : /* nothing */
                         | ',' option_parameters

option_parameters : paramassign '=' aexpr option_parameters_extend
                    {
		      if(execute)
			Parser::Instance()->SetValue<Options>(*$1,$3);
		    }   
                  | paramassign '=' string option_parameters_extend
                    {
		      if(execute)
			Parser::Instance()->SetValue<Options>(*$1,*$3);
		    }

// beam_parameter same as option_parameters, might change in future
beam_parameters : option_parameters

%%

int yyerror(const char *s)
{
  std::cout << s << " at line " << GMAD::line_num << " (might not be exact!), file " << yyfilename << std::endl;
  std::cout << "symbol '" << yytext << "' unexpected" << std::endl;
  exit(1);
}

extern "C" {
  int yywrap()
  {
    return 1;
  }
}
