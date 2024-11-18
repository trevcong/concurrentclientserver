#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cnaiapi.h>

#define BUFFSIZE 256
#define RECEIVED_PROMPT "Server> "  


int main(int argc, char *argv[])
{
    computer comp;
    appnum app;
    connection conn;
    char buff[BUFFSIZE];
    int len;

    if (argc < 2 || argc > 3) {
        (void) fprintf(stderr, "usage: %s <compname> [appnum]\n",
                      argv[0]);
        exit(1);
    }

    comp = cname_to_comp(argv[1]);
    if (comp == -1)
        exit(10);

    if (argc == 3)
        app = (appnum) atoi(argv[2]);
    else
        if ((app = appname_to_appnum("echo")) == -1)
            exit(11);

    conn = make_contact(comp, app);
    if (conn < 0)
        exit(12);

    printf("Connected to server. Waiting for countdown...\n");

   
    while ((len = recv(conn, buff, BUFFSIZE-1, 0)) > 0) {
        buff[len] = '\0';  
        printf("%s%s", RECEIVED_PROMPT, buff);
        fflush(stdout);
    }

    printf("\nServer closed connection.\n");
    return 0;
}
