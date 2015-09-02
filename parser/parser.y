/*
   bison grammar for the gmad parser
   Ilya Agapov, Steve Malton 2005-2007
   bdsim v.0.4
*/

%{

#include "array.h"
#include "parser.h"
#include "sym_table.h"
#include "elementtype.h"
  
  using namespace GMAD;

  extern char* yytext;

  namespace GMAD {
    extern int line_num;
    extern char* yyfilename;
  
    const int PEDANTIC = 1; ///< strict checking, exits when element or parameter is not known
    const int ECHO_GRAMMAR = 0; ///< print grammar rule expansion (for debugging)
    const int INTERACTIVE = 0; ///< print output of commands (like in interactive mode)
    /* for more debug with parser:
       1) set yydebug to 1 in parser.tab.cc (needs to be reset as this file gets overwritten from time to time!) 
       2) add %debug below
       3) compile bison with "-t" flag. This is automatically done when CMAKE_BUILD_TYPE equals Debug
    */

    int execute = 1;
    int element_count = 1; // for samplers , ranges etc.
  }
%}

/* define stack type */

%union{
  double dval;
  int ival; // ElementType, but underlying type as not possible to have enum class in union, rely on static_casts
  GMAD::symtab *symp;
  char *str;
  struct Array *array;
}

/* more debug output can be added with %debug" */
//%debug

%left '+' '-'
%left '*' '/'
%left '^' '<' '>' NE LE GE EQ
%nonassoc UMINUS
%nonassoc UPLUS

%token <dval> NUMBER
%token <symp> VARIABLE VECVAR FUNC 
%token <str> STR
%token MARKER ELEMENT DRIFT RF RBEND SBEND QUADRUPOLE SEXTUPOLE OCTUPOLE DECAPOLE MULTIPOLE SCREEN AWAKESCREEN
%token SOLENOID RCOL ECOL LINE SEQUENCE LASER TRANSFORM3D MUSPOILER
%token VKICK HKICK
%token PERIOD GAS XSECBIAS TUNNEL MATERIAL ATOM
%token BEAM OPTION PRINT RANGE STOP USE VALUE ECHO PRINTF SAMPLE CSAMPLE BETA0 TWISS DUMP
%token IF ELSE BEGN END LE GE NE EQ FOR

%type <dval> aexpr
%type <dval> expr
%type <symp> assignment
%type <array> vecexpr
%type <array> vectnum vectstr
%type <str> use_parameters
%type <ival> extension
%type <ival> newinstance
%type <symp> sample_options
%type <symp> csample_options
%type <symp> gas_options
%type <symp> tunnel_options
%type <symp> xsecbias_options

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



