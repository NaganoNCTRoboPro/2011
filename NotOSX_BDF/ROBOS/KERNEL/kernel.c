#include "kernel.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "kernel_setting.h"
#include "system_timer.h"
#include <uart.h>

struct sleep_task {
	TCB *task;
	unsigned int sleep_cnt;
};

static TCB *_current_task;
static TCB *_task_ready_queue[TASK_PRIORITY_NUM];
static int _ready_run_task_count[TASK_PRIORITY_NUM];
static struct sleep_task _sleep_task_list[MAX_TASK_NUM];
static TCB _idle_task;
static TCB *_task_list[MAX_TASK_NUM];
Stack _idle_task_stack[IDLETASK_STACK_SIZE];

void IdleTask(void);

void _dispatch(void);

/// <summary>
/// ROBOS�J�[�l���̏���������
/// </summary>
void KernelInit(void)
{
	int i;
	
	// Task List Initialize
	for( i=0; i<TASK_PRIORITY_NUM; i++ ){
		_task_ready_queue[i] = NULL;
		_ready_run_task_count[i] = 0;
	}
	// Task List Initialize
	
	// Sleep Task List Initialize
	for( i=0; i<MAX_TASK_NUM; i++ ){
		_sleep_task_list[i].task = NULL;
		_sleep_task_list[i].sleep_cnt = 0;
		
		_task_list[i] = NULL;
	}
	// Sleep Task List Initialize
	

	// IdleTask Initialize
	_idle_task.ID = 0;
	_idle_task.Status = RUN;
	_idle_task.pTask = IdleTask;
//	_idle_task.pStack = _idle_task_stack + IDLETASK_STACK_SIZE;
	_idle_task.pStack = (Stack*)RAMEND;
	_idle_task.StackSize = IDLETASK_STACK_SIZE;
	_idle_task.NextTask = &_idle_task;

	_task_ready_queue[LOWEST_TASK_PRIORITY] = &_idle_task;
	_ready_run_task_count[LOWEST_TASK_PRIORITY]++;
	_current_task = &_idle_task;
	
	_task_list[_idle_task.ID] = &_idle_task;
	// IdleTask Initialize
	
	// System Timer Initialize
	SystemTimerInit();
	// System Timer Initialize
}

void IdleTask(void)
{
	static unsigned char i;
	static bool flag = false;
	while(1)
	{
		for( i = HIGHEST_TASK_PRIORITY; i <= LOWEST_TASK_PRIORITY;  i++ ){
			if( _ready_run_task_count[i] != 0 && _task_ready_queue[i]->ID != _idle_task.ID){
				flag = true;
				break;
			}
		}
		if(flag){
			flag = false;
			Dispatch();
		}
	}	Dispatch();
}

/// <summary>
/// WAIT��������READY��Ԃ̃^�X�N��TaskList�ɒǉ�����
/// </summary>
/// <param name="task">�Ώ�Task��TCB�ւ̃|�C���^</param>
/// <returns>
/// ����I��: true
/// �s���I��: false
/// </returns>
bool AddTaskList(TCB *task)
{
	TCB	*tmp;
	unsigned char sreg;
	bool ret;

	sreg = SREG;
	cli();

	if( (task->Status == WAIT || task->Status == READY) &&
		task->Priority <= LOWEST_TASK_PRIORITY &&
		_task_list[ task->ID ] == NULL
		){
		
		if( _task_ready_queue[task->Priority] == NULL )
		{
			_task_ready_queue[task->Priority] = task;
			task->NextTask = task;
		}
		else
		{
			tmp = _task_ready_queue[task->Priority]->NextTask;
			_task_ready_queue[task->Priority]->NextTask = task;
			task->NextTask = tmp;
		}
		if( task->Status == READY )
		{
			_ready_run_task_count[task->Priority]++;
		}
		_task_list[ task->ID ] = task;
		 
		 // �X�^�b�N�|�C���^�̑���
//		task->pStack += task->StackSize;
/*		task->pStack--;
		*task->pStack = (Stack)ExitTask;		// PC
*/		task->pStack -= 1;
		*task->pStack = (Stack)(0x00 | (unsigned short)task->pTask);
		task->pStack -= 1;
		*task->pStack = (Stack)((0x00 | (unsigned short)task->pTask)>>8);
		task->pStack -= 33;
		
		ret = true;
	}else{
		ret = false;
	}
	SREG = sreg;
	
	return ret;
}


