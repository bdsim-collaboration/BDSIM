/*
   bison grammar for the gmad parser
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
%token SOLENOID RCOL ECOL LINE LASER TRANSFORM3D MUSPOILER DEGRADER
%token VKICK HKICK
%token ALL PERIOD XSECBIAS TUNNEL MATERIAL ATOM
%token BEAM OPTION PRINT RANGE STOP USE SAMPLE CSAMPLE DUMP
%token IF ELSE BEGN END LE GE NE EQ FOR

%type <dval> aexpr
%type <dval> expr
%type <symp> assignment
%type <array> vecexpr
%type <array> vectnum vectstr
%type <str> use_parameters
%type <ival> newinstance
%type <symp> sample_options
%type <symp> csample_options
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
     | VARIABLE ':' degrader
       {
	 if(execute)
	   {
	     // check parameters and write into element table
	     write_table(params,$1->name,ElementType::_DEGRADER);
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
       | VARIABLE ':' parameters
       {
	 if(execute)
	   {
	     if(ECHO_GRAMMAR) std::cout << "edit : VARIABLE parameters   -- " << $1->name << std::endl;
	     std::list<struct Element>::iterator it = element_list.find($1->name);
	     std::list<struct Element>::iterator iterEnd = element_list.end();
	     if(it == iterEnd)
	       {
		 std::cout << "element " << $1->name << " has not been defined" << std::endl;
		 if (PEDANTIC) exit(1);
	       }
	     else
	       {
		 // add and overwrite properties if set
		 (*it).set(params);
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
      | VARIABLE ':' error_noparams
      {
	if(execute)
	  {
	    yyerror("ERROR: Element needs parameters");
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
		$$ = copy_element_to_params($1->name,params);
	      }
	    }
            | VARIABLE
	    {
	      if(execute) {
		$$ = copy_element_to_params($1->name,params);
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
	    if(ECHO_GRAMMAR) std::cout << "aexpr-> " << $1->name << " [ " << $3->name << " ]" << std::endl; 
	    $$ = property_lookup(element_list,$1->name,$3->name);
	  }// element attributes
 ; 

assignment :  VARIABLE '=' aexpr  
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
        | SAMPLE ',' sample_options 
          {
	    if(execute)
	      {  
		if(ECHO_GRAMMAR) printf("command -> SAMPLE\n");
		add_sampler($3->name, element_count);
		element_count = -1;
		params.flush();
	      }
          }
        | CSAMPLE ',' csample_options // cylindrical sampler
          {
	    if(execute)
	      {  
		if(ECHO_GRAMMAR) printf("command -> CSAMPLE\n");
		add_csampler($3->name, element_count,params.l, params.r);
		element_count = -1;
		params.flush();
	      }
          }
        | DUMP ',' sample_options //  options for beam dump
          {
            if(execute)
              {
                if(ECHO_GRAMMAR) printf("command -> DUMP\n");
                add_dump($3->name, element_count);
                element_count = -1;
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
		  if(execute) $$ = $3;
                }
              | RANGE '=' VARIABLE '[' NUMBER ']'
                {
                  if(ECHO_GRAMMAR) std::cout << "sample_opt : RANGE =  " << $3->name << " [" << $5 << "]" << std::endl;
		  if(execute) { $$ = $3; element_count = (int)$5; }
                }
              | ALL
	        {
		  if(ECHO_GRAMMAR) std::cout << "sample, all" << std::endl;
		  // -2: convention to add to all elements
		  // empty name so that element name can be attached
		  // create variable name with empty string
		  if(execute) { $$ = symlook(""); element_count = -2; }
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
			  std::string errorstring = "Warning : CSAMPLER: unknown parameter : \"" + $1->name + "\"\n";
			  yyerror(errorstring.c_str());
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
			  std::string errorstring = "Warning : CSAMPLER: unknown parameter : \"" + $1->name + "\"\n";
			  yyerror(errorstring.c_str());
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

option_parameters : VARIABLE '=' aexpr ',' option_parameters
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
