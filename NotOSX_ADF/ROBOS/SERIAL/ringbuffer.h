#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

typedef struct{
	unsigned char	*buf;
	unsigned int	rp;
	unsigned int	wp;
	unsigned int	size;
	unsigned char	empty;
} RingBuffer;

void RingInit(RingBuffer *ring, unsigned char *buf, unsigned int size);
int RingPut(RingBuffer *ring, unsigned char value);
int RingGet(RingBuffer *ring, unsigned char *value);
int isRingEmpty(RingBuffer *ring);

#define TRUE	1
#define FALSE	0

#endif
