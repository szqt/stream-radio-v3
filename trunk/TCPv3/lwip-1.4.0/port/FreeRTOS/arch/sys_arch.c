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

//*****************************************************************************
//
// Include OS functionality.
//
//*****************************************************************************

/* ------------------------ System architecture includes ----------------------------- */
#include "arch/sys_arch.h"

/* ------------------------ lwIP includes --------------------------------- */
#include "lwip/opt.h"

#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/stats.h"

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
#if 0
/*
struct timeoutlist
{
	//struct sys_timeouts timeouts;
	sys_thread_t pid;
};

static struct timeoutlist s_timeoutlist[SYS_THREAD_MAX];

// keep track of how many threads have been created
static u16_t s_nextthread = 0;

int intlevel = 0;
*/
#endif
/*-----------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine:  sys_mbox_new
 *---------------------------------------------------------------------------*
 * Create a new mbox of specified size
 * @param mbox pointer to the mbox to create
 * @param size (miminum) number of messages in this mbox
 * @return ERR_OK if successful, another err_t otherwise
 *---------------------------------------------------------------------------*/
err_t sys_mbox_new(sys_mbox_t *mbox, int size){
	// Create the queue.
	*mbox = xQueueCreate(size, sizeof(void *));

	//LWIP_ASSERT( "sys_mbox_new: mbox != SYS_MBOX_NULL", mbox != SYS_MBOX_NULL );

	// Created succesfully?
	if (*mbox != 0){
#if SYS_STATS
		SYS_STATS_INC(mbox.used);
#endif /* SYS_STATS */
		return ERR_OK;
	}else{
		return ERR_MEM;
	}
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_mbox_free
 *---------------------------------------------------------------------------*
 * Description:
 * Delete an mbox
 * @param mbox mbox to delete
 *---------------------------------------------------------------------------*/
void sys_mbox_free(sys_mbox_t *mbox){
	if( uxQueueMessagesWaiting( *mbox ) ){
		/* Line for breakpoint.  Should never break here! */
//		__asm volatile ( "NOP" );
	}
	vQueueDelete( *mbox );

#if SYS_STATS
	SYS_STATS_DEC(mbox.used);
#endif /* SYS_STATS */
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_mbox_post
 *---------------------------------------------------------------------------*
 * Post a message to an mbox - may not fail
 * -> blocks if full, only used from tasks not from ISR
 * @param mbox mbox to posts the message
 * @param msg message to post (ATTENTION: can be NULL)
 *---------------------------------------------------------------------------*/
void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{

	if (xQueueSend( *mbox, &msg, ( portTickType ) portMAX_DELAY ) == pdTRUE)
	{
#if SYS_STATS
		SYS_STATS_INC(mbox.err);
#endif /* SYS_STATS */
	}

}


#if 0
int sys_mbox_valid(sys_mbox_t *mbox)
{
	return (*mbox == SYS_MBOX_NULL) ? 0 : 1;
}

void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
	*mbox = SYS_MBOX_NULL;
}
#endif


#if 0
int sys_sem_valid(sys_sem_t *sem)
{
	return (*sem == SYS_SEM_NULL) ? 0 : 1;
}


void sys_sem_set_invalid(sys_sem_t *sem)
{
	*sem = SYS_SEM_NULL;
}
#endif
/*---------------------------------------------------------------------------*
 * Routine:  sys_mbox_trypost
 *---------------------------------------------------------------------------*
 * Try to post a message to an mbox - may fail if full or ISR
 * @param mbox mbox to posts the message
 * @param msg message to post (ATTENTION: can be NULL)
 *---------------------------------------------------------------------------*/
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg){

    // Queue must not be full - Otherwise it is an error.
    if(xQueueSend( *mbox, &msg, 0 ) == pdPASS){
    	return ERR_OK;
    }
    else{

#if SYS_STATS
			SYS_STATS_INC(mbox.err);
#endif /* SYS_STATS */
		return ERR_MEM;
	}
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_mbox_fetch
 *---------------------------------------------------------------------------*
 * Wait for a new message to arrive in the mbox
 * @param mbox mbox to get a message from
 * @param msg pointer where the message is stored
 * @param timeout maximum time (in milliseconds) to wait for a message
 * @return time (in milliseconds) waited for a message, may be 0 if not waited
           or SYS_ARCH_TIMEOUT on timeout
 *         The returned time has to be accurate to prevent timer jitter!
 *---------------------------------------------------------------------------*/
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
	void *dummyptr;
	portTickType StartTime, EndTime;
	u32_t Elapsed;

	StartTime = xTaskGetTickCount();

	if (NULL == msg)
	{
		msg = &dummyptr;
	}

	if (timeout != 0)
	{
		if (pdTRUE == xQueueReceive(*mbox, &(*msg), timeout/portTICK_RATE_MS))
		{
			// Got message.  Calculate time it took
			EndTime = xTaskGetTickCount();
			Elapsed = portTICK_RATE_MS*(EndTime - StartTime);
			if (Elapsed == 0)
			{
				Elapsed = portTICK_RATE_MS*1;
			}
			return (Elapsed);
		}
		else
		{
			// timed out blocking for message
			*msg = NULL;
			return SYS_ARCH_TIMEOUT;
		}
	}
	else
	{
		// block forever for a message.
		if (pdTRUE == xQueueReceive( mbox, &(*msg), portMAX_DELAY))
		{
#if SYS_STATS
			SYS_STATS_INC(mbox.err);
#endif /* SYS_STATS */
		}

		// Calculate time it took for the message to come in
		EndTime = xTaskGetTickCount();
		Elapsed = portTICK_RATE_MS*(EndTime - StartTime);// TODO: Define a proper macro to count ms

		if (Elapsed == 0)
		{
			Elapsed = portTICK_RATE_MS*1;
		}

		// return time blocked TBD test
		return (Elapsed);
	}
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_mbox_tryfetch
 *---------------------------------------------------------------------------*
 * Wait for a new message to arrive in the mbox
 * @param mbox mbox to get a message from
 * @param msg pointer where the message is stored
 * @param timeout maximum time (in milliseconds) to wait for a message
 * @return 0 (milliseconds) if a message has been received
 *         or SYS_MBOX_EMPTY if the mailbox is empty
 *---------------------------------------------------------------------------*/
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
	void *dummyptr;

	if (msg == NULL)
	{
		msg = &dummyptr;
	}

	if (pdTRUE == xQueueReceive(*mbox, &(*msg), ( portTickType )0))
	{
		return 0;
	}
	else
	{
		return SYS_MBOX_EMPTY;
	}
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_sem_new
 *---------------------------------------------------------------------------*
 * Create a new semaphore
 * @param sem pointer to the semaphore to create
 * @param count initial count of the semaphore
 * @return ERR_OK if successful, another err_t otherwise
 *---------------------------------------------------------------------------*/
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
	portENTER_CRITICAL();
	vSemaphoreCreateBinary(*sem);
		// Means it can't be taken
	if (count == 0){
		xSemaphoreTake(*sem, 1);
	}
	portEXIT_CRITICAL();

	if(*sem != NULL ){
#if SYS_STATS
		SYS_STATS_INC(sem.used);
#endif
		return ERR_OK;
	}else{
#if SYS_STATS
		SYS_STATS_INC(sem.err);
#endif
		return ERR_MEM;
	}
}
/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_sem_wait
 *---------------------------------------------------------------------------*
 * Wait for a semaphore for the specified timeout
 * @param sem the semaphore to wait for
 * @param timeout timeout in milliseconds to wait (0 = wait forever)
 * @return time (in milliseconds) waited for the semaphore
 *         or SYS_ARCH_TIMEOUT on timeout
 *---------------------------------------------------------------------------*/
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
	portTickType StartTime, EndTime;
	u32_t Elapsed;

	StartTime = xTaskGetTickCount();

	if (timeout != 0)
	{
		if ( xSemaphoreTake( *sem, timeout/portTICK_RATE_MS ) == pdTRUE) // TODO: Define a proper macro to count ms
		{
			// Return time blocked.
			EndTime = xTaskGetTickCount();
			Elapsed = portTICK_RATE_MS*(EndTime - StartTime); // TODO: Define a proper macro to count ms
			if (Elapsed == 0)
			{
				Elapsed = 1*portTICK_RATE_MS;
			}
			return (Elapsed);
		}
		else
		{
			// Timed out
			return SYS_ARCH_TIMEOUT;
		}
	}
	else
	{
		// must block without a timeout
		if (xSemaphoreTake( *sem, portMAX_DELAY ) != pdTRUE)
		{
#if SYS_STATS
			SYS_STATS_INC(sem.err);
#endif /* SYS_STATS */
		}

		// Return time blocked.
		EndTime = xTaskGetTickCount();
		Elapsed = portTICK_RATE_MS*(EndTime - StartTime);// TODO: Define a proper macro to count ms
		if (Elapsed == 0)
		{
			Elapsed = portTICK_RATE_MS*1;
		}

		// return time blocked
		return (Elapsed);
	}
}

