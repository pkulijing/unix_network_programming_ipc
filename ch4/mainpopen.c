#include <stdio.h> // popen, pclose
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

const int MAXLINE = 1024;

int main(int argc, char** argv) {
    
    char buf[MAXLINE];

    // get path name
    fgets(buf, MAXLINE, stdin);
    int len = strlen(buf);
    if (buf[len - 1] == '\n') {
        buf[len - 1] = 0;
    }

    char command[MAXLINE];
    snprintf(command, MAXLINE, "cat -n %s 2>&1", buf);

    FILE *fp = popen(command, "r");

    sleep(3);

    printf("*******************************************\n");
    while (fgets(buf, MAXLINE, fp) != NULL) {
        fputs(buf, stdout);
    }
    printf("*******************************************\n");
    
    pclose(fp);
    exit(0);
}

/*
output with 2>&1:
lijing at 21:51:53 in ~/Downloads/unpv22e/myown bin/mainpopen                                                                                                               [master][0]
lalala
*******************************************
cat: lalala: No such file or directory
*******************************************


output without 2>&1:
lijing at 21:53:05 in ~/Downloads/unpv22e/myown bin/mainpopen                                                                                                               [master][0]
lalala
cat: lalala: No such file or directory
*******************************************
*******************************************

cat: lalala: No such file or directory is an error msg written to stderr. popen does nothing special with it without 2>&1
*/