// //
// // Created by msol0v on 23.07.2026.
// //
//
// #include "pwm_task.h"
//
// #include <stdio.h>
//
// osThreadId_t pwmTaskHandle;
//
// const osThreadAttr_t pwmTask_attributes = {
//     .name = "pwmTask",
//     .stack_size = 512 * 4,
//     .priority = (osPriority_t) osPriorityNormal,
//   };
//
// void PWM_Task(void *argument)
// {
//     //PWM_Message_t msg;
//     MotorCommand_t cmd;
//
//     for(;;)
//     {
//         if(osMessageQueueGet(pwmQueueHandle,&msg,NULL,osWaitForever) != osOK)
//         {
//             continue;
//         }
//
//         switch(msg.source)
//         {
//             case PWM_SOURCE_MVS:
//
//                 if(controlMode != CONTROL_MODE_MVS)
//                     break;
//
//                 // if(DecodeMVS(msg.width, &cmd))
//                 //     osMessageQueuePut(motorQueueHandle,&cmd,0,0);
//
//
//                 break;
//
//
//             case PWM_SOURCE_PLC:
//
//                 if(controlMode != CONTROL_MODE_PLC)
//                     break;
//
//                 // DecodePLC(msg.width,&cmd);
//
//                 break;
//         }
//     }
// }
//
// void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
// {
//     if(htim->Instance == TIM8 &&
//        htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
//     {
//         PWM_Message_t msg;
//
//         msg.source = PWM_SOURCE_MVS;
//         msg.width = TIM8->CCR2;
//
//         printf("pwm: %d\r\n", msg.width);
//
//         osMessageQueuePut(pwmQueueHandle,&msg,0,0);
//     }
//
//     if(htim->Instance == TIM9 &&
//        htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
//     {
//         PWM_Message_t msg;
//
//         msg.source = PWM_SOURCE_PLC;
//         msg.width = TIM9->CCR2;
//
//         osMessageQueuePut(pwmQueueHandle,&msg,0,0);
//     }
// }
//
// void changePWMsrc(PWM_Source_t source) {
//
// }