/** Create a new mutex
 * @param mutex pointer to the mutex to create
 * @return a new mutex */
err_t sys_mutex_new(sys_mutex_t *mutex) {

	*mutex = xSemaphoreCreateMutex();

	if( *mutex != NULL ){
#if SYS_STATS
		SYS_STATS_INC(mutex.used);
#endif
		return ERR_OK;
	} else {
#if SYS_STATS
		SYS_STATS_INC(mutex.err);
#endif
	}
	return ERR_MEM;
}

/** Lock a mutex
 * @param mutex the mutex to lock */
void sys_mutex_lock(sys_mutex_t *mutex)
{
	xSemaphoreTake(*mutex, portMAX_DELAY);
}

/** Unlock a mutex
 * @param mutex the mutex to unlock */
void sys_mutex_unlock(sys_mutex_t *mutex)
{
	xSemaphoreGive(*mutex);
}

/** Delete a semaphore
 * @param mutex the mutex to delete */
void sys_mutex_free(sys_mutex_t *mutex)
{
#if SYS_STATS
	SYS_STATS_DEC(mutex.used);
#endif /* SYS_STATS */
	vQueueDelete(*mutex);
}


/*---------------------------------------------------------------------------*
 * Routine:  sys_sem_signal
 *---------------------------------------------------------------------------*
 * Signals a semaphore
 * @param sem the semaphore to signal
 *---------------------------------------------------------------------------*/
