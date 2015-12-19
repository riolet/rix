#ifndef PREPROC_H
#define PREPROC_H

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "errors.h"

#define FLAG_ASSIGNMENT 1
#define FLAG_SUBJECT    2
#define FLAG_EXTERNAL   4
#define BUFFLEN 1024

int readFile(char name[], FILE * ofp, int *numline);
#endif