decl : VARIABLE ':' marker
       {
	 if(execute)  {
	   // check parameters and write into element table
	   write_table(params,$1->name,ElementType::_MARKER);
	   params.flush();
	 }
       }
     | VARIABLE ':' drift
       {
	 if(execute) {
	   // check parameters and write into element table
	   write_table(params,$1->name,ElementType::_DRIFT);
	   params.flush();
	 }
       }
     | VARIABLE ':' rf
       {
	 if(execute) {
	   // check parameters and write into element table
	   write_table(params,$1->name,ElementType::_RF);
	   params.flush();
	 }
       } 
     | VARIABLE ':' sbend
       {  
	 if(execute) {
	   // check parameters and write into element table
	   write_table(params,$1->name,ElementType::_SBEND);
	   params.flush();
	 }
       }
     | VARIABLE ':' rbend
       {
         if(execute) {
           // check parameters and write into element table
           write_table(params,$1->name,ElementType::_RBEND);
           params.flush();
         }
       }

    | VARIABLE ':' vkick
       {  
	 if(execute) {
	   // check parameters and write into element table
	   write_table(params,$1->name,ElementType::_VKICK);
	   params.flush();
	 }
       }
    | VARIABLE ':' hkick
       {  
	 if(execute) {
	   // check parameters and write into element table
	   write_table(params,$1->name,ElementType::_HKICK);
	   params.flush();
	 }
       }
     | VARIABLE ':' quad
       {
	 if(execute)       
	   {
	     // check parameters and write into element table
	     write_table(params,$1->name,ElementType::_QUAD);
	     params.flush();
	   }
       }
     | VARIABLE ':' sextupole
       {
	 if(execute)
	   {
	     // check parameters and write into element table
	     write_table(params,$1->name,ElementType::_SEXTUPOLE);
	     params.flush();
	   }
       }
     | VARIABLE ':' octupole
       {
	 if(execute)
	   {
	     // check parameters and write into element table
	     write_table(params,$1->name,ElementType::_OCTUPOLE);
	     params.flush();
	   }
       }
     | VARIABLE ':' decapole
       {
	 if(execute)
	   {
	     // check parameters and write into element table
	     write_table(params,$1->name,ElementType::_DECAPOLE);
	     params.flush();
	   }
       }
     | VARIABLE ':' multipole
       {
	 if(execute)
	   {	 
	     // check parameters and write into element table
	     write_table(params,$1->name,ElementType::_MULT);
	     params.flush();	 
	   }
       }
     | VARIABLE ':' solenoid
       {
	 if(execute)       
	   {
	     // check parameters and write into element table
	     write_table(params,$1->name,ElementType::_SOLENOID);
	     params.flush();
	   }
       }
     | VARIABLE ':' rcol
       {
	 if(execute)
	   {
	     // check parameters and write into element table
	     write_table(params,$1->name,ElementType::_RCOL);
	     params.flush();
	   }
       }
     | VARIABLE ':' ecol
       {
	 if(execute)
	   {
	     // check parameters and write into element table
	     write_table(params,$1->name,ElementType::_ECOL);
	     params.flush();
	   }
       }
     | VARIABLE ':' muspoiler
       {
	 if(execute)
	   {
	     // check parameters and write into element table
	     write_table(params,$1->name,ElementType::_MUSPOILER);
	     params.flush();
	   }
       }
     | VARIABLE ':' element
       {
	 if(execute)
	   {	 
	     // check parameters and write into element table
	     write_table(params,$1->name,ElementType::_ELEMENT);
	     params.flush();	 
	   }
       }
     | VARIABLE ':' laser
       {
	 if(execute)
	   {	 
	     // check parameters and write into element table
	     write_table(params,$1->name,ElementType::_LASER);
	     params.flush();	 
	   }
       }
     | VARIABLE ':' screen
       {
	 if(execute) {
	   // check parameters and write into element table
	   write_table(params,$1->name,ElementType::_SCREEN);
	   params.flush();
	 }
       }
     | VARIABLE ':' awakescreen
       {
	 if(execute) {
	   // check parameters and write into element table
	   write_table(params,$1->name,ElementType::_AWAKESCREEN);
	   params.flush();
	 }
       }
     | VARIABLE ':' transform3d
       {
	 if(execute)
	   {	 
	     // check parameters and write into element table
	     write_table(params,$1->name,ElementType::_TRANSFORM3D);
	     params.flush();	 
	   }
       }
     | VARIABLE ':' line 
       {
	 if(execute)
	   {
	     // copy tmp_list to params
	     write_table(params,$1->name,ElementType::_LINE,new std::list<struct Element>(tmp_list));
	     // clean list
	     tmp_list.clear();
	   }
       }     
     | VARIABLE ':' sequence
       {
	 if(execute)
	   {
	     // copy tmp_list to params
	     write_table(params,$1->name,ElementType::_SEQUENCE,new std::list<struct Element>(tmp_list));
	     // clean list
	     tmp_list.clear();
	   }
       }
     | VARIABLE ':' extension
       {
	 if(execute)
	   {
	     ElementType type = static_cast<ElementType>($3);
	     if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE : VARIABLE, " << $1->name << " : " << type << std::endl;
	     if(type != ElementType::_NONE)
	       {
		 write_table(params,$1->name,type);
	       }
	     params.flush();
	   }
       }
     | VARIABLE ':' newinstance
       {
         if(execute)
	   {
	     ElementType type = static_cast<ElementType>($3);
	     if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE : VARIABLE, " << $1->name << " : " << type << std::endl;
	     if(type != ElementType::_NONE)
	       {
		 write_table(params,$1->name,type);
	       }
	     params.flush();
	   }
       }
       | VARIABLE ',' parameters
       {
	 if(execute)
	   {
	     if(ECHO_GRAMMAR) std::cout << "edit : VARIABLE parameters   -- " << $1->name << std::endl;
	     std::list<struct Element>::iterator it = element_list.find($1->name);
	     std::list<struct Element>::iterator iterEnd = element_list.end();
	     if(it == iterEnd)
	       {
		 std::cout << "type " << $1->name << " has not been defined" << std::endl;
		 if (PEDANTIC) exit(1);
	       }
	     else
	       {
		 // inherit properties from the base type
		 params.inherit_properties(*it);
	       }
		
	     if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE : VARIABLE, " << $1->name << " : " << (*it).type << std::endl;
	     if((*it).type != ElementType::_NONE)
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
	     write_table(params,$1->name,ElementType::_MATERIAL);
	     params.flush();
	   }
       }
     | VARIABLE ':' atom
       {
         if(execute)
           {
             write_table(params,$1->name,ElementType::_ATOM);
             params.flush();
           }
       }
     | VARIABLE ':' tunnel
       {
         if(execute)
           {
	     if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << $1->name << " : tunnel" << std::endl;
	     tunnel.set_value("name",$1->name);
	     add_tunnel(tunnel);
           }
       }
     | VARIABLE ':' xsecbias
       {
         if(execute)
           {
	     if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << $1->name << " : xsecbias" << std::endl;
	     xsecbias.set_value("name",$1->name);
	     add_xsecbias(xsecbias);
           }
       }