/// <summary>
/// �f�B�X�p�b�`
/// </summary>
void _dispatch(void)
{
	__asm__ (
		".extern	_dispatch_in\n"
		".global	_dispatch_in_r\n"
			"jmp	_dispatch_in\n"
		"_dispatch_in_r:\n"
	);
	static unsigned char i=0;

	i=0;
	if( _current_task->Status == RUN )	_current_task->Status = READY;
	
	// ���Ɏ��s����^�X�N�̒T��
	for( i = HIGHEST_TASK_PRIORITY; i <= LOWEST_TASK_PRIORITY;  i++ ){
		if( _ready_run_task_count[i] != 0 ){
			unsigned char temp0,temp1;
			unsigned short temp2;
			// �X�^�b�N�|�C���^�̑ޔ�
			__asm__ ("in	%0, 0x3e" : "=r"(temp0));
			__asm__ ("in	%0, 0x3d" : "=r"(temp1));
			temp2 = temp0;
			temp2 <<= 8;
			_current_task->pStack = (Stack*)(temp2 | temp1);

			while( _task_ready_queue[i]->Status != READY ){
				_task_ready_queue[i] = _task_ready_queue[i]->NextTask;
			}
			_current_task = _task_ready_queue[i];
			_task_ready_queue[i] = _task_ready_queue[i]->NextTask;
			
			// �X�^�b�N�|�C���^�̕��A
			temp2 = (unsigned short)_current_task->pStack;
			temp1 = (unsigned char)temp2;
			temp0 = (unsigned char)(temp2>>8);
			__asm__ ("out	0x3e, %0" : : "r"(temp0));
			__asm__ ("out	0x3d, %0" : : "r"(temp1));
			break;
		}
	}	
	
	_current_task->Status = RUN;

	__asm__ (
		".extern	_dispatch_out\n"
		".global	_dispatch_out_r\n"
			"jmp	_dispatch_out\n"
		"_dispatch_out_r:\n"
	);
}

/// <summary>
/// ���ݎ��s����Task���擾
/// <returns>���s��Task��TCB�ւ̃|�C���^</returns>
TCB *GetCurrentTask(void)
{
	TCB *ret;
	unsigned char sreg;

	sreg = SREG;
	cli();
	ret = _current_task;
	SREG = sreg;

	return ret;
}

/// <summary>
/// ID����Task(TCB)���擾
/// </summary>
/// <returns>�Ώ�TCB�ւ̃|�C���^</returns>
TCB *GetTaskByID(TaskID id)
{
	TCB *ret;
	unsigned char sreg;

	sreg = SREG;
	cli();
	if( id < MAX_TASK_NUM )
		ret = _task_list[id];
	else
		ret = NULL;
	SREG = sreg;
	
	return ret;
}

/// <summary>
/// Task��READY��ԂɕύX
/// </summary>
/// <param name="task">�Ώ�Task��TCB�ւ̃|�C���^</param>
__inline__ void ReadyTask(TCB *task)
{
	unsigned char sreg;
	
	if( task->Status != READY || task->Status != RUN ){
		sreg = SREG;
		cli();
		task->Status = READY;
		_ready_run_task_count[ task->Priority ]++;
		SREG = sreg;
	}
}

/// <summary>
/// Task��WAIT��ԂɕύX
/// Current Task���w�肵���ꍇ�̓f�B�X�p�b�`���s��
/// </summary>
/// <param name="task">�Ώ�Task��TCB�ւ̃|�C���^</param>
__inline__ void WaitTask(TCB *task)
{
	unsigned char sreg;
	TCB *ctask;
	
	sreg = SREG;
	cli();
	if( task->Status == READY || task->Status == RUN ){
		task->Status = WAIT;
		_ready_run_task_count[ task->Priority ]--;
	}
	ctask = _current_task;
	SREG = sreg;
	if( task == ctask ){
		Dispatch();
	}
}

