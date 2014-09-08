#include <stdio.h>
#include <string.h>
#include <stdarg.h>

typedef enum {false, true} bool;
int ritch_i_;
#define foreach(item,array) for (ritch_i_=0;(item=array[ritch_i_])!=NULL;ritch_i_++)

#define BUFFLEN 256
#define LABELMAX 8096
#define STACKDEP 1024

typedef struct Function_
{
    char name[BUFFLEN];
    char type[BUFFLEN];
} Function;

Function functions[LABELMAX];
int funcIdx;

typedef struct shortCutFunction_
{
    char name[BUFFLEN];
    char object[BUFFLEN];
} shortCutFunction;

shortCutFunction shortFuncs[LABELMAX];
int shortFuncIdx;

typedef struct Identifier_
{
    char name[BUFFLEN];
    char type[BUFFLEN];
} Identifier;

Identifier idents[LABELMAX];
int identIdx;

const char * types[LABELMAX]=
{
    "String",NULL
};

const char * symnames[]=
{
    "eol", "eof", "ident", "intnumber", "stringlit", "floatnumber", "number",
    "lparen", "rparen", "times", "slash", "plus", "minus", "assign", "equal",
    "neq", "lss", "leq", "gtr", "geq", "callsym", "beginsym", "semicolon",
    "endsym", "ifsym", "whilesym", "becomes", "thensym", "dosym", "constsym",
    "comma", "varsym", "procsym", "period", "oddsym", "plusassign", "function",
    "evaluation"
};

const char * getFunctionType(const char * func)
{
    int i;
    for (i=0; i<funcIdx; i++)
    {
        if (strcmp(functions[i].name,func)==0)
        {
            return functions[i].type;
        }
    }
    return NULL;
}

const char * getIdentifierType(const char * identifier)
{
    int i;
    for (i=0; i<identIdx; i++)
    {
        if (strcmp(idents[i].name,identifier)==0)
        {
            return idents[i].type;
        }
    }
    return NULL;
}

const char *  getFunctionObject(const char * funcname)
{
    int i;
    for (i=0; i<shortFuncIdx; i++)
    {
        if (strcmp(shortFuncs[i].name,funcname)==0)
        {
            return shortFuncs[i].object;
        }
    }
    return NULL;
}

typedef enum
{
    eol, eof, ident, intnumber, stringlit, floatnumber, number, lparen, rparen, times, slash, plus,
    minus, assign, equal, neq, lss, leq, gtr, geq, callsym, beginsym, semicolon, endsym,
    ifsym, whilesym, becomes, thensym, dosym, constsym, comma, varsym, procsym, period, oddsym, plusassign, function,
    evaluation
}
Symbol;


Symbol sym;
char symStr[BUFFLEN];
int symStrIdx;
FILE *file;
FILE *outfile;


Symbol optrStack[STACKDEP];
int optrStackPtr;

Symbol oprnStack[STACKDEP];
int oprnStackPtr;

typedef struct SymElem_
{
    char symStr[BUFFLEN];
} SymElem;

SymElem optrSymStack[STACKDEP];
int optrSymStackPtr;

SymElem oprnSymStack[STACKDEP];
int oprnSymStackPtr;

char buff[BUFFLEN];
int linePos;
int lineNum;

#define EVAL_BUFF_MAX_LEN 1024

