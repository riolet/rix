#include <stdio.h>

typedef enum {false, true} bool;
int ritch_i_;
#define foreach(item,array) for (ritch_i_=0;(item=array[ritch_i_])!=NULL;ritch_i_++)

const char * functions[]= {
    "print",NULL
};

const char * symnames[]= {
    "eol", "eof", "ident", "intnumber", "stringlit", "floatnumber", "number",
    "lparen", "rparen", "times", "slash", "plus", "minus", "assign", "equal",
    "neq", "lss", "leq", "gtr", "geq", "callsym", "beginsym", "semicolon",
    "endsym", "ifsym", "whilesym", "becomes", "thensym", "dosym", "constsym",
    "comma", "varsym", "procsym", "period", "oddsym", "plusassign", "function",
    "evaluation"
};

bool isFunction(const char * funcname)
{
    char *f;
    foreach (f,functions) {
        if (strcmp(f,funcname)==0) {
            return true;
        }
    }
    return false;
}

typedef enum {
    eol, eof, ident, intnumber, stringlit, floatnumber, number, lparen, rparen, times, slash, plus,
    minus, assign, equal, neq, lss, leq, gtr, geq, callsym, beginsym, semicolon, endsym,
    ifsym, whilesym, becomes, thensym, dosym, constsym, comma, varsym, procsym, period, oddsym, plusassign, function,
    evaluation
}
Symbol;

#define BUFFLEN 256
Symbol sym;
char symStr[BUFFLEN];
int symStrIdx;
FILE *file;

#define STACKDEP 1024
Symbol optrStack[STACKDEP];
int optrStackPtr;

Symbol oprnStack[STACKDEP];
int oprnStackPtr;

typedef struct SymElem_ {
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
    int evalBuffLen=0;

    for (tor=optrStackPtr-1; tor>=0; tor--) {
        printf("tor/optrStackPtr %d/%d rnd/oprnStackPtr %d/%d\n",tor,optrStackPtr,rnd,oprnStackPtr);

        if (rnd>0) {
            if (oprnStack[rnd]==evaluation)
                evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s(%s,\"%s\",%s,%s)\n",
                       symnames[optrStack[tor]],
                       symnames[oprnStack[rnd-1]],
                       oprnSymStack[rnd-1].symStr,
                       symnames[oprnStack[rnd]],
                       oprnSymStack[rnd].symStr);
            else
                evalBuffLen=snprintf(evalBuff,EVAL_BUFF_MAX_LEN,"%s(%s,\"%s\",%s,\"%s\")\n",
                       symnames[optrStack[tor]],
                       symnames[oprnStack[rnd-1]],
                       oprnSymStack[rnd-1].symStr,
                       symnames[oprnStack[rnd]],
                       oprnSymStack[rnd].symStr);

            oprnStack[rnd-1]=evaluation;
            strncpy(oprnSymStack[oprnSymStackPtr].symStr,evalBuff,evalBuffLen);
            rnd--;
        } else {
            if (oprnStack[rnd]==evaluation)
                printf("%s(%s)\n",symnames[optrStack[tor]],symnames[oprnStack[rnd]]);
            else
                printf("%s(\"%s\")\n",symnames[optrStack[tor]],symnames[oprnStack[rnd]]);
        }
    }
}

void getln(void)
{
    if (!fgets(buff,BUFFLEN,file)) {
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

    strncpy(oprnSymStack[oprnSymStackPtr].symStr,symStr,symStrIdx);
    oprnSymStackPtr++;
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
    /* Identifier */
    else if ((buff[linePos]>='a'&&buff[linePos]<='z')||(buff[linePos]>='A'&&buff[linePos]<='Z')) {
        while ((buff[linePos]>='a'&&buff[linePos]<='z')||(buff[linePos]>='A'&&buff[linePos]<='Z')||(buff[linePos]>='0'&&buff[linePos]<='9')) {
            symStr[symStrIdx++]=buff[linePos++];
        }
        symStr[symStrIdx]=0;
        if (isFunction(symStr)) {
            sym=function;

            optrStack[optrStackPtr]=sym;
            optrStackPtr++;
        } else {
            sym=ident;

            opStackUpdate();
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

        opStackUpdate();

        linePos++;
    }

    /* Assignment and equal */
    else if ((buff[linePos]=='=')) {
        if (buff[linePos+1]=='=') {
            linePos++;
            sym=equal;

            optrStack[optrStackPtr]=sym;
            optrStackPtr++;

        } else {
            sym=assign;

            optrStack[optrStackPtr]=sym;
            optrStackPtr++;

        }
        linePos++;
    } else if ((buff[linePos]=='+')) {
        if (buff[linePos+1]=='=') {
            linePos++;
            sym=plusassign;

            optrStack[optrStackPtr]=sym;
            optrStackPtr++;
        } else {
            sym=plus;

            optrStack[optrStackPtr]=sym;
            optrStackPtr++;
        }
        linePos++;
    }

    printf ("S:%s",symnames[sym]);
    if (symStrIdx>0) {
        printf ("->%s\n",symStr);
    } else {
        printf ("\n");
    }
}

void error(const char msg[]);
void expression(void);

int accept(Symbol s)
{
    if (sym == s) {
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
    if (accept(ident)) {
        ;
    } else if (accept(number)) {
        ;
    } else if (accept(lparen)) {
        expression();
        expect(rparen);
    } else {
        error("factor: syntax error");
        getsym();
    }
}

void term(void)
{
    factor();
    while (sym == times || sym == slash) {
        getsym();
        factor();
    }
}

void expression(void)
{
    if (accept(stringlit)) {
        while (sym == plus ) {
            getsym();
            expression();
        }
    } else if (accept(ident)) {
        while (sym == plus ) {
            getsym();
            expression();
        }
    } else {
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
    if (accept(oddsym)) {
        expression();
    } else {
        expression();
        if (sym == equal || sym == neq || sym == lss || sym == leq || sym == gtr || sym == geq) {
            getsym();
            expression();
        } else {
            error("condition: invalid operator");
            getsym();
        }
    }
}

void statement(void)
{
    if (accept(ident)) {
        expect(assign);
        expression();
    } else if (accept(function)) {
        expression();
    } else {
        fprintf(stderr,"Expect statement: got %s syntax error\n",symnames[sym]);
        getsym();
    }
}

void block(void)
{
    do {
        statement();
    } while (accept(eol));
    printf ("End of block\n");
}


int main(int argc,char **argv)
{
    if (argc<2) {
        fprintf(stderr,"No file to compile\n");
        file=fopen("helloworld.rit","r");
    } else {
        file=fopen(argv[1],"r");
    }

    linePos=0;

    optrStackPtr=0;
    oprnStackPtr=0;
    optrSymStackPtr=0;
    oprnSymStackPtr=0;

    getln();
    getsym();
    block();
    if (sym!=eof)
        expect(eof);
}
