#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "minunit.h"
#include "helpers.h"

int tests_run = 0;

static char* test_parse_static_str() {
  // Tests a static string with no pointers
  // "3www6google3com"
  char test_str_static[] = {0x3, 0x77, 0x77, 0x77,
                            0x6, 0x67, 0x6f, 0x6f, 0x67, 0x6c, 0x65,
                            0x3, 0x63, 0x6f, 0x6d, 0x0};
  char* test_str = calloc(16, sizeof(char));
  strncpy(test_str, test_str_static, 16);
   

  // this will not be used but needs to be passed in
  char* og_buf = NULL;
  
  char* res = parse_static_str(&test_str, og_buf);

  printf("GOT: %s\n", res);
  mu_assert("ERROR in parse_static_str on 3www6google3com, wrong string returned",
             strcmp(res, test_str_static));
  tests_run++;

//  printf("GOT: %s\b", test_str);
  mu_assert("ERROR in parse_static_str on 3www6google3com, pointer not moved to end",
            strcmp(test_str, ""));

  return 0;
}

// Runs all our test functions
static char* all_tests() {
  
  // Run the tests
  test_parse_static_str();  
  return 0;
}

int main(int argc, char** argv) {
  char* result = all_tests();
  if (result != 0) {
    printf("%s\n", result);
  }
  else {
    printf("ALL TESTS PASSED\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}

