#ifndef PACKET_H
#define HEADER_H


#include <stdio.h>
#include <string.h>


#define BUFSIZE 512


struct gbn_packet {
 int type; // filename : 0, data : 1, ACK : 2, complete : 3
 int seq_no;
 char data[BUFSIZE];
 int data_len;
 int checksum;
};


struct gbn_packet make_pkt(int ptype, int pseq_no, char pdata[], int psize, int pchecksum) { 
 struct gbn_packet pkt;


 pkt.type = ptype;
 pkt.seq_no = pseq_no;
 strcpy(pkt.data, pdata);
 pkt.data_len = psize;
 pkt.checksum = pchecksum;


 return pkt;
}


int corrupt(struct gbn_packet pkt) {
 // return 1 if pkt is corrupted, or return 0
 if (pkt.checksum == 0xffff)
   return 0;
 else
   return 1;
}


#endif
