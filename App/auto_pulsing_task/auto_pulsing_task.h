/*
 * auto_pulsing_task.h
 *
 *  Created on: Jul 9, 2026
 *      Author: PV
 */

#ifndef AUTO_PULSING_TASK_AUTO_PULSING_TASK_H_
#define AUTO_PULSING_TASK_AUTO_PULSING_TASK_H_

typedef enum{
    AP_MODE_OFF = 0,
    AP_MODE_ON
}Auto_Pulsing_Mode;

extern Auto_Pulsing_Mode AP_Mode ;

void Auto_Pulsing_Trigger_ISR_Handle(void);

#endif /* AUTO_PULSING_TASK_AUTO_PULSING_TASK_H_ */
