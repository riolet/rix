#include "errors.h"


int errorMsg(const char * format,...)
{
    int ret;
    fprintf(stderr,"Line %d: Column:%d - ",lineNum,linePos);
    va_list arg;
    va_start(arg,format);
    ret = vfprintf(stderr, format, arg);
    va_end(arg);
    return ret;
}

void criticalError(ErrorCode code) {
    switch (code)
    {
    case ERROR_EndlessString:
        errorMsg("Error parsing string. Did not find the closing quote.");
        break;
    default:
        errorMsg("Unknown critical error. Aborting.");
    }
    exit((int)code);
}
