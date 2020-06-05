#ifndef __nscon_device_H
#define __nscon_device_H
#include "main.h"
#include "sw_usb_device_lib.h"
#include "nscon_def.h"

void nscon_init(PCD_HandleTypeDef *hpcd);
void nscon_seq_timmer_callback(void);
Get_Desc_Retval_TypeDef nscon_Get_Desc(uint8_t bDescType, uint8_t iDescIdx);
void nscon_Nonstnadard_Setup(USB_Request_Packet_TypeDef* request);
void nscon_Connected(void);
void nscon_DataIn(uint8_t epnum);
void nscon_DataOut(uint8_t epnum);
void nscon_copy_report(NSCON_Report_TypeDef* ext);

#endif /* __nscon_device_H */
