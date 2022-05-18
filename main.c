#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <dispatch/dispatch.h>


void *producer(void *idConsumer);
void *consumer(void *idProducer);
int produce();
int consume();

dispatch_semaphore_t empty, full, mutex;

int* buffer = NULL;
int limit = 0, x = 0;
int bufferIndexIn = 0, bufferIndexOut = 0;
int bufferSize = 0;

int main() {

        int quantProd, quantCons;

        scanf("%d %d %d %d", &quantProd, &quantCons, &limit, &bufferSize);

        mutex = dispatch_semaphore_create(1);
        empty = dispatch_semaphore_create(bufferSize);
        full = dispatch_semaphore_create(0);

        //Initialize global buffer
        buffer = calloc(bufferSize, sizeof(int));

        pthread_t threadsProducer[quantProd];
        int producersId[quantProd];
        for (int i = 0; i < quantProd; i++) {
                producersId[i] = i;
        }

        pthread_t threadsConsumer[quantCons];
        int consumersId[quantCons];
        for (int i = 0; i < quantCons; i++) {
                consumersId[i] = i;
        }

        //Create Producers
        for (int i = 0; i < quantProd; i++) {
                pthread_create(&threadsProducer[i], NULL, producer, &producersId[i]);

        }

        //Create Consumers
        for (int i = 0; i < quantCons; i++) {
                pthread_create(&threadsConsumer[i], NULL, consumer, &consumersId[i]);
                
        }

        //Producers Join
        for (int i = 0; i < quantProd; i++) {
                pthread_join(threadsProducer[i], NULL);

        }

        //Consumers Join
        for (int i = 0; i < quantCons; i++) {
                pthread_join(threadsConsumer[i], NULL);

        }
        
        free(buffer);
        return 0;
}

void *producer(void *idProducer) {
        int *producerId = idProducer;

        while (1) {
                int producedValue = produce();
                x = (x + 1)%(limit+1);

                dispatch_semaphore_wait(empty, DISPATCH_TIME_FOREVER);
                dispatch_semaphore_wait(mutex, DISPATCH_TIME_FOREVER);

                buffer[bufferIndexIn] = producedValue;
                printf("\nProdutor %d produzindo %d na posicao %d", *producerId, producedValue, bufferIndexIn);

                bufferIndexIn = (bufferIndexIn + 1)%bufferSize;

                dispatch_semaphore_signal(full);
                dispatch_semaphore_signal(mutex);
        }
        
}

int produce() {
        return (2 * x) + 1;
}


void *consumer(void *idConsumer) {
        int *consumerId = idConsumer;
        int aux = 0;

        while (1) {
                dispatch_semaphore_wait(full, DISPATCH_TIME_FOREVER);
                dispatch_semaphore_wait(mutex, DISPATCH_TIME_FOREVER);

                int consumedValue = consume();
                printf("\nConsumidor %d consumindo %d da posicao %d", *consumerId, consumedValue, bufferIndexOut);
                bufferIndexOut = (bufferIndexOut + 1)%bufferSize;

                dispatch_semaphore_signal(empty);
                dispatch_semaphore_signal(mutex);
        }

}

int consume() {

        int result = buffer[bufferIndexOut];
        buffer[bufferIndexOut] = NULL;

        return result;
}