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

    int execute = 1;
    int element_count = -1; // for samplers , ranges etc. -1 means add to all
    ElementType element_type = ElementType::_NONE; // for samplers, ranges etc.
  }
%}

/* define stack type */

%union{
  double dval;
  int ival; // ElementType, but underlying type as it is not possible to have enum class in union, rely on static_casts
  GMAD::Symtab *symp;
  std::string* str;
  GMAD::Array *array;
}

/* more debug output can be added with %debug" */
//%debug

%left '+' '-'
%left '*' '/'
%left '^' '<' '>' NE LE GE EQ
%nonassoc UMINUS
%nonassoc UPLUS

%token <dval> NUMBER
%token <symp> VECVAR FUNC
%token <str> STR VARIABLE
%token <ival> MARKER ELEMENT DRIFT RF RBEND SBEND QUADRUPOLE SEXTUPOLE OCTUPOLE DECAPOLE MULTIPOLE SCREEN AWAKESCREEN
%token <ival> SOLENOID RCOL ECOL LINE LASER TRANSFORM3D MUSPOILER DEGRADER
%token <ival> VKICK HKICK
%token <ival> MATERIAL ATOM
%token <dval> CONSTANT
%token ALL PERIOD XSECBIAS REGION CAVITYMODEL TUNNEL
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
     | VARIABLE ':' tunnel
       {
         if(execute)
           {
	     if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : tunnel" << std::endl;
	     Parser::Instance()->SetTunnelValue("name",*($1));
	     Parser::Instance()->add_tunnel();
           }
       }
     | VARIABLE ':' region
       {
         if(execute)
           {
	     if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : region" << std::endl;
	     Parser::Instance()->SetRegionValue("name",*($1));
	     Parser::Instance()->add_region();
           }
       }
     | VARIABLE ':' cavitymodel
       {
         if(execute)
           {
	     if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : cavitymodel" << std::endl;
	     Parser::Instance()->SetCavityModelValue("name",*($1));
	     Parser::Instance()->add_cavitymodel();
           }
       }
     | VARIABLE ':' xsecbias
       {
         if(execute)
           {
	     if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : xsecbias" << std::endl;
	     Parser::Instance()->SetPhysicsBiasValue("name",*($1));
	     Parser::Instance()->add_xsecbias();
           }
       }
      | VARIABLE ':' error_noparams
      {
	if(execute)
	  {
	    yyerror("ERROR: Element needs parameters");
	  }
      }
;

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
          | SOLENOID    {$$=static_cast<int>(ElementType::_SOLENOID);}
          | ECOL        {$$=static_cast<int>(ElementType::_ECOL);}
          | RCOL        {$$=static_cast<int>(ElementType::_RCOL);}
          | MUSPOILER   {$$=static_cast<int>(ElementType::_MUSPOILER);}
          | DEGRADER    {$$=static_cast<int>(ElementType::_DEGRADER);}
          | LASER       {$$=static_cast<int>(ElementType::_LASER);}
          | SCREEN      {$$=static_cast<int>(ElementType::_SCREEN);}
          | AWAKESCREEN {$$=static_cast<int>(ElementType::_AWAKESCREEN);}
          | TRANSFORM3D {$$=static_cast<int>(ElementType::_TRANSFORM3D);}
          | ELEMENT     {$$=static_cast<int>(ElementType::_ELEMENT);}
          | MATERIAL    {$$=static_cast<int>(ElementType::_MATERIAL);}
          | ATOM        {$$=static_cast<int>(ElementType::_ATOM);}
;

region : REGION ',' region_options ;
cavitymodel : CAVITYMODEL ',' cavitymodel_options ;
tunnel : TUNNEL ',' tunnel_options ;
xsecbias : XSECBIAS ',' xsecbias_options ;

error_noparams : DRIFT;
               | RF;
               | SBEND;
               | RBEND;
               | VKICK;
               | HKICK;
               | QUADRUPOLE;
               | SEXTUPOLE;
               | OCTUPOLE;
               | DECAPOLE;
               | MULTIPOLE;
               | SOLENOID;
               | ECOL;
               | MUSPOILER;
               | RCOL;
               | LASER;
               | SCREEN;
               | AWAKESCREEN;
               | TRANSFORM3D;
               | ELEMENT;
               | MATERIAL;
               | ATOM;
               | REGION;
               | CAVITYMODEL;
               | TUNNEL;
               | XSECBIAS;

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
;