int errorMsg(const char * format,...) {
    int ret;
    fprintf(stderr,"Line %d: Column:%d - ",lineNum,linePos);
    va_list arg;
    va_start(arg,format);
    ret = vfprintf(stderr, format, arg);
    va_end(arg);
    return ret;
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
    for (i=0; i<oprnStackPtr; i++)
    {
        //printf("%s\n",oprnSymStack[i].symStr);
    }

    holderSymbol=oprnStack[rnd];
    strcpy(holderSymStack,oprnSymStack[rnd].symStr);

    bool rTypeSet=false;
    char rtype[BUFFLEN];
    bool lTypeSet=false;
    char ltype[BUFFLEN];

    if (oprnStack[rnd]==stringlit)
    {
        strcpy(rtype,"String");
    }
    else if (oprnStack[rnd]==intnumber)
    {
        strcpy(rtype,"Integer");
    }
    else if (oprnStack[rnd]==floatnumber)
    {
        strcpy(rtype,"Float");
    }

    else if (oprnStack[rnd]==ident)
    {
        const char *idType=getIdentifierType(holderSymStack);
        if (idType!=NULL)
            strcpy(rtype,getIdentifierType(holderSymStack));
        else
            strcpy(rtype,"???");
    }

    char * semiColonStr="; ";
    for (tor=optrStackPtr-1; tor>=0; tor--)
    {
        //printf("OPTR %s\n",symnames[optrStack[rnd]]);

        //printf("tor/optrStackPtr %d/%d rnd/oprnStackPtr %d/%d %s\n",tor,optrStackPtr,rnd,oprnStackPtr,oprnSymStack[rnd].symStr);

        char fn[BUFFLEN];
        if (optrStack[tor]==function)
        {
            strcpy(fn,optrSymStack[tor].symStr);
        }
        else
        {
            strcpy(fn,symnames[optrStack[tor]]);
        }
        if (rnd>0)
        {
            if (optrStack[tor]==assign)
            {
                strcpy(ltype,rtype);
            }
            else
            {
                if (!lTypeSet)
                {

                    if (oprnStack[rnd-1]==stringlit)
                    {
                        strcpy(ltype,"String");
                    }
                    else if (oprnStack[rnd-1]==intnumber)
                    {
                        strcpy(ltype,"Integer");
                    }
                    else if (oprnStack[rnd-1]==floatnumber)
                    {
                        strcpy(ltype,"Float");
                    }
                    else if (oprnStack[rnd-1]==ident)
                    {

                        const char * ltype_=getIdentifierType(oprnSymStack[rnd-1].symStr);
                        if (ltype_==NULL)
                        {
                            errorMsg("Type of %s indeterminalbe",oprnSymStack[rnd-1].symStr);
                        }
                        else
                        {
                            strcpy(ltype,ltype_);
                        }

                    }
                    //lTypeSet=true;
                }
            }

            if (oprnStack[rnd-1]==ident&&optrStack[tor]==assign)
            {
                const char * idType=getIdentifierType(oprnSymStack[rnd-1].symStr);
                if (idType==NULL)
                {
                    strcpy(idents[identIdx].name,oprnSymStack[rnd-1].symStr);
                    strcpy(idents[identIdx].type,rtype);
                    //printf("New ident: %s %s;\n",idents[identIdx].type,idents[identIdx].name);
                    if (tor==0) {
                        if (!strcmp(idents[identIdx].type,"Integer"))
                        {
                            fprintf(outfile,"\tint %s = %s;\n",idents[identIdx].name,holderSymStack);
                        }
                        else  if (!strcmp(idents[identIdx].type,"Float"))
                        {
                            fprintf(outfile,"\tfloat %s = %s;\n",idents[identIdx].name,holderSymStack);
                        }
                        else
                        {
                            fprintf(outfile,"\t%s * %s = %s;\n",idents[identIdx].type,idents[identIdx].name,holderSymStack);
                        }
                        identIdx++;
                        /*TODO: This could be trouble*/
                        evalBuffLen=0;
                    } else {
                        if (!strcmp(idents[identIdx].type,"Integer"))
                        {
                            fprintf(outfile,"\tint %s;\n",idents[identIdx].name);
                        }
                        else  if (!strcmp(idents[identIdx].type,"Float"))
                        {
                            fprintf(outfile,"\tfloat %s;\n",idents[identIdx].name);
                        }
                        else
                        {
                            fprintf(outfile,"\t%s * %s;\n",idents[identIdx].type,idents[identIdx].name);
                        }
                        identIdx++;

                    }
                }
                else
                {
                    if (strcmp(idType,rtype)!=0)
                        errorMsg("You can't redefine %s. This is not PHP\n",oprnSymStack[rnd-1].symStr);
                }

            }

            char tempRtype[BUFFLEN];
            char funcName[BUFFLEN];

            errorMsg("TOR %s %d\n",symnames[optrStack[tor]],optrStack[tor]);
            if (optrStack[tor]==ifsym) {
                snprintf(funcName,BUFFLEN,"%s_%s_%s",ltype,fn,rtype);
                evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"if (%s) {",
                                             holderSymStack);
                semiColonStr="  ";
            }
            else if (optrStack[tor]==endsym) {
                semiColonStr=";}";
                continue;
            }
            else if (optrStack[tor]==assign)
            {
                snprintf(funcName,BUFFLEN,"%s_%s_%s",ltype,fn,rtype);
                if (tor!=0)
                    evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s = %s",
                                             oprnSymStack[rnd-1].symStr,
                                             holderSymStack);
            }
            else  if ((!strcmp(ltype,"Integer")||!strcmp(ltype,"Float"))&&(!strcmp(rtype,"Integer")||!strcmp(rtype,"Float")))
            {
                if (optrStack[tor]==assign)
                {
                    snprintf(funcName,BUFFLEN,"%s_%s_%s",ltype,fn,rtype);
                    if (tor!=0)
                    evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s = %s",
                                         oprnSymStack[rnd-1].symStr,
                                         holderSymStack);
                }
                else if (optrStack[tor]==plus)
                {
                    snprintf(funcName,BUFFLEN,"%s_%s_%s",ltype,fn,rtype);
                    evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s + %s",
                                         oprnSymStack[rnd-1].symStr,
                                         holderSymStack);
                }
                else if (optrStack[tor]==gtr)
                {
                    snprintf(funcName,BUFFLEN,"%s_%s_%s",ltype,fn,rtype);
                    evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s > %s",
                                         oprnSymStack[rnd-1].symStr,
                                         holderSymStack);
                }
                else
                {
                    snprintf(funcName,BUFFLEN,"%s_%s_%s",ltype,fn,rtype);
                    evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s(%s,%s)",
                                         funcName,
                                         oprnSymStack[rnd-1].symStr,
                                         holderSymStack);
                }
            }
            else
            {
                snprintf(funcName,BUFFLEN,"%s_%s_%s",ltype,fn,rtype);
                evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s(%s,%s)",
                                     funcName,
                                     oprnSymStack[rnd-1].symStr,
                                     holderSymStack);

            }

            if (strcmp(fn,"assign")) {
                const char * funcType=getFunctionType(funcName);
                if (funcType==NULL)
                {
                    errorMsg("Warning: Unknown method %s. Assuming void\n",funcName);
                    strcpy(rtype,"void");
                }
                else
                {
                    strcpy(rtype,funcType);
                }
            } else {
                //strcpy(rtype,"void");
            }
            rnd--;
        }
        else
        {
            /*Unary funcs*/
            char tempRtype[BUFFLEN];
            char funcName[BUFFLEN];
            if (optrStack[tor]==ifsym) {
                snprintf(funcName,BUFFLEN,"%s_%s",ltype,fn);
                evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"if (%s) {",
                                             holderSymStack);
                semiColonStr="  ";
            } else if (optrStack[tor]==endsym) {

            }  else {
                evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s_%s(%s)",rtype,fn,holderSymStack);
            }
            rnd--;
        }

        holderSymbol=evaluation;
        strncpy(holderSymStack,evalBuff,evalBuffLen+1);
        //printf("%d %s\n",evalBuffLen,evalBuff);
    }
    if (evalBuffLen>0)
        fprintf(outfile,"\t%s%s\n",evalBuff,semiColonStr);

}

