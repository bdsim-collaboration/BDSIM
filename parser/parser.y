/*
   bison grammar for the gmad parser
*/

%{

#include "array.h"
#include "parser.h"
#include "sym_table.h"
#include "elementtype.h"
#include <string>
  
  using namespace GMAD;

  extern char* yytext;

  namespace GMAD {
    // function that looks up a parser symbol
    extern struct symtab * symlook(std::string s);
    extern int line_num;
    extern std::string yyfilename;
  
    const int PEDANTIC = 1; ///< strict checking, exits when element or parameter is not known
    const int ECHO_GRAMMAR = 0; ///< print grammar rule expansion (for debugging)
    const int INTERACTIVE = 0; ///< print output of commands (like in interactive mode)
    /* for more debug with parser:
       1) set yydebug to 1 in parser.tab.cc (needs to be reset as this file gets overwritten from time to time!) 
       2) add %debug below
       3) compile bison with "-t" flag. This is automatically done when CMAKE_BUILD_TYPE equals Debug
    */

    int execute = 1;
    int element_count = -1; // for samplers , ranges etc. -1 means add to all
  }
%}

/* define stack type */

%union{
  double dval;
  int ival; // ElementType, but underlying type as it is not possible to have enum class in union, rely on static_casts
  GMAD::symtab *symp;
  char *str;
  std::string* name;
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
%token <symp> VECVAR FUNC 
%token <name> VARIABLE
%token <str> STR
%token MARKER ELEMENT DRIFT RF RBEND SBEND QUADRUPOLE SEXTUPOLE OCTUPOLE DECAPOLE MULTIPOLE SCREEN AWAKESCREEN
%token SOLENOID RCOL ECOL LINE LASER TRANSFORM3D MUSPOILER DEGRADER
%token VKICK HKICK
%token ALL PERIOD XSECBIAS TUNNEL MATERIAL ATOM
%token BEAM OPTION PRINT RANGE STOP USE SAMPLE CSAMPLE DUMP
%token IF ELSE BEGN END LE GE NE EQ FOR

%type <dval> aexpr
%type <dval> expr
%type <symp> assignment symdecl
%type <array> vecexpr
%type <array> vectnum vectstr
%type <str> use_parameters
%type <ival> newinstance
%type <name> sample_options
%type <name> csample_options

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
	   write_table(params,$1,ElementType::_MARKER);
	   params.flush();
	 }
	 delete $1;
       }
     | VARIABLE ':' drift
       {
	 if(execute) {
	   // check parameters and write into element table
	   write_table(params,$1,ElementType::_DRIFT);
	   params.flush();
	 }
	 delete $1;
       }
     | VARIABLE ':' rf
       {
	 if(execute) {
	   // check parameters and write into element table
	   write_table(params,$1,ElementType::_RF);
	   params.flush();
	 }
	 delete $1;
       } 
     | VARIABLE ':' sbend
       {  
	 if(execute) {
	   // check parameters and write into element table
	   write_table(params,$1,ElementType::_SBEND);
	   params.flush();
	 }
	 delete $1;
       }
     | VARIABLE ':' rbend
       {
         if(execute) {
           // check parameters and write into element table
           write_table(params,$1,ElementType::_RBEND);
           params.flush();
         }
	 delete $1;
       }

    | VARIABLE ':' vkick
       {  
	 if(execute) {
	   // check parameters and write into element table
	   write_table(params,$1,ElementType::_VKICK);
	   params.flush();
	 }
	 delete $1;
       }
    | VARIABLE ':' hkick
       {  
	 if(execute) {
	   // check parameters and write into element table
	   write_table(params,$1,ElementType::_HKICK);
	   params.flush();
	 }
	 delete $1;
       }
     | VARIABLE ':' quad
       {
	 if(execute)       
	   {
	     // check parameters and write into element table
	     write_table(params,$1,ElementType::_QUAD);
	     params.flush();
	   }
	 delete $1;
       }
     | VARIABLE ':' sextupole
       {
	 if(execute)
	   {
	     // check parameters and write into element table
	     write_table(params,$1,ElementType::_SEXTUPOLE);
	     params.flush();
	   }
	 delete $1;
       }
     | VARIABLE ':' octupole
       {
	 if(execute)
	   {
	     // check parameters and write into element table
	     write_table(params,$1,ElementType::_OCTUPOLE);
	     params.flush();
	   }
	 delete $1;
       }
     | VARIABLE ':' decapole
       {
	 if(execute)
	   {
	     // check parameters and write into element table
	     write_table(params,$1,ElementType::_DECAPOLE);
	     params.flush();
	   }
	 delete $1;
       }
     | VARIABLE ':' multipole
       {
	 if(execute)
	   {	 
	     // check parameters and write into element table
	     write_table(params,$1,ElementType::_MULT);
	     params.flush();	 
	   }
	 delete $1;
       }
     | VARIABLE ':' solenoid
       {
	 if(execute)       
	   {
	     // check parameters and write into element table
	     write_table(params,$1,ElementType::_SOLENOID);
	     params.flush();
	   }
	 delete $1;
       }
     | VARIABLE ':' rcol
       {
	 if(execute)
	   {
	     // check parameters and write into element table
	     write_table(params,$1,ElementType::_RCOL);
	     params.flush();
	   }
	 delete $1;
       }
     | VARIABLE ':' ecol
       {
	 if(execute)
	   {
	     // check parameters and write into element table
	     write_table(params,$1,ElementType::_ECOL);
	     params.flush();
	   }
	 delete $1;
       }
     | VARIABLE ':' muspoiler
       {
	 if(execute)
	   {
	     // check parameters and write into element table
	     write_table(params,$1,ElementType::_MUSPOILER);
	     params.flush();
	   }
	 delete $1;
       }
     | VARIABLE ':' degrader
       {
	 if(execute)
	   {
	     // check parameters and write into element table
	     write_table(params,$1,ElementType::_DEGRADER);
	     params.flush();
	   }
	 delete $1;
       }
     | VARIABLE ':' element
       {
	 if(execute)
	   {	 
	     // check parameters and write into element table
	     write_table(params,$1,ElementType::_ELEMENT);
	     params.flush();	 
	   }
	 delete $1;
       }
     | VARIABLE ':' laser
       {
	 if(execute)
	   {	 
	     // check parameters and write into element table
	     write_table(params,$1,ElementType::_LASER);
	     params.flush();	 
	   }
	 delete $1;
       }
     | VARIABLE ':' screen
       {
	 if(execute) {
	   // check parameters and write into element table
	   write_table(params,$1,ElementType::_SCREEN);
	   params.flush();
	 }
	 delete $1;
       }
     | VARIABLE ':' awakescreen
       {
	 if(execute) {
	   // check parameters and write into element table
	   write_table(params,$1,ElementType::_AWAKESCREEN);
	   params.flush();
	 }
	 delete $1;
       }
     | VARIABLE ':' transform3d
       {
	 if(execute)
	   {	 
	     // check parameters and write into element table
	     write_table(params,$1,ElementType::_TRANSFORM3D);
	     params.flush();	 
	   }
	 delete $1;
       }
     | VARIABLE ':' line 
       {
	 if(execute)
	   {
	     // copy tmp_list to params
	     write_table(params,$1,ElementType::_LINE,new std::list<struct Element>(tmp_list));
	     // clean list
	     tmp_list.clear();
	   }
	 delete $1;
       }     
     | VARIABLE ':' newinstance
       {
         if(execute)
	   {
	     ElementType type = static_cast<ElementType>($3);
	     if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE : VARIABLE, " << *($1) << " : " << type << std::endl;
	     if(type != ElementType::_NONE)
	       {
		 write_table(params,$1,type);
	       }
	     params.flush();
	   }
	 delete $1;
       }
       | VARIABLE ':' parameters
       {
	 if(execute)
	   {
	     if(ECHO_GRAMMAR) std::cout << "edit : VARIABLE parameters   -- " << *($1) << std::endl;
	     std::list<struct Element>::iterator it = element_list.find(*($1));
	     std::list<struct Element>::iterator iterEnd = element_list.end();
	     if(it == iterEnd)
	       {
		 std::cout << "element " << *($1) << " has not been defined" << std::endl;
		 if (PEDANTIC) exit(1);
	       }
	     else
	       {
		 // add and overwrite properties if set
		 (*it).set(params);
	       }
	     params.flush();
	   }
	 delete $1;
       }
     | VARIABLE ':' matdef
       {
	 if(execute)
	   {
	     write_table(params,$1,ElementType::_MATERIAL);
	     params.flush();
	   }
	 delete $1;
       }
     | VARIABLE ':' atom
       {
         if(execute)
           {
             write_table(params,$1,ElementType::_ATOM);
             params.flush();
           }
	 delete $1;
       }
     | VARIABLE ':' tunnel
       {
         if(execute)
           {
	     if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : tunnel" << std::endl;
	     tunnel.set_value("name",*($1));
	     add_tunnel(tunnel);
           }
	 delete $1;
       }
     | VARIABLE ':' xsecbias
       {
         if(execute)
           {
	     if(ECHO_GRAMMAR) std::cout << "decl -> VARIABLE " << *($1) << " : xsecbias" << std::endl;
	     xsecbias.set_value("name",*($1));
	     add_xsecbias(xsecbias);
           }
	 delete $1;
       }
      | VARIABLE ':' error_noparams
      {
	if(execute)
	  {
	    yyerror("ERROR: Element needs parameters");
	  }
	delete $1;
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
degrader : DEGRADER ',' parameters ;
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
               | TUNNEL;
               | XSECBIAS;

newinstance : VARIABLE ',' parameters
            {
	      if(execute) {
		$$ = copy_element_to_params(*($1),params);
	      }
	      delete $1;
	    }
            | VARIABLE
	    {
	      if(execute) {
		$$ = copy_element_to_params(*($1),params);
	      }
	      delete $1;
	    }
;

parameters: VARIABLE '=' aexpr ',' parameters
            {
	      if(execute)
		params.set_value(*($1),$3);
	      delete $1;
	    }
          | VARIABLE '=' aexpr
            {
	      if(execute)
		params.set_value(*($1),$3);
	      delete $1;
	    }
          | VARIABLE '=' vecexpr ',' parameters
            {
	      if(execute) 
		params.set_value(*($1),$3);
	      delete $1;
	    }
          | VARIABLE '=' vecexpr
            {
	      if(execute) 
		params.set_value(*($1),$3);
	      delete $1;
	    }
          | VARIABLE '=' STR ',' parameters
            {
	      if(execute) {
		params.set_value(*($1),$3);
	      }
	      delete $1;
	      free($3);
	    }
          | VARIABLE '=' STR
            {
	      if(execute) {
		params.set_value(*($1),$3);
	      }
	      delete $1;
	      free($3);
	    }

line : LINE '=' '(' element_seq ')'
;

line : LINE '=' '-' '(' rev_element_seq ')'
;

element_seq : 
            | VARIABLE ',' element_seq
              {
		if(execute) add_element_temp(*($1), 1, true, ElementType::_LINE);
		delete $1;
	      }
            | VARIABLE '*' NUMBER ',' element_seq
              {
		if(execute) add_element_temp(*($1), (int)$3, true, ElementType::_LINE);
		delete $1;
	      }
            | NUMBER '*' VARIABLE ',' element_seq
              {
		if(execute) add_element_temp(*($3), (int)$1, true, ElementType::_LINE);
		delete $3;
	      }
            | VARIABLE
              {
		if(execute) add_element_temp(*($1), 1, true, ElementType::_LINE);
		delete $1;
	      }
           | VARIABLE '*' NUMBER
              {
		if(execute) add_element_temp(*($1), (int)$3, true, ElementType::_LINE);
		delete $1;
	      }
            | NUMBER '*' VARIABLE
              {
		if(execute) add_element_temp(*($3), (int)$1, true, ElementType::_LINE);
		delete $3;
	      }
            | '-' VARIABLE ',' element_seq
              {
		if(execute) add_element_temp(*($2), 1, true, ElementType::_REV_LINE);
		delete $2;
	      }
            | '-' VARIABLE
              {
		if(execute) add_element_temp(*($2), 1, true, ElementType::_REV_LINE);
		delete $2;
	      }
;

rev_element_seq : 
            | VARIABLE ',' rev_element_seq 
              {
		if(execute) add_element_temp(*($1), 1, false, ElementType::_REV_LINE);
		delete $1;
	      }
            | VARIABLE '*' NUMBER ',' rev_element_seq
              {
		if(execute) add_element_temp(*($1), int($3), false, ElementType::_REV_LINE);
		delete $1;
	      }
            | NUMBER '*' VARIABLE ',' rev_element_seq
              {
		if(execute) add_element_temp(*($3), int($1), false, ElementType::_REV_LINE);
		delete $3;
	      }
            | VARIABLE
              {
		if(execute) add_element_temp(*($1), 1, false, ElementType::_REV_LINE);
		delete $1;
	      }
           | VARIABLE '*' NUMBER
              {
		if(execute) add_element_temp(*($1), int($3), false, ElementType::_REV_LINE);
		delete $1;
	      }
            | NUMBER '*' VARIABLE
              {
		if(execute) add_element_temp(*($3), int($1), false, ElementType::_REV_LINE);
		delete $3;
	      }
            | '-' VARIABLE ',' element_seq
              {
		if(execute) add_element_temp((*$2), 1, false, ElementType::_LINE);
		delete $2;
	      }
            | '-' VARIABLE
              {
		if(execute) add_element_temp((*$2), 1, false, ElementType::_LINE);
		delete $2;
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
	struct symtab *sp = symlook(*($1));
	if (!sp) {
	  std::string errorstring = "ERROR: use of undeclared variable " + *($1) + "\n";
	  yyerror(errorstring.c_str());
	}
	$$ = sp->value;
	delete $1;
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
	       for(unsigned int i=0;i<$2->data.size();i++)
		 $$ += $2->data[i] * $4->data[i];
	     }
	   else
	     {
	       yyerror("ERROR: vector dimensions do not match");
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
	    if(ECHO_GRAMMAR) std::cout << "aexpr-> " << *($1) << " [ " << *($3) << " ]" << std::endl; 
	    $$ = property_lookup(element_list,*($1),*($3));
	    delete $1;
	    delete $3;
	  }// element attributes
; 

symdecl : VARIABLE '='
        {
	  if(execute)
	    {
	      struct symtab *sp = symlook(*($1));
	      if (!sp) {
		sp = symcreate(*($1));
	      } else {
		std::cout << "WARNING redefinition of variable " << sp->name << " with old value: " << sp->value << std::endl;
	      }
	      $$ = sp;
	    }
	  delete $1;
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
           |  symdecl vecexpr
              {
		if(execute)
		  {
		    $1->array.clear();
		    for(unsigned int i=0;i<$2->data.size();i++)
		      $1->array.push_back($2->data[i]);
		    $1->type = symtab::symtabtype::_ARRAY;
		    $$ = $1;
		    $2->data.clear();
		  }
              }

           |  VECVAR '=' vecexpr
              {
		if(execute)
		  {
		    $1->array.clear();
		    for(unsigned int i=0;i<$3->data.size();i++)
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

vectnumexec : '{' numbers '}'
            | '[' numbers ']'

vectnum : vectnumexec
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

vectstrexec : '[' letters ']'
            | '{' letters '}'
;

vectstr : vectstrexec
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

numbers : aexpr ',' numbers 
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

letters : STR ',' letters
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
	      struct symtab *sp = symlook(*($3));
	      if (!sp) {
		std::cout << "Variable " << *($3) << "not defined!" << std::endl;
	      }
	      else {
		printf("\t%s = %.10g\n",sp->name.c_str(),sp->value);
	      }
	    }
	    delete $3;
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
        | SAMPLE ',' sample_options 
          {
	    if(execute)
	      {  
		if(ECHO_GRAMMAR) printf("command -> SAMPLE\n");
		add_sampler(*($3), element_count);
		element_count = -1;
		params.flush();
	      }
	    delete $3;
          }
        | CSAMPLE ',' csample_options // cylindrical sampler
          {
	    if(execute)
	      {  
		if(ECHO_GRAMMAR) printf("command -> CSAMPLE\n");
		add_csampler(*($3), element_count,params.l, params.r);
		element_count = -1;
		params.flush();
	      }
	    delete $3;
          }
        | DUMP ',' sample_options //  options for beam dump
          {
            if(execute)
              {
                if(ECHO_GRAMMAR) printf("command -> DUMP\n");
                add_dump(*($3), element_count);
                element_count = -1;
                params.flush();
              }
	    delete $3;
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
;

use_parameters :  VARIABLE
                  {
		    if(execute)
		      {
			char * cstr = new char [$1->length()+1];
			std::strcpy (cstr, (*$1).c_str());
			$$ = cstr;
			current_line = (*$1);
			current_start = "";
			current_end = "";
		      }
		    delete $1;
                  }
		| PERIOD '=' VARIABLE
                  {
		    if(execute)
		      {
			char * cstr = new char [$3->length()+1];
			std::strcpy (cstr, (*$3).c_str());
			$$ = cstr;
			current_line = *($3);
			current_start = "";
			current_end = "";
		      }
		    delete $3;
                  }
                | PERIOD '=' VARIABLE ',' RANGE '=' VARIABLE '/' VARIABLE
                  {
		    if(execute)
		      {
			char * cstr = new char [$3->length()+1];
			std::strcpy (cstr, (*$3).c_str());
			$$ = cstr;
			current_line = *($3);
			current_start = *($7);
			current_end = *($9);
		      }
		    delete $3, $7, $9;
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
		  if(ECHO_GRAMMAR) std::cout << "sample, all" << std::endl;
		  // -2: convention to add to all elements
		  // empty name so that element name can be attached
		  if(execute) { $$ = new std::string(""); element_count = -2; }
	        }
;

csample_options : VARIABLE '=' aexpr
                  {
		    if(ECHO_GRAMMAR) std::cout << "csample_opt ->csopt " << (*$1) << " = " << $3 << std::endl;
		    
		    if(execute)
		      {
			if( (*$1) == "r") params.r = $3;
			else if ((*$1) == "l") params.l = $3;
			else {
			  std::string errorstring = "Warning : CSAMPLER: unknown parameter : \"" + (*$1) + "\"\n";
			  yyerror(errorstring.c_str());
			}
		      }
		    delete $1;
		  }   
                | VARIABLE '=' aexpr ',' csample_options
                  {
		    if(ECHO_GRAMMAR) std::cout << "csample_opt ->csopt " << (*$1) << " = " << $3 << std::endl;
		    
		    if(execute)
		      {
			if( (*$1) == "r") params.r = $3;
			else if ((*$1) == "l") params.l = $3;
			else {
			  std::string errorstring = "Warning : CSAMPLER: unknown parameter : \"" + (*$1) + "\"\n";
			  yyerror(errorstring.c_str());
			}
		      }
		    delete $1;
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

tunnel_options : VARIABLE '=' aexpr ',' tunnel_options
                    {
		      if(execute)
			tunnel.set_value((*$1),$3);
		      delete $1;
		    }
                 | VARIABLE '=' aexpr
                    {
		      if(execute)
			tunnel.set_value((*$1),$3);
		      delete $1;
		    }
                 | VARIABLE '=' STR ',' tunnel_options
                    {
		      if(execute)
			tunnel.set_value((*$1),$3);
		      delete $1;
		      free($3);
		    }
                 | VARIABLE '=' STR
                    {
		      if(execute)
			tunnel.set_value((*$1),$3);
		      delete $1;
		      free($3);
		    }
;

xsecbias_options : VARIABLE '=' aexpr ',' xsecbias_options
                    {
		      if(execute)
			xsecbias.set_value((*$1),$3);
		      delete $1;
		    }
                 | VARIABLE '=' aexpr
                    {
		      if(execute)
			xsecbias.set_value((*$1),$3);
		      delete $1;
		    }
                 | VARIABLE '=' STR ',' xsecbias_options
                    {
		      if(execute)
			xsecbias.set_value((*$1),$3);
		      delete $1;
		      free($3);
		    }
                 | VARIABLE '=' STR
                    {
		      if(execute)
			xsecbias.set_value((*$1),$3);
		      delete $1;
		      free($3);
		    }
                 | VARIABLE '=' vecexpr ',' xsecbias_options
		    {
		      if(execute)
			xsecbias.set_value((*$1),$3);
		      delete $1;
		    }
                 | VARIABLE '=' vecexpr
		    {
		      if(execute)
			xsecbias.set_value((*$1),$3);
		      delete $1;
		    }
;

option_parameters : VARIABLE '=' aexpr ',' option_parameters
                    {
		      if(execute)
			options.set_value((*$1),$3);
		      delete $1;
		    }   
                  | VARIABLE '=' aexpr
                    {
		      if(execute)
			options.set_value((*$1),$3);
		      delete $1;
		    } 
                  | VARIABLE '=' STR ',' option_parameters
                    {
		      if(execute)
			options.set_value((*$1),$3);
		      delete $1;
		      free($3);
		    }   
                  | VARIABLE '=' STR
                    {
		      if(execute)
			options.set_value((*$1),$3);
		      delete $1;
		      free($3);
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
