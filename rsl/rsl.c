#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "rsl.h"
#include "rsl/RSL_String.h"
#include "rsl/RSL_Stream.h"

int int_$_int_$_String(IDENT_MPTR_RAW *s_)
{
    String *s = s_->obj;
    return atoi(s->buffer);
}

float float_$_exponent_$_int(float f, int i)
{
    return pow(f, i);
}

int int_$_exponent_$_int(int i1, int i2)
{
    return (int)pow(i1, i2);
}

float int_$_exponent_$_float(int i, float f)
{
    return pow(i, f);
}

float float_$_exponent_$_float(float f1, float f2)
{
    return pow(f1, f2);
}

int int_$_sqrt_$_(int i)
{
    return (int)sqrt(i);
}

int print_$_String(IDENT_MPTR_RAW *s_)
{
    String *s = (String *)s_->obj;
    int result = fwrite(s->buffer, sizeof(char), s->length, stdout) + fputc('\n', stdout);
    return result;
}

int print_$_Char(char c)
{
    return fprintf(stdout, "%c\n", c);
}

int print_$_int(int i)
{
    return fprintf(stdout, "%d\n", i);
}

int print_$_float(float f)
{
    return fprintf(stdout, "%f\n", f);
}

int echo_$_String(IDENT_MPTR_RAW *s_)
{
    String *s = (String *)s_->obj;
    int result = fwrite(s->buffer, sizeof(char), s->length, stdout);
    return result;
}

int echo_$_int(int i)
{
    return fprintf(stdout, "%d", i);
}

int echo_float(float f)
{
    return fprintf(stdout, "%f", f);
}

Ternary int_$_compare_$_int(int a, int b)
{
    if (a < b)
    {
        return lt;
    }
    else if (a == b)
    {
        return eq;
    }
    else
    {
        return gt;
    }
}

String Ternary_$_pick_$_String_$_String_$_String(Ternary ternary, String a, String b,
                                                 String c)
{
    if (ternary == lt)
    {
        return a;
    }
    else if (ternary == eq)
    {
        return b;
    }
    else if (ternary == gt)
    {
        return c;
    }
}

int _$$_argc;
char **_$$_argv;

IDENT_MPTR_RAW *args_$_int(int i, IDENT_MPTR_RAW *$_mptr_in)
{
    return String_$_stringlit(_$$_argv[i], $_mptr_in);
}

int args_$_()
{
    return _$$_argc;
}

void BaseType_$_destructor(IDENT_MPTR_RAW *$_mptr_in)
{
    free($_mptr_in->obj);
}

IDENT_MPTR_RAW *BaseType_$_BaseType_$_(IDENT_MPTR_RAW *$_mptr_in)
{
    BaseType *b = calloc(1, sizeof(BaseType));
    _$_returnAppointer($_mptr_in, b, BaseType_$_destructor);
}

IDENT_MPTR_RAW *IDENT_MPTR_RAW_TEMP_assign(IDENT_MPTR_RAW *a, IDENT_MPTR_RAW *b)
{
    debugPrintf("Temp assigning %s = %s:\n", a->debugName, b->debugName);

    if (a->obj)
    {
        debugPrintf("%s %d is being recycled\n", a->debugName, a->ctr);
        _$_cleanup_object(a);
    } else {
        debugPrintf("%s %d is does not need recycling\n", a->debugName, a->ctr);
    }    
    debugPrintf("Temp assigned %s = %s:\n", a->debugName, b->debugName);    
    a->destructor = b->destructor;
    a->obj = b->obj;
    //Objects that get returned should not be destroyed
    b->ctr = 1;
    return a;
}

IDENT_MPTR_RAW * IDENT_MPTR_RAW_assign_with_alloc (IDENT_MPTR_RAW **a_ptr, IDENT_MPTR_RAW *b,char * debugName)
{
    IDENT_MPTR_RAW * a = *a_ptr;
    if (!a) {
        /* TODO: Create better class field handling */        
        a = malloc(sizeof(IDENT_MPTR_RAW)); 
        IDENT_MPTR_RAW_initialize(a,"TempHeapVariable");
        a->ptr=0;
        a->ctr=0;
        size_t needed = snprintf(a->debugName,1000, "%s %llu", debugName,(long long unsigned int)a);
    }
    *a_ptr = a;
    return IDENT_MPTR_RAW_assign(a,b);
}