parameters: VARIABLE '=' aexpr ',' parameters
            {
	      if(execute)
		Parser::Instance()->SetParameterValue(*($1),$3);
	    }
          | VARIABLE '=' aexpr
            {
	      if(execute)
		Parser::Instance()->SetParameterValue(*($1),$3);
	    }
          | VARIABLE '=' vecexpr ',' parameters
            {
	      if(execute) 
		Parser::Instance()->SetParameterValue(*($1),$3);
	    }
          | VARIABLE '=' vecexpr
            {
	      if(execute) 
		Parser::Instance()->SetParameterValue(*($1),$3);
	    }
          | VARIABLE '=' VARIABLE
	    {
	      if(execute) {
		Symtab *sp = Parser::Instance()->symlook(*($3));
		if (!sp) {
		  std::string errorstring = "ERROR: use of undeclared variable " + *($3) + "\n";
		  yyerror(errorstring.c_str());
		}
		if (sp->type == Symtab::symtabtype::_NUMBER)
		  Parser::Instance()->SetParameterValue(*($1),sp->value);
		else if (sp->type == Symtab::symtabtype::_STRING)
		  Parser::Instance()->SetParameterValue(*($1),sp->str);
	      }
	    }
          | VARIABLE '=' STR ',' parameters
            {
	      if(execute) {
		Parser::Instance()->SetParameterValue(*($1),*$3);
	      }
	    }
          | VARIABLE '=' STR
            {
	      if(execute) {
		Parser::Instance()->SetParameterValue(*($1),*$3);
	      }
	    }

line : LINE '=' '(' element_seq ')'
;

line : LINE '=' '-' '(' rev_element_seq ')'
;

element_seq : 
            | VARIABLE ',' element_seq
              {
		if(execute) Parser::Instance()->add_element_temp(*($1), 1, true, ElementType::_LINE);
	      }
            | VARIABLE '*' NUMBER ',' element_seq
              {
		if(execute) Parser::Instance()->add_element_temp(*($1), (int)$3, true, ElementType::_LINE);
	      }
            | NUMBER '*' VARIABLE ',' element_seq
              {
		if(execute) Parser::Instance()->add_element_temp(*($3), (int)$1, true, ElementType::_LINE);
	      }
            | VARIABLE
              {
		if(execute) Parser::Instance()->add_element_temp(*($1), 1, true, ElementType::_LINE);
	      }
           | VARIABLE '*' NUMBER
              {
		if(execute) Parser::Instance()->add_element_temp(*($1), (int)$3, true, ElementType::_LINE);
	      }
            | NUMBER '*' VARIABLE
              {
		if(execute) Parser::Instance()->add_element_temp(*($3), (int)$1, true, ElementType::_LINE);
	      }
            | '-' VARIABLE ',' element_seq
              {
		if(execute) Parser::Instance()->add_element_temp(*($2), 1, true, ElementType::_REV_LINE);
	      }
            | '-' VARIABLE
              {
		if(execute) Parser::Instance()->add_element_temp(*($2), 1, true, ElementType::_REV_LINE);
	      }
;