;

marker : MARKER ;
drift : DRIFT ',' parameters ;
rf : RF ',' parameters ;
sbend : SBEND ',' parameters ;
rbend : RBEND ',' parameters ;
vkick : VKICK ',' parameters ;
hkick : HKICK ',' parameters ;
quad : QUADRUPOLE ',' parameters ;
sextupole : SEXTUPOLE ',' parameters ;
octupole : OCTUPOLE ',' parameters ;
decapole : DECAPOLE ',' parameters ;
multipole : MULTIPOLE ',' parameters ;
solenoid : SOLENOID ',' parameters ;
ecol : ECOL ',' parameters ;
muspoiler : MUSPOILER ',' parameters ;
rcol : RCOL ',' parameters ;
laser : LASER ',' parameters ;
screen : SCREEN ',' parameters ;
awakescreen : AWAKESCREEN ',' parameters ;
transform3d : TRANSFORM3D ',' parameters ;
element : ELEMENT ',' parameters ;
matdef : MATERIAL ',' parameters ;
atom : ATOM ',' parameters ;
tunnel : TUNNEL ',' tunnel_options ;
xsecbias : XSECBIAS ',' xsecbias_options ;
extension : VARIABLE ',' parameters
            {
	      if(execute)
		{	 
		  if(ECHO_GRAMMAR) std::cout << "extension : VARIABLE parameters   -- " << $1->name << std::endl;
		  std::list<struct Element>::iterator it = element_list.find($1->name);
		  std::list<struct Element>::iterator iterEnd = element_list.end();
		  if(it == iterEnd)
		    {
		      std::cout << "type " << $1->name << " has not been defined" << std::endl;
		      if (PEDANTIC) exit(1);
		      $$ = static_cast<int>(ElementType::_NONE);
		    }
		  else
		    {
		      // inherit properties from the base type
		      $$ = static_cast<int>((*it).type);
		      params.inherit_properties(*it);
		    }
		  
		}
	    }
;

newinstance : VARIABLE 
            {
	      if(execute)
		{	 
		  std::cout << "newinstance : VARIABLE -- " << $1->name << std::endl;
		  std::list<struct Element>::iterator it = element_list.find($1->name);
		  std::list<struct Element>::iterator iterEnd = element_list.end();
		  if(it == iterEnd)
		    {
		      std::cout << "type " << $1->name << " has not been defined" << std::endl;
		      if (PEDANTIC) exit(1);
		      $$ = static_cast<int>(ElementType::_NONE);
		    }
		  else
		    {
		      // inherit properties from the base type
		      $$ = static_cast<int>((*it).type);
		      params.inherit_properties(*it);
		    }
		  
		}
	    }
