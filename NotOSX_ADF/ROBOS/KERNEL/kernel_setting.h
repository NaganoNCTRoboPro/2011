#ifndef __KERNEL_SETTING_H__
#define __KERNEL_SETTING_H__

// サポートするタスクの優先度数
#define TASK_PRIORITY_NUM		(4)

// IdleTaskのスタックサイズ（OSの初期化処理にも使用）
#define IDLETASK_STACK_SIZE		(100)

// サポートするタスクの最大数
#define MAX_TASK_NUM		(16)

#define HIGHEST_TASK_PRIORITY	(0)
#define LOWEST_TASK_PRIORITY	(TASK_PRIORITY_NUM - 1)

#endif
