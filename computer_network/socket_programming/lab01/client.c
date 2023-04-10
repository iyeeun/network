#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFSIZE 1024

void error_handling(char *message);

int main(int argc, char **argv) {
        int sock;
        char message[BUFSIZE];
        int str_len, i;
        struct sockaddr_in serv_addr;

        char MSG1[] = "Hello\nWorld";
        char MSG2[] = "HI\nhi";
        char MSG3[] = "HH\njj";

        if(argc != 3) {
                printf("Usage : %s <IP> <port>\n", argv[0]);
                exit(1);
        }

        sock = socket(PF_INET, SOCK_STREAM, 0);
        if(sock == -1)
                error_handling("socket() error");

        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
        serv_addr.sin_port = htons(atoi(argv[2]));

        if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
                error_handling("connect() error");

        /*
        while(1) {
                fputs("Enter a message to send(q to quit) : ", stdout);
                fgets(message, BUFSIZE, stdin);

                if(!strcmp(message, "q\n")) break;
                write(sock, message, strlen(message));

                str_len = read(sock, message, BUFSIZE-1);
                message[str_len] = 0;
                printf("A message from the server : %s \n", message);
        }
        */

        int len_m1 = strlen(MSG1);
        write(sock, &len_m1, sizeof(len_m1));
        write(sock, MSG1, strlen(MSG1));

        int len_m2 = strlen(MSG2);
        write(sock, &len_m2, sizeof(len_m2));
        write(sock, MSG2, strlen(MSG2));

        int len_m3 = strlen(MSG3);
        write(sock, &len_m3, sizeof(len_m3));
        write(sock, MSG3, strlen(MSG3));

        for(i = 0; i < 3; i ++) {
                str_len = read(sock, message, BUFSIZE-1);
                message[str_len] = '\0';
                printf("Message %d : \n%s\n", i+1, message);
        }

        close(sock);
        return 0;
}


void error_handling(char *message) {
        fputs(message, stderr);
        fputc('\n', stderr);
        exit(1);
}
