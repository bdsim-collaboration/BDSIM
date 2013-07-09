/*
 *  Lexical analyzer for gmad bison parser  
 *  Ilya Agapov, 2005
 *  
*/



%x incl

/*
%option yylineno
*/

%{
  
#include <iostream>
#include <string>
#include <cstring>
#include "parser.tab.h"
#include "sym_table.h"
#include "getEnv.h"

#define MAXFILENAMELENGTH 200

  
using namespace std;

string sBDSHOME = (string)getEnv("BDSIMHOME");
 
int match_var(char *name);
struct symtab * symlook(char *s);
extern struct symtab *symtab;
extern int yyerror(char *s);

#ifdef _WIN32
#include <io.h>
#define YY_NO_UNISTD_H 1
#endif

#define MAX_INCLUDE_DEPTH 10
YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
char include_filename_stack[MAX_INCLUDE_DEPTH][MAXFILENAMELENGTH];
int include_linenum_stack[MAX_INCLUDE_DEPTH];
int include_stack_ptr = 0;

int line_num = 1;
char* yyfilename;



%}

%s ERROR

%%

[\t ]+     //ignore whitespaces

(([0-9]+)|([0-9]*\.[0-9]*))((e|E)[+|-]?[0-9]+)? { yylval.dval=atof(yytext); return NUMBER; }


"<=" { return LE; }
">=" { return GE; }
"<>" { return NE; }
"==" { return EQ; }

"!".*/\n { }    // comments - ignore

marker { return MARKER; } // reserved elements
drift { return DRIFT; }
pcldrift { return PCLDRIFT; }
rfcavity { return RF; }
dipole { return DIPOLE; }
sbend { return SBEND; } 
rbend {return RBEND; }
hkick { return HKICK; }
vkick { return VKICK; }
kicker { return KICK; } 
quadrupole { return QUADRUPOLE; }
sextupole { return SEXTUPOLE; }
octupole { return OCTUPOLE; }
multipole { return MULTIPOLE; }
solenoid { return SOLENOID; }
rcol { return RCOL;}
ecol { return ECOL; } 
muspoiler { return MUSPOILER; } 
element { return ELEMENT; }
transform3d { return TRANSFORM3D ; }
collimator { return COLLIMATOR; }
pipe { return PIPE; }
gas { return GAS; }
tunnel { return TUNNEL; }
matdef { return MATERIAL; }
atom { return ATOM; }
laser { return LASER; }

line       { return LINE; } 

aperture {return APERTURE; } // reserved keywords
filename { return FILENAME; }
period { return PERIOD; }
range { return RANGE; }
cut { return CUT; }

"if" { return IF; }
"for" { return FOR; }
"else" { return ELSE; } 
"begin" { return BEGN; }
"end" { return END; }


include   BEGIN(incl);  //reserved commands

beam { return BEAM; }
option { return OPTION; }
print { return PRINT; }
"return" { return STOP; }
stop {return STOP;}
use { return USE; }
sample { return SAMPLE; }
csample { return CSAMPLE; }
beta0 { return BETA0; } 
twiss { return TWISS; }
dump  { return DUMP; }

\"[^"]*\" {
  //strip quotes
  
  yylval.str=yytext+1;
  yylval.str[strlen(yylval.str)-1]='\0';
  return STR; 
}

":="    { return '=';}  // alternative assignment 

[a-zA-Z#][A-Za-z0-9_#.]* {
     struct symtab *sp = symlook(yytext);
     yylval.symp=sp;
     if(sp->funcptr)
       return FUNC;
     else 
     if(sp->type == _ARRAY)
       return VECVAR;
     else
       return VARIABLE; 
}
.    { return yytext[0]; }

<incl>[ \t]*  // eat the whitespace
<incl>[^ \t\n;]+ {
string filename = sBDSHOME+(string)yytext;
  printf("parser> reading file %s \n",(char*)filename.c_str());//yytext
  if( include_stack_ptr >= MAX_INCLUDE_DEPTH )
    {
      fprintf(stderr , "Error : Include depth exceeds %d\n",MAX_INCLUDE_DEPTH);
      exit(1);
    }
  else
    {
      yyin = fopen((char*)filename.c_str(), "r"); //yytext
      if(yyin)
	{
          //printf("saving to stack buffer n %d, file %s\n",include_stack_ptr,yyfilename );
          // save info to the stack and load new buffer
          include_linenum_stack[include_stack_ptr] = line_num;
          line_num = 1;
          strncpy(include_filename_stack[include_stack_ptr], yyfilename, MAXFILENAMELENGTH);
          strncpy(yyfilename,(char*)filename.c_str(),MAXFILENAMELENGTH); //yytext
	  include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
          strncpy(include_filename_stack[include_stack_ptr], yyfilename, MAXFILENAMELENGTH);
	  yy_switch_to_buffer(yy_create_buffer( yyin, YY_BUF_SIZE ) );
	  //printf("done saving to stack\n");
    	}
      else
	{
	  fprintf(stderr, "Error : can't open %s\n", (char*)filename.c_str()); //yytext
	  exit(1);
	}
    }
  BEGIN(INITIAL);
}

<<EOF>> {
  if (--include_stack_ptr < 0)
    {
      yyterminate();
    }
  else
    {
      // restore the previous buffer info
      //printf("switching to previous buffer with %s\n", include_filename_stack[include_stack_ptr]);
      yy_delete_buffer(YY_CURRENT_BUFFER);
      yy_switch_to_buffer(include_stack[include_stack_ptr]);
      strncpy( yyfilename, include_filename_stack[include_stack_ptr], MAXFILENAMELENGTH) ;
      line_num = include_linenum_stack[include_stack_ptr];
      
    }
}

[\n]  line_num++;
%%


struct symtab * symlook(char *s)
{
  struct symtab *sp;
  for( sp=symtab;sp<&symtab[NSYMS];sp++) {
    if(sp->name && !strcmp(sp->name,s)) return sp;
    if(!sp->name) {sp->name=strdup(s); return sp;}
  }
  yyerror("too many symbols");
  exit(1);
}