;

parameters: VARIABLE '=' aexpr ',' parameters
            {
	      if(execute)
		params.set_value($1->name,$3);
	    }
          | VARIABLE '=' aexpr
            {
	      if(execute)
		params.set_value($1->name,$3);
	    }
          | VARIABLE '=' vecexpr ',' parameters
            {
	      if(execute) 
		params.set_value($1->name,$3);
	    }
          | VARIABLE '=' vecexpr
            {
	      if(execute) 
		params.set_value($1->name,$3);
	    }
          | VARIABLE '=' STR ',' parameters
            {
	      if(execute) {
		params.set_value($1->name,$3);
	      }
	      free($3);
	    }
          | VARIABLE '=' STR
            {
	      if(execute) {
		params.set_value($1->name,$3);
	      }
	      free($3);
	    }

line : LINE '=' '(' element_seq ')'
;

line : LINE '=' '-' '(' rev_element_seq ')'
;

//sequence : SEQUENCE ',' params ',' '(' element_seq ')'
//;

//sequence : SEQUENCE ',' params ',' '-' '(' rev_element_seq ')'
//;

sequence : SEQUENCE '=' '(' seq_element_seq ')' ;

element_seq : 
            | VARIABLE ',' element_seq
              {
		if(execute) add_element_temp($1->name, 1, true, ElementType::_LINE);
	      }
            | VARIABLE '*' NUMBER ',' element_seq
              {
		if(execute) add_element_temp($1->name, (int)$3, true, ElementType::_LINE);
	      }
            | NUMBER '*' VARIABLE ',' element_seq
              {
		if(execute) add_element_temp($3->name, (int)$1, true, ElementType::_LINE);
	      }
            | VARIABLE
              {
		if(execute) add_element_temp($1->name, 1, true, ElementType::_LINE);
	      }
           | VARIABLE '*' NUMBER
              {
		if(execute) add_element_temp($1->name, (int)$3, true, ElementType::_LINE);
	      }
            | NUMBER '*' VARIABLE
              {
		if(execute) add_element_temp($3->name, (int)$1, true, ElementType::_LINE);
	      }
            | '-' VARIABLE ',' element_seq
              {
		if(execute) add_element_temp($2->name, 1, true, ElementType::_REV_LINE);
	      }
            | '-' VARIABLE
              {
		if(execute) add_element_temp($2->name, 1, true, ElementType::_REV_LINE);
	      }
;

rev_element_seq : 
            | VARIABLE ',' rev_element_seq 
              {
		if(execute) add_element_temp($1->name, 1, false, ElementType::_REV_LINE);
	      }
            | VARIABLE '*' NUMBER ',' rev_element_seq
              {
		if(execute) add_element_temp($1->name, int($3), false, ElementType::_REV_LINE);
	      }
            | NUMBER '*' VARIABLE ',' rev_element_seq
              {
		if(execute) add_element_temp($3->name, int($1), false, ElementType::_REV_LINE);
	      }
            | VARIABLE
              {
		if(execute) add_element_temp($1->name, 1, false, ElementType::_REV_LINE);
	      }
           | VARIABLE '*' NUMBER
              {
		if(execute) add_element_temp($1->name, int($3), false, ElementType::_REV_LINE);
	      }
            | NUMBER '*' VARIABLE
              {
		if(execute) add_element_temp($3->name, int($1), false, ElementType::_REV_LINE);
	      }
            | '-' VARIABLE ',' element_seq
              {
		if(execute) add_element_temp($2->name, 1, false, ElementType::_LINE);
	      }
            | '-' VARIABLE
              {
		if(execute) add_element_temp($2->name, 1, false, ElementType::_LINE);
	      }
;

