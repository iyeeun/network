#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "packet.h"
void error_handling(char *message);
int main(int argc, char **argv) {
   int expectedseqnum = 0;
   int serv_sock;
   int data_len;
   int debugging;
   FILE *fp;
   char filename[50];
   char buf[BUFSIZE];
   struct sockaddr_in serv_addr;
   struct sockaddr_in clnt_addr;
   int clnt_addr_size;
   struct gbn_packet pkt;
   struct gbn_packet ack;
   // initialize
   pkt.seq_no = expectedseqnum;
   if(argc!=3) {
       printf("Usage : %s <port> <debugging on>\n", argv[0]);
       exit(1);
   }


   /* Argument parsing */
   if(strcmp(argv[2], "true") == 0 || strcmp(argv[2], "True") == 0 || strcmp(argv[2], "TRUE") == 0 || atoi(argv[2]) == 1) {
       debugging = 1;
       printf("debuggin message on\n");
   } else if(strcmp(argv[2], "false") == 0 || strcmp(argv[2], "False") == 0 || strcmp(argv[2], "FALSE") == 0 || atoi(argv[2]) == 0) {
       debugging = 0;
   }


   serv_sock = socket(PF_INET, SOCK_DGRAM, 0);
   if(serv_sock == -1) {
       error_handling("UDP socket error");
   }
    /* connection */
   memset(&serv_addr, 0, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   serv_addr.sin_port = htons(atoi(argv[1]));


   if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
       error_handling("bind() error");
   while(1) {
       // receive
       clnt_addr_size = sizeof(clnt_addr);
       recvfrom(serv_sock, &pkt, sizeof(pkt), 0, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
       printf("received data : %d bytes\n", pkt.data_len);
       data_len = pkt.data_len;


       if(debugging == 1)
           printf("receive pkt%d (type : %d, data bytes : %d)\n", pkt.seq_no, pkt.type, data_len);


       if(pkt.seq_no == expectedseqnum) {
           if(pkt.type == 0) { // filename
               // set filename and open
               strcpy(filename, pkt.data);
               fp = fopen(filename, "wb");
               if(fp == NULL) {
                   error_handling("fopen error");
               }


               // send ack
               ack = make_pkt(2, pkt.seq_no+1, 1, 0xffff);
               sendto(serv_sock, &ack, sizeof(ack), 0, (struct sockaddr*)&clnt_addr, sizeof(clnt_addr));


               if(debugging == 1) {
                   printf("filename received : %s\n", pkt.data);
                   printf("send ack %d\n", ack.seq_no);
               }
                  
           } else if(pkt.type == 1) { // receive data
               // data storing
               fwrite(pkt.data, sizeof(char), data_len, fp);


               // send ack
               ack = make_pkt(2, pkt.seq_no+1, 1, 0xffff);
               sendto(serv_sock, &ack, sizeof(ack), 0, (struct sockaddr*)&clnt_addr, sizeof(clnt_addr));
               if(debugging == 1) {
                   printf("send ack %d\n", ack.seq_no);
               }
                 
           } else if(pkt.type == 3) {
               fclose(fp);
               ack = make_pkt(3, pkt.seq_no+1, 1, 0xffff);
               sendto(serv_sock, &ack, sizeof(ack), 0, (struct sockaddr*)&clnt_addr, sizeof(clnt_addr));
               if(debugging == 1) {
                   printf("send ack %d\n", ack.seq_no);
                   printf("transfer complete\n");
               }           
               expectedseqnum = 0;
           }
           expectedseqnum ++;
        
       } else { // not in order
           if(expectedseqnum == 0) {
               ack = make_pkt(2, 0, 1, 0xffff);
           }
           sendto(serv_sock, &ack, sizeof(ack), 0, (struct sockaddr*)&clnt_addr, sizeof(clnt_addr));
           if(debugging == 1) {
               printf(">> not in order\n");
               printf("resend ack %d\n", ack.seq_no);
           }
       }
   }
   // close(sock);
   return 0;
}
void error_handling(char *message) {
   fputs(message, stderr);
   fputc('\n', stderr);
   exit(1);
}


