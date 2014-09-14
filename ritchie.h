#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *buff;
    size_t length;
    size_t cap;
} String;

#define Boolean_if(B) if (B)
#define Boolean_while(B) while (B)
#define ident_for_Integer_Integer(i,a,b) for (i=a;i<b;i++)


String String_plus_String (String left, String right) {
    String newString;
    strncpy(newString.buff,left.buff,left.length);
    strncpy(newString.buff+left.length,right.buff,right.length);
    newString.length=left.length+right.length;
    return newString;
}

Stream_print_stringlit(FILE *stream,char *s) {
    fprintf(stream,"%s",s);
}
Stream_print_String(FILE *stream,String s) {
    fwrite(s.buff,sizeof(char),s.length,stream);
}

Stream_print_Integer(FILE *stream, int i) {
    fprintf(stream,"%d",i);
}

Stream_print_Float(FILE *stream, float f) {
    fprintf(stream,"%f",f);
}