seq_element_seq : 
            | VARIABLE ',' seq_element_seq
              {
		if(execute) add_element_temp($1->name, 1, true, ElementType::_SEQUENCE);
	      }
            | VARIABLE '*' NUMBER ',' seq_element_seq
              {
		if(execute) add_element_temp($1->name, int($3), true, ElementType::_SEQUENCE);
	      }
            | NUMBER '*' VARIABLE ',' seq_element_seq
              {
		if(execute) add_element_temp($3->name, int($1), true, ElementType::_SEQUENCE);
	      }
            | VARIABLE 
              {
		if(execute) add_element_temp($1->name, 1, true, ElementType::_SEQUENCE);
	      }
           | VARIABLE '*' NUMBER 
              {
		if(execute) add_element_temp($1->name, int($3), true, ElementType::_SEQUENCE);
	      }
            | NUMBER '*' VARIABLE 
              {
		if(execute) add_element_temp($3->name, int($1), true, ElementType::_SEQUENCE);
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
	       for(int i=0;i<$1->data.size();i++)
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
	       if($1->type == symtab::symtabtype::_ARRAY)
		 {
		   for(std::list<double>::iterator it = $1->array.begin();
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
	   if($2->data.size() == $4->data.size())
	     {
	       $$ = 0;
	       for(int i=0;i<$2->data.size();i++)
		 $$ += $2->data[i] * $4->data[i];
	     }
	   else
	     {
	       printf("vector dimensions do not match");
	       exit(1);
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
	    if(ECHO_GRAMMAR) std::cout << "aexpr-> " << $1->name << " [ " << $3->name << " ]" << std::endl; 
	    $$ = property_lookup(element_list,$1->name,$3->name);
	  }// element attributes
 ; 

assignment :  VARIABLE '=' aexpr  
              {
		if(ECHO_GRAMMAR) std::cout << $1->name << std::endl;
		if(execute)
		  {
		    if($1->is_reserved)
		      std::cout << $1->name << " is reserved" << std::endl;
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
		    $1->array.clear();
		    for(unsigned int i=0;i<$3->data.size();i++)
		      $1->array.push_back($3->data[i]);
		    $1->type = symtab::symtabtype::_ARRAY;
		    $$ = $1;
		    $3->data.clear();
		  }
              }

           |  VECVAR '=' vecexpr
              {
		if(execute)
		  {
		    $1->array.clear();
		    for(int i=0;i<$3->data.size();i++)
		      $1->array.push_back($3->data[i]);
		    $$ = $1;
		    $3->data.clear();
		  }
              }
;

vecexpr :   VECVAR  
        {
	  if(execute)
	    {
	      $$ = new struct Array;
	      std::list<double>::iterator it;
	      for(it=$1->array.begin();it!=$1->array.end();it++)
		{
		  $$->data.push_back(*it);
		}
	    }
        } 
        | vectnum
        {
	  if(execute)
	    {
	      $$ = new struct Array;
	      $$->data = $1->data;
	      // erase data in vect
	      $1->data.clear();
	    }
	}
       | vectstr
	{
	  if(execute)
	  {
	    $$ = new struct Array;
	    $$->symbols = $1->symbols;
	    $1->symbols.clear();
	  }
	}

       | vecexpr '+' vecexpr
        {
	  if(execute)
	    {
	      $$ = new struct Array;
	      unsigned int size = ($1->data.size() < $3->data.size() )? $1->data.size() : $3->data.size();
	      $$->data.resize(size);
	      for(unsigned int i=0;i<size;i++)
		{
		  $$->data[i] = $1->data[i] + $3->data[i];
		}
	      // erase data in vect
	      $1->data.clear();
	      $3->data.clear();
	    }
        }
      | vecexpr '-' vecexpr
        {
	  if(execute)
	    {
	      $$ = new struct Array;
	      unsigned int size = ($1->data.size() < $3->data.size() )? $1->data.size() : $3->data.size();
	      $$->data.resize(size);
	      for(unsigned int i=0;i<size;i++)
		{
		  $$->data[i] = $1->data[i] - $3->data[i];
		}
	      // erase data in vect
	      $1->data.clear();
	      $3->data.clear();
	    }
	}
       | vecexpr '+' aexpr
        {
	  if(execute)
	    {
	      $$ = new struct Array;
	      unsigned int size = $1->data.size();
	      $$->data.resize(size);
	      for(unsigned int i=0;i<size;i++)
		{
		  $$->data[i] = $1->data[i] + $3;
		}
	      // erase data in vect
	      $1->data.clear();
	    }
	}

      | vecexpr '*' aexpr
        {
	  if(execute)
	    {
	      $$ = new struct Array;
	      unsigned int size = $1->data.size();
	      $$->data.resize(size);
	      for(unsigned int i=0;i<size;i++)
		{
		  $$->data[i] = $1->data[i] * $3;
		}
	      // erase data in vect
	      $1->data.clear();
	    }
	}
      | vecexpr '/' aexpr
        {
	  if(execute)
	    {
	      $$ = new struct Array;
	      unsigned int size = $1->data.size();
	      $$->data.resize(size);
	      for(unsigned int i=0;i<size;i++)
		{
		  $$->data[i] = $1->data[i] / $3;
		}
	      // erase data in vect
	      $1->data.clear();
	    }
	}
       | aexpr '+' vecexpr
        {
	  if(execute)
	    {
	      $$ = new struct Array;
	      unsigned int size = $3->data.size();
	      $$->data.resize(size);
	      for(unsigned int i=0;i<size;i++)
		{
		  $$->data[i] = $3->data[i] + $1;
		}
	      // erase data in vect
	      $3->data.clear();
	    }
	}
       | aexpr '-' vecexpr
        {
	  if(execute)
	    {
	      $$ = new struct Array;
	      unsigned int size = $3->data.size();
	      $$->data.resize(size);
	      for(unsigned int i=0;i<size;i++)
		{
		  $$->data[i] = $3->data[i] - $1;
		}
	      // erase data in vect
	      $3->data.clear();
	    }
	}
      | aexpr '*' vecexpr
        {
	  if(execute)
	    {
	      $$ = new struct Array;
	      unsigned int size = $3->data.size();
	      $$->data.resize(size);
	      for(unsigned int i=0;i<size;i++)
		{
		  $$->data[i] = $1 * $3->data[i];
		}
	      // erase data in vect
	      $3->data.clear();
	    }
	}

;

vectnum : '{' numbers '}' 
	  {
	    if(execute)
	      {
	        //printf("matched vector of size %d\n",_tmparray.size());
	        $$ = new struct Array;
	        std::list<double>::iterator it;
	        for(it=_tmparray.begin();it!=_tmparray.end();it++)
		  {
		    $$->data.push_back(*it);
		  }
    	        _tmparray.clear();

	        std::list<std::string>::iterator lIter;
	        for(lIter = _tmpstring.begin(); lIter != _tmpstring.end(); lIter++)
		  {
		    $$->symbols.push_back(*lIter);
		  }
	        _tmpstring.clear();
	      }
	}
;

vectstr : '[' letters ']'
	{
	  if(execute)
	  {
	    $$ = new struct Array;
	    std::list<std::string>::iterator iter;
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
	    free($1);
          }
	| STR
         {
           if(execute)
             _tmpstring.push_front($1);
	   free($1);
         }
;

command : STOP             { if(execute) quit(); }
        | BEAM ',' beam_parameters
        | PRINT            { if(execute) element_list.print(); }
        | PRINT ',' LINE   { if(execute) beamline_list.print(); }
        | PRINT ',' OPTION { if(execute) options.print(); }
//        | PRINT ',' OPTION ',' VARIABLE { if(execute) options.print($5->name);}
        | PRINT ',' VARIABLE 
          {
	    if(execute) {
	      printf("\t%s = %.10g\n",$3->name.c_str(),$3->value);
	    }
	  } 
        | PRINT ',' VECVAR
          {
	    if(execute)
	      {
		printf("\t%s = {",$3->name.c_str());
		std::list<double>::iterator it;
		for(it=$3->array.begin();it!=$3->array.end();it++)
		  {
		    printf(" %.10g ",(*it));
		  }
		printf("} \n");
	      } 
	  }
        | USE ',' use_parameters { if(execute) expand_line(current_line,current_start, current_end);}
        | OPTION  ',' option_parameters
	| ECHO STR { if(execute) {printf("%s\n",$2);} free($2); }
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
        | TUNNEL ',' tunnel_options // tunnel
          {
	    if(execute)
	      {  
		if(ECHO_GRAMMAR) printf("command -> TUNNEL\n");
		add_tunnel(tunnel);
	      }
          }
        | XSECBIAS ',' xsecbias_options // xsecbias
          {
	    if(execute)
	      {  
		if(ECHO_GRAMMAR) printf("command -> XSECBIAS\n");
		add_xsecbias(xsecbias);
	      }
          }
        | BETA0 ',' option_parameters // beta 0 (is a synonym of option, for clarity)
          {
	    if(execute)
	      {  
		if(ECHO_GRAMMAR) printf("command -> BETA0\n");
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
			char * cstr = new char [$1->name.length()+1];
			std::strcpy (cstr, $1->name.c_str());
			$$ = cstr;
			current_line = $1->name;
			current_start = "";
			current_end = "";
		      }
                  }
		| PERIOD '=' VARIABLE
                  {
		    if(execute)
		      {
			char * cstr = new char [$3->name.length()+1];
			std::strcpy (cstr, $3->name.c_str());
			$$ = cstr;
			current_line = $3->name;
			current_start = "";
			current_end = "";
		      }
                  }
                | PERIOD '=' VARIABLE ',' RANGE '=' VARIABLE '/' VARIABLE
                  {
		    if(execute)
		      {
			char * cstr = new char [$3->name.length()+1];
			std::strcpy (cstr, $3->name.c_str());
			$$ = cstr;
			current_line = $3->name;
			current_start = $7->name;
			current_end = $9->name;
		      }
		  }
;

sample_options: RANGE '=' VARIABLE
                {
		  if(ECHO_GRAMMAR) std::cout << "sample_opt : RANGE =  " << $3->name << std::endl;
		  {
		    if(execute) $$ = $3;
		  }
                }
              | RANGE '=' VARIABLE '[' NUMBER ']'
                {
                  if(ECHO_GRAMMAR) std::cout << "sample_opt : RANGE =  " << $3->name << " [" << $5 << "]" << std::endl;
		  {
		    if(execute) { $$ = $3; element_count = (int)$5; }
		  }
                }
;

csample_options : VARIABLE '=' aexpr
                  {
		    if(ECHO_GRAMMAR) std::cout << "csample_opt ->csopt " << $1->name << " = " << $3 << std::endl;
		    
		    if(execute)
		      {
			if( $1->name == "r") params.r = $3;
			else if ($1->name == "l") params.l = $3;
			else {
			  std::cout << "Warning : CSAMPLER: unknown parameter : \"" << $1->name << "\"" << std::endl;
			  exit(1);
			}
		      }
		  }   
                | VARIABLE '=' STR
                  {
		    if(ECHO_GRAMMAR) std::cout << "csample_opt -> " << $1->name << " = " << $3 << std::endl;
		    /* if(execute) */
		    /*   { */
		    /* 	;//options.set_value($1->name,string($3)); */
		    /*   } */
		    free($3);
		  }   
                | VARIABLE '=' aexpr ',' csample_options
                  {
		    if(ECHO_GRAMMAR) std::cout << "csample_opt ->csopt " << $1->name << " = " << $3 << std::endl;
		    
		    if(execute)
		      {
			if( $1->name == "r") params.r = $3;
			else if ($1->name == "l") params.l = $3;
			else {
			  std::cout << "Warning : CSAMPLER: unknown parameter : \"" << $1->name << "\"" << std::endl;
			  exit(1);
			}
		      }

		  }   
                | VARIABLE '=' STR ',' csample_options
                  {
		    if(ECHO_GRAMMAR) std::cout << "csample_opt -> " << $1->name << " = " << $3 << std::endl;
		    // if(execute) //options.set_value($1->name,string($3));
		    free($3);
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
		    if(ECHO_GRAMMAR) std::cout << "gas_opt -> , " << $1->name << " = " << $3 << std::endl;
		    
		    if(execute)
		      {
			if( $1->name == "r") params.r = $3;
			else if ($1->name == "l") params.l = $3;
			else {
			  std::cout << "Warning : GAS: unknown parameter : \"" << $1->name << "\"" << std::endl;
			  exit(1);
			}
		      }
		  }   
                | VARIABLE '=' STR
                  {
		    if(ECHO_GRAMMAR) std::cout << "gas_opt -> " << $1->name << " = " << $3 << std::endl;
		    if(execute)
		      {
			if( $1->name == "material")
			  {
			    params.material = $3;
			    params.materialset = 1;
			  }
			//options.set_value($1->name,$3);
		      }
		    free($3);
		  }   
                | VARIABLE '=' aexpr ',' gas_options
                  {
		    if(ECHO_GRAMMAR) std::cout << "gas_opt -> " << $1->name << " = " << $3 << std::endl;
		    
		    if(execute)
		      {
			if( $1->name == "r") params.r = $3;
			else if ($1->name == "l") params.l = $3;
			else {
			  std::cout << "Warning : GAS: unknown parameter : \"" << $1->name << "\"" << std::endl;
			  exit(1);
			}
		      }

		  }   
                | VARIABLE '=' STR ',' gas_options
                  {
		    if(ECHO_GRAMMAR) std::cout << "gas_opt -> " << $1->name << " = " << $3 << std::endl;
		    if(execute)
		      {
			if( $1->name == "material")
			  {
			    params.material = $3;
			    params.materialset = 1;
			  }
		      }
		    free($3);
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

tunnel_options : VARIABLE '=' aexpr ',' tunnel_options
                    {
		      if(execute)
			tunnel.set_value($1->name,$3);
		    }
                 | VARIABLE '=' aexpr
                    {
		      if(execute)
			tunnel.set_value($1->name,$3);
		    }
                 | VARIABLE '=' STR ',' tunnel_options
                    {
		      if(execute)
			tunnel.set_value($1->name,$3);
		      free($3);
		    }
                 | VARIABLE '=' STR
                    {
		      if(execute)
			tunnel.set_value($1->name,$3);
		      free($3);
		    }
;

xsecbias_options : VARIABLE '=' aexpr ',' xsecbias_options
                    {
		      if(execute)
			xsecbias.set_value($1->name,$3);
		    }
                 | VARIABLE '=' aexpr
                    {
		      if(execute)
			xsecbias.set_value($1->name,$3);
		    }
                 | VARIABLE '=' STR ',' xsecbias_options
                    {
		      if(execute)
			xsecbias.set_value($1->name,$3);
		      free($3);
		    }
                 | VARIABLE '=' STR
                    {
		      if(execute)
			xsecbias.set_value($1->name,$3);
		      free($3);
		    }
                 | VARIABLE '=' vecexpr ',' xsecbias_options
		    {
		      if(execute)
			xsecbias.set_value($1->name,$3);
		    }
                 | VARIABLE '=' vecexpr
		    {
		      if(execute)
			xsecbias.set_value($1->name,$3);
		    }
;

option_parameters : 
                  | VARIABLE '=' aexpr ',' option_parameters
                    {
		      if(execute)
			options.set_value($1->name,$3);
		    }   
                  | VARIABLE '=' aexpr
                    {
		      if(execute)
			options.set_value($1->name,$3);
		    } 
                  | VARIABLE '=' STR ',' option_parameters
                    {
		      if(execute)
			options.set_value($1->name,$3);
		      free($3);
		    }   
                  | VARIABLE '=' STR
                    {
		      if(execute)
			options.set_value($1->name,$3);
		      free($3);
		    }
;

beam_parameters :
                | VARIABLE '=' aexpr ',' beam_parameters
                  {
		    if(execute)
		      options.set_value($1->name,$3);
		  }   
                | VARIABLE '=' aexpr
                  {
		    if(execute)
		      options.set_value($1->name,$3);
		  }   
                | VARIABLE '=' STR ',' beam_parameters
                  {
		    if(execute)
		      options.set_value($1->name,$3);
		    free($3);
		  }   
                | VARIABLE '=' STR
                  {
		    if(execute)
		      options.set_value($1->name,$3);
		    free($3);
		  }   
;

%%

int yyerror(const char *s)
{
  printf("%s at line %d (might not be exact!), file %s \nsymbol '%s' unexpected\n",s, line_num, yyfilename, yytext);
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
