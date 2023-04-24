#include <iostream>
#include <sstream>
#include "producer_consumer.h"

int main(int argc, char* argv[]) {
  if (argc >= 3) {
    bool debug = false;
    unsigned short int N;
    unsigned short int ms;

    std::stringstream n_stream(argv[1]);
    n_stream >> N;
    if (N < 3) N = 3;

    std::stringstream ms_stream(argv[2]);
    ms_stream >> ms;

    if (argc > 3 and strcmp(argv[3], "-debug") == 0) debug = true;

    std::cout << run_threads(N, ms, debug) << std::endl;
  } else {
    exit(-1);
  }

  return 0;
}
