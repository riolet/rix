#include <stdio.h>
#include <string.h>
#define LINESIZE 1024

int readFile(char name[], FILE * ofp)
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
        perror("open");
        return 1;
    }
    while (fgets(line, LINESIZE, fp)) {
        if (sscanf(line, "%s%s", word, word2) == 2 && strcmp(word, "import") == 0) {
            readFile(word2, ofp);
        } else
            fprintf(ofp, "%s", line);
    }
}

int main(void)
{
    FILE *fp = fopen("/tmp/abc.xxx", "w");
    if (fp == 0) {
        perror("fopen");
        return 1;
    }
    readFile("abc", fp);
    return 0;
}
