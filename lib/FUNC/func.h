#ifndef __FUNC_H__
#define __FUNC_H__

#include "para.h"

typedef enum {
    READY = 0,
    TRIGGED,
    FILTERING,
    TIMEOUT
} CH_STATE;


//
typedef enum {
    SYS_MODE_INI,
    SYS_MODE_NORMAL,
    SYS_MODE_FACTORY_LOAD,
    SYS_MODE_REMOTE,
    SYS_MODE_DEBUG
} SYS_MODE;

void Update_State(void);

void Run(void);

SYS_MODE SystemModeSelect(void);

// 擦除一个flash page
void UserErase(void);

// 读取flash数据
void UserRead(void);

// 写入数据
void UserWrite(void);

#endif // __FUNC_H__