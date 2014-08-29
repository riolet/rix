#include <stdio.h>

const char * symnames[]={
		"eol", "eof", "ident", "intnumber", "stringlit", "floatnumber", "number",
		"lparen", "rparen", "times", "slash", "plus", "minus", "assign", "equal",
		"neq", "lss", "leq", "gtr", "geq", "callsym", "beginsym", "semicolon",
		"endsym", "ifsym", "whilesym", "becomes", "thensym", "dosym", "constsym",
		"comma", "varsym", "procsym", "period", "oddsym", "plusassign"
};

typedef enum {
	eol, eof, ident, intnumber, stringlit, floatnumber, number, lparen, rparen, times, slash, plus,
    minus, assign, equal, neq, lss, leq, gtr, geq, callsym, beginsym, semicolon, endsym,
    ifsym, whilesym, becomes, thensym, dosym, constsym, comma, varsym, procsym, period, oddsym, plusassign} Symbol;

#define BUFFLEN 256
Symbol sym;
char pSym[BUFFLEN];
int pSymIdx;
FILE *file;

#define STACKDEP 1024
Symbol opStack[STACKDEP];
int opStackPtr;
typedef struct SymElem_ {
	char pSym[BUFFLEN];
} SymElem;

SymElem symStack[STACKDEP];
int symStackPtr;

char buff[BUFFLEN];
int idx;

void getln(void) {
	if (!fgets(buff,BUFFLEN,file)) {
		sym=eof;
	}
	idx=0;
}


void getsym(void) {
	printf("We are at %c %d\n",buff[idx],idx);
	/* End of line */
	pSymIdx=0;
	if (buff[idx]=='\n') {
		sym=eol;
		getln();
	}
	/* Identifier */
	else if ((buff[idx]>='a'&&buff[idx]<='z')||(buff[idx]>='A'&&buff[idx]<='Z')) {
		while ((buff[idx]>='a'&&buff[idx]<='z')||(buff[idx]>='A'&&buff[idx]<='Z')||(buff[idx]>='0'&&buff[idx]<='9')) {
			pSym[pSymIdx++]=buff[idx++];
		}
		pSym[pSymIdx]=0;
		sym=ident;
	}

	/* String Literal */
	else if ((buff[idx]=='"')) {
		idx++;
		do {
			pSym[pSymIdx++]=buff[idx++];
		} while (buff[idx]!='"');
		pSym[pSymIdx]=0;
		sym=stringlit;
		idx++;
	}

	/* Assignment and equal */
	else if ((buff[idx]=='=')) {
		if (buff[idx+1]=='='){
			idx++;
			sym=equal;
		} else {
			sym=assign;
		}
		idx++;
	}
	else if ((buff[idx]=='+')) {
		if (buff[idx+1]=='='){
			idx++;
			sym=plusassign;
		} else {
			sym=plus;
		}
		idx++;
	}

	printf ("S:%s",symnames[sym]);
	if (pSymIdx>0) {
		printf ("->%s\n",pSym);
	} else {
		printf ("\n");
	}
}

void error(const char msg[]);
void expression(void);

int accept(Symbol s) {
    if (sym == s) {
        getsym();
        return 1;
    }
    return 0;
}

int expect(Symbol s) {
    if (accept(s))
        return 1;
    fprintf(stderr,"expect: %s unexpected symbol %s\n",symnames[s],symnames[sym]);
    return 0;
}

void factor(void) {
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

void term(void) {
    factor();
    while (sym == times || sym == slash) {
        getsym();
        factor();
    }
}

void expression(void) {
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

void condition(void) {
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

void statement(void) {
    if (accept(ident)) {
        expect(assign);
        expression();
    } else {
    	fprintf(stderr,"Expect statement: got %s syntax error\n",sym);
        getsym();
    }
}

void block(void) {
    do {
    	statement();
    } while (accept(eol));
    printf ("End of block\n");
}


int main(int argc,char **argv) {
	if (argc<2) {
		fprintf(stderr,"No file to compile\n");
		file=fopen("helloworld.rit","r");
	} else {
		file=fopen(argv[1],"r");
	}
	idx=0;
	pSymIdx=0;
	opStackPtr=0;
	symStackPtr=0;
	getln();
    getsym();
    block();
    if (sym!=eof)
    	expect(eof);
}
