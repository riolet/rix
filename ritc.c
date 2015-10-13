#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>

typedef enum {false, true} bool;
int ritch_i_;
#define foreach(item,array) for (ritch_i_=0;(item=array[ritch_i_])!=NULL;ritch_i_++)

#define BUFFLEN 1024
#define LABELMAX 8096
#define STACKDEP 1024
#define MAXSCOPE 64
#define EVAL_BUFF_MAX_LEN 4096

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


typedef struct Function_ {
    char name[BUFFLEN];
    char type[BUFFLEN];
    bool codeBlocks;
    char defaultObject[BUFFLEN];
    bool assigns;
    int numberOfParams;
} Function;

Function funcList[LABELMAX];
int funcListIdx;

typedef struct Identifier_ {
    char name[BUFFLEN];
    char type[BUFFLEN];
    char ofType[BUFFLEN];
} Identifier;

Identifier idents[LABELMAX][MAXSCOPE];
int identIdx[MAXSCOPE]= {0};

typedef struct Type_ {
    char name[BUFFLEN];
    char parent[BUFFLEN];
} Type;

Type typeList[LABELMAX];
int typeIdx;
char currentType[BUFFLEN];

typedef enum {
    nss, eol, eof, ident, intnumber, stringlit, floatnumber, character, number, lparen, rparen, times, slash, plus,
    minus, assign, equal, neq, lss, leq, gtr, geq, booland, boolor, callsym, beginsym, semicolon, endsym, comma, varsym, procsym, period, oddsym, plusassign,minusassign,timesassign,slashassign,
    function, evaluation, range, exponent, cinc, comment, notsym, type, retsym, fundec, colon, bitwisexor,compare,emptyParam
}
Symbol;


const char * symnames[]= {
    "nss", "eol", "eof", "ident", "intnumber", "stringlit", "floatnumber", "character", "number",
    "lparen", "rparen", "times", "slash", "plus", "minus", "assign", "equal",
    "neq", "lss", "leq", "gtr", "geq", "booland", "boolor", "callsym", "beginsym", "semicolon",
    "endsym", "comma", "varsym", "procsym", "period", "oddsym", "plusassign", "minusassign","timesassign","slashassign","function",
    "evaluation","range", "exponent", "cinc", "comment", "notsym", "type", "retsym", "fundec", "colon", "bitwisexor","compare","emptyParam"
};

typedef enum { object, method } ExpType;
ExpType expType;

Symbol sym;
char symStr[BUFFLEN];
int symStrIdx;
FILE *file;
FILE *outfile;
FILE *outMainFile;
FILE *outHeaderFile;

typedef struct OperStruct_ {
    Symbol oper;
    char operSymStr[BUFFLEN];
    int args;
    char type[BUFFLEN];
} OperStruct;

OperStruct optrStack[STACKDEP];
int optrStackPtr;

OperStruct oprnStack[STACKDEP];
int oprnStackPtr;

int lParenList[STACKDEP];
int lParenPtr;

char buff[BUFFLEN];
int linePos;
int lineNum;
int scopeLevel;
int indentLevel[MAXSCOPE];
int args;
bool expectScopeIncrease;

void createType (const char * name, const char * parent)
{
    strcpy(typeList[typeIdx].name,name);
    strcpy(typeList[typeIdx].parent,parent);
    typeIdx++;
}

void createObject(const char * name, const char * type)
{
    strcpy(idents[identIdx[scopeLevel]][scopeLevel].name,name);
    strcpy(idents[identIdx[scopeLevel]][scopeLevel].type,type);
    identIdx[scopeLevel]++;
}

void createFunction(const char * name, const char * type, bool codeBlock, char * defaultObject, bool assigns, const int numParams)
{
    strcpy(funcList[funcListIdx].name,name);
    strcpy(funcList[funcListIdx].type,type);
    if (defaultObject!=NULL)
        strcpy(funcList[funcListIdx].defaultObject,defaultObject);
    else
        funcList[funcListIdx].defaultObject[0]=0;
    funcList[funcListIdx].codeBlocks=codeBlock;
    funcList[funcListIdx].assigns=assigns;
    funcList[funcListIdx].numberOfParams=numParams;
    funcListIdx++;
}

const char * getFunctionType(const char * func)
{
    int i;
    for (i=0; i<funcListIdx; i++) {
        if (strcmp(funcList[i].name,func)==0) {
            return funcList[i].type;
        }
    }
    return NULL;
}

const int * getFunctionParamCount(const char * func)
{
    int i;
    for (i=0; i<funcListIdx; i++) {
        if (strcmp(funcList[i].name,func)==0) {
            return &funcList[i].numberOfParams;
        }
    }
    return NULL;
}

const char * getTypeParent(const char * func)
{
    int i;
    for (i=0; i<typeIdx; i++) {
        if (strcmp(typeList[i].name,func)==0) {
            return typeList[i].parent;
        }
    }
    return NULL;
}

const char * getIdentifierType(const char * identifier)
{
    int i,scope;
    printf (ANSI_COLOR_CYAN "Identifier %s\n" ANSI_COLOR_RESET,identifier);
    for (scope=scopeLevel; scope>=0; scope--) {
        for (i=0; i<identIdx[scope]; i++) {
            if (strcmp(idents[i][scope].name,identifier)==0) {
                return idents[i][scope].type;
            }
        }
    }
    return NULL;
}

