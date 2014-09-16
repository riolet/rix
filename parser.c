#include <stdio.h>
#include <string.h>
#include <stdarg.h>

typedef enum {false, true} bool;
int ritch_i_;
#define foreach(item,array) for (ritch_i_=0;(item=array[ritch_i_])!=NULL;ritch_i_++)

#define BUFFLEN 256
#define LABELMAX 8096
#define STACKDEP 1024

typedef struct Function_ {
    char name[BUFFLEN];
    char type[BUFFLEN];
    bool codeBlocks;
    char defaultObject[BUFFLEN];
    bool assigns;
} Function;

Function funcList[LABELMAX];
int funcListIdx;

typedef struct Identifier_ {
    char name[BUFFLEN];
    char type[BUFFLEN];
    char ofType[BUFFLEN];
} Identifier;

Identifier idents[LABELMAX];
int identIdx;

const char * types[LABELMAX]= {
    "String",NULL
};

typedef enum {
    nss, eol, eof, ident, intnumber, stringlit, floatnumber, character, number, lparen, rparen, times, slash, plus,
    minus, assign, equal, neq, lss, leq, gtr, geq, callsym, beginsym, semicolon, endsym, forsym,
    ifsym, whilesym, becomes, thensym, dosym, constsym, comma, varsym, procsym, period, oddsym, plusassign,minusassign,timesassign,slashassign,
    function, evaluation, range, raisedto
}
Symbol;


const char * symnames[]= {
    "nss", "eol", "eof", "ident", "intnumber", "stringlit", "floatnumber", "character", "number",
    "lparen", "rparen", "times", "slash", "plus", "minus", "assign", "equal",
    "neq", "lss", "leq", "gtr", "geq", "callsym", "beginsym", "semicolon",
    "endsym", "forsym", "ifsym", "whilesym", "becomes", "thensym", "dosym", "constsym",
    "comma", "varsym", "procsym", "period", "oddsym", "plusassign", "minusassign","timesassign","slashassign","function",
    "evaluation","range", "raisedto"
};

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

const char * getIdentifierType(const char * identifier)
{
    int i;
    for (i=0; i<identIdx; i++) {
        if (strcmp(idents[i].name,identifier)==0) {
            return idents[i].type;
        }
    }
    return NULL;
}

const char *  getFunctionObject(const char * funcname)
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


typedef enum { object, method } ExpType;
ExpType expType;

Symbol sym;
char symStr[BUFFLEN];
int symStrIdx;
FILE *file;
FILE *outfile;

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
int args;
#define EVAL_BUFF_MAX_LEN 1024

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
    const char * idType=getIdentifierType(oprnStack[rnd-1].operSymStr);
    Symbol torOper=optrStack[tor].oper;
    int i;
    for (i=0; i<=scopeLevel; i++) {
        fprintf(outfile,"\t");
    }
    if (idType==NULL) {
        strcpy(idents[identIdx].name,oprnStack[rnd-1].operSymStr);
        if (!strcmp(rtype,"stringlit")) {
            strcpy(idents[identIdx].type,"String");
        } else {
            strcpy(idents[identIdx].type,rtype);
        }
        //printf("New ident: %s %s;\n",idents[identIdx].type,idents[identIdx].name);

        bool isInteger=!strcmp(rtype,"Integer");
        bool isFloat=!strcmp(rtype,"Float");
        bool isStringlit=!strcmp(rtype,"stringlit");
        bool isString=!strcmp(rtype,"String");

        if (tor==0&&torOper==assign&&(isInteger||isFloat||isStringlit||isString)) {
            if (isInteger) {
                fprintf(outfile,"int %s = %s;\n",idents[identIdx].name,holderSymStack);
            } else  if (isFloat) {
                fprintf(outfile,"float %s = %s;\n",idents[identIdx].name,holderSymStack);
            }  else  if (isStringlit) {
                fprintf(outfile,"String %s;%s.buff = %s;%s.length=sizeof(%s);\n",idents[identIdx].name,idents[identIdx].name,holderSymStack,idents[identIdx].name,holderSymStack);
            }  else  if (isString) {
                fprintf(outfile,"String %s = %s;\n",idents[identIdx].name,holderSymStack);
            }  else {
                errorMsg("You shouldn't be here. Really!");
            }
            identIdx++;
            /*TODO: This could be trouble*/
            return true;
        } else {
            if (!strcmp(rtype,"Integer")) {
                fprintf(outfile,"int %s;\n",idents[identIdx].name);
            } else  if (!strcmp(rtype,"Float")) {
                fprintf(outfile,"float %s;\n",idents[identIdx].name);
            } else  if (!strcmp(rtype,"stringlit")) {
                fprintf(outfile,"String %s;%s.buffer = %s;%s.length=sizeof(%s);\n",idents[identIdx].name,idents[identIdx].name,holderSymStack,idents[identIdx].name,holderSymStack);
            } else {
                fprintf(outfile,"%s * %s;\n",idents[identIdx].type,idents[identIdx].name);
            }
            identIdx++;
        }
    } else {
        if (strcmp(idType,rtype)!=0)
            errorMsg("You can't redefine %s. This is not PHP\n",oprnStack[rnd-1].operSymStr);
        else
            fprintf(outfile,"%s = %s;\n",oprnStack[rnd-1].operSymStr,holderSymStack);
        return true;
    }
    return false;
}

