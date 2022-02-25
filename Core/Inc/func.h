#ifndef __FUNC_H__
#define __FUNC_H__

void Update_State(void);

void Run(void);

// 擦除一个flash page
void UserErase(void);

// 读取flash数据
void UserRead(void);

// 写入数据
void UserWrite(void);

#endif // __FUNC_H__