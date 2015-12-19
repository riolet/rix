#include "preproc.h"
#include "stdlib.h"
#include "stdio.h"
#include <string.h>
#include "errors.h"

#define LINESIZE 1024

int readFile(char name[], FILE * ofp, int *numline)
{
    FILE *fp;
    char line[LINESIZE], word[LINESIZE], word2[LINESIZE];
    size_t len = strlen(name);
    if (name[len - 1] == '"')
        name[len - 1] = '\0';
    if (name[0] == '"') {
        name++;
    }
    if ((fp = fopen(name, "r")) == 0) {
        printf("Cannot find import file %s in working directory. Trying RITCHIE_HOME\n",
               name);
        const char *RITCHIE_HOME = getenv("RITCHIE_HOME");
        char importPath[BUFFLEN];
        if (RITCHIE_HOME != 0) {
            snprintf(importPath, BUFFLEN, "%s/%s", RITCHIE_HOME, name);
            if ((fp = fopen(importPath, "r")) == 0) {
                printf
                    ("Cannot find import file %s in working directory or RITCHIE_HOME\n",
                     importPath);
                perror("open");
                return 1;
            }
        } else {
            criticalError(0, "RITCHIE_HOME not set.\n");
            perror("open");
            return 1;
        }
    }
    while (fgets(line, LINESIZE, fp)) {
        if (sscanf(line, "%s%s", word, word2) == 2 && strcmp(word, "import") == 0) {
            readFile(word2, ofp, numline);
            (*numline)++;
        } else
            fprintf(ofp, "%s", line);
    }
}
