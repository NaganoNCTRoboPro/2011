#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

typedef struct{
	unsigned char	*buf;
	unsigned int	rp;
	unsigned int	wp;
	unsigned int	size;
	unsigned char	empty;
} RingBuffer;

extern __inline__ void RingInit(RingBuffer *ring, unsigned char *buf, unsigned int size);
extern __inline__ int RingPut(RingBuffer *ring, unsigned char value);
extern __inline__ int RingGet(RingBuffer *ring, unsigned char *value);
extern __inline__ int isRingEmpty(RingBuffer *ring);

#define TRUE	1
#define FALSE	0

__inline__ void RingInit(RingBuffer *ring, unsigned char *buf, unsigned int size)
{
	ring->buf = buf;
	ring->size = size;
	ring->rp = 0;
	ring->wp = 0;
	ring->empty = TRUE;
}

__inline__ int RingPut(RingBuffer *ring, unsigned char value)
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

__inline__ int RingGet(RingBuffer *ring, unsigned char *value)
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

__inline__ int isRingEmpty(RingBuffer *ring)
{
   return ring->empty;
}

#endif
