#include "core/circular_queue.h"
#include "core/utils.h"
#include <pthread.h>
#include <stdlib.h>

typedef struct threadStats{
  pthread_t threadID;
  long numberItems;
  long valueItems;
  double runtime;
} threadStats;

typedef struct argumentStructure{
  pthread_mutex_t *queueLock;
  long numberOfItems;
  CircularQueue* queue;
  long valueProduced;
  long valueConsumed;
  long numberConsumed;
  long numberProduced;
  int totalProducers;
  int totalConsumers;
  int activeConsumerCount;
  int activeProducerCount;
  bool* consuming;
  threadStats* threadStatsProducers;
  threadStats* threadStatsConsumers;
} argumentStructure;

class ProducerConsumerProblem {
  argumentStructure threadArguments;
  long n_items;
  int n_producers;
  int n_consumers;
  bool consuming;
  CircularQueue production_buffer;
  pthread_t *producer_threads;
  pthread_t *consumer_threads;
  int active_producer_count;
  int active_consumer_count;

public:
  ProducerConsumerProblem(long _n_items, int _n_producers, int _n_consumers,
                          long _queue_size);
  ~ProducerConsumerProblem();
  void startProducers();
  void startConsumers();
  void joinProducers();
  void joinConsumers();
  void printStats();
};