void getln(void)
{
    lineNum++;

    if (!fgets(buff,BUFFLEN,file))
    {
        sym=eof;
    }
    evaluate();

    linePos=0;
    optrStackPtr=0;
    oprnStackPtr=0;
    optrSymStackPtr=0;
    oprnSymStackPtr=0;
}

void oprnStackUpdate()
{
    oprnStack[oprnStackPtr]=sym;
    oprnStackPtr++;

    if (sym!=stringlit)
        strncpy(oprnSymStack[oprnSymStackPtr].symStr,symStr,symStrIdx+1);
    else
        snprintf(oprnSymStack[oprnSymStackPtr].symStr,BUFFLEN,"String_from_stringlit(\"%s\")",symStr);
    oprnSymStackPtr++;
}

void getsym(void)
{
    //printf("We are at %c %d\n",buff[idx],idx);
    /* End of line */
    symStrIdx=0;

    while(buff[linePos]==' ')
    {
        linePos++;
    }

    if (buff[linePos]=='\n')
    {
        sym=eol;
        getln();
    }
    /* Identifier */
    else if ((buff[linePos]>='a'&&buff[linePos]<='z')||(buff[linePos]>='A'&&buff[linePos]<='Z'))
    {
        while ((buff[linePos]>='a'&&buff[linePos]<='z')||(buff[linePos]>='A'&&buff[linePos]<='Z')||(buff[linePos]>='0'&&buff[linePos]<='9'))
        {
            symStr[symStrIdx++]=buff[linePos++];
        }
        symStr[symStrIdx]=0;
        //Todo: Optimize, we know the string lenght
        if (!strcmp(symStr,"if")) {
            sym=ifsym;
            optrStack[optrStackPtr]=sym;
            optrStackPtr++;
        } else {
            const char * fnObj=getFunctionObject(symStr);
            if (fnObj!=NULL)
            {
                sym=function;
                strcpy(optrSymStack[optrStackPtr].symStr,symStr);
                optrStack[optrStackPtr]=sym;
                optrStackPtr++;
                optrSymStackPtr++;

                oprnStack[oprnStackPtr]=ident;
                oprnStackPtr++;
                strcpy(oprnSymStack[oprnSymStackPtr].symStr,fnObj);
                oprnSymStackPtr++;
            }
            else
            {
                sym=ident;
                oprnStackUpdate();
            }
        }
    }

    /* String Literal */
    else if ((buff[linePos]=='"'))
    {
        linePos++;
        do
        {
            symStr[symStrIdx++]=buff[linePos++];
        }
        while (buff[linePos]!='"');
        symStr[symStrIdx]=0;
        sym=stringlit;

        oprnStackUpdate();

        linePos++;
    }

    /* Number */
    else if ((buff[linePos]>='0'&&buff[linePos]<='9'))
    {
        while ((buff[linePos]>='0'&&buff[linePos]<='9'))
        {
            symStr[symStrIdx++]=buff[linePos++];
        }
        if (buff[linePos]=='.')
        {

            if ((buff[linePos+1]>='0'&&buff[linePos+1]<='9'))
            {
                symStr[symStrIdx++]=buff[linePos++];
                while ((buff[linePos]>='0'&&buff[linePos]<='9'))
                {
                    symStr[symStrIdx++]=buff[linePos++];
                }
                symStr[symStrIdx]=0;
                sym=floatnumber;
                oprnStackUpdate();
            }
            else
            {
                /*sym=equal;
                optrStack[optrStackPtr]=sym;
                optrStackPtr++;;*/

            }
        }
        else
        {
            symStr[symStrIdx]=0;
            sym=intnumber;
            oprnStackUpdate();
        }
    }


    /* Assignment and equal */
    else if ((buff[linePos]=='='))
    {
        if (buff[linePos+1]=='=')
        {
            linePos++;
            sym=equal;
            optrStack[optrStackPtr]=sym;
            optrStackPtr++;

        }
        else
        {
            sym=assign;

            optrStack[optrStackPtr]=sym;
            optrStackPtr++;

        }
        linePos++;
    }
    else if ((buff[linePos]=='+'))
    {
        if (buff[linePos+1]=='=')
        {
            linePos++;
            sym=plusassign;

            optrStack[optrStackPtr]=sym;
            optrStackPtr++;
        }
        else
        {
            sym=plus;
            optrStack[optrStackPtr]=sym;
            optrStackPtr++;
        }
        linePos++;
    }
    else if ((buff[linePos]=='>'))
    {
        if (buff[linePos+1]=='>')
        {
            /* Todo : << */
        }
        else
        {
            sym=gtr;
            optrStack[optrStackPtr]=sym;
            optrStackPtr++;
        }
        linePos++;
    }
    else if ((buff[linePos]=='.'))
    {
        if (buff[linePos+1]=='.')
        {
            /* Todo : .. */
        }
        else
        {
            printf ("->%s\n",symStr);
            sym=endsym;
            optrStack[optrStackPtr]=sym;
            optrStackPtr++;
        }
        linePos++;
    }
    //printf ("S:%s",symnames[sym]);
    if (symStrIdx>0)
    {
        //printf ("->%s\n",symStr);
    }
    else
    {
        //printf ("\n");
    }
}

