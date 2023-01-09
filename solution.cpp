#include "solution.h"

//REMOVE
#include <iostream>
#include <unistd.h>

void *producerFunction(void *_arg) {
  argumentStructure* inputArguments = (argumentStructure*) _arg;
  // Parse the _arg passed to the function.
  // Enqueue `n` items into the `production_buffer`. The items produced should
  // be 0, 1, 2,..., (n-1).
  // Keep track of the number of items produced and the value produced by the
  // thread.
  // The producer that was last active should ensure that all the consumers have
  // finished. NOTE: Each thread will enqueue `n` items.
  // Use mutex variables and conditional variables as necessary.
  for(int i = 0; i < 10; i++){
    //pthread_mutex_lock();
    pthread_mutex_lock(inputArguments->queueLock);
    std::cout << " I am producer thread " << pthread_self() << " I will attempt to store " << i << std::endl;
    pthread_mutex_unlock(inputArguments->queueLock);
    sleep(rand()%3 + 1);
  }
  return 0;
}

void *consumerFunction(void *_arg) {
  // Parse the _arg passed to the function.
  // The consumer thread will consume items by dequeueing the items from the
  // `production_buffer`.
  // Keep track of the number of items consumed and the value consumed by the
  // thread.
  // Once the productions is complete and the queue is also empty, the thread
  // will exit. NOTE: The number of items consumed by each thread need not be
  // same.
  // Use mutex variables and conditional variables as necessary.
  return 0;
}

ProducerConsumerProblem::ProducerConsumerProblem(long _n_items,
                                                 int _n_producers,
                                                 int _n_consumers,
                                                 long _queue_size)
    : n_items(_n_items), n_producers(_n_producers), n_consumers(_n_consumers),
      production_buffer(_queue_size) {
  std::cout << "Constructor\n";
  std::cout << "Number of items: " << n_items << "\n";
  std::cout << "Number of producers: " << n_producers << "\n";
  std::cout << "Number of consumers: " << n_consumers << "\n";
  std::cout << "Queue size: " << _queue_size << "\n";

  producer_threads = new pthread_t[n_producers];
  consumer_threads = new pthread_t[n_consumers];
  // Initialize all mutex and conditional variables here.
  producerArguments.queueLock = new pthread_mutex_t;
}

ProducerConsumerProblem::~ProducerConsumerProblem() {
  std::cout << "Destructor\n";
  delete[] producer_threads;
  delete[] consumer_threads;
  // Destroy all mutex and conditional variables here.
}

void ProducerConsumerProblem::startProducers() {
  std::cout << "Starting Producers\n";
  n_producers = 2;
  active_producer_count = n_producers;
  
  pthread_t* thread = new pthread_t;

  for(int i = 0; i < n_producers; i++){
    std::cout << "producer spawned : " << i << std::endl;
    pthread_create(thread,NULL,producerFunction,&producerArguments);
  }
  // Create producer threads P1, P2, P3,.. using pthread_create.
}

void ProducerConsumerProblem::startConsumers() {
  std::cout << "Starting Consumers\n";
  active_consumer_count = n_consumers;
  // Create consumer threads C1, C2, C3,.. using pthread_create.
}

void ProducerConsumerProblem::joinProducers() {
  std::cout << "Joining Producers\n";
  // Join the producer threads with the main thread using pthread_join
}

void ProducerConsumerProblem::joinConsumers() {
  std::cout << "Joining Consumers\n";
  // Join the consumer threads with the main thread using pthread_join
}

void ProducerConsumerProblem::printStats() {
  std::cout << "Producer stats\n";
  std::cout << "producer_id, items_produced, value_produced, time_taken \n";
  // Make sure you print the producer stats in the following manner
  // 0, 1000, 499500, 0.00123596
  // 1, 1000, 499500, 0.00154686
  // 2, 1000, 499500, 0.00122881
  long total_produced = 0;
  long total_value_produced = 0;
  for (int i = 0; i < n_producers; i++) {
    // ---
    //
    // ---
  }
  std::cout << "Total produced = " << total_produced << "\n";
  std::cout << "Total value produced = " << total_value_produced << "\n";
  std::cout << "Consumer stats\n";
  std::cout << "consumer_id, items_consumed, value_consumed, time_taken \n";
  // Make sure you print the consumer stats in the following manner
  // 0, 677, 302674, 0.00147414
  // 1, 648, 323301, 0.00142694
  // 2, 866, 493382, 0.00139689
  // 3, 809, 379143, 0.00134516
  long total_consumed = 0;
  long total_value_consumed = 0;
  for (int i = 0; i < n_consumers; i++) {
    // ---
    //
    // ---
  }
  std::cout << "Total consumed = " << total_consumed << "\n";
  std::cout << "Total value consumed = " << total_value_consumed << "\n";
}