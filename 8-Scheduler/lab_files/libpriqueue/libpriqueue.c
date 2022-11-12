/** @file libpriqueue.c
 */

#include <stdlib.h>
#include <stdio.h>

#include "libpriqueue.h"


/**
  Initializes the priqueue_t data structure.
  
  Assumtions
    - You may assume this function will only be called once per instance of priqueue_t
    - You may assume this function will be the first function called using an instance of priqueue_t.
  @param q a pointer to an instance of the priqueue_t data structure
  @param comparer a function pointer that compares two elements. If comparer(x, y) < 0, then the priority of x is higher than the priority of y and therefore should be placed earlier in the queue.
 */
void priqueue_init(priqueue_t *q, int(*comparer)(const void *, const void *))
{
  q->queue     = NULL;
  q->tail      = NULL;
  q->size      = 0;
  q->comparer  = comparer;
}

/**
  Insert the specified element into this priority queue. You should use the queue's `comparer` function to determine where to place the item.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{

  node_t *newNode = (node_t *)malloc(sizeof(node_t));
  newNode->data = ptr;
  newNode->next = NULL;
  newNode->prev = NULL;

  node_t * curr = q->queue;

  if(curr == NULL){ 
    q->queue = newNode;
    q->tail = newNode;
    q->size++;
    return 0;
  }
  int ctr = 0;
  while(curr != NULL){
    if(q->comparer(newNode->data, curr->data) < 0)
    {
      if(curr->prev == NULL){
        newNode->next = curr;
        newNode->prev = NULL; 
        q->queue = newNode;
        curr->prev = newNode;
        q->size++;
        return ctr;
      }
      else{
        newNode->prev = curr->prev;
        ((node_t *)(curr->prev))->next = newNode;
        curr->prev = newNode;
        newNode->next = curr;
        q->size++;
        return ctr;
      }
    }
    curr = curr->next;
    ctr++;
  }
  
  q->tail->next = newNode;
  newNode->prev = q->tail;
  q->tail = newNode;
  q->size++;
  return ctr;
}


/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
 */
void *priqueue_peek(priqueue_t *q)
{
  if(q->size > 0){
    return q->queue[0].data;
  }
  else{
    return NULL;
  }
}


/**
  Retrieves and removes the head of this queue, or NULL if this queue
  is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the head of this queue
  @return NULL if this queue is empty
 */
void *priqueue_poll(priqueue_t *q)
{
	if(q->size > 0){
    int * store = (int *)malloc(sizeof(q->queue->data));//INT
    *store = *(int *)(q->queue->data);//INT
    q->queue = q->queue->next;
    q->queue->prev = NULL;
    q->size--;
    return (void *) store;
  }
  else{
    return NULL;
  };
}


/**
  Returns the element at the specified position in this list, or NULL if
  the queue does not contain an index'th element.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of retrieved element
  @return the index'th element in the queue
  @return NULL if the queue does not contain the index'th element
 */
void *priqueue_at(priqueue_t *q, int index)
{
	node_t * element;
  if(q->size > index)
  {
    element = q->queue;
    for(int i=0; i<index; i++){
      element = element->next;
    }
    return element->data;
  }
  else{
    return NULL;
  }

}


/**
  Removes all instances of ptr from the queue. 
  
  This function should not use the comparer function, but check if the data contained in each element of the queue is equal (==) to ptr.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr address of element to be removed
  @return the number of entries removed
 */
int priqueue_remove(priqueue_t *q, void *ptr)
{
	node_t * elem;
  elem = q->queue;
  int rem =0;
  while(elem != NULL){
    if(*(int *)(elem->data) == *(int *)ptr){
      node_t * toDelete = elem;
      if(elem->prev == NULL && elem->next == NULL){
        q->queue = NULL;
        
      }
      else if(elem->prev == NULL){
        ((node_t *)(elem->next))->prev = NULL;
        q->queue = ((node_t *)(elem->next));
        
      }
      else if(elem->next == NULL){
        ((node_t *)(elem->prev))->next = NULL;
        q->tail = (node_t *)(elem->prev);
        
      }
      else{
        ((node_t *)elem->next)->prev = elem->prev;
        ((node_t *)elem->prev)->next = elem->next;
      }
      elem = elem->next;

      q->size--;
      rem++;
    }
    else{
      elem = (node_t *)elem->next;
    }
  }
  return rem;

}


/**
  Removes the specified index from the queue, moving later elements up
  a spot in the queue to fill the gap.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of element to be removed
  @return the element removed from the queue
  @return NULL if the specified index does not exist
 */
void *priqueue_remove_at(priqueue_t *q, int index)
{
  if(index < 0 || index >= q->size){
    return NULL;
  }
  node_t * elem = q->queue;
  while(elem != NULL){
    if(index == 0){
      if(elem->prev != NULL)
        ((node_t *)elem->prev)->next = elem->next;
      else
        q->queue = (node_t *)elem->next;
      if(elem->next != NULL)
        ((node_t *)elem->next)->prev = elem->prev;
      else
        q->queue = (node_t *)elem->prev;
      q->size--;
      return elem->data;
    }
    elem = elem->next;
    index--;
}


/**
  Return the number of elements in the queue.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{
	return q->size;
}


/**
  Destroys and frees all the memory associated with q.
  
  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{

  //a while loop on all elements on the queue until it gets size of zero


}
