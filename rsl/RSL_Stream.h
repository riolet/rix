#ifndef RSL_STREAM_H
#define RSL_STREAM_H

struct Stream{
    NonPrimObj * $super;
    BaseType * $super_;
    FILE *stream;
};

void Stream_$_destructor_$_(NonPrimObj * _$_mptr_in);
NonPrimObj * Stream_$_Stream_$_ (NonPrimObj * _$_mptr_in);
NonPrimObj * Stream_$_Stream_$_String_$_String (String filename, String mode, NonPrimObj * _$_mptr_in);

/* Print prints the param and a newline char */
int Stream_$_print_$_String(NonPrimObj * stream_, NonPrimObj * s_);

int Stream_$_print_$_int(NonPrimObj * stream_, int i);

int Stream_$_print_$_float(NonPrimObj * stream_, float f);

/* Echo omits the newline char */
int Stream_$_echo_$_String(NonPrimObj * stream_, NonPrimObj * s_);

int Stream_$_echo_$_int(NonPrimObj * stream_, int i);

int Stream_$_echo_$_float(NonPrimObj * stream_, float f);
#endif