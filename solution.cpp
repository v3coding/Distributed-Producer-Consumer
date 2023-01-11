#include "solution.h"

//REMOVE
//#include <iostream>
//#include <unistd.h>
//REMOVE

//I was verifying that all of the items were enqueued correctly but I need to fix joining and tracking threads first

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
  timer t1;
  t1.start();
  int errorPrint;
  long item = 0;
  long items_produced = 0;
  long value_produced = 0;
  while(items_produced < inputArguments->numberOfItems){
  //for(int i = 0; i < inputArguments->numberOfItems; i++){
    errorPrint = pthread_mutex_lock(inputArguments->queueLock);
    //std::cout << " I am producer thread " << pthread_self() << " Item = " << item << " Total items = " << value_produced << std::endl;
    bool enqueued = inputArguments->queue->enqueue(item);
    if(!enqueued){
      //std::cout << "enqueue failed" << std::endl;
    }
    else{
      //std::cout << item << " enqueued" << std::endl;
      value_produced += item;
      items_produced++;
      item++;
    }
    pthread_mutex_unlock(inputArguments->queueLock);
  }
  pthread_mutex_lock(inputArguments->queueLock);
  inputArguments->valueProduced += value_produced;
  inputArguments->numberProduced += items_produced;
  inputArguments->activeProducerCount--;
  for(int i = 0; i < inputArguments->totalProducers; i++){
    if(pthread_self() == inputArguments->threadStatsProducers->threadID){
      inputArguments->threadStatsProducers->runtime = t1.stop();
      inputArguments->threadStatsProducers->numberItems = items_produced;
      inputArguments->threadStatsProducers->valueItems = value_produced;
      //std::cout << "STORED METRICS PRODUCERS : UID = " << inputArguments->threadStatsProducers->threadID << " ITEMS PRODUCED " << inputArguments->threadStatsProducers->numberItems
      //<< " VALUE PRODUCED " << inputArguments->threadStatsProducers->valueItems << " TIME " << inputArguments->threadStatsProducers->runtime << std::endl;
    }
  }
  pthread_mutex_unlock(inputArguments->queueLock);
  //std::cout << "LINE 40 Active Producer Count " << inputArguments->activeProducerCount << std::endl; 
  return 0;
}

void *consumerFunction(void *_arg) {
  argumentStructure* inputArguments = (argumentStructure*) _arg;
  // Parse the _arg passed to the function.
  // The consumer thread will consume items by dequeueing the items from the
  // `production_buffer`.
  // Keep track of the number of items consumed and the value consumed by the
  // thread.
  // Once the productions is complete and the queue is also empty, the thread
  // will exit. NOTE: The number of items consumed by each thread need not be
  // same.
  // Use mutex variables and conditional variables as necessary.
  timer t1;
  t1.start();
  int errorPrint;
  long item;
  long items_consumed = 0;
  long value_consumed = 0; 

  while(inputArguments->consuming){
    pthread_mutex_lock(inputArguments->queueLock);
    bool ret = inputArguments->queue->dequeue(&item);
    if(ret){
      items_consumed++;
      value_consumed += item;
      //std::cout << "Number " << item << " was sucessfully Dequeued" << std::endl;
    }
    pthread_mutex_unlock(inputArguments->queueLock);
    if(inputArguments->activeProducerCount == 0 && inputArguments->queue->isEmpty()){
      inputArguments->consuming = 0;
    }
  }
    pthread_mutex_lock(inputArguments->queueLock);
    inputArguments->valueConsumed += value_consumed;
    inputArguments->numberConsumed += items_consumed;
    

    for(int i = 0; i < inputArguments->totalConsumers; i++){
    if(pthread_self() == inputArguments->threadStatsConsumers->threadID){
      inputArguments->threadStatsConsumers->runtime = t1.stop();
      inputArguments->threadStatsConsumers->numberItems = items_consumed;
      inputArguments->threadStatsConsumers->valueItems = value_consumed;
      //std::cout << "STORED METRICS CONSUMERS : UID = " << inputArguments->threadStatsConsumers->threadID << " ITEMS CONSUMED " << inputArguments->threadStatsConsumers->numberItems
      //<< " VALUE CONSUMED " << inputArguments->threadStatsConsumers->valueItems << " TIME " << inputArguments->threadStatsConsumers->runtime << std::endl;
    }
  }
  pthread_mutex_unlock(inputArguments->queueLock);
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
  threadArguments.threadStatsProducers = new threadStats[n_producers];
  threadArguments.threadStatsConsumers = new threadStats[n_consumers];
  threadArguments.queueLock = new pthread_mutex_t;
  threadArguments.numberOfItems = n_items;
  threadArguments.queue = &production_buffer;
  pthread_mutex_init(threadArguments.queueLock,NULL);
  threadArguments.consuming = &consuming;
  threadArguments.numberConsumed = 0;
  threadArguments.numberProduced = 0;
  threadArguments.valueConsumed = 0;
  threadArguments.valueProduced = 0;
  threadArguments.totalProducers = n_producers;
  threadArguments.totalConsumers = n_consumers;
}

