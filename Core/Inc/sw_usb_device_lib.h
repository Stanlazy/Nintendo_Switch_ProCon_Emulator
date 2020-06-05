#ifndef __sw_usb_device_lib_H
#define __sw_usb_device_lib_H
#include "main.h"
#include "usb_otg.h"
#include "sw_usb_def.h"

typedef struct {
	uint8_t* desc;
	uint16_t length;
} Get_Desc_Retval_TypeDef;

typedef struct {
	Get_Desc_Retval_TypeDef (*Get_Desc)(uint8_t bDescType, uint8_t iDescIdx);
	void (*Nonstnadard_Setup)(USB_Request_Packet_TypeDef* request);
	void (*Connected)(void);
	void (*DataInCmplt)(uint8_t epnum);
	void (*DataOutCmplt)(uint8_t epnum);
} SW_USBD_Callback_Init_TypeDef;

extern volatile uint8_t USBD_Connected;

void SW_USBD_Init(PCD_HandleTypeDef *hpcd, SW_USBD_Callback_Init_TypeDef* callbacks);
void SW_USBD_DeInit();
void SW_USBD_Transfer(uint8_t epnum, uint8_t* buf, uint32_t len);

#endif /* __sw_usb_device_lib_H */
