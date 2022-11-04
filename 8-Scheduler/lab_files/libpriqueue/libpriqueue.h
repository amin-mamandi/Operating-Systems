/** @file libpriqueue.h
 */

#ifndef LIBPRIQUEUE_H_
#define LIBPRIQUEUE_H_

/**
  Priqueue Data Structure

  This struct type represents a priority queue containing elements with polymorphic/arbitrary type (represented by void *).

  The "comparer" function with signature '(void *, void *) -> int' tells the queue how to compare any 2 elements based on priority.
  If comparer(x, y) < 0, x is higher priority than y and should therefore appear earlier in the queue.
  If comparer(x, y) = 0, x and y are considered to be the same priority and the queue order of these elements doesn't matter.
  If comparer(x, y) > 0, x is lower priority than y and should therefore be placed later in the queue.
  
  The user/programmer must use caution to ensure the underlying types in the queue are of the same underlying type (when casting to and from void *).

  Hint: You may want to define an additional struct type (recursive) that represents a "list"-like structure which will then be pointed to by the queue structure.
  Alternatively, you could make this queue structure itself recursive.
*/

typedef struct _node_t {
	void * item;
	struct _node_t * next;        
} node_t
       
typedef struct _priqueue_t
{
	int (* comparer)(void *, void *);
	node_t * top;
} priqueue_t;


void   priqueue_init     (priqueue_t *q, int(*comparer)(const void *, const void *));

int    priqueue_offer    (priqueue_t *q, void *ptr);
void * priqueue_peek     (priqueue_t *q);
void * priqueue_poll     (priqueue_t *q);
void * priqueue_at       (priqueue_t *q, int index);
int    priqueue_remove   (priqueue_t *q, void *ptr);
void * priqueue_remove_at(priqueue_t *q, int index);
int    priqueue_size     (priqueue_t *q);

void   priqueue_destroy  (priqueue_t *q);

#endif /* LIBPQUEUE_H_ */