/// <summary>
/// Task����莞��WAIT��Ԃɂ���
/// </summary>
/// <param name="sleep_time">�X���[�v����</param>
void Sleep(unsigned int sleep_time)
{
	unsigned char i=0;
	unsigned char sreg;
	
	sreg = SREG;
	cli();
	for( i=0; i<MAX_TASK_NUM; i++ ){
		if( _sleep_task_list[i].task == NULL ){
			if( _current_task->Status == RUN ){
				_current_task->Status = WAIT;
				_ready_run_task_count[ _current_task->Priority ]--;
			}
			_sleep_task_list[i].task = _current_task;
			_sleep_task_list[i].sleep_cnt = sleep_time;
			break;
		}
	}
	SREG = sreg;
	
	Dispatch();
}




/// <summary>
/// 1ms�Ԋu�ŋN������V�X�e���^�C�}�[�n���h��
/// </summary>
ISR(TIMER1_COMPA_vect)
{
	int i;
	
	SystemTimerIntvalProcess();
	for( i=0; i<MAX_TASK_NUM; i++ ){
		if( _sleep_task_list[i].task != NULL ){
			_sleep_task_list[i].sleep_cnt--;
			if( _sleep_task_list[i].sleep_cnt == 0 ){
				ReadyTask(_sleep_task_list[i].task);
				_sleep_task_list[i].task = NULL;
			}
		}
	}
}


__asm__ (
	".extern	_dispatch_in_r\n"
	".global	_dispatch_in\n"
	"_dispatch_in:\n"
		"in		r25, 0x3f\n"	// SREG �̕ۑ�
		"push	r25\n"
		"cli	\n"
		"push	r31\n"
		"push   r30\n"
		"push   r29\n"
		"push   r28\n"
		"push   r27\n"
		"push   r26\n"
	//	"push   r25\n"
		"push	r24\n"
		"push   r23\n"
		"push   r22\n"
		"push   r21\n"
		"push   r20\n"
		"push   r19\n"
		"push   r18\n"
		"push   r17\n"
		"push   r16\n"
		"push   r15\n"
		"push   r14\n"
		"push   r13\n"
		"push   r12\n"
		"push   r11\n"
		"push   r10\n"
		"push   r9\n"
		"push   r8\n"
		"push   r7\n"
		"push   r6\n"
		"push   r5\n"
		"push   r4\n"
		"push   r3\n"
		"push   r2\n"
		"push   r1\n"
		"push   r0\n"
		"jmp	_dispatch_in_r"
);

__asm__ (
	".extern	_dispatch_out_r\n"
	".global _dispatch_out\n"
	"_dispatch_out:\n"
		"pop	r0\n"
		"pop	r1\n"
		"pop	r2\n"
		"pop	r3\n"
		"pop	r4\n"
		"pop	r5\n"
		"pop	r6\n"
		"pop	r7\n"
		"pop	r8\n"
		"pop	r9\n"
		"pop	r10\n"
		"pop	r11\n"
		"pop	r12\n"
		"pop	r13\n"
		"pop	r14\n"
		"pop	r15\n"
		"pop	r16\n"
		"pop	r17\n"
		"pop	r18\n"
		"pop	r19\n"
		"pop	r20\n"
		"pop	r21\n"
		"pop	r22\n"
		"pop	r23\n"
		"pop	r24\n"
	//	"pop	r25\n"
		"pop	r26\n"
		"pop	r27\n"
		"pop	r28\n"
		"pop	r29\n"
		"pop	r30\n"
		"pop	r31\n"
		"pop	r25\n"
		"out	0x3f, r25\n"	// SREG�̕��A
		"jmp	_dispatch_out_r"
);

