/*
 * ringbuff.c
 *
 *  Created on: Feb 27, 2022
 *      Author: 84963
 */


#include "ringbuffer.h"


void ringbuff_init(Ringbuff_t *ring,uint32_t *buff,unsigned int size){
	ring->count = 0;
	ring->tail = 0;
	ring->head = 0;
	ring->size = size;
	ring->buff = buff;
}
bool ringbuff_put_head(Ringbuff_t *ring,uint32_t c){
	unsigned int next = (unsigned int)(ring->head+1)% ring->size;
	if(next != ring->tail){
		ring->buff[ring->head] = c;
		ring->head = next;
		ring->count++;
		return true;
	}
	return false;
}
uint32_t ringbuff_pop_tail(Ringbuff_t *ring){
	if (ring->head == ring->tail)
	{
		ring->count = 0;
		return -1;        // quit with an error
	}
	else
	{
		uint32_t data = ring->buff[ring->tail];
		ring->tail = (unsigned int)(ring->tail + 1) % ring->size;
		ring->count -= 1;
		return data;
	}
	return -1;
}
unsigned int ringbuff_available(Ringbuff_t *ring){
	return ring->count;
}
void ringbuff_free(Ringbuff_t *ring){
	ring->count = 0;
	ring->tail = 0;
	ring->head = 0;
}