IDENT_MPTR_RAW *IDENT_MPTR_RAW_assign(IDENT_MPTR_RAW *a, IDENT_MPTR_RAW *b)
{
    if (!a) {
        /* TODO: Create better class field handling */        
        a = malloc(sizeof(IDENT_MPTR_RAW)); 
        IDENT_MPTR_RAW_initialize(a,"TempHeapVariable");
        a->ptr=0;
        a->ctr=0;
    }

    debugPrintf("Assigning %s = %s:", a->debugName, b->debugName);

    if (a->obj)
    {
        debugPrintf("%s %d is being recycled\n", a->debugName, a->ctr);
        _$_cleanup_object(a);
    } else {
        debugPrintf("%s %d is does not need recycling\n", a->debugName, a->ctr);
    }    
    debugPrintf("assigned %s to %s\n", a->debugName, b->debugName);

    b->ctr++;

    debugPrintf("##  %d objects point to to %s\n", b->ctr, b->debugName);
    
    a->destructor = b->destructor;

    if (a->ptr) {
        a->ptr->ctr--;
    }

    a->ptr = b;
    a->obj = b->obj;
    return a;
}

IDENT_MPTR_RAW *IDENT_MPTR_RAW_point(IDENT_MPTR_RAW *a, IDENT_MPTR_RAW *b)
{

    debugPrintf("Pointing to %s to %s\n", a->debugName, b->debugName);
    b->ctr++;

    a->ptr = b;
    a->obj = b->obj;
    a->ctr = 0;
}



char *snprintfauto(const char *format, ...)
{
    va_list arg;
    long done;
    va_start(arg, format);
    size_t needed = vsnprintf(NULL,0, format, arg) + 1;
    char *buffer = malloc(needed);
    vsprintf(buffer, format, arg);
    va_end(arg);
    return buffer;
}

void IDENT_MPTR_RAW_initialize(IDENT_MPTR_RAW *mptr, char *name)
{
    mptr->ctr = 0;
    mptr->ptr = 0;
    mptr->obj = 0;
    mptr->destructor = 0;
    size_t needed = snprintf(mptr->debugName,1000, "%s %llu", name,(long long unsigned int)mptr);
}

void _$_cleanup_object(IDENT_MPTR_RAW *p)
{
    if (p->obj)
    {
        void (*destructor)(void *) = p->destructor;
        if (p->destructor)
        {
            debugPrintf("%s has destructor\n", p->debugName);
            destructor(p);
        }
        else
        {
            errorMsg("%s has no destructor\n", p->debugName);
            criticalError(ERROR_RuntimeError, "No destructor found");
        }
        p->obj = 0;
        debugPrintf("Gone !\n");
    }
    else
    {
        debugPrintf("Already Cleaned !\n");
    }
}

void _$_cleanup(IDENT_MPTR_RAW *p)
{
    debugPrintf("Cleaning up temp %d %s : ", p->ctr, p->debugName);

    if (p->ctr > 0)
    {
        debugPrintf("Survives !\n");
    }
    else
    {
        if (p->ptr == 0)
        {
            if (p->obj)
            {
                _$_cleanup_object(p);
            }
            else
            {
                debugPrintf("Already Cleaned !\n");
            }
        }
        else
        {
            IDENT_MPTR_RAW *next = p->ptr;
            debugPrintf("Check up stream -> %d %d",next,p->ptr);
            next->ctr--;
            _$_cleanup(next);
        }
        debugPrintf("Done cleaning up temp %d %s : ", p->ctr, p->debugName);
    }
}

void _$_cleanup_var(IDENT_MPTR_RAW **p)
{
    debugPrintf("Cleaning up var %d %s => ", (*p)->ctr, (*p)->debugName);
    IDENT_MPTR_RAW *pDeref = (*p);
    _$_cleanup(pDeref);
}

IDENT_MPTR_RAW *_$_returnAppointer(IDENT_MPTR_RAW *$_mptr_in, void *obj, void *destructor)
{
    if ($_mptr_in->obj)
    {
        debugPrintf("%s is being recycled\n", $_mptr_in->debugName);
        _$_cleanup_object($_mptr_in);
    }    
    $_mptr_in->ptr = 0;
    $_mptr_in->obj = obj;
    $_mptr_in->destructor = destructor;
    return $_mptr_in;
}

void _$_object_ownership_transfer(IDENT_MPTR_RAW *$_mptr_temp, IDENT_MPTR_RAW *$_mptr_in)
{
    debugPrintf("Transferring ownership of obj from %s to %s\n", $_mptr_temp->debugName, $_mptr_in->debugName);
    IDENT_MPTR_RAW_TEMP_assign($_mptr_in, $_mptr_temp);
}