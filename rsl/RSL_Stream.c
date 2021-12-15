#include <stdio.h>
#include "rsl/rsl.h"
#include "rsl/RSL_Stream.h"
#include "rsl/RSL_String.h"

void Stream_$_destructor_$_( Stream * s)
{
    fclose(*s);
}

Stream Stream_$_Stream_$_ (Stream   _$_mptr_in)
{
    Stream self;
    self=0;
    return self;
}

Stream Stream_$_Stream_$_String_$_String (String filename, String mode)
{
    Stream self = fopen(filename.buffer,mode.buffer);
    if (!self) {
        criticalError(ERROR_RuntimeError,"Unable to open file");
    }
    return self;
}

/* Print prints the param and a newline char */
int Stream_$_print_$_String(Stream   stream, String  s)
{
    int result = fwrite(stream, sizeof(char), s.length, stream) + fputc('\n', stream);
    return result;
}

int Stream_$_print_$_int(Stream   stream, int i)
{
    return fprintf(stream, "%d\n", i);
}

int Stream_$_print_$_float(Stream   stream, float f)
{
    return fprintf(stream, "%f\n", f);
}

/* Echo omits the newline char */
int Stream_$_echo_$_String(Stream   stream, String   s)
{
    int result = fwrite(s.buffer, sizeof(char), s.length, stream);
    return result;
}

int Stream_$_echo_$_int(Stream   stream, int i)
{
    return fprintf(stream, "%d", i);
}

int Stream_$_echo_$_float(Stream   stream, float f)
{
    return fprintf(stream, "%f", f);
}

Stream   Stream_$_fromFile_$_ (FILE *fp,Stream   _$_mptr_in)
{
    return fp;
}