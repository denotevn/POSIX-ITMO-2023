#include "producer_consumer.h"
#include "producer_consumer.cpp"
#include <iostream>
#include <sstream>
#include <cstring>

int main(int argc, char* argv[]) {
  if (argc >= 3) {
    bool debug = false;
    int N;
    int ms;

    std::stringstream n_stream(argv[1]);
    n_stream >> N;
    if (N == 0)
        N = 1;

    std::stringstream ms_stream(argv[2]);
    ms_stream >> ms;

    if(argc > 3 && strcmp(argv[3], "-debug") == 0)
    {
      debug = true;
    }
    std::cout << run_threads(N, ms, debug) << std::endl;
  } else {
      exit(-1);
  }

  return 0;
}