const char * getFunctionObject(const char * funcname)
{
    int i;
    for (i=0; i<funcListIdx; i++) {
        if (strcmp(funcList[i].name,funcname)==0) {
            return funcList[i].defaultObject;
        }
    }
    return NULL;
}

bool  getFunctionCodeBlocks(const char * funcname)
{
    int i;
    for (i=0; i<funcListIdx; i++) {
        if (strcmp(funcList[i].name,funcname)==0) {
            return funcList[i].codeBlocks;
        }
    }
    return false;
}

bool  getFunctionAssigns(const char * funcname)
{
    int i;
    for (i=0; i<funcListIdx; i++) {
        if (strcmp(funcList[i].name,funcname)==0) {
            return funcList[i].assigns;
        }
    }
    return false;
}




int errorMsg(const char * format,...)
{
    int ret;
    fprintf(stderr,"Line %d: Column:%d - ",lineNum,linePos);
    va_list arg;
    va_start(arg,format);
    ret = vfprintf(stderr, format, arg);
    va_end(arg);
    return ret;
}

bool doAssignDeclare(int tor, int rnd, char * holderSymStack, char * ltype, char * rtype)
{
    printf("=====------ doAssignDeclare()\n");
    const char * idType=getIdentifierType(oprnStack[rnd-1].operSymStr);
    //Symbol torOper=optrStack[tor].oper;
    int i;
    for (i=0; i<=scopeLevel; i++) {
        fprintf(outfile,"\t");
    }
    if (idType==NULL) {

        bool isInteger=!strcmp(rtype,"Integer");
        bool isFloat=!strcmp(rtype,"Float");
        bool isStringlit=!strcmp(rtype,"stringlit");
        bool isString=!strcmp(rtype,"String");

        strcpy(idents[identIdx[scopeLevel]][scopeLevel].name,oprnStack[rnd-1].operSymStr);
        if (isStringlit) {
            strcpy(idents[identIdx[scopeLevel]][scopeLevel].type,"String");
        } else {
            strcpy(idents[identIdx[scopeLevel]][scopeLevel].type,rtype);
        }
        //printf("New ident: %s %s;\n",idents[identIdx].type,idents[identIdx].name);

        if (isInteger) {
            fprintf(outfile,"int %s;\n",idents[identIdx[scopeLevel]][scopeLevel].name);
        } else  if (isFloat) {
            fprintf(outfile,"float %s;\n",idents[identIdx[scopeLevel]][scopeLevel].name);
        } else  if (isString) {
            fprintf(outfile,"String %s;\n",idents[identIdx[scopeLevel]][scopeLevel].name);
        } else {
            fprintf(outfile,"%s * %s;\n",idents[identIdx[scopeLevel]][scopeLevel].type,idents[identIdx[scopeLevel]][scopeLevel].name);
        }
        identIdx[scopeLevel]++;

    } else {
        if (strcmp(idType,rtype)!=0) {
            errorMsg("You can't redefine %s. This is not PHP\n",oprnStack[rnd-1].operSymStr);
            exit(0);
        }
    }
    return false;
}

