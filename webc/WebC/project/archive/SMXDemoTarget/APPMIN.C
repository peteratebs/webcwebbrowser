/*
* APPMIN.C                                                  Version 3.7.1
*
* Minimal ARM Protosystem application. Use instead of APP.C to use
* less of smx and reduce code and RAM size.
*
* Replace this with your application files.
*
* Copyright (c) 1994-2009 Micro Digital Associates, Inc.
*               All Rights Reserved.
*
* Author: David Moore
*
***********************************************************************/

#include "smx.h"
#include "core.h"
#ifdef SMXNS_DEMO
#include "smxns.h"
#endif

#if (DEVICE_CONSOLE_IN) && (!MIN_RAM)
#define USE_OPCON_TASK 1
#else
#define USE_OPCON_TASK 0
#endif

#ifdef __cplusplus
 extern "C" {
#endif

#if (USE_OPCON_TASK)
TCB_PTR opcon;         /* operation control task (for kbd) */
void opcon_main(void);
#endif

#ifdef SMXFLOG_DEMO
void fldemo_init(void);
void fldemo_exit(void);
#endif

#ifdef SMXFFS_DEMO
void ffsdemo_init(void);
void ffsdemo_exit(void);
#endif

#ifdef SMXFS_DEMO
void fsdemo_init(void);
void fsdemo_exit(void);
#endif

#ifdef SMXNS_DEMO
void nsdemo_init(void);
void nsdemo_exit(void);
extern int nsdemo_count;
#endif

#ifdef SMXPP_DEMO
void sppdemo_init(void);
void sppdemo_exit(void);
#endif

#ifdef SMXSSL_DEMO
void ssldemo_init(void);
void ssldemo_exit(void);
#endif

#ifdef SMXUSB_DEMO
void usbdemo_init(void);
void usbdemo_exit(void);
#endif

#ifdef SMXUSBD_DEMO
void usbddemo_init(void);
void usbddemo_exit(void);
#endif

#ifdef SMXUSBO_DEMO
void usbodemo_init(void);
void usbodemo_exit(void);
extern PXCB_PTR usbo_demo_pipe;
#endif

#ifdef SMXWIFI_DEMO
void wifidemo_init(void);
void wifidemo_exit(void);
#endif

TMCB_PTR LED_lsr_tmr;
VOID_PTR LED_lsr_h;
void LED_lsr(void);
#if (defined(SMXNS_DEMO) && SNS_PROTO_WEBS)
#define LED_LSR_PERIOD (cf.sec / 10)
#else
#define LED_LSR_PERIOD (cf.sec)
#endif

#ifdef __cplusplus
}
#endif


/***** APPLICATION INITIALIZATION
* This is a typical application initialization function. Create
* something equivalent.
**********************************************************************/

void _cdecl appl_init(void)
{
   wr_string(1,1,WHITE,BLACK,!BLINK,"Protosystem for ARM  (Built "__DATE__" "__TIME__")");
   wr_string(1,2,WHITE,BLACK,!BLINK,"Watch LEDs");

  #if (USE_OPCON_TASK)
   opcon = create_task(opcon_main, MAX, 0);
   startx(opcon);
   BUILD_HT(opcon, "opcon");
   wr_string(60,1,WHITE,BLACK,!BLINK,"Esc to Exit");
  #endif

   /* Create pseudo handle to log LED_lsr runs in event buffer. */
   LED_lsr_h = create_pseudo_handle();
   BUILD_HT(LED_lsr_h, "LED_lsr");

   /* Start timer to invoke LED lsr. If board has no LED's, it does nothing. */
  #ifdef __cplusplus
   start_timer(LED_lsr_tmr, (LSR_PTR)LED_lsr, 0, 0, LED_LSR_PERIOD);
  #else
   start_timer(&LED_lsr_tmr, (LSR_PTR)LED_lsr, 0, 0, LED_LSR_PERIOD);
  #endif
   BUILD_HT(LED_lsr_tmr, "LED_lsr_tmr");

   bspDemoInit();    /* entry to BSP demo */

  #if defined(SMXPP_DEMO)
   sppdemo_init();   /* entry to smx++ demo */
  #endif
  #if defined(SMXNS_DEMO)
   nsdemo_init();    /* entry to smxNS demo */
  #endif
  #if defined(SMXSSL_DEMO)
   ssldemo_init();   /* entry to smxSSL demo */
  #endif
  #if defined(SMXFLOG_DEMO)
   fldemo_init();    /* entry to smxFLog demo */
  #endif
  #if defined(SMXFFS_DEMO)
   ffsdemo_init();   /* entry to smxFFS demo */
  #endif
  #if defined(SMXFS_DEMO)
   fsdemo_init();    /* entry to smxFS demo */
  #endif
  #if defined(SMXUSB_DEMO)
   usbdemo_init();   /* entry to smxUSB demo */
  #endif
  #if defined(SMXUSBD_DEMO)
   usbddemo_init();  /* entry to smxUSBD demo */
  #endif
  #if defined(SMXUSBO_DEMO)
   usbodemo_init();  /* entry to smxUSBO demo */
  #endif
  #if defined(SMXWIFI_DEMO)
   wifidemo_init();  /* entry to smxWiFi demo */
  #endif
}


