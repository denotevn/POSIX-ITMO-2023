#include <iostream>
#include <sstream>
#include "producer_consumer.h"

int main(int argc, char* argv[]) {
  if (argc >= 3) {
    bool debug = false;
    int N;
    int ms;

    stringstream n_stream(argv[1]);
    n_stream >> N;
    if (N == 0) {
      N = 1;
    }
    stringstream ms_stream(argv[2]);
    ms_stream >> ms;

    if (argc == 4 && strcmp(argv[3], "-debug") == 0) {
      debug = true;
    }
    int res = run_threads(N, ms, debug);
    cout << res << std::endl;
  } else {
    exit(-1);
  }

  return 0;
}
