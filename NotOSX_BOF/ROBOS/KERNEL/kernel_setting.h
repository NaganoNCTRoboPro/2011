#ifndef __KERNEL_SETTING_H__
#define __KERNEL_SETTING_H__

// �T�|�[�g����^�X�N�̗D��x��
#define TASK_PRIORITY_NUM		(4)

// IdleTask�̃X�^�b�N�T�C�Y�iOS�̏����������ɂ��g�p�j
#define IDLETASK_STACK_SIZE		(100)

// �T�|�[�g����^�X�N�̍ő吔
#define MAX_TASK_NUM		(16)

#define HIGHEST_TASK_PRIORITY	(0)
#define LOWEST_TASK_PRIORITY	(TASK_PRIORITY_NUM - 1)

#endif
