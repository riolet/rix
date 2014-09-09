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
} Function;

Function functions[LABELMAX];
int funcIdx;

typedef struct shortCutFunction_ {
    char name[BUFFLEN];
    char object[BUFFLEN];
} shortCutFunction;

shortCutFunction shortFuncs[LABELMAX];
int shortFuncIdx;

typedef struct Identifier_ {
    char name[BUFFLEN];
    char type[BUFFLEN];
} Identifier;

Identifier idents[LABELMAX];
int identIdx;

const char * types[LABELMAX]= {
    "String",NULL
};

const char * symnames[]= {
    "eol", "eof", "ident", "intnumber", "stringlit", "floatnumber", "number",
    "lparen", "rparen", "times", "slash", "plus", "minus", "assign", "equal",
    "neq", "lss", "leq", "gtr", "geq", "callsym", "beginsym", "semicolon",
    "endsym", "forsym", "ifsym", "whilesym", "becomes", "thensym", "dosym", "constsym",
    "comma", "varsym", "procsym", "period", "oddsym", "plusassign", "minusassign","timesassign","slashassign","function",
    "evaluation"
};

const char * getFunctionType(const char * func)
{
    int i;
    for (i=0; i<funcIdx; i++) {
        if (strcmp(functions[i].name,func)==0) {
            return functions[i].type;
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
    for (i=0; i<shortFuncIdx; i++) {
        if (strcmp(shortFuncs[i].name,funcname)==0) {
            return shortFuncs[i].object;
        }
    }
    return NULL;
}

typedef enum {
    eol, eof, ident, intnumber, stringlit, floatnumber, number, lparen, rparen, times, slash, plus,
    minus, assign, equal, neq, lss, leq, gtr, geq, callsym, beginsym, semicolon, endsym, forsym,
    ifsym, whilesym, becomes, thensym, dosym, constsym, comma, varsym, procsym, period, oddsym, plusassign,minusassign,timesassign,slashassign,
    function, evaluation
}
Symbol;


Symbol sym;
char symStr[BUFFLEN];
int symStrIdx;
FILE *file;
FILE *outfile;

typedef struct SymElem_ {

} SymElem;


typedef struct OperStruct_ {
    Symbol oper;
    char operSymStr[BUFFLEN];
    next * OperStruct_;
} OperStruct;


OperStruct optrStack[STACKDEP];
int optrStackPtr;

OperStruct oprnStack[STACKDEP];
int oprnStackPtr;

char buff[BUFFLEN];
int linePos;
int lineNum;
int scopeLevel;
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
                if (idType==NULL) {
                    strcpy(idents[identIdx].name,oprnStack[rnd-1].operSymStr);
                    strcpy(idents[identIdx].type,rtype);
                    //printf("New ident: %s %s;\n",idents[identIdx].type,idents[identIdx].name);
                    if (tor==0) {
                        if (!strcmp(idents[identIdx].type,"Integer")) {
                            fprintf(outfile,"\tint %s = %s;\n",idents[identIdx].name,holderSymStack);
                        } else  if (!strcmp(idents[identIdx].type,"Float")) {
                            fprintf(outfile,"\tfloat %s = %s;\n",idents[identIdx].name,holderSymStack);
                        } else {
                            fprintf(outfile,"\t%s * %s = %s;\n",idents[identIdx].type,idents[identIdx].name,holderSymStack);
                        }
                        identIdx++;
                        /*TODO: This could be trouble*/
                        return true;
                    } else {
                        if (!strcmp(idents[identIdx].type,"Integer")) {
                            fprintf(outfile,"\tint %s;\n",idents[identIdx].name);
                        } else  if (!strcmp(idents[identIdx].type,"Float")) {
                            fprintf(outfile,"\tfloat %s;\n",idents[identIdx].name);
                        } else {
                            fprintf(outfile,"\t%s * %s;\n",idents[identIdx].type,idents[identIdx].name);
                        }
                        identIdx++;

                    }
                } else {
                    if (strcmp(idType,rtype)!=0)
                        errorMsg("You can't redefine %s. This is not PHP\n",oprnStack[rnd-1].operSymStr);
                }
                return false;
}

void evaluate(void)
{
    int tor;
    int rnd=oprnStackPtr-1;

    char evalBuff[EVAL_BUFF_MAX_LEN];
    Symbol holderSymbol;
    char holderSymStack[BUFFLEN];
    char holderType[BUFFLEN];
    int evalBuffLen=0;

    int i;
    for (i=0; i<oprnStackPtr; i++) {
        //printf("%s\n",oprnSymStack[i].symStr);
    }

    holderSymbol=oprnStack[rnd].oper;
    strcpy(holderSymStack,oprnStack[rnd].operSymStr);

    bool rTypeSet=false;
    char rtype[BUFFLEN];
    bool lTypeSet=false;
    char ltype[BUFFLEN];

    bool singleLineAssign=false;

    if (oprnStack[rnd].oper==stringlit) {
        strcpy(rtype,"String");
    } else if (oprnStack[rnd].oper==intnumber) {
        strcpy(rtype,"Integer");
    } else if (oprnStack[rnd].oper==floatnumber) {
        strcpy(rtype,"Float");
    }

    else if (oprnStack[rnd].oper==ident) {
        const char *idType=getIdentifierType(holderSymStack);
        if (idType!=NULL)
            strcpy(rtype,getIdentifierType(holderSymStack));
        else
            strcpy(rtype,"???");
    }

    char * semiColonStr="; ";
    bool arithmetic=false;
    bool floatArith=false;
    /* Iterate through the operators right to left */
    for (tor=optrStackPtr-1; tor>=0; tor--) {
        Symbol torOper = optrStack[tor].oper;
        char * torSym =  optrStack[tor].operSymStr;
        printf ("%s %s\n",symnames[torOper],torSym);
        char fn[BUFFLEN];
        if (optrStack[tor].oper==function) {
            strcpy(fn,torSym);
        } else {
            strcpy(fn,symnames[torOper]);
        }
        if (rnd>0) {
            if (torOper==assign) {
                strcpy(ltype,rtype);
            } else {
                if (!lTypeSet) {
                    if (oprnStack[rnd-1].oper==stringlit) {
                        strcpy(ltype,"String");
                    } else if (oprnStack[rnd-1].oper==intnumber) {
                        strcpy(ltype,"Integer");
                    } else if (oprnStack[rnd-1].oper==floatnumber) {
                        strcpy(ltype,"Float");
                    } else if (oprnStack[rnd-1].oper==ident) {
                        const char * ltype_=getIdentifierType(oprnStack[rnd-1].operSymStr);
                        if (ltype_==NULL) {
                            errorMsg("Type of %s indeterminalbe",oprnStack[rnd-1].operSymStr);
                        } else {
                            strcpy(ltype,ltype_);
                        }
                    }
                    //lTypeSet=true;
                }
            }

            if (oprnStack[rnd-1].oper==ident&&torOper==assign) {
                singleLineAssign=doAssignDeclare(tor, rnd, holderSymStack, ltype, rtype);
            }

            char tempRtype[BUFFLEN];
            char funcName[BUFFLEN];

            //errorMsg("TOR %s %d\n",symnames[optrStack[tor]],optrStack[tor]);
            if (torOper==ifsym) {
                snprintf(funcName,BUFFLEN,"%s_%s_%s",ltype,fn,rtype);
                evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"if (%s) {",
                                     holderSymStack);
                semiColonStr="  ";
                scopeLevel++;
            } else if (torOper==endsym) {
                semiColonStr=";}";
                scopeLevel--;
                continue;
            } else if (torOper==assign) {
                snprintf(funcName,BUFFLEN,"%s_%s_%s",ltype,fn,rtype);
                if (tor>0)
                    evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s = %s",
                                         oprnStack[rnd-1].operSymStr,
                                         holderSymStack);
            } else  if ((!strcmp(ltype,"Integer")||!strcmp(ltype,"Float"))&&(!strcmp(rtype,"Integer")||!strcmp(rtype,"Float"))) {
                arithmetic=true;
                /* ToDo: Too many strcmps */
                if (!strcmp(ltype,"Float")||!strcmp(rtype,"Float")) {
                    floatArith=true;
                }
                if (torOper==function) {
                    snprintf(funcName,BUFFLEN,"%s_%s_%s",ltype,fn,rtype);
                    evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s(%s,%s)",
                                         funcName,
                                         oprnStack[rnd-1].operSymStr,
                                         holderSymStack);
                } else {
                    snprintf(funcName,BUFFLEN,"%s_%s_%s",ltype,fn,rtype);
                    evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s %s %s",
                                         oprnStack[rnd-1].operSymStr,
                                         torSym,
                                         holderSymStack);
                }
            } else {
                snprintf(funcName,BUFFLEN,"%s_%s_%s",ltype,fn,rtype);
                evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s(%s,%s)",
                                     funcName,
                                     oprnStack[rnd-1].operSymStr,
                                     holderSymStack);

            }

            if (strcmp(fn,"assign")) {
                if (arithmetic) {
                    if (floatArith) {
                        strcpy(rtype,"Float");
                    } else {
                        strcpy(rtype,"Integer");
                    }
                } else {
                    const char * funcType=getFunctionType(funcName);
                    if (funcType==NULL) {
                        errorMsg("Warning: Unknown method %s. Assuming void\n",funcName);
                        strcpy(rtype,"void");
                    } else {
                        strcpy(rtype,funcType);
                    }
                }
            } else {
                //strcpy(rtype,"void");
            }
            rnd--;
        } else {
            /*Unary funcs*/
            char tempRtype[BUFFLEN];
            char funcName[BUFFLEN];
            if (torOper==ifsym) {
                snprintf(funcName,BUFFLEN,"%s_%s",ltype,fn);
                evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"if (%s) {",
                                     holderSymStack);
                semiColonStr="  ";
                scopeLevel++;
            } else if (torOper==endsym) {
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
    }
    if (evalBuffLen>0&&!singleLineAssign) {
        i=0;
        if (!strcmp(semiColonStr,"  ")) {
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
        for (i=0; i<=(scopeLevel+1); i++) {
            fprintf(outfile,"\t");
        }
        fprintf(outfile,"}\n");
    }
}

