/*
 * calibration.h
 *
 *  Created on: 23-06-2012
 *      Author: ja
 */

#ifndef CALIBRATION_H_
#define CALIBRATION_H_
/* Default calibration parameters */
#define	AX0			15
#define AX1			344
#define AY0			224
#define AY1			15
#define AX0PHY		475
#define AX1PHY		3743
#define AY0PHY		3477
#define AY1PHY		527

void _ExecCalibration(void);
void _DefaultCalibration(void);

#endif /* CALIBRATION_H_ */
