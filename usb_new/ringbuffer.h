#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

typedef struct{
	unsigned char	*buf;
	unsigned int	rp;
	unsigned int	wp;
	unsigned int	size;
	unsigned char	empty;
} RingBuffer;

extern void RingInit(RingBuffer *ring, unsigned char *buf, unsigned int size);
extern int RingPut(RingBuffer *ring, unsigned char value);
extern int RingGet(RingBuffer *ring, unsigned char *value);
extern int isRingEmpty(RingBuffer *ring);

#define TRUE	1
#define FALSE	0


#endif
