/*
 * ringbuff.h
 *
 *  Created on: Feb 27, 2022
 *      Author: 84963
 */

#ifndef INC_RINGBUFF_H_
#define INC_RINGBUFF_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
#include "math.h"
#include "stdlib.h"
#include "stdint.h"
typedef struct{
	uint32_t *buff;
	unsigned int size;
	volatile unsigned int tail;
	volatile unsigned int head;
	volatile unsigned int count;
}Ringbuff_t;

void ringbuff_init(Ringbuff_t *ring,uint32_t *buff,unsigned int size);
bool ringbuff_put_head(Ringbuff_t *ring,uint32_t c);
uint32_t ringbuff_pop_tail(Ringbuff_t *ring);
unsigned int ringbuff_available(Ringbuff_t *ring);
void ringbuff_free(Ringbuff_t *ring);
#ifdef __cplusplus
}
#endif

#endif /* INC_RINGBUFF_H_ */
