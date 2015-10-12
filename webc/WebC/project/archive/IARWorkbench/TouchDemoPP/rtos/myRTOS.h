
#ifndef MY_RTOS_H_INCLUDED        /* Avoid multiple inclusion          */
#define MY_RTOS_H_INCLUDED


typedef struct _MailXY_t {
  Int32U X;
  Int32U Y;
} MailXYType_t, *pMailXYType_t;

extern Int32U SDRAM_BASE_ADDR;
extern FontType_t Terminal_6_8_6;
extern FontType_t Terminal_9_12_6;
extern FontType_t Terminal_18_24_12;

void OS_InitTimer(void);
void TouchGetTask (void);

extern OS_STACKPTR int StackScreenUpdate[128], StackTick[128];          /* Task stacks */
extern OS_TASK TCBScreenUpdate, TCBTick;                        /* Task-control-blocks */

extern OS_MAILBOX MailXY;
extern char MailXYBuffer[6*sizeof(MailXYType_t)];


#define EVENT_KEY_TUCHED      (0x0001)
#define EVENT_KEY_PRINT       (0x0002)
#define EVENT_KEY_NOT_TUCHED  (0x0004)

#endif //MY_RTOS_H_INCLUDED
