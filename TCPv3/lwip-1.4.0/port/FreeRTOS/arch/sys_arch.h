/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __ARCH_SYS_ARCH_H__
#define __ARCH_SYS_ARCH_H__

//*****************************************************************************
//
// Include OS functionality.
//
//*****************************************************************************
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* ------------------------ Defines --------------------------------------- */
#define SYS_MBOX_NULL           (xQueueHandle)0
#define SYS_THREAD_NULL         NULL
#define SYS_SEM_NULL            (xSemaphoreHandle)0
#define SYS_MUTEX_NULL          (xSemaphoreHandle)0
#define SYS_DEFAULT_THREAD_STACK_DEPTH	configMINIMAL_STACK_SIZE


/* This is the number of threads that can be started with sys_thread_new() */
#define SYS_THREAD_MAX 4

#define LWIP_TASK_MAX    5

/* Message queue constants. */
#define archMESG_QUEUE_LENGTH	( 6 )
#define archPOST_BLOCK_TIME_MS	( ( unsigned long ) 10000 )

/* ------------------------ Type definitions ------------------------------ */
typedef xSemaphoreHandle sys_sem_t;
typedef xSemaphoreHandle sys_mutex_t;
typedef xQueueHandle sys_mbox_t;
typedef xTaskHandle sys_thread_t;
typedef int  sys_prot_t;

/** Check if an mbox is valid/allocated: return 1 for valid, 0 for invalid */
#define sys_mbox_valid(mbox) (*mbox != SYS_MBOX_NULL )
/** Set an mbox invalid so that sys_mbox_valid returns 0 */
#define sys_mbox_set_invalid(mbox) (*mbox = SYS_MBOX_NULL)

/** Check if a sempahore is valid/allocated: return 1 for valid, 0 for invalid */
#define sys_sem_valid(mbox) (*mbox != SYS_SEM_NULL)
/** Set a semaphore invalid so that sys_sem_valid returns 0 */
#define sys_sem_set_invalid(mbox) (*mbox = SYS_SEM_NULL)

/** Check if a mutex is valid/allocated: return 1 for valid, 0 for invalid */
#define sys_mutex_valid(mutex) (*mutex != SYS_MUTEX_NULL)
/** Set a mutex invalid so that sys_mutex_valid returns 0 */
#define sys_mutex_set_invalid  (*mutex = SYS_MUTEX_NULL)

//#define sys_msleep(x)	vTaskDelay(x)  NO_SYS = 0 nie potrzeba tej definicji, LWIP realizuje sobie opóźnienie na semaforze

#endif /* __ARCH_SYS_ARCH_H__ */

