/*
 * CS3600, Spring 2014
 * Project 3 Starter Code
 * (c) 2013 Alan Mislove
 *
 */

#include <math.h>
#include <ctype.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <alloca.h>

#include "3600dns.h"

int* port;
char* server;

/**
 * This function will print a hex dump of the provided packet to the screen
 * to help facilitate debugging.  In your milestone and final submission, you 
 * MUST call dump_packet() with your packet right before calling sendto().  
 * You're welcome to use it at other times to help debug, but please comment those
 * out in your submissions.
 *
 * DO NOT MODIFY THIS FUNCTION
 *
 * data - The pointer to your packet buffer
 * size - The length of your packet
 */
static void dump_packet(unsigned char *data, int size) {
    unsigned char *p = data;
    unsigned char c;
    int n;
    char bytestr[4] = {0};
    char addrstr[10] = {0};
    char hexstr[ 16*3 + 5] = {0};
    char charstr[16*1 + 5] = {0};
    for(n=1;n<=size;n++) {
        if (n%16 == 1) {
            /* store address for this line */
            snprintf(addrstr, sizeof(addrstr), "%.4x",
               ((unsigned int)p-(unsigned int)data) );
        }
            
        c = *p;
        if (isprint(c) == 0) {
            c = '.';
        }

        /* store hex str (for left side) */
        snprintf(bytestr, sizeof(bytestr), "%02X ", *p);
        strncat(hexstr, bytestr, sizeof(hexstr)-strlen(hexstr)-1);

        /* store char str (for right side) */
        snprintf(bytestr, sizeof(bytestr), "%c", c);
        strncat(charstr, bytestr, sizeof(charstr)-strlen(charstr)-1);

        if(n%16 == 0) { 
            /* line completed */
            printf("[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
            hexstr[0] = 0;
            charstr[0] = 0;
        } else if(n%8 == 0) {
            /* half line: add whitespaces */
            strncat(hexstr, "  ", sizeof(hexstr)-strlen(hexstr)-1);
            strncat(charstr, " ", sizeof(charstr)-strlen(charstr)-1);
        }
        p++; /* next byte */
    }

    if (strlen(hexstr) > 0) {
        /* print rest of buffer if not empty */
        printf("[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
    }
}

int main(int argc, char *argv[]) {
  /**
   * I've included some basic code for opening a socket in C, sending
   * a UDP packet, and then receiving a response (or timeout).  You'll 
   * need to fill in many of the details, but this should be enough to
   * get you started.
   */

  // process the arguments

  // If the improper amount of arguments is supplied
  if (argc < 3 || argc > 4) {
    fprintf(stderr, "Correct usage is './3600dns [-ns|-ms] @<server:port> <name>'\n\
        port(optional): the UDP port number of the DNS server. Default value is 53\n\
        -ns|-ms (opti:onal): specify whether this is a name or mail server query \n\
        server (required): The IP address of the DNS server, in a.b.c.d format\n\
        name (required): the name to query for\n");
    exit(1);
  }

  // Set the server name appropriately, according to argc
  if (argc == 3) {
    server = argv[1];
  } else {
    server = argv[2];
  }

  // If the given server name is not in the right format, throw error
  if (*server != '@') {
    fprintf(stderr, "Incorrect name format for query.\n\
        Please use the following: @<server:port>\n\
        If port is unspecified, the default (53) will be used\n");
    return -1;
  }

  // Parse the server and port.
  // Removes the '@', and will
  // Fill in the server and port buffer with the proper values
  port = alloca(sizeof(int));
  parse_server(server, port);

  // construct the DNS request

  // Set up the packet header
  packet_head* ph = alloca(sizeof(packet_head));
  ph->id = htons(ID_CODE);
  ph->qr = 0;
  ph->opcode = 0;
  ph->rd = 1;
  ph->z = 0;
  ph->qdcount = htons(1);
  ph->ancount = htons(0);
  ph->nscount = htons(0);
  ph->arcount = htons(0);

  // Set up the question
  question * q = alloca(sizeof(question));
  
  // We need to parse the argument into a string with numerical
  // information letting us know the length of each . delimeted
  // part of the string
  char* qname = argv[2];
  int qnamelen = strlen(qname) + 1; // An extra byte for the prepended length
  format_name(qname, qnamelen);
  qnamelen++;

  int qsize = sizeof(question) + (qnamelen * sizeof(char));
  q->qtype = htons(1);
  q->qclass = htons(1);

  // send the DNS request (and call dump_packet with your request)
  
  // the size of a packet
  int packet_size = sizeof(packet_head) + qsize;
  unsigned char* mypacket = alloca(packet_size);//alloca(packet_size); // TODO need an actual size here, currently pseudo-code.
                                                          // Not taking into account answer, authority, and additional fields
  
  // Copy the packet into the allocated address space
  int packetlen = 0;
  // Copy the packet header
  memcpy(mypacket, ph, sizeof(packet_head));
  packetlen += sizeof(packet_head);
  // Copy the question string
  memcpy(mypacket + packetlen, qname, qnamelen * sizeof(char));
  packetlen += (qnamelen * sizeof(char));
  // Copy the rest of the question
  memcpy(mypacket + packetlen, q, sizeof(question));
  packetlen += sizeof(question);

  // TODO For sake of debugging, remove later
  if (packetlen != packet_size) {
    fprintf(stderr, "Conflicting packet sizes");
    return -1;
  }

  dump_packet(mypacket, packetlen);

  // first, open a UDP socket  
  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  // next, construct the destination address
  struct sockaddr_in out;
  out.sin_family = AF_INET;
  out.sin_port = htons((short) *port);
  out.sin_addr.s_addr = inet_addr(server);

  if (sendto(sock, mypacket, packetlen, 0, (struct sockaddr*)&out, sizeof(out)) < 0) {
    // an error occurred
    fprintf(stderr, "ERROR IN SENDTO\n");
    return -1;
  }

  /* END MILESTONE MARK AREA */
  // wait for the DNS reply (timeout: 5 seconds)
  struct sockaddr_in in;
  socklen_t in_len;

  // construct the socket set
  fd_set socks;
  FD_ZERO(&socks);
  FD_SET(sock, &socks);

  // construct the timeout
  struct timeval t;
  t.tv_sec = 5; // temporary value? TODO
  t.tv_usec = 0;

  char* tmpbuf = alloca(MAX_PACKET_SIZE);

  int res_len = 0;
  // wait to receive, or for a timeout
  if (select(sock + 1, &socks, NULL, NULL, &t)) {
    res_len = recvfrom(sock, tmpbuf, MAX_PACKET_SIZE, 0, &in, &in_len);
    if (res_len < 0) {
      // an error occured
      fprintf(stderr, "ERROR: An error occured in recvfrom\n");
    }
  } else {
    // a timeout occurredi
    fprintf(stderr, "ERROR: A Timeout occured when receiving a packet\n");
    return -1;
  }

  printf("RECEIVED PACKET OF SIZE: %i\n", res_len);
  dump_packet((unsigned char*)tmpbuf, res_len);

  // Parse DNS Header
  packet_head* a_packet_head = alloca(sizeof(packet_head));
  memcpy(a_packet_head, tmpbuf, sizeof(packet_head));
  tmpbuf += sizeof(packet_head);

  // Parse NAME, should be same length as name we sent
  char a_name[qnamelen];
  memcpy(a_name, tmpbuf, qnamelen);
  tmpbuf += qnamelen;

  // Parse the rest of the answer 
  answer* myanswer = alloca(sizeof(answer));
  memcpy(myanswer, tmpbuf, sizeof(answer));
  tmpbuf += sizeof(answer);

  // Display answer data for debugging
  printf("NAME: %s\n", a_name);
  printf("TYPE: %i\n", ntohs(myanswer->type));
  printf("CLASS: %i\n", ntohs(myanswer->class));
  printf("TTL: %i\n", ntohs(myanswer->ttl));
  printf("LEN: %i\n", ntohs(myanswer->rdlength));

  // print out the result
  return 0;
}

void format_name(char* name, int len) {
  char result[len + 1];

  int cur_len = 0;
  for(int i = 0; i < len; i++) {
    if (name[i] == '.' || name[i] == '\0') {
      // Copy the current length and then the string until this point 
      // into the result string starting at the i byte
      memcpy(result + (i - cur_len), &cur_len, 1);
      memcpy(result + (i - cur_len + 1), &name[i - cur_len], cur_len); 
      cur_len = 0;
    } else {
      cur_len++;
    }
  }
  result[len] = '\0';

  strcpy(name, result);
}

// Parse the server input so that we can extract a port, if supplied
// Modify the buffers given
void parse_server(char* s, int* p) {

  // remove the '@' at the beginning
  int tmp;
  char* end = strchr(s, ':');
 
  // If the given servername contains no ':', then no port is specified and the default should be used
  if (end == NULL) {
    tmp = 53;
  // otherwise, convert the end the string into an integer and set the port equal to it
  } else {
    tmp = atoi(end+1);
    *end = '\0';
  }

  strcpy(s, s+1);
  memcpy(p, &tmp, sizeof(int));
}
