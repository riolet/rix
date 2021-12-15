#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "rsl.h"
#include "rsl/RSL_String.h"
#include "rsl/RSL_Stream.h"

int int_$_int_$_String(String s)
{
    return atoi(s.buffer);
}

double double_$_exponent_$_int(double f, int i)
{
    return pow(f, i);
}

int int_$_exponent_$_int(int i1, int i2)
{
    return (int)pow(i1, i2);
}

double int_$_exponent_$_double(int i, double f)
{
    return pow(i, f);
}

double double_$_exponent_$_double(double f1, double f2)
{
    return pow(f1, f2);
}

int int_$_sqrt_$_(int i)
{
    return (int)sqrt(i);
}

int print_$_String(String s)
{
    int result = fwrite(s.buffer, sizeof(char), s.length, stdout) + fputc('\n', stdout);
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

int print_$_double(double f)
{
    return fprintf(stdout, "%f\n", f);
}

int echo_$_String(String s)
{
    int result = fwrite(s.buffer, sizeof(char), s.length, stdout);
    return result;
}

int echo_$_int(int i)
{
    return fprintf(stdout, "%d", i);
}

int echo_double(double f)
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

String args_$_int(int i)
{
    return String_$_stringlit(_$$_argv[i]);
}

int args_$_()
{
    return _$$_argc;
}

void BaseType_$_destructor(void * $_mptr_in)
{
    free($_mptr_in);
}

