#ifndef RSL_OPTIONS_H
#define RSL_OPTIONS_H
#include <unistd.h>
#include "rsl/RSL_String.h"
#include "rsl/rsl.h"

typedef char * Options;
Options Options_$_Options_$_String (String s);
bool Options_$_getBool_$_char (Options options, char option);
String Options_$_getString_$_char (Options options,char option);
String Options_$_getString_$_ (Options options);
#endif