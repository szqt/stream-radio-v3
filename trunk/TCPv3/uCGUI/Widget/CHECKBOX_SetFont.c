/*
*********************************************************************************************************
*                                                uC/GUI
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              �C/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : CHECKBOX_SetFont.c
Purpose     : Implementation of CHECKBOX_SetFont
---------------------------END-OF-HEADER------------------------------
*/

#include "CHECKBOX_Private.h"

#if GUI_WINSUPPORT

/*********************************************************************
*
*       Exported routines
*
**********************************************************************
*/
/*********************************************************************
*
*       CHECKBOX_SetFont
*/
void CHECKBOX_SetFont(CHECKBOX_Handle hObj, const GUI_FONT GUI_UNI_PTR * pFont) {
  CHECKBOX_Obj * pObj;
  if (hObj) {
    WM_LOCK();
    pObj = CHECKBOX_H2P(hObj);
    if (pObj->Props.pFont != pFont) {
      pObj->Props.pFont = pFont;
      WM_Invalidate(hObj);
    }
    WM_UNLOCK();
  }
}

#else                            /* Avoid problems with empty object modules */
  void CHECKBOX_SetFont_C(void);
  void CHECKBOX_SetFont_C(void) {}
#endif
