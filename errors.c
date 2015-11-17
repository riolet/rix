#include "errors.h"


int warningMsg(const char * format, ...) {
    int ret;
    //fprintf(stderr,"Line %d: Column:%d - ",lineNum,linePos);
    fprintf(stderr,ANSI_COLOR_YELLOW);
    va_list arg;
    va_start(arg,format);
    ret = vfprintf(stderr, format, arg);
    va_end(arg);
    fprintf(stderr,ANSI_COLOR_RESET);
    return ret;
}

int errorMsg(const char * format,...)
{
    int ret;
    fprintf(stderr,"Line %d: Column:%d - ",g_lineNum, g_lineCol);
    va_list arg;
    va_start(arg,format);
    ret = vfprintf(stderr, format, arg);
    va_end(arg);
    return ret;
}

void criticalError(ErrorCode code, char* message) {
    fprintf(stderr, "\t");
    switch (code)
    {
    case ERROR_EndlessString:
        errorMsg("Error parsing string. No closing quote.\n");
        break;
    case ERROR_IncompatibleTypes:
        errorMsg("Type mismatch.\n");
        break;
    case ERROR_UnexpectedIndent:
        errorMsg("Unexpected scope increase\n");
        break;
    case ERROR_AssignToLiteral:
        errorMsg("Cannot assign to a literal.\n");
        break;
    case ERROR_UnrecognizedSymbol:
        errorMsg("Unknown symbol detected.\n");
        break;
    case ERROR_CannotAllocateMemory:
        errorMsg("Cannot allocate new space.\n");
        break;
    case ERROR_UndefinedVerb:
        errorMsg("Verb encountered without definition.\n");
        break;
    case ERROR_UndefinedVariable:
        errorMsg("Variable encountered without definition.\n");
        break;
    case ERROR_InvalidArguments:
        errorMsg("You attempted to run a function with invalid arguments.\n");
        break;
    case ERROR_ParseError:
        errorMsg("Error while parsing file.\n");
        break;
    default:
        errorMsg("Unknown critical error. Aborting.\n");
    }
    if (message) {
        fprintf(stderr, "\t");
        fprintf(stderr, "%s", message);
    }
    exit((int)code);
}
