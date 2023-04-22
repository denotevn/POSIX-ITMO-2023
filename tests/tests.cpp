#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <ciso646>
#include <iostream>
#include <string>
#include "doctest.h"
#include "producer_consumer.h"

#define SUCCESS 0

TEST_CASE("Test run_threads with N = 3, ms = 500, debug = false") {
  int N = 3;
  int ms = 500;
  bool debug = false;
  int res = run_threads(N, ms, debug);
  CHECK(res == SUCCESS);
}

TEST_CASE("Test run_threads with N = 5, ms = 0, debug = true") {
  int N = 5;
  int ms = 0;
  bool debug = true;
  int res = run_threads(N, ms, debug);
  CHECK(res == SUCCESS);
}

