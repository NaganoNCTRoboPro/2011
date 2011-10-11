#include "ringbuffer.h"

void RingInit(RingBuffer *ring, unsigned char *buf, unsigned int size)
{
	ring->buf = buf;
	ring->size = size;
	ring->rp = 0;
	ring->wp = 0;
	ring->empty = TRUE;
}

int RingPut(RingBuffer *ring, unsigned char value)
{
	if( !ring->empty && ring->rp == ring->wp ){
		return FALSE;
	}
	ring->buf[ring->wp] = value;
	ring->wp++;
	if( ring->wp == ring->size ) ring->wp = 0;

	ring->empty = FALSE;
	
	return TRUE;
}

int RingGet(RingBuffer *ring, unsigned char *value)
{
	if( !ring->empty ){
		*value = ring->buf[ring->rp];
		ring->rp++;
		if( ring->rp == ring->size ) ring->rp = 0;
		if( ring->rp == ring->wp )	ring->empty = TRUE;

		return TRUE;
	}else{			// リングバッファが空のとき
		*value = 0;
		return FALSE;
	}
}

int isRingEmpty(RingBuffer *ring)
{
   return ring->empty;
}