void error(const char msg[])
{

}

void expression(void);

int accept(Symbol s)
{
    if (sym == s)
    {
        getsym();
        return 1;
    }
    return 0;
}

int expect(Symbol s)
{
    if (accept(s))
        return 1;
    errorMsg("expect: %s unexpected symbol %s\n",symnames[s],symnames[sym]);
    return 0;
}

void expression(void)
{
    if (accept(intnumber))
    {
        while (sym == plus )
        {
            getsym();
            expression();
        }
    }
    else if (accept(floatnumber))
    {
        while (sym == plus )
        {
            getsym();
            expression();
        }
    }
    else if (accept(stringlit))
    {
        while (sym == plus )
        {
            getsym();
            expression();
        }
    }
    else if (accept(ident))
    {
        if (accept(gtr))
        {
            expression();
        } else if (accept(assign)) {
            getsym();
            expression();
        } else {
            while (sym == plus )
            {
                getsym();
                expression();
            }
        }

    }
    else if (accept(function))
    {
        expression();
    }
    else
    {
        errorMsg("Expecting expression. Got %s \n",symnames[sym]);
    }
    /*
    if (sym == plus || sym == minus)
        getsym();
    term();
    while (sym == plus || sym == minus) {
        getsym();
        term();
    }
    */
}