void evaluate(void)
{
    printf("=====------ evaluate()\n");
    int tor=optrStackPtr-1;
    int rnd=oprnStackPtr-1;
    char evalBuff[EVAL_BUFF_MAX_LEN];
    int evalBuffLen=0;

    Symbol holderSymbol;
    char holderSymStack[BUFFLEN];

    Symbol invTorOper = optrStack[tor].oper;
    char * invTorSym =  optrStack[tor].operSymStr;
    bool parenMode = (invTorOper == rparen);
    holderSymbol=oprnStack[rnd].oper;
    strcpy(holderSymStack,oprnStack[rnd].operSymStr);

    printf("Paren mode %d\n",parenMode);

    bool rTypeSet=false;
    char rtype[BUFFLEN];
    bool lTypeSet=false;
    char ltype[BUFFLEN];

    bool singleLineAssign=false;

    //determine the rtype of this operator
    //  note: the rtype for a function is the type of it's first (non-self) parameter.
    if (oprnStack[rnd].type[0]!=0) {
        strcpy(rtype,oprnStack[rnd].type);
    } else if (oprnStack[rnd].oper==stringlit) {
        strcpy(rtype,"String");
    } else if (oprnStack[rnd].oper==intnumber) {
        strcpy(rtype,"Integer");
    } else if (oprnStack[rnd].oper==floatnumber) {
        strcpy(rtype,"Float");
    } else if (oprnStack[rnd].oper==ident) {
        const char *idType=getIdentifierType(holderSymStack);
        if (idType!=NULL)
            strcpy(rtype,getIdentifierType(holderSymStack));
        else
            strcpy(rtype,"???");
    } else if (oprnStack[rnd].oper==emptyParam) {
        strcpy(rtype, "");
    }

    char * semiColonStr="; ";


    /* Iterate through the operators right to left */
    char addParam[BUFFLEN];
    char addParamTypes[BUFFLEN];

    strcpy(addParam,"");
    strcpy(addParamTypes,"");
    bool skip = false;
    for (tor=optrStackPtr-1; tor>=0; tor--) {
        int arg;

        bool boolean=false;
        bool arithmetic=false;
        bool floatArith=false;
        Symbol torOper = optrStack[tor].oper;
        char * torSym =  optrStack[tor].operSymStr;

        if (torOper==retsym) {
            fprintf(outfile,"return (%s);",holderSymStack);
            skip=true;
            continue;
        }

        if (torOper==endsym) {
            errorMsg(ANSI_COLOR_YELLOW "TOR %s %d\n" ANSI_COLOR_RESET,symnames[torOper],torOper);
            semiColonStr=";}";
            scopeLevel--;
            continue;
        }

        if (parenMode&&torOper==rparen) {
            printf("Right paren optrStackPtr %d\n",optrStackPtr);
            continue;
        }

        if (parenMode&&torOper==lparen) {
            printf("%s oprnStack[rnd].operSymStr: %s, HOLDER: %s\n",torSym,oprnStack[rnd].operSymStr,holderSymStack);
            lParenPtr--;
            int lParenIdx=lParenList[lParenPtr];
            //TODO: high coupling: editing the stack here
            strcpy(oprnStack[lParenIdx].operSymStr,holderSymStack);
            strcpy(oprnStack[lParenIdx].type,rtype);
            optrStackPtr=tor;
            oprnStackPtr=lParenIdx+1;
            printf("Symstack: %s Type: %s optrStackPtr %d oprnStackPtr %d\n ",holderSymStack,rtype,optrStackPtr,oprnStackPtr);
            return;
        }
        else
        {

            char * openingBracket="(";
            char * closingBracket=")";
            if (tor==0 || optrStack[tor-1].oper==comma) {
                openingBracket="";
                closingBracket="";
            }

            if (torOper==nss) {
                continue;
            } else if (torOper==comma) {
                char temp[BUFFLEN];
                snprintf(temp,BUFFLEN,",(%s)%s",holderSymStack, addParam);
                strcpy(addParam,temp);
                snprintf(temp,BUFFLEN,"_%s%s",rtype, addParamTypes);
                strcpy(addParamTypes,temp);
                rnd--;
                strcpy(holderSymStack,oprnStack[rnd].operSymStr);
                continue;
            }


            printf ("%s %s\n",symnames[torOper],torSym);
            char fn[BUFFLEN];
            if (optrStack[tor].oper==function) {
                strcpy(fn,torSym);
            } else {
                strcpy(fn,symnames[torOper]);
            }
            //bool assigns=getFunctionCodeBlocks(funcName)
            printf("RND %d oprnStackPtr %d\n",rnd,oprnStackPtr);
            if ( (!parenMode && rnd>0) || (parenMode && rnd>lParenList[lParenPtr-1]) )
            {

                //determine ltype of operator. (what type needs to be returned)
                printf ("oprnStack[rnd-1].type : %s\n",oprnStack[rnd-1].type);
                if (oprnStack[rnd-1].type[0]!=0) {
                    strcpy(ltype,oprnStack[rnd-1].type);
                } else if (oprnStack[rnd-1].oper==stringlit) {
                    strcpy(ltype,"String");
                } else if (oprnStack[rnd-1].oper==intnumber) {
                    strcpy(ltype,"Integer");
                } else if (oprnStack[rnd-1].oper==floatnumber) {
                    strcpy(ltype,"Float");
                } else if (oprnStack[rnd-1].oper==ident) {
                    const char * ltype_=getIdentifierType(oprnStack[rnd-1].operSymStr);
                    if (ltype_==NULL) {
                        strcpy(ltype,"Identifier");
                    } else {
                        strcpy(ltype,ltype_);
                    }
                }

                char funcName[BUFFLEN];

                errorMsg("TOR %s %d\n",symnames[torOper],torOper);

                //If ltype is in [identifier, integer, float] and rtype is in [integer, float]
                if ((!strcmp(ltype,"Identifier")||!strcmp(ltype,"Integer")||!strcmp(ltype,"Float"))&&(!strcmp(rtype,"Integer")||!strcmp(rtype,"Float")))
                {
                    if (torOper==range||torOper==exponent||torOper==compare) {
                        torOper=function;
                    } else if (torOper==gtr||torOper==equal||torOper==lss||torOper==leq||torOper==geq) {
                        boolean=true;
                    } else {
                        arithmetic=true;
                    }
                    /* ToDo: Too many strcmps */
                    if (!strcmp(ltype,"Float")||!strcmp(rtype,"Float")) {
                        floatArith=true;
                    }
                    if (torOper==function)
                    {
                        //for statements are handled here
                        snprintf(funcName,BUFFLEN,"%s_%s_%s%s",ltype,fn,rtype,addParamTypes);
                        evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s%s(%s,%s%s)%s",
                                             openingBracket,
                                             funcName,
                                             oprnStack[rnd-1].operSymStr,
                                             holderSymStack,
                                             addParam,
                                             closingBracket);
                    }
                    else
                    {
                        snprintf(funcName,BUFFLEN,"%s_%s_%s%s",ltype,fn,rtype,addParamTypes);
                        if (addParamTypes!=0) {
                            // infix operators (+, -, *, /, =, <, >) end up here
                            evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s%s %s %s%s%s",
                                                 openingBracket,
                                                 oprnStack[rnd-1].operSymStr,
                                                 torSym,
                                                 holderSymStack,
                                                 addParam,
                                                 closingBracket);
                        }
                        else
                        {
                            snprintf(funcName,BUFFLEN,"%s_%s_%s%s",ltype,fn,rtype,addParamTypes);
                            evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s%s(%s,%s%s)%s",
                                                 openingBracket,
                                                 funcName,
                                                 oprnStack[rnd-1].operSymStr,
                                                 holderSymStack,
                                                 addParam,
                                                 closingBracket);
                        }
                    }
                }

                //boolean operations:  && and || should go here
                else if (!strcmp(rtype,"Boolean")&&!strcmp(ltype,"Boolean")&&(torOper==booland||torOper==boolor))
                {
                    evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s%s %s %s%s%s",
                                         openingBracket,
                                         oprnStack[rnd-1].operSymStr,
                                         torSym,
                                         holderSymStack,
                                         addParam,
                                         closingBracket);
                    boolean = true;
                }

                //if rtype is String and we're doing assignment
                else  if (!strcmp(rtype,"String")&&torOper==assign)
                {
                    //For things like (me = "Bob"; myself = me; me = "differentBob"
                    //note: funcName ("String_assign_String") isn't actually used here.
                    snprintf(funcName,BUFFLEN,"%s_%s_%s%s",ltype,fn,rtype,addParamTypes);
                    evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s%s %s %s%s%s",
                                         openingBracket,
                                         oprnStack[rnd-1].operSymStr,
                                         torSym,
                                         holderSymStack,
                                         addParam,
                                         closingBracket);
                }

                else
                {
                    //if the function takes no arguments
                    if (rtype[0]==0)
                        snprintf(funcName,BUFFLEN,"%s_%s%s",ltype,fn,addParamTypes);
                    else
                        snprintf(funcName,BUFFLEN,"%s_%s_%s%s",ltype,fn,rtype,addParamTypes);
                    if (holderSymStack[0]==0) { //to handle excess commas.
                        //print, echo, if, while statements go here
                        evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s%s(%s%s)%s",
                                         openingBracket,
                                         funcName,
                                         oprnStack[rnd-1].operSymStr,
                                         addParam,
                                         closingBracket);
                    } else {
                        evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s%s(%s,%s%s)%s",
                                         openingBracket,
                                         funcName,
                                         oprnStack[rnd-1].operSymStr,
                                         holderSymStack,
                                         addParam,
                                         closingBracket);
                     }

                }

                bool funcAssigns = getFunctionAssigns(funcName);

                if ((arithmetic&&torOper==assign)||(!strcmp(rtype,"String")&&torOper==assign)||funcAssigns) {
                    singleLineAssign=doAssignDeclare(tor, rnd, holderSymStack, ltype, rtype);
                }

                if (arithmetic&&torOper!=function) {
                    if (floatArith) {
                        strcpy(rtype,"Float");
                    } else {
                        strcpy(rtype,"Integer");
                    }
                }  else if (boolean) {
                    strcpy(rtype,"Boolean");
                } else {
                    const char * funcType=getFunctionType(funcName);
                    if (funcType==NULL) {
                        errorMsg(ANSI_COLOR_RED "Warning: Unknown method %s. Assuming void\n" ANSI_COLOR_RESET,funcName);
                        strcpy(rtype,"void");

                    } else {
                        strcpy(rtype,funcType);
                    }
                }
                printf ("Code blocks %s %d\n",funcName,getFunctionCodeBlocks(funcName));
                if (getFunctionCodeBlocks(funcName)) {
                    semiColonStr="{";
                    scopeLevel++;
                    expectScopeIncrease=true;
                }

                rnd--;
            }
            else
            {
                /*Unary funcs*/
                char funcName[BUFFLEN];
                if (torOper==function) {
                    snprintf(funcName,BUFFLEN,"%s_%s",rtype,fn);
                    evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s(%s)",
                                         funcName,
                                         holderSymStack);
                    if (getFunctionCodeBlocks(funcName)) {
                        semiColonStr="{";
                        scopeLevel++;
                        expectScopeIncrease=true;
                    }
                }


                if (torOper==endsym) {
                    semiColonStr=";}";
                    scopeLevel--;
                    continue;
                }  else {
                    evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s_%s(%s)",rtype,fn,holderSymStack);
                }
                rnd--;
            }

            holderSymbol=evaluation;
            strncpy(holderSymStack,evalBuff,evalBuffLen+1);
            //printf("%d %s\n",evalBuffLen,evalBuff);
            strcpy(addParam,"");
            strcpy(addParamTypes,"");
        }
    }
    if (skip)
        return;
    if (evalBuffLen>0&&!singleLineAssign) {
        printf ("Scope level %d %s\n",scopeLevel,evalBuff);
        int i=0;

        if (!strcmp(semiColonStr,"{")) {
            i=1;
        }
        if (!strcmp(semiColonStr,";}")) {
            i=-1;
        }
        for (; i<=scopeLevel; i++) {
            fprintf(outfile,"\t");
        }
        fprintf(outfile,"%s%s\n",evalBuff,semiColonStr);
    } else if (!strcmp(semiColonStr,";}")) {
        int i;
        for (i=0; i<=scopeLevel; i++) {
            fprintf(outfile,"\t");
        }
        fprintf(outfile,"}\n");
    }
    if (!strcmp(semiColonStr,";}")&&(scopeLevel==0)&&(outfile==outHeaderFile)) {
        outfile=outMainFile;
    }
}

