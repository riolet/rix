#ifndef RSL_STREAM_H
#define RSL_STREAM_H

struct Stream{
    IDENT_MPTR_RAW * $super;
    BaseType * $super_;
    FILE *stream;
};

void Stream_$_destructor_$_(IDENT_MPTR_RAW * _$_mptr_in);
IDENT_MPTR_RAW * Stream_$_Stream_$_ (IDENT_MPTR_RAW * _$_mptr_in);
IDENT_MPTR_RAW * Stream_$_Stream_$_String_$_String (IDENT_MPTR_RAW * filename_, IDENT_MPTR_RAW * mode_, IDENT_MPTR_RAW * _$_mptr_in);

/* Print prints the param and a newline char */
int Stream_$_print_$_String(IDENT_MPTR_RAW * stream_, IDENT_MPTR_RAW * s_);

int Stream_$_print_$_int(IDENT_MPTR_RAW * stream_, int i);

int Stream_$_print_$_float(IDENT_MPTR_RAW * stream_, float f);

/* Echo omits the newline char */
int Stream_$_echo_$_String(IDENT_MPTR_RAW * stream_, IDENT_MPTR_RAW * s_);

int Stream_$_echo_$_int(IDENT_MPTR_RAW * stream_, int i);

int Stream_$_echo_$_float(IDENT_MPTR_RAW * stream_, float f);
#endif