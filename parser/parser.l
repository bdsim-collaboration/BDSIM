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
#include <cstring>
#include <iostream>
#include "parser.tab.hh"
#include "sym_table.h"
#include "getEnv.h"
#include <unistd.h>

const int maxfilenamelength = 200;

int match_var(char *name);
struct symtab * symlook(const char *s);
extern std::map<std::string, struct symtab*> symtab_map;
extern int yyerror(const char *s);

#ifdef _WIN32
#include <io.h>
#define YY_NO_UNISTD_H 1
#endif

const int max_include_depth = 10;
YY_BUFFER_STATE include_stack[max_include_depth];
char include_filename_stack[max_include_depth][maxfilenamelength];
int include_linenum_stack[max_include_depth];
int include_stack_ptr = 0;

int line_num = 1;
char* yyfilename;

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
pcldrift { return DRIFT; }
rf { return RF; }
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
screen { return SCREEN; }
awakescreen { return AWAKESCREEN; }

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
std::string mainfilename    = (std::string)yyfilename; //get the supplied main filename
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
   printf("parser> reading file %s \n",(char*)includefilename.c_str());//yytext
     if( include_stack_ptr >= max_include_depth )
       {
         fprintf(stderr , "Error : Include depth exceeds %d\n",max_include_depth);
         exit(1);
       }
     else
    {
      yyin = fopen((char*)includefilename.c_str(), "r"); //yytext
      if(yyin)
	{
          //printf("saving to stack buffer n %d, file %s\n",include_stack_ptr,yyfilename );
          // save info to the stack and load new buffer
          include_linenum_stack[include_stack_ptr] = line_num;
          line_num = 1;
          strncpy(include_filename_stack[include_stack_ptr], yyfilename, maxfilenamelength);
          strncpy(yyfilename,(char*)includefilename.c_str(),maxfilenamelength); //yytext
	  include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
          strncpy(include_filename_stack[include_stack_ptr], yyfilename, maxfilenamelength);
	  yy_switch_to_buffer(yy_create_buffer( yyin, YY_BUF_SIZE ) );
	  //printf("done saving to stack\n");
    	}
      else
	{
	  fprintf(stderr, "Error : can't open %s\n", (char*)includefilename.c_str()); //yytext
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
      strncpy( yyfilename, include_filename_stack[include_stack_ptr], maxfilenamelength) ;
      line_num = include_linenum_stack[include_stack_ptr];
    }
}

\n   line_num++; // unix line separator
\r\n line_num++; // windows line separator

%%


struct symtab * symlook(const char *s)
{
  std::string sstring = std::string(s);
  std::map<std::string,symtab*>::iterator it = symtab_map.find(sstring);
  if (it!=symtab_map.end()) {
     return (*it).second;
  } else {
    struct symtab* sp = new symtab(sstring);
    std::pair<std::map<std::string,symtab*>::iterator,bool> ret = symtab_map.insert(std::make_pair(sstring,sp));
     return (*(ret.first)).second;
  }

  yyerror("too many symbols");
  exit(1);
}

