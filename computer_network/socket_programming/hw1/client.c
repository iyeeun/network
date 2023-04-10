#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


#include "packet.h"


#define TIMEOUT_TIME 5


sigset_t mask;
timer_t timerID;


int send_base = 0;
int nextseqnum = 0;


struct gbn_packet *sndpkt;
int pkt_num;
int sock;
struct sockaddr_in serv_addr;
struct sockaddr_in from_addr;


int N;
int debugging;


void error_handling(char *message);
int createTimer(timer_t *_timerID, int sec, int msec);
void timeoutHandler();


int main(int argc, char **argv) {
  
   int send_done = 0;
   int receive_done = 0;
   int file_count = 0;


   char buf[BUFSIZE];
   char filename[BUFSIZE];
   int file_size;
   struct gbn_packet pkt;
   int str_len, addr_size, i;


  


   /* Argument parsing */


   if(argc != 6) {
       printf("Usage : %s <IP> <port> <SWS> <debugging on>\n", argv[0]);
       exit(1);
   }


   if(atoi(argv[3]) >= 1 && atoi(argv[3]) <= 10) {
       N = atoi(argv[3]);
   } else {
       printf("<SWS> : 1-10\n");
       exit(1);
   }
  
   // strcmp
   if(argv[4] == "true" || argv[4] == "True" || atoi(argv[4]) == 1) {
       debugging = 1;
   } else if(argv[4] == "false" || argv[4] == "False" || atoi(argv[4]) == 0) {
       debugging = 0;
   }


   strcpy(filename, argv[5]);


   /* --------------------------------------------------------------------------- */


   sock = socket(PF_INET, SOCK_DGRAM, 0);
   if(sock == -1)
       error_handling("UDP socket error");


   memset(&serv_addr, 0, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
   serv_addr.sin_port = htons(atoi(argv[2]));


   /* File loading */
   FILE* fp = fopen(filename, "rb");
   if(fp == NULL) {
       error_handling("File open error");
   } else {
       // file size check
       fseek(fp, 0, SEEK_END);
       file_size = ftell(fp);
       fseek(fp, 0, SEEK_SET);
       if(debugging == 1) {
           printf("File size : %d\n", file_size);
       }


       // packet number check
       pkt_num = 2 + (file_size / 500); // filename, complete, data
       if(file_size % 500 != 0) {
           pkt_num ++;
       }
       if(debugging == 1) {
           printf("Total packet number : %d\n", pkt_num);
       }


       // memory allocation
       sndpkt = (struct gbn_packet *) malloc(sizeof(struct gbn_packet) * pkt_num);


       // file name send
       pkt = make_pkt(0, nextseqnum, filename, strlen(filename), 0xffff);
       sndpkt[pkt.seq_no] = pkt; // sndpkt storing
       sendto(sock, &pkt, sizeof(pkt), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
       printf("send pkt%d\n", pkt.seq_no);
       nextseqnum ++;
   }
  
   while(1) {


       // file data reading
       if(send_done == 0 && nextseqnum < send_base + N && send_done == 0) {
           file_count = fread(buf, sizeof(char), 500, fp);
       }
       if(file_count == 0 && send_done == 0) {
           printf("all data transfered.\n");
           pkt = make_pkt(3, nextseqnum, "", 1, 0xffff);
           sndpkt[pkt.seq_no] = pkt;
           sendto(sock, &pkt, sizeof(pkt), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
           printf("send pkt%d\n", pkt.seq_no);
           nextseqnum ++;
           send_done = 1;
       }


       if(nextseqnum < send_base + N && send_done == 0) {
           pkt = make_pkt(1, nextseqnum, buf, file_count, 0xffff);
           sndpkt[pkt.seq_no] = pkt;
           sendto(sock, &pkt, sizeof(pkt), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
           printf("send pkt%d (file bytes : %d)\n", pkt.seq_no, pkt.data_len);
           //printf("** type : %d, seq_no : %d, data : %s, checksum : %x**\n", pkt.type, pkt.seq_no, pkt.data, pkt.checksum);
           if(send_base == nextseqnum) {
               createTimer(&timerID, TIMEOUT_TIME, 0);
               if(debugging == 1) {
                   printf(">>> start_timer\n");
               }
           }
           nextseqnum ++;
       } else {
           printf("window full !!!\n");
           printf("-------------------------------\n");
       }


       if(receive_done == 0) {


           // receive
           addr_size = sizeof(from_addr);
           str_len = recvfrom(sock, &pkt, sizeof(pkt), 0, (struct sockaddr *)&from_addr, &addr_size);


           if(pkt.type == 2) { // if ACK,
               send_base = pkt.seq_no;
               if(debugging == 1) {
                   printf("receive ack %d\n", pkt.seq_no);
               }
               if(send_base == nextseqnum+1) {
                   createTimer(&timerID, 0, 0);
                   if(debugging == 1) {
                       printf(">>> stop_timer\n");
                   }
                   break;
               } else {
                   createTimer(&timerID, TIMEOUT_TIME, 0);
                   if(debugging == 1) {
                       printf(">>> start_timer\n");
                   }
                   continue;
               }
           } else if(pkt.type == 3) {
               printf("transfer complete\n");
               free(sndpkt);
               fclose(fp);
               break;
           }
       }
      
   }


   close(sock);
   return 0;
}




void error_handling(char *message) {
   fputs(message, stderr);
   fputc('\n', stderr);
   exit(1);
}


void timeoutHandler() {
   int i;


   if(debugging == 1) {
       printf("time-out!\n");
   }
   createTimer(&timerID, TIMEOUT_TIME, 0);
   for(i = send_base; i < nextseqnum; i ++) {
       sendto(sock, &sndpkt[i], sizeof(sndpkt[i]), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
       if(debugging == 1) {
           printf("resend pkt%d\n", sndpkt[i].seq_no);
       }
   }


}


int createTimer(timer_t *_timerID, int sec, int msec) {
   struct sigevent te;
   struct itimerspec its;
   struct sigaction sa;


   sa.sa_flags = SA_SIGINFO;
   sa.sa_sigaction = timeoutHandler;
   sigemptyset(&sa.sa_mask);


   if(sigaction(SIGRTMIN, &sa, NULL) == -1) {
       printf("sigaction error\n");
       exit(1);
   }
  
   te.sigev_notify = SIGEV_SIGNAL;
   te.sigev_signo = SIGRTMIN;
   te.sigev_value.sival_ptr = _timerID;
   timer_create(CLOCK_REALTIME, &te, _timerID);


   its.it_interval.tv_sec = 0;
   its.it_interval.tv_nsec = 0;
   its.it_value.tv_sec = sec;
   its.it_value.tv_nsec = msec * 1000000;
   timer_settime(*_timerID, 0, &its, NULL);


   return 0;
}