rev_element_seq : 
            | VARIABLE ',' rev_element_seq 
              {
		if(execute) Parser::Instance()->add_element_temp(*($1), 1, false, ElementType::_REV_LINE);
	      }
            | VARIABLE '*' NUMBER ',' rev_element_seq
              {
		if(execute) Parser::Instance()->add_element_temp(*($1), int($3), false, ElementType::_REV_LINE);
	      }
            | NUMBER '*' VARIABLE ',' rev_element_seq
              {
		if(execute) Parser::Instance()->add_element_temp(*($3), int($1), false, ElementType::_REV_LINE);
	      }
            | VARIABLE
              {
		if(execute) Parser::Instance()->add_element_temp(*($1), 1, false, ElementType::_REV_LINE);
	      }
           | VARIABLE '*' NUMBER
              {
		if(execute) Parser::Instance()->add_element_temp(*($1), int($3), false, ElementType::_REV_LINE);
	      }
            | NUMBER '*' VARIABLE
              {
		if(execute) Parser::Instance()->add_element_temp(*($3), int($1), false, ElementType::_REV_LINE);
	      }
            | '-' VARIABLE ',' element_seq
              {
		if(execute) Parser::Instance()->add_element_temp((*$2), 1, false, ElementType::_LINE);
	      }
            | '-' VARIABLE
              {
		if(execute) Parser::Instance()->add_element_temp((*$2), 1, false, ElementType::_LINE);
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
;

aexpr  :  NUMBER              { $$ = $1;                         }
       | CONSTANT             { $$ = $1;                         }
       | FUNC '(' aexpr ')'   { $$ = (*($1->funcptr))($3);       } 
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
        | VARIABLE '[' VARIABLE ']' 
          { 
	    if(ECHO_GRAMMAR) std::cout << "aexpr-> " << *($1) << " [ " << *($3) << " ]" << std::endl; 
	    $$ = Parser::Instance()->property_lookup(*($1),*($3));
	  }// element attributes
; 

symdecl : VARIABLE '='
        {
	  if(execute)
	    {
	      Symtab *sp = Parser::Instance()->symlook(*($1));
	      if (!sp) {
		sp = Parser::Instance()->symcreate(*($1));
	      } else {
		std::cout << "WARNING redefinition of variable " << sp->name << " with old value: " << sp->value << std::endl;
	      }
	      $$ = sp;
	    }
	}
;

assignment :  symdecl aexpr  
              {
		if(ECHO_GRAMMAR) std::cout << $1->name << std::endl;
		if(execute)
		  {
		    if($1->is_reserved) {
		      std::string errorstring = "ERROR: " + $1->name + " is reserved\n";
		      yyerror(errorstring.c_str());
		    }
		    else
		      {
			$1->value = $2; $$=$1;       
		      }
		  }
	      }
           |  symdecl STR
	      {
		if (execute)
		  {
		    if($1->is_reserved) {
		      std::string errorstring = "ERROR: " + $1->name + " is reserved\n";
		      yyerror(errorstring.c_str());
		    }
		    else
		      {
			$1->str = *$2; $$=$1;
			$1->type = Symtab::symtabtype::_STRING;
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

           |  VECVAR '=' vecexpr
              {
		if(execute)
		  {
		    $1->Set($3);
		    $$=$1;
		  }
              }
;

vecexpr :   VECVAR  
        {
	  if(execute)
	    {
	      $$ = new Array($1);
	    }
        } 
        | vectnum
        {
	  if(execute)
	    {
	      $$ = $1;
	    }
	}
       | vectstr
	{
	  if(execute)
	  {
	    $$ = $1;
	  }
	}

       | vecexpr '+' vecexpr
        {
	  if(execute)
	    {
	      $$ = Array::Add($1,$3);
	    }
        }
      | vecexpr '-' vecexpr
        {
	  if(execute)
	    {
	      $$ = Array::Subtract($1,$3);
	    }
	}
       | vecexpr '+' aexpr
        {
	  if(execute)
	    {
	      $$ = Array::Add($1,$3);
	    }
	}
       | aexpr '+' vecexpr
        {
	  if(execute)
	    {
	      $$ = Array::Add($3,$1);
	    }
	}
      | vecexpr '*' aexpr
        {
	  if(execute)
	    {
	      $$ = Array::Multiply($1,$3);
	    }
	}
      | aexpr '*' vecexpr
        {
	  if(execute)
	    {
	      $$ = Array::Multiply($3,$1);
	    }
	}
      | vecexpr '/' aexpr
        {
	  if(execute)
	    {
	      $$ = Array::Divide($1,$3);
	    }
	}
       | vecexpr '-' aexpr
        {
	  if(execute)
	    {
	      $$ = Array::Subtract($1,$3);
	    }
	}
       | aexpr '-' vecexpr
        {
	  if(execute)
	    {
	      Array* a = Array::Multiply($3,-1);
	      $$ = Array::Add(a,$1);
	    }
	}
;

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
;

vectstrexec : '[' letters ']'
            | '{' letters '}'
;

vectstr : vectstrexec
	{
	  if(execute)
	  {
	    $$ = new Array;
	    Parser::Instance()->FillString($$);
	  }
	}

numbers : aexpr ',' numbers 
          {
	    if(execute)
	      Parser::Instance()->Store($1);
          } 
       | aexpr
         {
	   if(execute)
	     Parser::Instance()->Store($1);
        }
;

letters : STR ',' letters
          {
            if(execute)
              Parser::Instance()->Store(*$1);
          }
	| STR
         {
           if(execute)
             Parser::Instance()->Store(*$1);
         }
;

command : STOP             { if(execute) Parser::Instance()->quit(); }
        | BEAM ',' beam_parameters
        | PRINT            { if(execute) Parser::Instance()->PrintElements(); }
        | PRINT ',' LINE   { if(execute) Parser::Instance()->PrintBeamline(); }
        | PRINT ',' OPTION { if(execute) Parser::Instance()->PrintOptions(); }
        | PRINT ',' VARIABLE
          {
	    if(execute) {
	      Symtab *sp = Parser::Instance()->symlook(*($3));
	      if (!sp) {
		std::cout << "Variable " << *($3) << " not defined!" << std::endl;
	      }
	      else {
		sp->Print();
	      }
	    }
	  }
        | PRINT ',' VECVAR
	  {
	    if(execute)
	      {
	        $3->Print();
	      } 
	  }
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
        | TUNNEL ',' tunnel_options // tunnel
          {
	    if(execute)
	      {  
		if(ECHO_GRAMMAR) printf("command -> TUNNEL\n");
		Parser::Instance()->add_tunnel();
	      }
          }
        | REGION ',' region_options // region
          {
	    if(execute)
	      {  
		if(ECHO_GRAMMAR) printf("command -> REGION\n");
		Parser::Instance()->add_region();
	      }
          }
        | CAVITYMODEL ',' cavitymodel_options // cavitymodel
          {
	    if(execute)
	      {  
		if(ECHO_GRAMMAR) printf("command -> CAVITYMODEL\n");
		Parser::Instance()->add_cavitymodel();
	      }
          }
        | XSECBIAS ',' xsecbias_options // xsecbias
          {
	    if(execute)
	      {  
		if(ECHO_GRAMMAR) printf("command -> XSECBIAS\n");
		Parser::Instance()->add_xsecbias();
	      }
          }
;

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
;

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
;

csample_options : VARIABLE '=' aexpr
                  {
		    if(ECHO_GRAMMAR) std::cout << "csample_opt ->csopt " << (*$1) << " = " << $3 << std::endl;
		    if(execute)
		      Parser::Instance()->SetParameterValue(*($1),$3);
		  }
                | VARIABLE '=' aexpr ',' csample_options
                  {
		    if(ECHO_GRAMMAR) std::cout << "csample_opt ->csopt " << (*$1) << " = " << $3 << std::endl;
		    if(execute)
		      Parser::Instance()->SetParameterValue(*($1),$3);
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

cavitymodel_options : VARIABLE '=' aexpr ',' cavitymodel_options
                    {
		      if(execute)
			Parser::Instance()->SetCavityModelValue((*$1),$3);
		    }
                 | VARIABLE '=' aexpr
                    {
		      if(execute)
			Parser::Instance()->SetCavityModelValue((*$1),$3);
		    }
                 | VARIABLE '=' STR ',' cavitymodel_options
                    {
		      if(execute)
			Parser::Instance()->SetCavityModelValue(*$1,*$3);
		    }
                 | VARIABLE '=' STR
                    {
		      if(execute)
			Parser::Instance()->SetCavityModelValue(*$1,*$3);
		    }
;

region_options : VARIABLE '=' aexpr ',' region_options
                    {
		      if(execute)
			Parser::Instance()->SetRegionValue((*$1),$3);
		    }
                 | VARIABLE '=' aexpr
                    {
		      if(execute)
			Parser::Instance()->SetRegionValue((*$1),$3);
		    }
                 | VARIABLE '=' STR ',' region_options
                    {
		      if(execute)
			Parser::Instance()->SetRegionValue(*$1,*$3);
		    }
                 | VARIABLE '=' STR
                    {
		      if(execute)
			Parser::Instance()->SetRegionValue(*$1,*$3);
		    }
;

tunnel_options : VARIABLE '=' aexpr ',' tunnel_options
                    {
		      if(execute)
			Parser::Instance()->SetTunnelValue((*$1),$3);
		    }
                 | VARIABLE '=' aexpr
                    {
		      if(execute)
			Parser::Instance()->SetTunnelValue((*$1),$3);
		    }
                 | VARIABLE '=' STR ',' tunnel_options
                    {
		      if(execute)
			Parser::Instance()->SetTunnelValue(*$1,*$3);
		    }
                 | VARIABLE '=' STR
                    {
		      if(execute)
			Parser::Instance()->SetTunnelValue(*$1,*$3);
		    }
;

xsecbias_options : VARIABLE '=' aexpr ',' xsecbias_options
                    {
		      if(execute)
			Parser::Instance()->SetPhysicsBiasValue(*$1,$3);
		    }
                 | VARIABLE '=' aexpr
                    {
		      if(execute)
			Parser::Instance()->SetPhysicsBiasValue(*$1,$3);
		    }
                 | VARIABLE '=' STR ',' xsecbias_options
                    {
		      if(execute)
			Parser::Instance()->SetPhysicsBiasValue(*$1,*$3);
		    }
                 | VARIABLE '=' STR
                    {
		      if(execute)
			Parser::Instance()->SetPhysicsBiasValue(*$1,*$3);
		    }
                 | VARIABLE '=' vecexpr ',' xsecbias_options
		    {
		      if(execute)
			Parser::Instance()->SetPhysicsBiasValue(*$1,$3);
		    }
                 | VARIABLE '=' vecexpr
		    {
		      if(execute)
			Parser::Instance()->SetPhysicsBiasValue(*$1,$3);
		    }
;

option_parameters : VARIABLE '=' aexpr ',' option_parameters
                    {
		      if(execute)
			Parser::Instance()->SetOptionsValue(*$1,$3);
		    }   
                  | VARIABLE '=' aexpr
                    {
		      if(execute)
			Parser::Instance()->SetOptionsValue(*$1,$3);
		    } 
                  | VARIABLE '=' STR ',' option_parameters
                    {
		      if(execute)
			Parser::Instance()->SetOptionsValue(*$1,*$3);
		    }   
                  | VARIABLE '=' STR
                    {
		      if(execute)
			Parser::Instance()->SetOptionsValue(*$1,*$3);
		    }
;

// beam_parameter same as option_parameters, might change in future
beam_parameters : option_parameters
;

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
