#include <stdio.h>
#include "rsl/rsl.h"
#include "rsl/RSL_Stream.h"
#include "rsl/RSL_String.h"

void Stream_$_destructor_$_(NonPrimObj * _$_mptr_in)
{
    Stream * s = (Stream *) _$_mptr_in->obj;

    fclose(s->stream);

    _$_cleanup(s->$super);
    free(s->$super);
    free(s);
}

NonPrimObj * Stream_$_Stream_$_ (NonPrimObj * _$_mptr_in)
{
    _$_HEAP_VARIABLE(_$_mptr_super);
    Stream * self_ = calloc(1, sizeof(Stream));
    _$_mptr * self = _$_returnAppointer(_$_mptr_in,self_,Stream_$_destructor_$_);
    self_->$super= BaseType_$_BaseType_$_(_$_mptr_super);
    self_->$super_= self_->$super->obj;
    return self;
}

NonPrimObj * Stream_$_Stream_$_String_$_String (String filename, String mode, NonPrimObj * _$_mptr_in)
{
    NonPrimObj * self = Stream_$_Stream_$_(_$_mptr_in);
    Stream * self_ = self->obj;
    self_->stream = fopen(filename.buffer,mode.buffer);
    if (!self_->stream) {
        criticalError(ERROR_RuntimeError,"Unable to open file");
    }
    return self;
}

/* Print prints the param and a newline char */
int Stream_$_print_$_String(NonPrimObj * stream_, NonPrimObj * s_)
{
    String * s = (String *) s_->obj;
    FILE * stream = ((Stream *) stream_->obj)->stream;
    int result = fwrite(s->buffer, sizeof(char), s->length, stream) + fputc('\n', stream);
    return result;
}

int Stream_$_print_$_int(NonPrimObj * stream_, int i)
{
    FILE * stream = ((Stream *) stream_->obj)->stream;
    return fprintf(stream, "%d\n", i);
}

int Stream_$_print_$_float(NonPrimObj * stream_, float f)
{
    FILE * stream = ((Stream *) stream_->obj)->stream;
    return fprintf(stream, "%f\n", f);
}

/* Echo omits the newline char */
int Stream_$_echo_$_String(NonPrimObj * stream_, NonPrimObj * s_)
{
    String * s = (String *) s_->obj;
    FILE * stream = ((Stream *) stream_->obj)->stream;
    int result = fwrite(s->buffer, sizeof(char), s->length, stream);
    return result;
}

int Stream_$_echo_$_int(NonPrimObj * stream_, int i)
{
    FILE * stream = ((Stream *) stream_->obj)->stream;
    return fprintf(stream, "%d", i);
}

int Stream_$_echo_$_float(NonPrimObj * stream_, float f)
{
    FILE * stream = ((Stream *) stream_->obj)->stream;
    return fprintf(stream, "%f", f);
}

NonPrimObj * Stream_$_fromFile_$_ (FILE *fp,NonPrimObj * _$_mptr_in)
{
    NonPrimObj * self = Stream_$_Stream_$_(_$_mptr_in);    
    Stream * selfStream = self->obj;
    selfStream->stream=fp;
    return self;
}