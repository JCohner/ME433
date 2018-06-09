#include<xc.h>           // processor SFR definitions
#include <stdlib.h>
#include "dsp.h"
#include <stdio.h>
#include <limits.h>
#include "ST7735.h"
 
// function to create a queue of given capacity. 
// It initializes size of queue as 0
struct Queue* createQueue(unsigned capacity)
{
    struct Queue* queue;// = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = queue->rear = 0; 
    //queue->rear = capacity - 1;  // This is important, see the enqueue starts by pointing to array el last
    int storage[queue->capacity];
    int i;
    for (i = 0; i < queue->capacity; ++i){
        storage[i] = 0;
    }
    queue->array =  storage; //original implementation //(int*) malloc(queue->capacity * sizeof(int));
    return queue;
}
 
// Queue is full when size becomes equal to the capacity 
int isFull(struct Queue* queue)
{  return (queue->size == queue->capacity);  }
 
// Queue is empty when size is 0
int isEmpty(struct Queue* queue)
{  return (queue->size == 0); }
 
float MAFnqueue(struct Queue* queue, int item){
  
  char data_msg[50];
  LCD_clearScreen(WHITE);
  sprintf(data_msg,"Made it here dog");
  LCD_drawString(5,5, data_msg, WHITE, BLACK);  
  queue->array[queue->rear] = item;
  
  queue->rear = (queue->rear + 1)%queue->capacity;
  int i;
  float average; 
  for (i = 0; i < queue->capacity; ++i){
    average += *(queue->array + i);
  }
  average = average/((float) queue->size);
  return average;
}

// Function to add an item to the queue.  
// It changes rear and size
void enqueue(struct Queue* queue, int item)
{
    /*
    if (isEmpty(queue)){
        //adding first element
        queue->array[queue->rear] = item;
        queue->rear = 1;
        queue->size = 1;
        return;
    } else if (queue->rear == queue->front){
        queue->front = queue->front + 1;
        queue->rear = queue->rear + 1;
        
    } else {
        queue->rear = (queue->rear + 1)%queue->capacity; //increment locational pointer to rear modulo with size
        queue->array[queue->rear] = item; //
        queue->size = queue->size + 1;
     * 
    }*/
}
 
// Function to remove an item from queue. 
// It changes front and size
int dequeue(struct Queue* queue)
{
    if (isEmpty(queue))
        return 0;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1)%queue->capacity;
    queue->size = queue->size - 1;
    return item;
}
 
// Function to get front of queue
int front(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->front];
}
 
// Function to get rear of queue
int rear(struct Queue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->rear];
}

float MAF(struct Queue* queue){
    float average = 0;
    
    unsigned lim = queue->capacity;
    int ii;
    for (ii = 0; ii
            < lim; ++ii){
        average += *(queue->array + ii);
    }
    return average;
}