void evaluate(void)
{
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

    if (oprnStack[rnd].type[0]!=0) {
        strcpy(rtype,oprnStack[rnd].type);
    } else if (oprnStack[rnd].oper==stringlit) {
        strcpy(rtype,"stringlit");
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
    }

    char * semiColonStr="; ";


    /* Iterate through the operators right to left */
    char addParam[BUFFLEN];
    char addParamTypes[BUFFLEN];

    strcpy(addParam,"");
    strcpy(addParamTypes,"");

    for (tor=optrStackPtr-1; tor>=0; tor--) {
        int arg;

        bool boolean=false;
        bool arithmetic=false;
        bool floatArith=false;
        Symbol torOper = optrStack[tor].oper;
        char * torSym =  optrStack[tor].operSymStr;

        if (parenMode&&torOper==rparen) {
            printf("Right paren optrStackPtr %d\n",optrStackPtr);
            continue;
        }
        if (parenMode&&torOper==lparen) {
            printf("%s oprnStack[rnd].operSymStr: %s, HOLDER: %s\n",torSym,oprnStack[rnd].operSymStr,holderSymStack);
            lParenPtr--;
            int lParenIdx=lParenList[lParenPtr];
            strcpy(oprnStack[lParenIdx].operSymStr,holderSymStack);
            strcpy(oprnStack[lParenIdx].type,rtype);
            optrStackPtr=tor;
            oprnStackPtr=lParenIdx+1;
            printf("Symstack: %s Type: %s optrStackPtr %d oprnStackPtr %d\n ",holderSymStack,rtype,optrStackPtr,oprnStackPtr);
            return;
        } else {

            char * openingBracket="(";
            char * closingBracket=")";
            if (tor==0||optrStack[tor-1].oper==comma) {
                openingBracket="";
                closingBracket="";
            }

            if (torOper==nss) {
                continue;
            } else if (torOper==comma) {
                char temp[BUFFLEN];
                snprintf(temp,BUFFLEN,"%s,(%s)",addParam,holderSymStack);
                strcpy(addParam,temp);
                snprintf(temp,BUFFLEN,"%s_%s",addParamTypes,rtype);
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
            if (rnd>0) {

                printf ("oprnStack[rnd-1].type : %s\n",oprnStack[rnd-1].type);
                if (oprnStack[rnd-1].type[0]!=0) {
                    strcpy(ltype,oprnStack[rnd-1].type);
                } else if (oprnStack[rnd-1].oper==stringlit) {
                    strcpy(ltype,"stringlit");
                } else if (oprnStack[rnd-1].oper==intnumber) {
                    strcpy(ltype,"Integer");
                } else if (oprnStack[rnd-1].oper==floatnumber) {
                    strcpy(ltype,"Float");
                } else if (oprnStack[rnd-1].oper==ident) {
                    const char * ltype_=getIdentifierType(oprnStack[rnd-1].operSymStr);
                    if (ltype_==NULL) {
                        strcpy(ltype,"ident");
                    } else {
                        strcpy(ltype,ltype_);
                    }
                }


                char tempRtype[BUFFLEN];
                char funcName[BUFFLEN];

                errorMsg("TOR %s %d\n",symnames[torOper],torOper);
                if (torOper==endsym) {
                    semiColonStr=";}";
                    scopeLevel--;
                    continue;
                } else  if ((!strcmp(ltype,"ident")||!strcmp(ltype,"Integer")||!strcmp(ltype,"Float"))&&(!strcmp(rtype,"Integer")||!strcmp(rtype,"Float"))) {

                    if (torOper==range||torOper==raisedto) {
                        torOper=function;
                    }
                    if (torOper==gtr||torOper==equal||torOper==lss||torOper==leq||torOper==geq) {
                        boolean=true;
                    } else {
                        arithmetic=true;
                    }
                    /* ToDo: Too many strcmps */
                    if (!strcmp(ltype,"Float")||!strcmp(rtype,"Float")) {
                        floatArith=true;
                    }
                    if (torOper==function) {
                        snprintf(funcName,BUFFLEN,"%s_%s_%s%s",ltype,fn,rtype,addParamTypes);
                        evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s%s(%s,%s%s)%s",
                                             openingBracket,
                                             funcName,
                                             oprnStack[rnd-1].operSymStr,
                                             holderSymStack,
                                             addParam,
                                             closingBracket);
                    } else {
                        snprintf(funcName,BUFFLEN,"%s_%s_%s%s",ltype,fn,rtype,addParamTypes);
                        evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s%s %s %s%s%s",
                                             openingBracket,
                                             oprnStack[rnd-1].operSymStr,
                                             torSym,
                                             holderSymStack,
                                             addParam,
                                             closingBracket);
                    }
                } else {
                    snprintf(funcName,BUFFLEN,"%s_%s_%s%s",ltype,fn,rtype,addParamTypes);
                    evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s%s(%s,%s%s)%s",
                                         openingBracket,
                                         funcName,
                                         oprnStack[rnd-1].operSymStr,
                                         holderSymStack,
                                         addParam,
                                         closingBracket);

                }

                bool funcAssigns = getFunctionAssigns(funcName);
                if ((arithmetic&&torOper==assign)||funcAssigns) {
                    printf("Assigning %s %s\n",holderSymStack,oprnStack[rnd-1].operSymStr);
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
                        errorMsg("Warning: Unknown method %s. Assuming void\n",funcName);
                        strcpy(rtype,"void");

                    } else {
                        strcpy(rtype,funcType);
                    }
                }
                printf ("Code blocks %s %d\n",funcName,getFunctionCodeBlocks(funcName));
                if (getFunctionCodeBlocks(funcName)) {
                    semiColonStr="{";
                    scopeLevel++;
                }

                rnd--;
            } else {
                /*Unary funcs*/
                char tempRtype[BUFFLEN];
                char funcName[BUFFLEN];
                if (torOper==function) {
                    snprintf(funcName,BUFFLEN,"%s_%s",rtype,fn);
                    evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s(%s,%s)",
                                         funcName,
                                         holderSymStack);
                    if (getFunctionCodeBlocks(funcName)) {
                        semiColonStr="{";
                        scopeLevel++;
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
    if (evalBuffLen>0&&!singleLineAssign) {
        printf ("Scope level %d %s\n",scopeLevel,evalBuff);
        int
        i=0;
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
}

void evaluateAndReset(void)
{
    evaluate();
    expType=method;
    optrStackPtr=0;
    oprnStackPtr=0;
    lParenPtr=0;
}

void getln(void)
{
    lineNum++;
    if (!fgets(buff,BUFFLEN,file)) {
        sym=eof;
    }
    linePos=0;
    evaluateAndReset();
}



void oprnStackUpdate()
{
    oprnStack[oprnStackPtr].oper=sym;
    oprnStack[oprnStackPtr].type[0]=0;
    if (sym!=stringlit)
        strncpy(oprnStack[oprnStackPtr].operSymStr,symStr,symStrIdx+1);
    else
        snprintf(oprnStack[oprnStackPtr].operSymStr,BUFFLEN,"\"%s\"",symStr);
    oprnStackPtr++;
    expType=object;
}

void optrStackUpdate()
{
    optrStack[optrStackPtr].oper=sym;
    optrStackPtr++;
    args=0;
    expType=method;
}

void getsym(void)
{
    //printf("We are at %c %d\n",buff[idx],idx);
    /* End of line */
    symStrIdx=0;

    while(buff[linePos]==' '||buff[linePos]=='\t') {
        linePos++;
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
    /* Identifier */
    else if ((buff[linePos]>='a'&&buff[linePos]<='z')||(buff[linePos]>='A'&&buff[linePos]<='Z')) {
        while ((buff[linePos]>='a'&&buff[linePos]<='z')||(buff[linePos]>='A'&&buff[linePos]<='Z')||(buff[linePos]>='0'&&buff[linePos]<='9')) {
            symStr[symStrIdx++]=buff[linePos++];
        }
        symStr[symStrIdx]=0;
        //Todo: Optimize, we know the string lenght
        printf ("%s %d\n",symStr,expType);
        if (expType==object) {
            sym=function;
            strcpy(optrStack[optrStackPtr].operSymStr,symStr);
            optrStackUpdate();
        } else {
            const char * fnObj=getFunctionObject(symStr);
            if (fnObj!=NULL) {


                sym=function;
                strcpy(optrStack[optrStackPtr].operSymStr,symStr);
                optrStackUpdate();

                if (fnObj[0]!=0) {
                    oprnStack[oprnStackPtr].oper=ident;
                    strcpy(oprnStack[oprnStackPtr].operSymStr,fnObj);
                    oprnStackPtr++;
                }

            } else {
                sym=ident;
                oprnStackUpdate();
            }
        }
    }

    /* String Literal */
    else if ((buff[linePos]=='"')) {
        linePos++;
        do {
            symStr[symStrIdx++]=buff[linePos++];
        } while (buff[linePos]!='"');
        symStr[symStrIdx]=0;
        sym=stringlit;

        oprnStackUpdate();

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
                oprnStackUpdate();
            } else {
                symStr[symStrIdx]=0;
                sym=intnumber;
                oprnStackUpdate();
            }
        } else {
            symStr[symStrIdx]=0;
            sym=intnumber;
            oprnStackUpdate();
        }
    }


    /* Assignment and equal */
    else if ((buff[linePos]=='=')) {
        if (buff[linePos+1]=='=') {
            linePos++;
            sym=equal;
            strcpy(optrStack[optrStackPtr].operSymStr,"==");
            optrStackUpdate();
        } else {
            sym=assign;
            strcpy(optrStack[optrStackPtr].operSymStr,"=");
            optrStackUpdate();

        }
        linePos++;
    } else if ((buff[linePos]=='/')) {
        if (buff[linePos+1]=='=') {
            linePos++;
            sym=slashassign;
            strcpy(optrStack[optrStackPtr].operSymStr,"/=");
            optrStackUpdate();

        } else {
            sym=slash;
            strcpy(optrStack[optrStackPtr].operSymStr,"/");
            optrStackUpdate();
        }
        linePos++;
    } else if ((buff[linePos]=='*')) {
        if (buff[linePos+1]=='=') {
            linePos++;
            sym=timesassign;
            strcpy(optrStack[optrStackPtr].operSymStr,"*=");
            optrStackUpdate();

        } else {
            sym=times;
            strcpy(optrStack[optrStackPtr].operSymStr,"*");
            optrStackUpdate();
        }
        linePos++;
    } else if ((buff[linePos]=='+')) {
        if (buff[linePos+1]=='=') {
            linePos++;
            sym=plusassign;
            strcpy(optrStack[optrStackPtr].operSymStr,"+=");
            optrStackUpdate();

        } else {
            sym=plus;
            strcpy(optrStack[optrStackPtr].operSymStr,"+");
            optrStackUpdate();
        }
        linePos++;
    } else if ((buff[linePos]=='>')) {
        if (buff[linePos+1]=='>') {
            /* Todo : << */
        } else if (buff[linePos+1]=='=') {
            linePos++;
            sym=geq;
            strcpy(optrStack[optrStackPtr].operSymStr,">=");
            optrStackUpdate();
        } else {
            sym=gtr;
            strcpy(optrStack[optrStackPtr].operSymStr,">");
            optrStackUpdate();
        }
        linePos++;
    } else if ((buff[linePos]=='<')) {
        if (buff[linePos+1]=='<') {
            /* Todo : << */
        } else if (buff[linePos+1]=='=') {
            linePos++;
            sym=leq;
            strcpy(optrStack[optrStackPtr].operSymStr,"<=");
            optrStackUpdate();
        } else {
            sym=lss;
            strcpy(optrStack[optrStackPtr].operSymStr,"<");
            optrStackUpdate();
        }
        linePos++;
    } else if ((buff[linePos]=='.')) {
        if (buff[linePos+1]=='.') {
            linePos++;
            sym=range;
            strcpy(optrStack[optrStackPtr].operSymStr,"..");
            optrStackUpdate();
        } else {
            sym=endsym;
            strcpy(optrStack[optrStackPtr].operSymStr,"}");
            optrStackUpdate();
        }
        linePos++;
    } else if ((buff[linePos]==',')) {
        sym=comma;
        strcpy(optrStack[optrStackPtr].operSymStr,",");
        optrStackUpdate();
        linePos++;
    } else if ((buff[linePos]=='^')) {
        sym=raisedto;
        strcpy(optrStack[optrStackPtr].operSymStr,"^");
        optrStackUpdate();
        linePos++;
    } else if ((buff[linePos]=='(')) {
        sym=lparen;
        strcpy(optrStack[optrStackPtr].operSymStr,"(");
        optrStackUpdate();
        lParenList[lParenPtr]=oprnStackPtr;
        lParenPtr++;
        linePos++;
    } else if ((buff[linePos]==')')) {
        sym=rparen;
        strcpy(optrStack[optrStackPtr].operSymStr,")");
        optrStackUpdate();
        linePos++;
    } else {
        errorMsg("Urecognized symbol |%c|%d\n",buff[linePos],buff[linePos]);
        linePos++;
        exit(0);
    }
    //printf ("S:%s",symnames[sym]);
    if (symStrIdx>0) {
        //printf ("->%s\n",symStr);
    } else {
        //printf ("\n");
    }
}

void expression(void);

void parse(void)
{
    do {
        getsym();
        if (sym==rparen) {
            evaluate();
        }
    } while (sym!=eof);
}

void createObject(const char * name, const char * type)
{
    strcpy(idents[identIdx].name,name);
    strcpy(idents[identIdx].type,type);
    identIdx++;
}

void createFunction(const char * name, const char * type, bool codeBlock, char * defaultObject, bool assigns)
{
    strcpy(funcList[funcListIdx].name,name);
    strcpy(funcList[funcListIdx].type,type);
    if (defaultObject!=NULL)
        strcpy(funcList[funcListIdx].defaultObject,defaultObject);
    else
        funcList[funcListIdx].defaultObject[0]=0;
    funcList[funcListIdx].codeBlocks=codeBlock;
    funcList[funcListIdx].assigns=assigns;
    funcListIdx++;
}

int main(int argc,char **argv)
{
    if (argc<2) {
        errorMsg("No file to compile\n");
        file=fopen("helloworld.rit","r");
    } else {
        file=fopen(argv[1],"r");
    }

    outfile=fopen("out.c","w");

    fprintf(outfile,"#include \"ritchie.h\"\n");
    fprintf(outfile,"int main(void) {\n");


    linePos=0;
    lineNum=0;
    scopeLevel=0;
    args=0;
    optrStackPtr=0;
    oprnStackPtr=0;
    lParenPtr=0;

    identIdx=0;
    funcListIdx=0;

    expType=method;
    /*TODO: This should be done in RL itself *
    /* print -> stdout.pring */
    createObject("stdout","Stream");
    createFunction("print","Stream",false,"stdout",false);
    createFunction("echo","Stream",false,"stdout",false);

    /* Create Language object */
    createObject("ritchie","Language");
    createFunction("if","Language",true,NULL,false);
    createFunction("Boolean_if","Language",true,NULL,false);
    createFunction("while","Language",true,NULL,false);
    createFunction("Boolean_while","Language",true,NULL,false);
    //createFunction("for","Integer",true,NULL,true);


    /*Setup some functions signatures */
    createFunction("String_plus_String","String",false,NULL,false);
    createFunction("stringlit_plus_String","String",false,NULL,false);
    createFunction("String_plus_stringlit","String",false,NULL,false);
    createFunction("ident_assign_stringlit","String",false,NULL,true);

    createFunction("ident_assign_String","String",false,NULL,true);
    createFunction("ident_assign_Float","Float",false,NULL,true);
    createFunction("ident_assign_Integer","Integer",false,NULL,true);

    createFunction("ident_for_Integer_Integer","Integer",true,NULL,true);

    /* Some math func signatures */
    createFunction("Float_raisedto_Integer","Float",false,NULL,false);
    createFunction("Integer_raisedto_Integer","Float",false,NULL,false);

    int i;
    for (i=0; i<STACKDEP; i++) {
        optrStack[optrStackPtr].args=0;
    }
    getln();
    getsym();
    parse();
    fprintf(outfile,"\treturn 0;\n}\n");
    close(outfile);
    close(file);

    return 0;
}
