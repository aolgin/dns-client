#include "helpers.h"

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
// Modify the buffers given so that s becomes the name without a port or @
// and that the p points to the appropriate port
void parse_server(char* s, int* p) {

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
  
  // remove the @
  strcpy(s, s+1);
  memcpy(p, &tmp, sizeof(int));
}

// Uses the pointer offset to parse the name string from the original buffer
// and moves the buffer pointer past 
char* parse_pointer_str(char** buf_ptr, char* og_buf) {
  char* buf = *buf_ptr;

  // Find the offset of the NAME data
  // Pointer bytes are the two bytes that comprise the pointer,
  // e.g. 1100 1011
  // to get this as an integer we take the first byte and shift it
  // to the left 8, then add the second byte
  int pointer_bytes = ((int)buf[0] << 8) + (int)buf[1]; 
  // To get the offset we have to mask off the first two bits of the 
  // 16 bit integer (0011 1111 1111 1111 = base2(16383))
  int offset = 16383 & pointer_bytes;

  // Get the pointer to the offset
  char* offset_buf = og_buf + offset;

  // Move the buffer past the NAME pointer
  *buf_ptr = buf + 2;

  return parse_static_str(&offset_buf, og_buf);
}

// Return a pointer to a string that contains the name from the buffer
// and move the buffer forward past the name
char* parse_static_str(char** buf_ptr, char* og_buf) {
  char* buf = *buf_ptr;

  // The result string
  char* result = NULL;

  // Iterate through the string until we hit the null bit
  int i = 0;
  int result_size;
  while (buf[i] != '\0') {
    // The size of result is 0 if result is NULL
    if (result == NULL) { result_size = 0; } 
    else { result_size = strlen(result); }
    
    if ((buf[i] & 192) == 192) {
      // This is a pointer to another static string, 
      // parse it and add it to the result
      // Get a pointer to the location of the pointer in the buffer
      char* ptr_loc = buf + i;
      char* s = parse_pointer_str(&ptr_loc, og_buf);
      result = realloc(result, result_size + strlen(s));
      strncpy(result + result_size, s, strlen(s));
    } else {
      result = realloc(result, result_size + 1);
      // Just add this byte from the buffer
      strncpy(result + result_size, buf + i, 1);
    }
    i++;
  }
  // Move our buffer pointer past the string we just parsed
  *buf_ptr = buf + i;
  return result;
}