void statement(void)
{
    if (accept(ident))
    {
        expect(assign);
        expression();
    }
    else if (accept(function))
    {
        expression();
    }
    else if (accept(ifsym))
    {
        expression();
        bool done=false;
        accept(eol);
        do
        {
            statement();
            if (sym==endsym) {
                getsym();
                done=true;
            }
        }
        while (accept(eol)&&!done);
    }
    else
    {
        errorMsg("Expect statement: got %s syntax error\n",symnames[sym]);
        getsym();
    }
}

void block(void)
{
    do
    {
        statement();
    }
    while (accept(eol));
    //printf ("End of block\n");
}


int main(int argc,char **argv)
{
    if (argc<2)
    {
        errorMsg("No file to compile\n");
        file=fopen("helloworld.rit","r");
    }
    else
    {
        file=fopen(argv[1],"r");
    }

    outfile=fopen("out.c","w");

    fprintf(outfile,"#include \"ritchie.h\"\n");
    fprintf(outfile,"int main(void) {\n");


    linePos=0;
    lineNum=0;
    optrStackPtr=0;
    oprnStackPtr=0;
    optrSymStackPtr=0;
    oprnSymStackPtr=0;

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

    strcpy(functions[funcIdx].name,"Integer_plus_Integer");
    strcpy(functions[funcIdx].type,"Integer");
    funcIdx++;

    strcpy(functions[funcIdx].name,"Integer_plus_Float");
    strcpy(functions[funcIdx].type,"Float");
    funcIdx++;

    strcpy(functions[funcIdx].name,"Float_plus_Integer");
    strcpy(functions[funcIdx].type,"Float");
    funcIdx++;

    strcpy(functions[funcIdx].name,"Float_plus_Float");
    strcpy(functions[funcIdx].type,"Float");
    funcIdx++;

    getln();
    getsym();
    block();
    if (sym!=eof)
        expect(eof);

    fprintf(outfile,"\treturn 0;\n}\n");
    close(outfile);
    close(file);

    return 0;
}
