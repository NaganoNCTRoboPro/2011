#ifndef __KERNEL_H__
#define __KERNEL_H__

#include <stddef.h>
#include <stdbool.h>

// ç≈à´ÇÃèÍçá: Sleep(11) + SystemTimerHandler(5)
#define KERNEL_RESERVED_STACK_SIZE	(80)
#define DEFAULT_STACK_SIZE (200)

typedef unsigned char Stack;
typedef Stack * StackPointer;
typedef unsigned short StackSize;

typedef unsigned char TaskID;
typedef unsigned char TaskPriority;
typedef unsigned char TaskStatus;
typedef void (*TaskPointer)(void);

struct st_tcb {
	TaskID			ID;
	TaskPriority	Priority;
	TaskStatus		Status;
	TaskPointer		pTask;
	StackPointer	pStack;
	StackSize		StackSize;
	struct st_tcb	*NextTask;
};

typedef struct st_tcb TCB;

enum EnumTaskStatus {
	WAIT,
	READY,
	RUN
};

enum EnumTaskAttribute {
	IDLE_TASK,
	NORMAL_TASK
};

#define Dispatch()	_dispatch()

extern bool AddTaskList(TCB *task);
extern void ReadyTask(TCB *task);
extern void WaitTask(TCB *task);
extern void Sleep(unsigned int sleep_time);
extern void ChangeTaskPriority(TCB *task, TaskPriority priority);
extern void KernelInit(void);
extern void _Dispatch(void);
extern void IdleTask(void);

//extern void _Dispatch(void) __attribute__ ((interrupt_handler));
//extern void SystemTimerHandler(void) __attribute__ ((interrupt_handler));

#endif
