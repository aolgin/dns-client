/*
 * CS3600, Spring 2014
 * Project 2 Starter Code
 * (c) 2013 Alan Mislove
 *
 */

#ifndef __3600DNS_H__
#define __3600DNS_H__


#endif


// size of this is 14 bits
typedef struct flag_s {
  int qr:1;
  int opcode:4; // A standard query
  int aa:1;
  int tc:1;
  int rd:1;
  int ra:1;
  int z:1;
  int rcode:4;
} flag;

// size of this is 94 bits 
typedef struct packet_head_s {
  int id:16;
  flag flags;
  int qdcount:16;
  int ancount:16;
  int nscount:16;
  int arcount:16;
} packet_head;