void evaluateAndReset(void)
{
    printf("=====------ evaluateAndReset()\n");
    evaluate();
    expType=method;
    optrStackPtr=0;
    oprnStackPtr=0;
    lParenPtr=0;
}

void oprnStackUpdate(Symbol operSymbol, const char* operSymbolString, int symStrLen, int args, const char* type)
{
    oprnStack[oprnStackPtr].oper=operSymbol;

    if (operSymbol==stringlit)
        snprintf(oprnStack[oprnStackPtr].operSymStr,BUFFLEN,"String_stringlit(\"%s\")",operSymbolString);
    else
        strncpy(oprnStack[oprnStackPtr].operSymStr,operSymbolString,symStrLen+1);

    oprnStack[oprnStackPtr].args=args;

    if (type==NULL)
        oprnStack[oprnStackPtr].type[0]=0;
    else
        strcpy(oprnStack[oprnStackPtr].type, type);

    oprnStackPtr++;
    expType=object;
}

void optrStackUpdate(Symbol operSymbol, const char* operSymbolString, int symStrLen, int args, const char* type)
{
    optrStack[optrStackPtr].oper=operSymbol;

    strncpy(optrStack[optrStackPtr].operSymStr,operSymbolString,symStrLen+1);

    optrStack[optrStackPtr].args=args;

    if (type==NULL)
        optrStack[optrStackPtr].type[0]=0;
    else
        strcpy(optrStack[optrStackPtr].type, type);


    optrStackPtr++;
    args=0;
    expType=method;
}

