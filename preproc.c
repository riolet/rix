#include "preproc.h"
#include "ObjectTree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "errors.h"
#include "ext/List/cliblist.h"

#define LINESIZE 1024

int XopenFiles(char name[], list_t *fileList)
{
    FILE *fp;
    char line[LINESIZE], word[LINESIZE], word2[LINESIZE];
    size_t len = strlen(name);
    if (name[len - 1] == '"')
        name[len - 1] = '\0';
    
    if (name[0] == '"')
    {
        name++;
    }

    if ((fp = fopen(name, "r")) == 0)
    {
        compilerDebugPrintf("Cannot find import file %s in working directory. Trying RIX_HOME\n",
                            name);
        const char *RIX_HOME = getenv("RIX_HOME");
        char importPath[BUFFLEN];
        if (RIX_HOME != 0)
        {
            snprintf(importPath, BUFFLEN, "%s/%s", RIX_HOME, name);
            if ((fp = fopen(importPath, "r")) == 0)
            {
                printf("Cannot find import file %s in working directory or RIX_HOME\n",
                       importPath);
                perror("open");
                return 1;
            }
        }
        else
        {
            criticalError(0, "RIX_HOME not set.\n");
            perror("open");
            return 1;
        }
    }
    else
    {
        
        while (fgets(line, LINESIZE, fp))
        {
            char *importPos = strstr(line, "import");
            if (importPos != NULL)
            {
                char *openParenPos = strchr(importPos, '(');
                if (openParenPos != NULL)
                {
                    char *word2 = malloc(BUFFLEN);
                    char *cursor = openParenPos + 1;
                    int i = 0;
                    while (*(cursor + i) != ')')
                    {
                        word2[i] = *(cursor + i);
                        i++;
                    }
                    word[i] = '\0';
                    openFiles(word2, fileList);
                    continue;
                }
            }
        }
        printf("Adding file %s \n",name);
        list_rpush(fileList, list_node_new(fp));
    }
}

int readFile(char name[], FILE *ofp, int *numline)
{
    FILE *fp;
    char line[LINESIZE], word[LINESIZE], word2[LINESIZE];
    size_t len = strlen(name);
    if (name[len - 1] == '"')
        name[len - 1] = '\0';
    if (name[0] == '"')
    {
        name++;
    }
    if ((fp = fopen(name, "r")) == 0)
    {
        compilerDebugPrintf("Cannot find import file %s in working directory. Trying RIX_HOME\n",
                            name);
        const char *RIX_HOME = getenv("RIX_HOME");
        char importPath[BUFFLEN];
        if (RIX_HOME != 0)
        {
            snprintf(importPath, BUFFLEN, "%s/%s", RIX_HOME, name);
            if ((fp = fopen(importPath, "r")) == 0)
            {
                printf("Cannot find import file %s in working directory or RIX_HOME\n",
                       importPath);
                perror("open");
                return 1;
            }
        }
        else
        {
            criticalError(0, "RIX_HOME not set.\n");
            perror("open");
            return 1;
        }
    }
    while (fgets(line, LINESIZE, fp))
    {
        char *importPos = strstr(line, "import");
        if (importPos != NULL)
        {
            char *openParenPos = strchr(importPos, '(');
            if (openParenPos != NULL)
            {
                char *word2 = malloc(BUFFLEN);
                char *cursor = openParenPos + 1;
                int i = 0;
                while (*(cursor + i) != ')')
                {
                    word2[i] = *(cursor + i);
                    i++;
                }
                word[i] = '\0';
                readFile(word2, ofp, numline);
                continue;
            }
        }
        (*numline)++;
        fprintf(ofp, "%s", line);
    }
}
