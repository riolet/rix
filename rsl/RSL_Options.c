#include "rsl/RSL_Options.h"
/* Options Stuff */

Options Options_$_Options_$_String ( String s) {
    return s.buffer;
}

bool Options_$_getBool_$_char (Options options, char option) {
    optind=1;
    char c;
    while ((c = getopt(_$$_argc, _$$_argv, "o:tqWL")) != -1)
    {
        if (c==option) {
            return true;
        }
    }
    return false;
}

String Options_$_getString_$_char (Options options, char option) {
    optind=1;
    char c;
    String s;
    s.buffer=0;
    while ((c = getopt(_$$_argc, _$$_argv, "o:tqWL")) != -1)
    {
        if (c==option) {
            return String_$_stringlit(optarg);
        }
    }
    return s;
}

String Options_$_getString_$_ (Options options) {
    optind=1;
    char c;
    String s;
    while ((c = getopt(_$$_argc, _$$_argv, "o:tqWL")) != -1)
    {

    }
    if (optind<_$$_argc) {
        return String_$_stringlit(_$$_argv[optind]);
    } else {
        s.buffer=0;
        s.length=0;
    }
    return s;
}