void getln(void)
{
    printf("=====------ getln()\n");
    lineNum++;
    if (!fgets(buff,BUFFLEN,file)) {
        sym=eof;
    }
    linePos=0;
    evaluateAndReset();
}

void getsym(void)
{
    printf("=====------ getSym()\n");
    symStrIdx=0;
    bool lineBegins = false;

    if (linePos==0)
        lineBegins = true;

    while(buff[linePos]==' '||buff[linePos]=='\t') {
        linePos++;
    }

    //make sure code is indented as expected in relation to nesting code
    if ((buff[linePos]!='\n')&&lineBegins&&scopeLevel>0) {
        errorMsg(ANSI_COLOR_YELLOW "Top Linepos %d identLevel %d scopeLevel %d\n" ANSI_COLOR_RESET,linePos,indentLevel[scopeLevel-1],scopeLevel);
        if (linePos>indentLevel[scopeLevel-1]) {
            if (expectScopeIncrease) {
                indentLevel[scopeLevel-1]=linePos;
                errorMsg(ANSI_COLOR_YELLOW "If Linepos %d identLevel %d scopeLevel %d\n" ANSI_COLOR_RESET,linePos,indentLevel[scopeLevel-1],scopeLevel);
                //errorMsg(ANSI_COLOR_YELLOW "After Linepos %d identLevel %d %d\n" ANSI_COLOR_RESET,linePos,indentLevel[scopeLevel-1],indentLevel[scopeLevel]);
                //expectScopeIncrease=false;
            } else {
                printf(ANSI_COLOR_RED "Unexpected scope increase\n" ANSI_COLOR_RESET);
                exit(0);
            }
        } else if (linePos<indentLevel[scopeLevel-1]) {
            while (linePos<indentLevel[scopeLevel-1]) {
                errorMsg(ANSI_COLOR_YELLOW "while before Linepos %d identLevel %d scopeLevel %d\n" ANSI_COLOR_RESET,linePos,indentLevel[scopeLevel-1],scopeLevel);
                sym=endsym;
                optrStackUpdate(endsym, "}", 1, args, NULL);
                evaluateAndReset();
                errorMsg(ANSI_COLOR_YELLOW "while after Linepos %d identLevel %d scopeLevel %d\n" ANSI_COLOR_RESET,linePos,indentLevel[scopeLevel-1],scopeLevel);
            }
        }
    }

    if (buff[linePos]=='\n') {
        sym=eol;
        getln();
    }

    else if (buff[linePos]==';') {
        sym=semicolon;
        evaluateAndReset();
        linePos++;
    }
    /* Identifier (variable, function, loop, conditional) */
    else if ((buff[linePos]>='a'&&buff[linePos]<='z')||(buff[linePos]>='A'&&buff[linePos]<='Z')) {
        while ((buff[linePos]>='a'&&buff[linePos]<='z')||(buff[linePos]>='A'&&buff[linePos]<='Z')||(buff[linePos]>='0'&&buff[linePos]<='9')) {
            symStr[symStrIdx++]=buff[linePos++];
        }
        symStr[symStrIdx]=0;
        //Todo: Optimize, we know the string length
        printf ("%s %d\n",symStr,expType);
        const char * typeParent=getTypeParent(symStr);
        //If the Identifier is a type
        if (typeParent!=NULL) {
            sym=type;
            optrStackUpdate(type, symStr, symStrIdx, 0, NULL);
        } else if (expType==object) {
            //for loops get processed here.
            sym=function;
            optrStackUpdate(function, symStr, symStrIdx, 0, NULL);

        } else {
            const char * fnObj=getFunctionObject(symStr);
            //If the Identifier is a function
            if (fnObj!=NULL) {
                const int * numParams;
                sym=function;
                optrStackUpdate(function, symStr, symStrIdx, 0, NULL);

                printf (ANSI_COLOR_MAGENTA "symstr fnObj %s:%s\n" ANSI_COLOR_RESET,fnObj,symStr);

                //TODO why is expType here?
                ExpType old = expType;
                //If the function has a default parameter
                if (fnObj[0]!=0) {
                    oprnStackUpdate(ident, fnObj, strlen(fnObj), 0, getIdentifierType(fnObj));
                //else the function does not have a default parameter
                } else {
                    oprnStackUpdate(ident, "UNKNOWNOBJECT", 13, 0, "UNKNOWNTYPE");
                }

                numParams = getFunctionParamCount(symStr);
                if (numParams && *numParams == 0) {
                    oprnStackUpdate(emptyParam, "", 0, 0, NULL);
                }
                //TODO see todo above.
                expType = old;

                printf (ANSI_COLOR_MAGENTA "operSymStr type %s:%s\n" ANSI_COLOR_RESET,
                        oprnStack[oprnStackPtr-1].operSymStr,
                        oprnStack[oprnStackPtr-1].type);

            //else identifier is a variable
            } else {
                sym=ident;
                oprnStackUpdate(ident, symStr, symStrIdx, 0, NULL);
            }
        }
    }

    /* String Literal */
    else if ((buff[linePos]=='"')) {
        linePos++;

        while(buff[linePos]!='"' || buff[linePos-1]=='\\') {
            symStr[symStrIdx++]=buff[linePos++];
        }

        symStr[symStrIdx]=0;
        sym=stringlit;
        oprnStackUpdate(stringlit, symStr, symStrIdx, 0, NULL);

        linePos++;
    }

    /* Number */
    else if ((buff[linePos]>='0'&&buff[linePos]<='9')) {
        while ((buff[linePos]>='0'&&buff[linePos]<='9')) {
            symStr[symStrIdx++]=buff[linePos++];
        }
        if (buff[linePos]=='.') {

            if ((buff[linePos+1]>='0'&&buff[linePos+1]<='9')) {
                symStr[symStrIdx++]=buff[linePos++];
                while ((buff[linePos]>='0'&&buff[linePos]<='9')) {
                    symStr[symStrIdx++]=buff[linePos++];
                }
                symStr[symStrIdx]=0;
                sym=floatnumber;
                oprnStackUpdate(floatnumber, symStr, symStrIdx, 0, NULL);
            } else {
                symStr[symStrIdx]=0;
                sym=intnumber;
                oprnStackUpdate(intnumber, symStr, symStrIdx, 0, NULL);
            }
        } else {
            symStr[symStrIdx]=0;
            sym=intnumber;
            oprnStackUpdate(intnumber, symStr, symStrIdx, 0, NULL);
        }
    }


    /* Assignment and equality */
    else if ((buff[linePos]=='=')) {
        if (buff[linePos+1]=='=') {
            linePos++;
            sym=equal;
            optrStackUpdate(equal, "==", 2, 0, NULL);
        } else {
            sym=assign;
            optrStackUpdate(assign, "=", 1, 0, NULL);

        }
        linePos++;
    } else if ((buff[linePos]=='/')) {
        if (buff[linePos+1]=='=') {
            linePos++;
            sym=slashassign;
            optrStackUpdate(slashassign, "/=", 2, 0, NULL);
        } else if (buff[linePos+1]=='/') {
            linePos++;
            sym=comment;
        } else {
            sym=slash;
            optrStackUpdate(slash, "/", 1, 0, NULL);
        }
        linePos++;
    } else if ((buff[linePos]=='*')) {
        if (buff[linePos+1]=='=') {
            linePos++;
            sym=timesassign;
            optrStackUpdate(timesassign, "*=", 2, 0, NULL);

        } else {
            sym=times;
            optrStackUpdate(times, "*", 1, 0, NULL);
        }
        linePos++;
    } else if ((buff[linePos]=='+')) {
        if (buff[linePos+1]=='=') {
            linePos++;
            sym=plusassign;
            optrStackUpdate(plusassign, "+=", 2, 0, NULL);

        } else {
            sym=plus;
            optrStackUpdate(plus, "+", 1, 0, NULL);
        }
        linePos++;
    } else if ((buff[linePos]=='-')) {
        if (buff[linePos+1]=='=') {
            linePos++;
            sym=minusassign;
            optrStackUpdate(minusassign, "-=", 2, 0, NULL);
        } else if (buff[linePos+1]=='>') {
            linePos++;
            sym=retsym;
            optrStackUpdate(retsym, "return ", 7, 0, NULL);
        } else {
            sym=minus;
            optrStackUpdate(minus, "-", 1, 0, NULL);
        }
        linePos++;
    } else if ((buff[linePos]=='>')) {
        if (buff[linePos+1]=='>') {
            /* Todo : >> */
        } else if (buff[linePos+1]=='=') {
            linePos++;
            sym=geq;
            optrStackUpdate(geq, ">=", 2, 0, NULL);
        } else {
            sym=gtr;
            optrStackUpdate(gtr, ">", 1, 0, NULL);
        }
        linePos++;
    } else if ((buff[linePos]=='<')) {
        if (buff[linePos+1]=='<') {
            /* Todo : << */
        }  else if (buff[linePos+1]=='=') {
            linePos++;
            sym=leq;
            optrStackUpdate(leq, "<=", 2, 0, NULL);
        } else if (buff[linePos+1]=='>') {
            linePos++;
            sym=compare;
            optrStackUpdate(compare, "<>", 2, 0, NULL);
        } else {
            sym=lss;
            optrStackUpdate(lss, "<", 1, 0, NULL);
        }
        linePos++;
    } else if ((buff[linePos]=='&') && (buff[linePos+1]=='&')) {
        sym=booland;
        optrStackUpdate(booland, "&&", 2, 0, NULL);
        linePos += 2;
    } else if ((buff[linePos]=='|') && (buff[linePos+1]=='|')) {
        sym=boolor;
        optrStackUpdate(boolor, "||", 2, 0, NULL);
        linePos += 2;
    }

    /* Other operators */
    else if ((buff[linePos]=='.')) {
        sym=endsym;
        optrStackUpdate(endsym, "}", 1, 0, NULL);
        evaluateAndReset();
        linePos++;
    } else if ((buff[linePos]==',')) {
        sym=comma;
        optrStackUpdate(comma, ",", 1, 0, NULL);
        linePos++;
    } else if ((buff[linePos]=='^')) {
        if (buff[linePos+1]=='^') {
            linePos++;
            sym=exponent;
            optrStackUpdate(exponent, "^^", 2, 0, NULL);
        } else {
            sym=bitwisexor;
            optrStackUpdate(bitwisexor, "^", 1, 0, NULL);
        }
        linePos++;
    } else if ((buff[linePos]=='(')) {
        sym=lparen;
        optrStackUpdate(lparen, "(", 1, 0, NULL);
        lParenList[lParenPtr]=oprnStackPtr;
        lParenPtr++;
        linePos++;
    } else if ((buff[linePos]==')')) {
        sym=rparen;
        optrStackUpdate(rparen, ")", 1, 0, NULL);
        expType=object;
        linePos++;
    } else if ((buff[linePos]=='#')) {
        sym=cinc;
        linePos++;
    } else if ((buff[linePos]==':')) {
        sym=colon;
        //to handle void functions
        if (optrStackPtr == 0)
            optrStackUpdate(type, "void", 4, 0, NULL);
        optrStackUpdate(colon, ":", 1, 0, NULL);
        linePos++;
    } else {
        errorMsg("Unrecognized symbol |%c|%d\n",buff[linePos],buff[linePos]);
        exit(0);
        linePos++;
    }
    //printf ("S:%s",symnames[sym]);
    if (symStrIdx>0) {
        //printf ("->%s\n",symStr);
    } else {
        //printf ("\n");
    }
}


