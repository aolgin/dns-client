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

#define TEST_SERVER1 "129.10.112.152" // the test server all queries should be sent to
void unformat_name(char* name);
void format_name(char* name, int len);
void parse_server(char* s, int* p);
char* parse_pointer_str(char** buf_ptr, char* og_buf);
char* parse_static_str(char** buf_ptr, char* og_buf);
