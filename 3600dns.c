/*
 * CS3600, Spring 2014
 * Project 3 Starter Code
 * (c) 2013 Alan Mislove
 *
 */
#include "3600dns.h"
#include "helpers.h"

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

  ////////// process the arguments ///////////

  // If the improper amount of arguments is supplied
  if (argc !=  3) {
    fprintf(stderr, "Correct usage is './3600dns @<server:port> <name>'\n\
        port(optional): the UDP port number of the DNS server. Default value is 53\n\
        server (required): The IP address of the DNS server, in a.b.c.d format\n\
        name (required): the name to query for\n");
    exit(1);
  }

  // Set the query type to an A record by default
  querytype = 1;

  // Set the server name appropriately, according to argc
  //if (argc == 3) {
  //  server = argv[1];
  //} else {
  server = argv[1];

    /*
     * ---------Set-up for mx and ns support-------
    // Set the query type to a Name Server Record
    if (strcmp(argv[1], "-ns")) {
      querytype = 2;
    // Set the query type to a Mail Exchange Record
    } else if (strcmp(argv[1], "-mx")) {
      querytype = 15;
    } else {
      fprintf(stderr, "Flag is not recognized. Please either leave blank\n\
          or use either '-mx' or '-ns'\n");
      return -1;
    } */
  //}

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

  ////////// construct the DNS request ////////////

  // Set up the packet header
  packet_head* ph = alloca(sizeof(packet_head));
  ph->id = htons(ID_CODE);
  ph->qr = 0;
  ph->opcode = 0;
  ph->rd = 1;
  ph->z = 0;
  ph->ra = htons(0); // may not need to set? TODO
  ph->tc = 0;
  ph->aa = 0;
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
  q->qtype = htons(querytype);
  q->qclass = htons(1);

  // send the DNS request (and call dump_packet with your request)
  
  // the size of a packet
  int packet_size = sizeof(packet_head) + qsize;
  unsigned char* mypacket = alloca(packet_size);
                                               
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

  mypacket[3] = '\000';
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

  /*  ///////////// END MILESTONE MARK AREA //////////// */
  // wait for the DNS reply (timeout: 5 seconds)
  struct sockaddr_in in;
  socklen_t in_len;

  // construct the socket set
  fd_set socks;
  FD_ZERO(&socks);
  FD_SET(sock, &socks);

  // construct the timeout
  struct timeval t;
  t.tv_sec = 5;
  t.tv_usec = 0;

  char* tmpbuf = alloca(MAX_PACKET_SIZE);

  int res_len = 0;
  // wait to receive, or for a timeout
  if (select(sock + 1, &socks, NULL, NULL, &t)) {
    res_len = recvfrom(sock, tmpbuf, MAX_PACKET_SIZE, 0, (struct sockaddr*)&in, &in_len);
    if (res_len < 0) {
      // an error occured
      fprintf(stdout, "NORESPONSE\n");
      return -1;
    }
  } else {
    // a timeout occurred
    fprintf(stdout, "NORESPONSE\n");
    return -1;
  }

  //printf("RECEIVED PACKET OF SIZE: %i\n", res_len);
  //dump_packet((unsigned char*)tmpbuf, res_len);

  // Keep the original pointer for later reference
  char* og_buffer = tmpbuf;

  // Parse DNS Header
  packet_head* r_packet_head = alloca(sizeof(packet_head));
  memcpy(r_packet_head, tmpbuf, sizeof(packet_head));
  tmpbuf += sizeof(packet_head);

  // Move the pointer past the question, which we've already stored
  tmpbuf += qsize;

  // Create an array of answers and of rdata
  answer* answers[r_packet_head->ancount];
  char* rdatas[r_packet_head->ancount];

  // Parse each answer and add it to the array
  for (int i = 0; i < ntohs(r_packet_head->ancount); i++) { 

    // Parse the name of the answer
    // Check if the first two bits are 11
    char* a_name;
    if (((int)*tmpbuf & 192) == 192) {
      // This is a pointer to a string elsewhere
      a_name = parse_pointer_str(&tmpbuf, og_buffer);
    } else {
      // The string is right here
      a_name = parse_static_str(&tmpbuf, og_buffer);
    }

    //printf("AnswerName: %s\n", a_name);

    // Parse the rest of the answer 
    answer* myanswer = alloca(sizeof(answer));
    memcpy(myanswer, tmpbuf, sizeof(answer));
    tmpbuf += 10;

    // Store the answer in the array
    answers[i] = myanswer;
    
    // Get the RDATA
    rdatas[i] = alloca(ntohs(myanswer->rdlength));
    memcpy(rdatas[i], tmpbuf, ntohs(myanswer->rdlength));
    tmpbuf += ntohs(myanswer->rdlength);
  }

  // print out the result
  char* auth;
  if (r_packet_head->aa == 1) { auth = "auth"; }
  else { auth = "nonauth"; }

  for (int i = 0; i < ntohs(r_packet_head->ancount); i++) {
    // Check if an IP or a CNAME
    if (ntohs(answers[i]->type) == 1) {
      uint32_t ip;
      memcpy(&ip, rdatas[i], 4);
      struct in_addr ip_addr;
      ip_addr.s_addr = ip;

      printf("IP\t%s\t%s\n", inet_ntoa(ip_addr), auth);
    }

    if (ntohs(answers[i]->type) == 5) {
      char* name;
      if (((int)*rdatas[i] & 192) == 192) {
        // This is a pointer to a string elsewhere
        name = parse_pointer_str(&rdatas[i], og_buffer);
      } else {
        // The string is right here
        name = parse_static_str(&rdatas[i], og_buffer);
      } 
      unformat_name(name);
      name++;
      printf("CNAME\t%s\t%s\n", name, auth);
    } 
  }

  if (ntohs(r_packet_head->ancount) == 0) {
    printf("NOTFOUND\n");
  }
  return 0;
}