void readCinc (void)
{
    while(buff[linePos]!='\n') {
        fputc(buff[linePos],stdout);
        fputc(buff[linePos],outfile);
        linePos++;
    }
    fputc('\n',outfile);
    sym=eol;
    getln();
}

void readFunDec (void)
{
    printf("=====------ readFunDec()\n");
    outfile=outHeaderFile;
    scopeLevel++;
    expectScopeIncrease=true;
    int i=0;
    int paramCount = 0;
    char funcName[BUFFLEN]="";
    char funcSymStr[BUFFLEN]="";
    char argList[BUFFLEN]="";
    char returnType[BUFFLEN];
    char lastArgType[BUFFLEN];

    strcpy(returnType,optrStack[0].operSymStr);
    fprintf(outfile,"%s ",returnType);
    optrStackPtr=0;
    getsym(); //Skip :
    errorMsg(ANSI_COLOR_MAGENTA "symstr %s\n" ANSI_COLOR_RESET,symStr);
    do {
        optrStackPtr=0;
        i++;
        if (i==1) {
            snprintf(funcName,BUFFLEN,"%s_%s",currentType,symStr);
            errorMsg(ANSI_COLOR_MAGENTA "%s %s_%s\n" ANSI_COLOR_RESET,returnType,currentType,symStr);
            strcpy(funcSymStr,symStr);
            snprintf(argList,BUFFLEN,"(%s %s",currentType,"self");
        } else {
            if (sym!=comma) {
                if (i%2==0) {
                    char temp[BUFFLEN];
                    snprintf(temp,BUFFLEN,"%s_%s",funcName,symStr);
                    strcpy(lastArgType,symStr);
                    strcpy(funcName,temp);
                }
                createObject(symStr,lastArgType);
                char temp[BUFFLEN];
                if (i%2==0) {
                    paramCount++;
                    snprintf(temp,BUFFLEN,"%s, %s",argList,symStr);
                } else {
                    snprintf(temp,BUFFLEN,"%s %s",argList,symStr);
                }
                strcpy(argList,temp);
            } else {
                //char temp[BUFFLEN];
                //snprintf(temp,BUFFLEN,"%s,",argList);
                //strcpy(argList,temp);
                i--;
            }
        }
        getsym();
    } while (sym!=eol);
    printf("Creating function %s\n",funcName);
    //creating function as "sum" and "UNKNOWNTYPE_sum_Integer_Integer"
    createFunction(funcSymStr,returnType,false,NULL,false, paramCount);
    createFunction(funcName,returnType,false,NULL,false, paramCount);
    fprintf(outfile,"%s%s)\n{\n",funcName,argList);
    printf("=====------ leaving readFunDec()\n");
}

