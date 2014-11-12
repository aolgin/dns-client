/*
 * CS3600, Spring 2014
 * Project 2 Starter Code
 * (c) 2013 Alan Mislove
 *
 */

#ifndef __3600DNS_H__
#define __3600DNS_H__

#define ID_CODE 1337 // The query id for each outgoing packet
#define MAX_PACKET_SIZE 65536 // the maximum packet size
#define TEST_SERVER "129.10.112.152" // the test server all queries should be sent to

#define _POSIX_C_SOURCE 1

#include "helpers.h"
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


int* port;
char* server;
unsigned int querytype;

void format_name(char* name, int len);
void parse_server(char* s, int* p);
char* parse_pointer_str(char** buf_ptr, char* og_buf);
char* parse_static_str(char** buf_ptr, char* og_buf);

// Basic structure for the packet header
typedef struct packet_head_s {
  unsigned int id:16;
  unsigned int rd:1;
  unsigned int tc:1;
  unsigned int aa:1;
  unsigned int opcode:4;
  unsigned int qr:1;
  unsigned int ra:1;
  unsigned int z:3;
  unsigned int qdcount:16;
  unsigned int ancount:16;
  unsigned int nscount:16;
  unsigned int arcount:16;
} packet_head; // size is 94 bits

// Basic structure for a question, minus the name
typedef struct question_s {
  unsigned int qtype:16;
  unsigned int qclass:16;
} question; // size is 16 bits

// Basic structure for an answer, minus the name and rdata
typedef struct answer_s {
  unsigned int type:16;
  unsigned int class:16;
  unsigned int ttl:16;
  unsigned int rdlength:16;
} answer; // size is 64 bits

#endif
