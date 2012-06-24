/*
 * Tasks.h
 *
 *  Created on: 02-06-2012
 *      Author: ja
 */

#ifndef TASKS_H_
#define TASKS_H_

void 			vWinMngTask(void * pvParameters);
void 			vHeartbeatTask (void * pvParameters);
void 			vVsTask(void * pvParameters);
void 			vTouchPanelTask(void * pvParameters);
void 			vDHCP_TimerCallback(xTimerHandle pxTimer);

#endif /* TASKS_H_ */