void parse(void)
{
    do {
        printf("=====------ parse()\n");
        if (sym==rparen) {
            evaluate();
        } else if (sym==cinc) {
            readCinc();
        } else if (sym==comment) {
            fputs("//",outfile);
            readCinc();
        } else if ((sym==colon)&&(optrStackPtr==2)) {
            sym=fundec;
            readFunDec();
        }
        if (sym!=eof)
            getsym();
    } while (sym!=eof);
    int i;
    for (i=scopeLevel;i>0;i--) {
        fprintf(outfile,"}\n");
    }
}


int main(int argc,char **argv)
{
    int c,i;
    int bflg, aflg, errflg=0;
    char *ifile = NULL;
    char *ofile = NULL;
    extern char *optarg;
    extern int optind, optopt;

    while ((c = getopt(argc, argv, "o:")) != -1) {
        switch (c) {
        case 'o':
            ofile = optarg;
            break;
        case ':':       /* -f or -o without operand */
            fprintf(stderr,
                    "Option -%c requires an operand\n", optopt);
            errflg++;
            break;
        };
    }

    if (errflg) {
        fprintf(stderr, "usage: . . . ");
        exit(2);
    }


    for (i=0; optind < argc; optind++,i++) {
        if (i==0) {
            ifile=argv[optind];
        }
    }


    if (ifile==NULL) {
        errorMsg("No file to compile\n");
        file=fopen("helloworld.rit","r");
    } else {
        file=fopen(ifile,"r");
    }

    char oMainFileName[BUFFLEN];
    char oHeaderFileName[BUFFLEN];

    if (ofile==NULL) {
        strcpy(oMainFileName,"out.c");
        strcpy(oHeaderFileName,"out.h");
    } else {
        snprintf(oMainFileName,BUFFLEN,"%s.c",ofile);
        snprintf(oHeaderFileName,BUFFLEN,"%s.h",ofile);
    }
    outMainFile=fopen(oMainFileName,"w");
    outHeaderFile=fopen(oHeaderFileName,"w");
    outfile=outMainFile;


    linePos=0;
    lineNum=0;
    scopeLevel=0;
    args=0;
    optrStackPtr=0;
    oprnStackPtr=0;
    lParenPtr=0;


    funcListIdx=0;
    typeIdx=0;
    expType=method;

    expectScopeIncrease=false;
    /*TODO: This should be done in RL itself */
    /*Create some Types */
    char * baseType = "RitcheBaseType";
    strcpy(currentType,"UNKNOWNTYPE");
    createType("Number",baseType);
    createType("String",baseType);
    createType("Integer","Number");
    createType("Float","Number");
    createType("Boolean",baseType);
    createType("Char",baseType);

    /* print -> stdout.pring */
    createObject("stdout","Stream");
    createObject("UNKNOWNOBJECT","UNKNOWNTYPE");
    createFunction("print","Stream",false,"stdout",false, 1);
    createFunction("echo","Stream",false,"stdout",false, 1);

    /* Create Language object */
    createObject("ritchie","Language");
    createFunction("if","Language",true,NULL,false, 1);
    createFunction("UNKNOWNTYPE_if_Boolean","Language",true,NULL,false, 1);
    createFunction("elif","Language",true,NULL,false, 1);
    createFunction("UNKNOWNTYPE_elif_Boolean","Language",true,NULL,false, 1);
    createFunction("else","Language",true,NULL,false, 0);
    createFunction("UNKNOWNTYPE_else","Language",true,NULL,false, 0);
    createFunction("while","Language",true,NULL,false, 1);
    createFunction("UNKNOWNTYPE_while_Boolean","Language",true,NULL,false, 1);
    //createFunction("for","Integer",true,NULL,true);


    /*Setup some functions signatures */
    createFunction("String_plus_String","String",false,NULL,false, 2);
    createFunction("String_plus_Float","String",false,NULL,false, 2);
    createFunction("Float_plus_String","String",false,NULL,false, 2);
    createFunction("String_plus_Integer","String",false,NULL,false, 2);
    createFunction("Integer_plus_String","String",false,NULL,false, 2);
    createFunction("String_stringlit","String",false,NULL,false, 2);
    createFunction("String_assign_String","String",false,NULL,true, 2);

    createFunction("Identifier_assign_String","String",false,NULL,true, 2);
    createFunction("Identifier_assign_Float","Float",false,NULL,true, 2);
    createFunction("Identifier_assign_Integer","Integer",false,NULL,true, 2);

    createFunction("Identifier_for_Integer_Integer","Integer",true,NULL,true, 2);
    createFunction("Integer_for_Integer_Integer","Integer",true,NULL,true, 2);

    /* Some math func signatures */
    createFunction("Float_exponent_Float","Float",false,NULL,false, 2);
    createFunction("Float_exponent_Integer","Float",false,NULL,false, 2);
    createFunction("Integer_exponent_Integer","Integer",false,NULL,false, 2);
    createFunction("Integer_exponent_Float","Float",false,NULL,false, 2);



    /*Some Ternary Functions */
    createFunction("Integer_compare_Integer","Ternary",false,NULL,false, 2);
    createFunction("Ternary_pick_String_String_String","String",false,NULL,false, 4);

    errorMsg(ANSI_COLOR_MAGENTA "**********************************\n"
    "**********************************\n"
    "**********************************\n"
    "**********************************\n" ANSI_COLOR_RESET);
    fprintf(outfile,"#include \"rsl.h\"\n");
    fprintf(outfile,"#include \"%s\"\n",oHeaderFileName);
    fprintf(outfile,"int main(void) {\n");

    getln();
    getsym();
    parse();
    fprintf(outfile,"\treturn 0;\n}\n");


    //print functionlist into out.c
    /*
    while (funcListIdx-- > 0) {
        fprintf(outfile, "Function:\n");
        fprintf(outfile, "\tName: %s\n", funcList[funcListIdx].name);
        fprintf(outfile, "\tType: %s\n", funcList[funcListIdx].type);
        fprintf(outfile, "\tParamCount: %d\n", funcList[funcListIdx].numberOfParams);
        fprintf(outfile, "\tBlock/Assign/Default: %d %d \"%s\"\n", funcList[funcListIdx].codeBlocks, funcList[funcListIdx].assigns, funcList[funcListIdx].defaultObject);
    }
    */

    fclose(outHeaderFile);
    fclose(outMainFile);
    fclose(file);


    return 0;
}
