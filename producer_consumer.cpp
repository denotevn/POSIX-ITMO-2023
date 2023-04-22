#include "producer_consumer.h"

using namespace std;

pthread_cond_t consumer_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t producer_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;

int get_tid(int id) {
  // 1 to 3+N thread ID
  static thread_local shared_ptr<int> tid(new int);
  if (id > 0) {
    *tid = id;
  }
  return *tid;
}

void* producer_routine(void* arg) {
  // wait for consumer to process
  producer_args* args = (producer_args*)arg;
  get_tid(2);

  pthread_mutex_lock(&my_mutex);
  while (!*args->consumer_started) {
    pthread_cond_wait(&consumer_cond, &my_mutex);
  }
  pthread_mutex_unlock(&my_mutex);
  // Read data, loop through each value and update the value, notify consumer,
  // wait for consumer to process
  string numbers_raw;
  getline(cin, numbers_raw);
  istringstream in(numbers_raw, istringstream::in);
  int number;

  while (in >> number) {
    pthread_mutex_lock(&my_mutex);
    *args->number_ptr = number;
    *args->new_number = true;
    pthread_cond_signal(&producer_cond);
    while (*args->new_number) {
      pthread_cond_wait(&consumer_cond, &my_mutex);
    }
    pthread_mutex_unlock(&my_mutex);
  }

  pthread_mutex_lock(&my_mutex);
  *args->completed = true;
  pthread_cond_broadcast(&producer_cond);
  pthread_mutex_unlock(&my_mutex);

  return nullptr;
}

void* consumer_routine(void* arg) {
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
  consumer_args* args = (consumer_args*)arg;
  get_tid(args->tid);

  pthread_mutex_lock(&my_mutex);
  if (*args->consumer_started == false) {
    *args->consumer_started = true;
    pthread_cond_broadcast(&consumer_cond);
  }
  pthread_mutex_unlock(&my_mutex);

  // for every update issued by producer, read the value and add to sum
  while (!*args->completed) {
    pthread_mutex_lock(&my_mutex);
    while (!*args->new_number && !*args->completed) {
      pthread_cond_wait(&producer_cond, &my_mutex);
    }
    if (*args->completed) {
      pthread_mutex_unlock(&my_mutex);
      break;
    }
    *args->new_number = false;
    args->psum += *args->number_ptr;
    pthread_cond_signal(&consumer_cond);
    pthread_mutex_unlock(&my_mutex);

    if (args->debug) {
      printf("(%d, %d)\n", get_tid(), args->psum);
    }
    if (args->ms > 0) {
      int ms = rand() % args->ms;
      if (ms > 0) {
        usleep(ms * 1000);
      }
    }
  }
  // return pointer to result (for particular consumer)
  return (void*)args;
}

void* consumer_interrupter_routine(void* arg) {
  // interrupt random consumer while producer is running
  interrupter_args* args = (interrupter_args*)arg;
  get_tid(3);
  pthread_mutex_lock(&my_mutex);
  while (!*args->consumer_started) {
    pthread_cond_wait(&consumer_cond, &my_mutex);
  }
  pthread_mutex_unlock(&my_mutex);
  while (!*args->completed) {
    int i = 0;
    if (args->n > 1) {
      i = rand() % (args->n - 1);
      pthread_cancel(args->consumers[i]);
    }
  }

  return nullptr;
}

// the declaration of run threads can be changed as you like
int run_threads(int N, int ms, bool debug) {
  // start N threads and wait until they're done
  int number;
  bool completed = false;
  bool consumer_started = false;
  bool new_number = false;
  int amount = 0;
  int status;

  pthread_t producer;
  producer_args p_args;
  p_args.number_ptr = &number;
  p_args.n = N;
  p_args.completed = &completed;
  p_args.consumer_started = &consumer_started;
  p_args.new_number = &new_number;
  status = pthread_create(&producer, NULL, producer_routine, (void*)&p_args);
  if (status != 0) {
    printf("error: can't create producer thread, status = %d\n", status);
    exit(-2);
  }

  pthread_t* consumers = new pthread_t[N];
  consumer_args* c_args = new consumer_args[N];
  consumer_args consumers_result;
  for (int i = 0; i < N; i++) {
    c_args[i].number_ptr = &number;
    c_args[i].debug = debug;
    c_args[i].completed = &completed;
    c_args[i].consumer_started = &consumer_started;
    c_args[i].new_number = &new_number;
    c_args[i].ms = ms;
    c_args[i].psum = 0;
    c_args[i].tid = 4 + i;

    status = pthread_create(&consumers[i], NULL, consumer_routine,
                            (void*)&c_args[i]);
    if (status != 0) {
      printf("error: can't create consumers[%d] thread, status = %d\n", i,
             status);
      exit(-2);
    }
  }

  pthread_t interrupter;
  interrupter_args_tag i_args;
  i_args.consumers = consumers;
  i_args.n = N;
  i_args.completed = &completed;
  i_args.consumer_started = &consumer_started;
  i_args.consumer_started = &consumer_started;
  status = pthread_create(&interrupter, NULL, consumer_interrupter_routine,
                          (void*)&i_args);
  if (status != 0) {
    printf("error: can't create interrupter thread, status = %d\n", status);
    exit(-2);
  }

  // return aggregated sum of values
  for (int i = 0; i < N; i++) {
    status = pthread_join(consumers[i], (void**)&consumers_result);
    // cout << "Thread: " << i << endl;
    if (status != 0) {
      printf("error: can't join consumers[%d] thread, status = %d\n", i,
             status);
      exit(-3);
    }
    amount += c_args[i].psum;
  }
  // free alocated
  delete[] consumers;
  delete[] c_args;
  pthread_mutex_destroy(&my_mutex);
  pthread_cond_destroy(&producer_cond);
  pthread_cond_destroy(&consumer_cond);

  // return aggregated sum of values
  return amount;
}