ProducerConsumerProblem::~ProducerConsumerProblem() {
  //std::cout << "Destructor\n";
  delete[] producer_threads;
  delete[] consumer_threads;
  delete[] threadArguments.queueLock;
  // Destroy all mutex and conditional variables here.
}

void ProducerConsumerProblem::startProducers() {
  //std::cout << "Starting Producers\n";
  active_producer_count = n_producers;
  threadArguments.activeProducerCount = n_producers;
  consuming = 1;
  for(int i = 0; i < n_producers; i++){
    pthread_create(&producer_threads[i],NULL,producerFunction,&threadArguments);
    threadArguments.threadStatsProducers->threadID = producer_threads[i];
    //std::cout << "Thread Spawned : " << threadArguments.threadStatsProducers->threadID << std::endl;
  }
}

void ProducerConsumerProblem::startConsumers() {
  //std::cout << "Starting Consumers\n";
  active_consumer_count = n_consumers;

  //pthread_t* thread = new pthread_t;

  for(int i = 0; i < n_consumers; i++){
    pthread_create(&consumer_threads[i],NULL,consumerFunction,&threadArguments);
    threadArguments.threadStatsConsumers->threadID = consumer_threads[i];
        //std::cout << "consumer spawned : " << i << " with UID " << consumer_threads[i] << std::endl;
  }

  // Create consumer threads C1, C2, C3,.. using pthread_create.
}

void ProducerConsumerProblem::joinProducers() {
  //std::cout << "Joining Producers\n";
  for(int i = 0; i < n_producers; i++){
    pthread_join(producer_threads[i],NULL);
    //std::cout << "Producer " << producer_threads[i] << " Joined! " << std::endl;
   }
  //std::cout << "Number of Items Produced = " << threadArguments.numberProduced << " Total Value of Items Produced = " << threadArguments.valueProduced << std::endl;
  //std::cout << "Final Active Producer Count = " << threadArguments.activeProducerCount << std::endl;



  // Join the producer threads with the main thread using pthread_join
}

void ProducerConsumerProblem::joinConsumers() {
  //std::cout << "Joining Consumers\n";
    for(int i = 0; i < n_consumers; i++){
    pthread_join(consumer_threads[i],NULL);
    //std::cout << "Consumer " << consumer_threads[i] << " Joined! " << std::endl;
   }
   //std::cout << "Number of Items Consumed = " << threadArguments.numberConsumed << " Total Value of Items Consumed = " << threadArguments.valueConsumed << std::endl;
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
    std::cout << i << ", " << threadArguments.threadStatsProducers->numberItems << ", " << threadArguments.threadStatsProducers->valueItems << ", "
    << threadArguments.threadStatsProducers->runtime << std::endl;
  }
  total_value_produced = threadArguments.valueProduced;
  total_produced = threadArguments.numberProduced;

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
    std::cout << i << ", " << threadArguments.threadStatsConsumers->numberItems << ", " << threadArguments.threadStatsConsumers->valueItems << ", "
    << threadArguments.threadStatsConsumers->runtime << std::endl;
  }
  total_value_consumed = threadArguments.valueConsumed;
  total_consumed = threadArguments.valueConsumed;

  std::cout << "Total consumed = " << total_consumed << "\n";
  std::cout << "Total value consumed = " << total_value_consumed << "\n";
}