void sys_sem_signal(sys_sem_t * sem)
{
	//LWIP_ASSERT( "sys_sem_signal: sem != SYS_SEM_NULL", sem != SYS_SEM_NULL );
	xSemaphoreGive(*sem);
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_sem_free
 *---------------------------------------------------------------------------*
 * Delete a semaphore
 * @param sem semaphore to delete
 *---------------------------------------------------------------------------*/
void sys_sem_free(sys_sem_t * sem)
{
	//LWIP_ASSERT( "sys_sem_free: sem != SYS_SEM_NULL", sem != SYS_SEM_NULL );

#if SYS_STATS
	SYS_STATS_DEC(sem.used);
#endif /* SYS_STATS */

	vQueueDelete(*sem);
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_init
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize sys arch
 *---------------------------------------------------------------------------*/
void sys_init(void)
{
#if 0
	int i;

	// Initialize the the per-thread sys_timeouts structures
	// make sure there are no valid pids in the list
	for (i = 0; i < SYS_THREAD_MAX; i++)
	{
		s_timeoutlist[i].pid = SYS_THREAD_NULL;
	//	s_timeoutlist[i].timeouts.next = NULL;
	}

	// keep track of how many threads have been created
	s_nextthread = 0;
#endif
}

u32_t sys_now(void)
{
	portTickType TickTime;
	TickTime = xTaskGetTickCount();
	return portTICK_RATE_MS*TickTime;
}

u32_t sys_jiffies(void)
{
	portTickType TickTime;
	TickTime = xTaskGetTickCount();
	return portTICK_RATE_MS*TickTime;
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_timeouts
 *---------------------------------------------------------------------------*
 * Description:
 *      Returns a pointer to the per-thread sys_timeouts structure. In lwIP,
 *      each thread has a list of timeouts which is represented as a linked
 *      list of sys_timeout structures. The sys_timeouts structure holds a
 *      pointer to a linked list of timeouts. This function is called by
 *      the lwIP timeout scheduler and must not return a NULL value.
 *
 *      In a single threaded sys_arch implementation, this function will
 *      simply return a pointer to a global sys_timeouts variable stored in
 *      the sys_arch module.
 * Outputs:
 *      sys_timeouts *          -- Pointer to per-thread timeouts.
 *---------------------------------------------------------------------------*/
#if 0
struct sys_timeouts *sys_arch_timeouts(void)
{
	int i;
	xTaskHandle pid;
	struct timeoutlist *tl;

	pid = xTaskGetCurrentTaskHandle();

	for (i = 0; i < s_nextthread; i++)
	{
		tl = &(s_timeoutlist[i]);
		if (tl->pid == pid)
		{
		//	return &(tl->timeouts);
		}
	}

	// Error
	return NULL;
}
#endif
/*---------------------------------------------------------------------------*
 * Routine:  sys_thread_new
 *---------------------------------------------------------------------------*
 * Description:
 *      Starts a new thread with priority "prio" that will begin its
 *      execution in the function "thread()". The "arg" argument will be
 *      passed as an argument to the thread() function. The id of the new
 *      thread is returned. Both the id and the priority are system
 *      dependent.
 * Inputs:
 *      char *name              -- Name of thread
 *      void (* thread)(void *arg) -- Pointer to function to run.
 *      void *arg               -- Argument passed into function
 *      int stacksize           -- Required stack amount in bytes
 *      int prio                -- Thread priority
 * Outputs:
 *      sys_thread_t            -- Pointer to per-thread timeouts.
 *---------------------------------------------------------------------------*/
sys_thread_t sys_thread_new(const char *name, void(* thread)(void *arg), void *arg, int stacksize, int prio)
{
	sys_thread_t CreatedTask = SYS_THREAD_NULL;

#if 0
	T_uezError result;
	if (s_nextthread < SYS_THREAD_MAX)
	{
#endif
		// Prevent the real time kernel swapping out the task.
		vPortEnterCritical();

		if( xTaskCreate( thread, (const signed char *)name, stacksize, arg, prio, &CreatedTask) != pdPASS )
		{
			CreatedTask = SYS_THREAD_NULL;
		}

		//LWIP_ASSERT( "sys_thread_new: result != UEZ_ERROR_NONE", UEZ_ERROR_NONE == result );
#if 0
		if (UEZ_ERROR_NONE == result)
		{

		// For each task created, store the task handle (pid) in the timers array.
			// This scheme doesn't allow for threads to be deleted
			s_timeoutlist[s_nextthread++].pid = CreatedTask;
		}
		else
		{
			CreatedTask = 0;
		}

	}
#endif
	// The operation is complete.  Restart the kernel.
	vPortExitCritical();

	return CreatedTask;

}

/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_protect
 *---------------------------------------------------------------------------*
 * Description:
 *      This optional function does a "fast" critical region protection and
 *      returns the previous protection level. This function is only called
 *      during very short critical regions. An embedded system which supports
 *      ISR-based drivers might want to implement this function by disabling
 *      interrupts. Task-based systems might want to implement this by using
 *      a mutex or disabling tasking. This function should support recursive
 *      calls from the same task or interrupt. In other words,
 *      sys_arch_protect() could be called while already protected. In
 *      that case the return value indicates that it is already protected.
 *
 *      sys_arch_protect() is only required if your port is supporting an
 *      operating system.
 * Outputs:
 *      sys_prot_t              -- Previous protection level (not used here)
 *---------------------------------------------------------------------------*/
sys_prot_t sys_arch_protect(void)
{
	vPortEnterCritical();
	return 1;
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_unprotect
 *---------------------------------------------------------------------------*
 * Description:
 *      This optional function does a "fast" set of critical region
 *      protection to the value specified by pval. See the documentation for
 *      sys_arch_protect() for more information. This function is only
 *      required if your port is supporting an operating system.
 * Inputs:
 *      sys_prot_t              -- Previous protection level (not used here)
 *---------------------------------------------------------------------------*/
void sys_arch_unprotect(sys_prot_t pval)
{
	(void) pval;
	vPortExitCritical();
}

/*
 * Prints an assertion messages and aborts execution.
 */
void sys_assert(const char *msg)
{
	(void) msg;

	for (;;)
	{
	}
}
/*-------------------------------------------------------------------------*
 * End of File:  sys_arch.c
 *-------------------------------------------------------------------------*/

