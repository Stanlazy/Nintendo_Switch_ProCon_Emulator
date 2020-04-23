#ifndef __NS_CON_DEVICE_H__
#define __NS_CON_DEVICE_H__

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "usbd_ctlreq.h"
#include "usbd_ioreq.h"
#include "usbd_core.h"
#include "usbd_def.h"

extern USBD_HandleTypeDef NSCON_USB_Handle;

typedef struct NSCON_ButtonList_TypeDef_ {
	uint8_t buttA;
	uint8_t buttB;
	uint8_t buttX;
	uint8_t buttY;
	uint8_t buttLPadL;
	uint8_t buttLPadR;
	uint8_t buttLPadU;
	uint8_t buttLPadD;
	uint8_t buttMinus;
	uint8_t buttPlus;
	uint8_t buttHome;
	uint8_t buttShare;
	uint8_t buttL1;
	uint8_t buttL2;
	uint8_t buttR1;
	uint8_t buttR2;
	uint8_t buttLStickM;
	int16_t analLStickX;
	int16_t analLStickY;
	uint8_t buttRStickM;
	int16_t analRStickX;
	int16_t analRStickY;
} NSCON_ButtonList_TypeDef;

typedef enum
{
  HID_IDLE = 0,
  HID_BUSY,
}
HID_StateTypeDef;

typedef struct
{
  uint32_t             Protocol;
  uint32_t             IdleState;
  uint32_t             AltSetting;
  HID_StateTypeDef     state;
}
USBD_HID_HandleTypeDef;

void NSCON_init(void);
void NSCON_Timer1_Callback(void);
void NSCON_Timer2_Callback(void);
void NSCON_Parse_Report(NSCON_ButtonList_TypeDef* list);

#endif /* __NS_CON_DEVICE_H__ */
