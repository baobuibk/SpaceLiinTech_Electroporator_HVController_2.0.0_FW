// APP HEADER //
#include "app.h"

#define MY_GPIO_H_
#include "stm32f4_header.h"
#include "cap_controller_task.h"
#include "uart_driver.h"

//UART_Driver_t*		CMD_line_handle;
//UART_Driver_t  		DEBUG_UART;
//uint8_t               g_DEBUG_UART_TX_buffer[512];
//uint8_t               g_DEBUG_UART_RX_buffer[64];


static void Status_Led(void*);
int test;

#define         SCHEDULER_TASK_COUNT  5
uint32_t        g_ui32SchedulerNumTasks = SCHEDULER_TASK_COUNT;
tSchedulerTask 	g_psSchedulerTable[SCHEDULER_TASK_COUNT] =
                {
                        {
                                &Cap_Controller_Charge_Task,
                                (void *) 0,
                                2,                          //call every 248us
                                0,			                //count from start
                                true		                //is active
                        },
                        {
                                &Cap_Controller_Monitor_Task,
                                (void *) 0,
                                10,                        //call every 248us
                                0,			                //count from start
                                true		                //is active
                        },
                        {
                                &Calib_Task,
                                (void *) 0,
                                50,                         //call every 500us
                                0,                          //count from start
                                false                       //is active
                        },
                        {
                                &Debug_CMD_Line_Task,
                                (void *) 0,
                                50,                         //call every 500us
                                0,                          //count from start
                                true                       //is active
                        },
                        {
                                &Status_Led,
                                (void *) 0,
                                100000,                          //call every 248us
                                0,			                //count from start
                                true		                //is active
                        },

                };

void App_Main(void)
{
    CMD_Line_Task_Init();
    Cap_Controller_Init();
    SchedulerInit(10000);

    while (1)
    {

        SchedulerRun();
    }
}

static void Status_Led(void*)
{

}