void evaluateAndReset(void)
{
    evaluate();
    optrStackPtr=0;
    oprnStackPtr=0;
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
    if (sym!=stringlit)
        strncpy(oprnStack[oprnStackPtr].operSymStr,symStr,symStrIdx+1);
    else
        snprintf(oprnStack[oprnStackPtr].operSymStr,BUFFLEN,"String_from_stringlit(\"%s\")",symStr);
    oprnStackPtr++;
}

void getsym(void)
{
    //printf("We are at %c %d\n",buff[idx],idx);
    /* End of line */
    symStrIdx=0;

    while(buff[linePos]==' ') {
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
        if (!strcmp(symStr,"if")) {
            sym=ifsym;
            optrStack[optrStackPtr].oper=sym;
            optrStackPtr++;
        } else {
            const char * fnObj=getFunctionObject(symStr);
            if (fnObj!=NULL) {
                sym=function;
                optrStack[optrStackPtr].oper=sym;
                strcpy(optrStack[optrStackPtr].operSymStr,symStr);
                optrStackPtr++;

                oprnStack[oprnStackPtr].oper=ident;
                strcpy(oprnStack[oprnStackPtr].operSymStr,fnObj);
                oprnStackPtr++;

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
                /*sym=equal;
                optrStack[optrStackPtr]=sym;
                optrStackPtr++;;*/

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
            optrStack[optrStackPtr].oper=sym;
            strcpy(optrStack[optrStackPtr].operSymStr,"==");
            optrStackPtr++;

        } else {
            sym=assign;
            optrStack[optrStackPtr].oper=sym;
            strcpy(optrStack[optrStackPtr].operSymStr,"=");
            optrStackPtr++;

        }
        linePos++;
    } else if ((buff[linePos]=='/')) {
        if (buff[linePos+1]=='=') {
            linePos++;
            sym=slashassign;
            optrStack[optrStackPtr].oper=sym;
            strcpy(optrStack[optrStackPtr].operSymStr,"/=");
            optrStackPtr++;

        } else {
            sym=slash;
            optrStack[optrStackPtr].oper=sym;
            strcpy(optrStack[optrStackPtr].operSymStr,"/");
            optrStackPtr++;
        }
        linePos++;
    } else if ((buff[linePos]=='*')) {
        if (buff[linePos+1]=='=') {
            linePos++;
            sym=times;
            optrStack[optrStackPtr].oper=sym;
            strcpy(optrStack[optrStackPtr].operSymStr,"*=");
            optrStackPtr++;

        } else {
            sym=timesassign;
            optrStack[optrStackPtr].oper=sym;
            strcpy(optrStack[optrStackPtr].operSymStr,"*");
            optrStackPtr++;
        }
        linePos++;
    } else if ((buff[linePos]=='+')) {
        if (buff[linePos+1]=='=') {
            linePos++;
            sym=plusassign;
            optrStack[optrStackPtr].oper=sym;
            strcpy(optrStack[optrStackPtr].operSymStr,"+=");
            optrStackPtr++;

        } else {
            sym=plus;
            optrStack[optrStackPtr].oper=sym;
            strcpy(optrStack[optrStackPtr].operSymStr,"+");
            optrStackPtr++;
        }
        linePos++;
    } else if ((buff[linePos]=='>')) {
        if (buff[linePos+1]=='>') {
            /* Todo : << */
        } else if (buff[linePos+1]=='=') {
            linePos++;
            sym=geq;
            optrStack[optrStackPtr].oper=sym;
            strcpy(optrStack[optrStackPtr].operSymStr,">=");
            optrStackPtr++;
        } else {
            sym=gtr;
            optrStack[optrStackPtr].oper=sym;
            strcpy(optrStack[optrStackPtr].operSymStr,">");
            optrStackPtr++;
        }
        linePos++;
    } else if ((buff[linePos]=='<')) {
        if (buff[linePos+1]=='<') {
            /* Todo : << */
        } else if (buff[linePos+1]=='=') {
            linePos++;
            sym=leq;
            optrStack[optrStackPtr].oper=sym;
            strcpy(optrStack[optrStackPtr].operSymStr,"<=");
            optrStackPtr++;
        } else {
            sym=lss;
            optrStack[optrStackPtr].oper=sym;
            strcpy(optrStack[optrStackPtr].operSymStr,"<");
            optrStackPtr++;
        }
        linePos++;
    } else if ((buff[linePos]=='.')) {
        if (buff[linePos+1]=='.') {
            /* Todo : .. */
        } else {
            sym=endsym;
            optrStack[optrStackPtr].oper=sym;
            strcpy(optrStack[optrStackPtr].operSymStr,"}");
            optrStackPtr++;
        }
        linePos++;
    }
    //printf ("S:%s",symnames[sym]);
    if (symStrIdx>0) {
        //printf ("->%s\n",symStr);
    } else {
        //printf ("\n");
    }
}

void error(const char msg[])
{

}

void expression(void);

void parse(void)
{
    do {
        getsym();
    } while (sym!=eof);
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
    optrStackPtr=0;
    oprnStackPtr=0;

    identIdx=0;
    shortFuncIdx=0;
    funcIdx=0;

    /*TODO: This should be done in RL itself *
    /*Create stdout */
    strcpy(idents[identIdx].name,"stdout");
    strcpy(idents[identIdx].type,"Stream");
    identIdx++;

    /*Setup some function shortcuts */
    strcpy(shortFuncs[shortFuncIdx].name,"print");
    strcpy(shortFuncs[shortFuncIdx].object,"stdout");
    shortFuncIdx++;

    /*Setup some functions signatures */
    strcpy(functions[funcIdx].name,"String_plus_String");
    strcpy(functions[funcIdx].type,"String");
    funcIdx++;

    int i;
    for (i=0;i<STACKDEP;i++) {
        oprnStack[i].next=NULL;
    }
    getln();
    getsym();
    parse();
    fprintf(outfile,"\treturn 0;\n}\n");
    close(outfile);
    close(file);

    return 0;
}
