#include "solution.h"

void *producerFunction(void *_arg) {
  argumentStructure* inputArguments = (argumentStructure*) _arg;
  timer t1;
  t1.start();
  int errorPrint;
  long item = 0;
  long items_produced = 0;
  long value_produced = 0;
  while(items_produced < inputArguments->numberOfItems){
    errorPrint = pthread_mutex_lock(inputArguments->queueLock);
    bool enqueued = inputArguments->queue->enqueue(item);
    if(!enqueued){
      std::cout << "enqueue failed" << std::endl;
    }
    else{
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
    if(pthread_self() == inputArguments->threadStatsProducers[i].threadID){
      inputArguments->threadStatsProducers[i].runtime = t1.stop();
      inputArguments->threadStatsProducers[i].numberItems = items_produced;
      inputArguments->threadStatsProducers[i].valueItems = value_produced;
    }
  }
  pthread_mutex_unlock(inputArguments->queueLock);
  return 0;
}

void *consumerFunction(void *_arg) {
  argumentStructure* inputArguments = (argumentStructure*) _arg;
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
    if(pthread_self() == inputArguments->threadStatsConsumers[i].threadID){
      inputArguments->threadStatsConsumers[i].runtime = t1.stop();
      inputArguments->threadStatsConsumers[i].numberItems = items_consumed;
      inputArguments->threadStatsConsumers[i].valueItems = value_consumed;
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
  delete[] producer_threads;
  delete[] consumer_threads;
  delete[] threadArguments.queueLock;
}

void ProducerConsumerProblem::startProducers() {
  active_producer_count = n_producers;
  threadArguments.activeProducerCount = n_producers;
  consuming = 1;
  for(int i = 0; i < n_producers; i++){
    pthread_create(&producer_threads[i],NULL,producerFunction,&threadArguments);
    threadArguments.threadStatsProducers[i].threadID = producer_threads[i];
  }
}

void ProducerConsumerProblem::startConsumers() {
  active_consumer_count = n_consumers;
  for(int i = 0; i < n_consumers; i++){
    pthread_create(&consumer_threads[i],NULL,consumerFunction,&threadArguments);
    threadArguments.threadStatsConsumers[i].threadID = consumer_threads[i];
  }
}

void ProducerConsumerProblem::joinProducers() {
  std::cout << "Joining Producers\n";
  for(int i = 0; i < n_producers; i++){
  pthread_join(producer_threads[i],NULL);
  }
}

void ProducerConsumerProblem::joinConsumers() {
  std::cout << "Joining Consumers\n";
  for(int i = 0; i < n_consumers; i++){
  pthread_join(consumer_threads[i],NULL);
  }
}

void ProducerConsumerProblem::printStats() {
  std::cout << "Producer stats\n";
  std::cout << "producer_id, items_produced, value_produced, time_taken \n";
  long total_produced = 0;
  long total_value_produced = 0;
  for (int i = 0; i < n_producers; i++) {
    std::cout << i << ", " << threadArguments.threadStatsProducers[i].numberItems << ", " << threadArguments.threadStatsProducers[i].valueItems << ", "
    << threadArguments.threadStatsProducers[i].runtime << std::endl;
  }
  total_value_produced = threadArguments.valueProduced;
  total_produced = threadArguments.numberProduced;

  std::cout << "Total produced = " << total_produced << "\n";
  std::cout << "Total value produced = " << total_value_produced << "\n";
  std::cout << "Consumer stats\n";
  std::cout << "consumer_id, items_consumed, value_consumed, time_taken \n";
  long total_consumed = 0;
  long total_value_consumed = 0;
  for (int i = 0; i < n_consumers; i++) {
    std::cout << i << ", " << threadArguments.threadStatsConsumers[i].numberItems << ", " << threadArguments.threadStatsConsumers[i].valueItems << ", "
    << threadArguments.threadStatsConsumers[i].runtime << std::endl;
  }
  total_value_consumed = threadArguments.valueConsumed;
  total_consumed = threadArguments.valueConsumed;

  std::cout << "Total consumed = " << total_consumed << "\n";
  std::cout << "Total value consumed = " << total_value_consumed << "\n";
}