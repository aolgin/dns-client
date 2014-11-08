/*
 * CS3600, Spring 2014
 * Project 2 Starter Code
 * (c) 2013 Alan Mislove
 *
 */

#ifndef __3600DNS_H__
#define __3600DNS_H__


#endif

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

// Basic structure for an answer, minus the name
typedef struct answer_s {
  unsigned int atype:16;
  unsigned int aclass:16;
  unsigned int ttl:16;
  unsigned int rdlength:16;
  unsigned int rdata:16;
} answer; // size is 80 bits