void appl_exit(void)
{
  #if defined(SMXWIFI_DEMO)
   wifidemo_exit();  /* cleanup for smxWiFi demo */
  #endif
  #if defined(SMXUSBO_DEMO)
   usbodemo_exit();  /* cleanup for smxUSBO demo */
  #endif
  #if defined(SMXUSBD_DEMO)
   usbddemo_exit();  /* cleanup for smxUSBD demo */
  #endif
  #if defined(SMXUSB_DEMO)
   usbdemo_exit();   /* cleanup for smxUSB demo */
  #endif
  #if defined(SMXFS_DEMO)
   fsdemo_exit();    /* cleanup for smxFS demo */
  #endif
  #if defined(SMXFFS_DEMO)
   ffsdemo_exit();   /* cleanup for smxFFS demo */
  #endif
  #if defined(SMXFLOG_DEMO)
   fldemo_exit();    /* cleanup for smxFLog demo */
  #endif
  #if defined(SMXSSL_DEMO)
   ssldemo_exit();   /* cleanup for smxSSL demo */
  #endif
  #if defined(SMXNS_DEMO)
   nsdemo_exit();    /* cleanup for smxNS demo */
  #endif
  #if defined(SMXPP_DEMO)
   sppdemo_exit();   /* cleanup for smx++ demo */
  #endif

   bspDemoExit();    /* cleanup for BSP demo */
}


#if (USE_OPCON_TASK)

/***** OPERATION CONTROL
* This task assumes a UART driver puts ASCII chars into op_pipe from
* a terminal keyboard.
**********************************************************************/

void _cdecl opcon_main(void)
{
   char  key;

   while(TRUE)
   {
     #if DEVICE_CONSOLE_IN_USES_INT
      key = pget_char(op_pipe, INF);   /* get from op_pipe, which is filled by lsr due to isr */
     #else
      key = get_kbd_char_polled();     /* poll; don't have interrupt-driven UART driver */
     #endif

      if (key == '\0')
         continue;

      if(key == Esc)
        #if MIN_RAM
         exitx_main(OK);  /* call its code directly */
        #else
         start_par(exitx, OK);
        #endif

     #if defined(SMXUSBO_DEMO)
      else                             /* send chars to OTG task */
         pput_char(key, usbo_demo_pipe, INF);
     #endif
   }
}
#endif /* USE_OPCON_TASK */


/***** LED LSR
* Display count or pattern on LED's. A board could have one or more
* of the different types of LED's, or none. This is done as an lsr
* in this minimal app to save RAM (saves a task stack). In the normal
* app.c, this is implemented as a task.
**********************************************************************/

#include "led.h"

/* select 1 pattern */

#if BOARD_LED_NUM_7SEG
#define LED_7SEG_COUNT      1
#define LED_7SEG_SNAKE      0
#define LED_7SEG_CIRCLE     0
#endif

#if BOARD_LED_NUM_IN_ROW
#define LED_ROW_COUNT       1
#define LED_ROW_ALTERNATE   0
#define LED_ROW_FLASH       0
#define LED_ROW_SCAN        0
#endif

#if BOARD_LED_NUM_7SEG
#if LED_7SEG_SNAKE
const dword LED_SnakeSegs[8] =
   {LED_SEG_A, LED_SEG_B, LED_SEG_G, LED_SEG_E,
    LED_SEG_D, LED_SEG_C, LED_SEG_G, LED_SEG_F};
#elif LED_7SEG_CIRCLE
const dword LED_CircleSegs[6] =
   {LED_SEG_A, LED_SEG_B, LED_SEG_C, LED_SEG_D,
    LED_SEG_E, LED_SEG_F};
#endif
#endif


void LED_lsr(void)
{
   static u32 ctr = 1;  /* start at 1 because wait is before this runs, unlike LED_task which is after */
#if (defined(SMXNS_DEMO) && SNS_PROTO_WEBS)
   static u32 next_count = 0;
#endif

   EVB_LOG_LSR(LED_lsr_h, 0, 0);  /* Log lsr entry in Event Buffer. The last 2 pars are dwords that the user can use to display variables. */

  #if (defined(SMXNS_DEMO) && SNS_PROTO_WEBS)
   /* This is for smxNS AJAX demo. Slider controls how often LEDs change.
      Since this is an lsr, we can't do it like in LED_task_main() in app.c. */
   if (etime >= next_count)
      next_count += cf.sec * nsdemo_count / 1000;
   else
      return;
  #endif

 #if BOARD_LED_NUM_IN_ROW
  #if LED_ROW_COUNT
   WriteLEDs(ctr);

  #elif LED_ROW_ALTERNATE
   WriteLEDs((ctr & 1) ? LED_PATTERN_ODD : LED_PATTERN_EVEN);

  #elif LED_ROW_FLASH
   WriteLEDs((ctr & 1) ? LED_PATTERN_ALL : LED_PATTERN_NONE);

  #elif LED_ROW_SCAN
   WriteLEDs(1 << (ctr % BOARD_LED_NUM_IN_ROW));
  #endif
 #endif /* BOARD_LED_NUM_IN_ROW */

 #if BOARD_LED_NUM_7SEG
  #if LED_7SEG_COUNT
   #if (BOARD_LED_NUM_7SEG == 2)
// Write7SegNum(ctr % 100);  /* 0 to 99 */
   Write7SegNum(ctr % 256);  /* 0 to FF */
   #else
// Write7SegNum(ctr % 10);   /* 0 to 9 */
   Write7SegNum(ctr % 16);   /* 0 to F */
   #endif

  #elif LED_7SEG_SNAKE
   Write7Seg(LED_SnakeSegs[ctr % 8]);

  #elif LED_7SEG_CIRCLE
   Write7Seg(LED_CircleSegs[ctr % 6]);
  #endif

  #else
 #endif /* BOARD_LED_NUM_7SEG */

   ctr++;

   EVB_LOG_LSR_RETURN(LED_lsr_h, 0, 0);  /* Log lsr exit in Event Buffer. The last 2 pars are dwords that the user can use to display variables. */
}
