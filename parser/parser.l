/*
 *  Lexical analyzer for gmad bison parser  
 *  Ilya Agapov, 2005
 *  
*/



%x incl

/*
%option yylineno // use own improved line number info
*/

%{
  
#include <cstdio>
#include <map>
#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include "sym_table.h"
#include "parser.tab.hh"
#include "getEnv.h"
#include <unistd.h>

#ifdef _WIN32
#include <io.h>
#define YY_NO_UNISTD_H 1
#endif

extern int yyerror(const char *s);

using namespace GMAD;

namespace GMAD {

struct symtab * symlook(std::string s);
extern std::map<std::string, struct symtab*> symtab_map;

const int max_include_depth = 10;
YY_BUFFER_STATE include_stack[max_include_depth];
std::vector<std::string> include_filename_stack(max_include_depth);
int include_linenum_stack[max_include_depth];
int include_stack_ptr = 0;

int line_num = 1;
std::string yyfilename;

}

%}

%s ERROR

%option nounput

%%

[\t ]+     //ignore whitespaces

(([0-9]+)|([0-9]*\.[0-9]*))((e|E)[+|-]?[0-9]+)? { yylval.dval=atof(yytext); return NUMBER; }


"<=" { return LE; }
">=" { return GE; }
"<>" { return NE; }
"==" { return EQ; }

"!".*$    {line_num++;}    // comments - ignore
"!".*     {line_num++;}    // comments at end of file (without return) - ignore

marker { return MARKER; } // reserved elements
drift { return DRIFT; }
rf { return RF; }
rfcavity { return RF; }
sbend { return SBEND; } 
rbend {return RBEND; }
hkick { return HKICK; }
hkicker { return HKICK; }
vkick { return VKICK; }
vkicker { return VKICK; }
quadrupole { return QUADRUPOLE; }
sextupole { return SEXTUPOLE; }
octupole { return OCTUPOLE; }
decapole { return DECAPOLE; }
multipole { return MULTIPOLE; }
solenoid { return SOLENOID; }
rcol { return RCOL;}
ecol { return ECOL; } 
muspoiler { return MUSPOILER; } 
element { return ELEMENT; }
screen { return SCREEN; }
awakescreen { return AWAKESCREEN; }
transform3d { return TRANSFORM3D ; }
laser { return LASER; }
degrader { return DEGRADER;}

tunnel { return TUNNEL; }

xsecBias {return XSECBIAS; }

matdef { return MATERIAL; }
atom { return ATOM; }

line { return LINE; } 

period { return PERIOD; }
range { return RANGE; }

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
dump  { return DUMP; }

\"[^"]*\" {
  //strip quotes and return string

  // copy string without quotes, 
  // memory needs to be allocated
  // strndup allocates memory with malloc 
  // strndup will add an end of string character
  // memory needs to be freed with free()
  // see e.g. http://linux.die.net/man/3/strdup
  yylval.str = strndup(yytext+1,strlen(yytext)-2);
  return STR;
}

":="    { return '=';}  // alternative assignment 

[a-zA-Z#][A-Za-z0-9_#.]* {
     struct symtab *sp = symlook(std::string(yytext));
     yylval.symp=sp;
     if(sp->funcptr)
       return FUNC;
     else 
     if(sp->type == symtab::symtabtype::_ARRAY)
       return VECVAR;
     else
       return VARIABLE; 
}
.    { return yytext[0]; } // return characters like * and /

<incl>[ \t]*  // eat the whitespace
<incl>[^ \t\n;]+ {

//this is perhaps unnecessary now...
std::string bdsimpath = (std::string)getEnv("BDSIMPATH");
std::string includefilename = "";
std::string mainfilename    = yyfilename; //get the supplied main filename
std::string mainfilepath    = "";
if(bdsimpath.length()>0){
#ifdef BDSDEBUG
   printf("parser> using BDSIMPATH to build included filepaths\n");
#endif
   includefilename = bdsimpath + (std::string)yytext;
} else {
   // get the path part of the supplied path to the main input file
   std::string::size_type found = mainfilename.rfind("/"); // find the last '/'
   if (found != std::string::npos){
     mainfilepath = mainfilename.substr(0,found+1); // the path is the bit before that, including the '/'
   } // else remains empty string
   // need to know whether it's an absolute or relative path
   if ((mainfilename.substr(0,1)) == "/"){
     // the main file has an absolute path
     includefilename = mainfilepath + (std::string)yytext;
   } else {
     // the main file has a relative path or just the file name
     char cwdchars[200]; //filepath up to 200 characters
     // get current working directory
     std::string cwd = (std::string)getcwd(cwdchars, sizeof(cwdchars)) + "/";
     includefilename = cwd + mainfilepath + (std::string)yytext;
   }
}
#ifdef BDSDEBUG
printf("parser> include filename is: %s \n",includefilename.c_str());
#endif
   printf("parser> reading file %s \n",includefilename.c_str());//yytext
     if( include_stack_ptr >= max_include_depth )
       {
         fprintf(stderr , "Error : Include depth exceeds %d\n",max_include_depth);
         exit(1);
       }
     else
    {
      yyin = fopen(includefilename.c_str(), "r"); //yytext
      if(yyin)
	{
          //printf("saving to stack buffer n %d, file %s\n",include_stack_ptr,yyfilename );
          // save info to the stack and load new buffer
          include_linenum_stack[include_stack_ptr] = line_num;
          line_num = 1;
          include_filename_stack[include_stack_ptr] = yyfilename;
          yyfilename = includefilename; //yytext
	  include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
          include_filename_stack[include_stack_ptr] = yyfilename;
	  yy_switch_to_buffer(yy_create_buffer( yyin, YY_BUF_SIZE ) );
	  //printf("done saving to stack\n");
    	}
      else
	{
	  fprintf(stderr, "Error : can't open %s\n", includefilename.c_str()); //yytext
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
      yyfilename = include_filename_stack[include_stack_ptr];
      line_num = include_linenum_stack[include_stack_ptr];
    }
}

\n   line_num++; // unix line separator
\r\n line_num++; // windows line separator

%%


struct symtab * GMAD::symlook(std::string s)
{
  std::map<std::string,symtab*>::iterator it = symtab_map.find(s);
  if (it!=symtab_map.end()) {
     return (*it).second;
  } else {
    struct symtab* sp = new symtab(s);
    std::pair<std::map<std::string,symtab*>::iterator,bool> ret = symtab_map.insert(std::make_pair(s,sp));
     return (*(ret.first)).second;
  }

  yyerror("too many symbols");
  exit(1);
}

