#ifndef RSL_STREAM_H
#define RSL_STREAM_H

typedef FILE * Stream;

void Stream_$_destructor_$_(Stream *s);
Stream Stream_$_Stream_$_ ();
Stream Stream_$_Stream_$_String_$_String (String filename, String mode);

/* Print prints the param and a newline char */
int Stream_$_print_$_String(Stream stream_, String s);

int Stream_$_print_$_int(Stream stream_, int i);

int Stream_$_print_$_float(Stream stream_, float f);

/* Echo omits the newline char */
int Stream_$_echo_$_String(Stream stream, String s);

int Stream_$_echo_$_int(Stream stream_, int i);

int Stream_$_echo_$_float(Stream stream_, float f);
#endif