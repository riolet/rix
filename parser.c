#include <stdio.h>
#include <string.h>

typedef enum {false, true} bool;
int ritch_i_;
#define foreach(item,array) for (ritch_i_=0;(item=array[ritch_i_])!=NULL;ritch_i_++)

#define BUFFLEN 256
#define LABELMAX 8096
#define STACKDEP 1024

typedef struct StaticFunction_
{
    char name[BUFFLEN];
    char object[BUFFLEN];
} StaticFunction;

StaticFunction statFuncs[LABELMAX];
int statFuncIdx;

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

const char * getIdentifierType(const char * identifier)
{
    Identifier id;
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
    for (i=0; i<statFuncIdx; i++)
    {
        if (strcmp(statFuncs[i].name,funcname)==0)
        {
            return statFuncs[i].object;
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

#define EVAL_BUFF_MAX_LEN 1024
void evaluate(void)
{
    int tor;
    int rnd=oprnStackPtr-1;

    char evalBuff[EVAL_BUFF_MAX_LEN];
    char holderName[EVAL_BUFF_MAX_LEN];
    char holderSymStack[BUFFLEN];
    int evalBuffLen=0;



    strcpy(holderName,symnames[oprnStack[rnd]]);
    strcpy(holderSymStack,oprnSymStack[rnd].symStr);


    bool rTypeSet=false;
    char rtype[BUFFLEN];
    bool lTypeSet=false;
    char ltype[BUFFLEN];

    for (tor=optrStackPtr-1; tor>=0; tor--)
    {
        if (!rTypeSet)
        {
            if (oprnStack[rnd]==stringlit)
            {
                strcpy(rtype,"String");
            }
            else if (oprnStack[rnd]==intnumber)
            {
                strcpy(rtype,"Integer");
            }
            else if (oprnStack[rnd]==ident)
            {
                const char *idType=getIdentifierType(holderSymStack);
                if (idType!=NULL)
                    strcpy(rtype,getIdentifierType(holderSymStack));
                else
                    strcpy(rtype,"???");
            }
            else
            {
                strcpy(rtype,holderName);
            }
            rTypeSet=true;
        }
        printf("tor/optrStackPtr %d/%d rnd/oprnStackPtr %d/%d %s\n",tor,optrStackPtr,rnd,oprnStackPtr,oprnSymStack[rnd].symStr);

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

            if (!lTypeSet)
            {
                if (oprnStack[rnd]==stringlit)
                {
                    strcpy(ltype,"String");
                }
                else if (oprnStack[rnd]==intnumber)
                {
                    strcpy(ltype,"Integer");
                }
                else if (oprnStack[rnd]==ident)
                {
                    const char *idType=getIdentifierType(holderSymStack);
                    if (idType!=NULL)
                        strcpy(rtype,getIdentifierType(holderSymStack));
                    else
                        strcpy(rtype,"???");
                }
                else
                {
                    strcpy(ltype,holderName);
                }
                lTypeSet=true;
            }

            evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s.%s_%s(%s,%s)",
                                 rtype,
                                 ltype,
                                 fn,
                                 oprnSymStack[rnd-1].symStr,
                                 holderSymStack);
            if (oprnStack[rnd-1]==ident&&optrStack[tor]==assign)
            {
                const char * idType=getIdentifierType(oprnSymStack[rnd-1].symStr);
                if (idType==NULL)
                {
                    strcpy(idents[identIdx].name,oprnSymStack[rnd-1].symStr);
                    strcpy(idents[identIdx].type,rtype);
                    printf("New ident: %s %s;\n",idents[identIdx].type,idents[identIdx].name);
                    fprintf(outfile,"%s %s;\n",idents[identIdx].type,idents[identIdx].name);
                    identIdx++;
                }
                else
                {
                    if (strcmp(idType,rtype)!=0)
                        fprintf(stderr,"You can't redefine %s. This is not PHP\n",oprnSymStack[rnd-1].symStr);
                }

            }
            strcpy(holderName,symnames[oprnStack[rnd-1]]);
            strncpy(holderSymStack,evalBuff,evalBuffLen+1);
            rnd--;
        }
        else
        {

            evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s.%s(%s)",rtype,fn,holderSymStack);

            strcpy(holderName,symnames[oprnStack[rnd]]);
            strncpy(holderSymStack,evalBuff,evalBuffLen+1);
            rnd--;
        }
        printf("%d %s\n",evalBuffLen,evalBuff);
    }
    if (evalBuffLen>0)
        fprintf(outfile,"%s;\n",evalBuff);
}

void getln(void)
{
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

void opStackUpdate()
{
    oprnStack[oprnStackPtr]=sym;
    oprnStackPtr++;

    if (sym!=stringlit)
        strncpy(oprnSymStack[oprnSymStackPtr].symStr,symStr,symStrIdx+1);
    else
        snprintf(oprnSymStack[oprnSymStackPtr].symStr,BUFFLEN,"\"%s\"",symStr);
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
        const char * fn=getFunctionObject(symStr);
        if (fn!=NULL)
        {
            sym=function;
            strcpy(optrSymStack[optrStackPtr].symStr,fn);
            optrStack[optrStackPtr]=sym;
            optrStackPtr++;
        }
        else
        {
            sym=ident;

            opStackUpdate();
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

        opStackUpdate();

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
            symStr[symStrIdx++]=buff[linePos++];
            if ((buff[linePos]>='0'&&buff[linePos]<='9'))
            {
                while ((buff[linePos]>='0'&&buff[linePos]<='9'))
                {
                    symStr[symStrIdx++]=buff[linePos++];
                }
                symStr[symStrIdx]=0;
                sym=floatnumber;
                opStackUpdate();
            }
            else
            {
                fprintf(stderr,"Malformed decimal number\n");
            }
        }
        else
        {
            symStr[symStrIdx]=0;
            sym=intnumber;
            opStackUpdate();
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

    printf ("S:%s",symnames[sym]);
    if (symStrIdx>0)
    {
        printf ("->%s\n",symStr);
    }
    else
    {
        printf ("\n");
    }
}

void error(const char msg[]);
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
    fprintf(stderr,"expect: %s unexpected symbol %s\n",symnames[s],symnames[sym]);
    return 0;
}

void factor(void)
{
    if (accept(ident))
    {
        ;
    }
    else if (accept(number))
    {
        ;
    }
    else if (accept(lparen))
    {
        expression();
        expect(rparen);
    }
    else
    {
        error("factor: syntax error");
        getsym();
    }
}

void term(void)
{
    factor();
    while (sym == times || sym == slash)
    {
        getsym();
        factor();
    }
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
        while (sym == plus )
        {
            getsym();
            expression();
        }
    }
    else
    {
        fprintf(stderr,"Expecting expression. Got %s \n",symnames[sym]);
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

void condition(void)
{
    if (accept(oddsym))
    {
        expression();
    }
    else
    {
        expression();
        if (sym == equal || sym == neq || sym == lss || sym == leq || sym == gtr || sym == geq)
        {
            getsym();
            expression();
        }
        else
        {
            error("condition: invalid operator");
            getsym();
        }
    }
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
    else
    {
        fprintf(stderr,"Expect statement: got %s syntax error\n",symnames[sym]);
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
    printf ("End of block\n");
}


int main(int argc,char **argv)
{
    if (argc<2)
    {
        fprintf(stderr,"No file to compile\n");
        file=fopen("helloworld.rit","r");
    }
    else
    {
        file=fopen(argv[1],"r");
    }

    outfile=fopen("out.c","w");
    linePos=0;

    optrStackPtr=0;
    oprnStackPtr=0;
    optrSymStackPtr=0;
    oprnSymStackPtr=0;

    identIdx=0;
    statFuncIdx=0;

    /*TODO: This should be done in RL itself *
    /*Create stdout */
    strcpy(idents[identIdx].name,"out");
    strcpy(idents[identIdx].type,"Stream");
    statFuncIdx++;

    /*Setup some function shortcuts */
    strcpy(statFuncs[statFuncIdx].name,"print");
    strcpy(statFuncs[statFuncIdx].object,"out");
    statFuncIdx++;

    getln();
    getsym();
    block();
    if (sym!=eof)
        expect(eof);
    close(outfile);
    close(file);
}
