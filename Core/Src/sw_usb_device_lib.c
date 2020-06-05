#include "sw_usb_device_lib.h"
#include <string.h>

static PCD_HandleTypeDef* this_hpcd;
static SW_USBD_Callback_Init_TypeDef* this_callbacks;

static uint8_t ignore_next_reset = 0;
volatile uint8_t USBD_Connected = 0;

static struct {
	uint8_t is_busy;
	uint8_t* buffer;
	int32_t length;
} SW_USBD_EP_States[2][16];

void SW_USBD_Init(PCD_HandleTypeDef *hpcd, SW_USBD_Callback_Init_TypeDef* callbacks){
	this_hpcd = hpcd;
	this_callbacks = callbacks;
	USBD_Connected = 0;
	ignore_next_reset = 1;
	memset(SW_USBD_EP_States, 0, sizeof(SW_USBD_EP_States));
	HAL_PCD_Start(hpcd);
}

void SW_USBD_DeInit(){
	HAL_PCD_Stop(this_hpcd);
	this_hpcd = NULL;
}

void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum){
	#define ep_state SW_USBD_EP_States[0][epnum]
	if(ep_state.is_busy){
		if(ep_state.length <= 0) { //CMPLT
			ep_state.is_busy = 0;
			if(epnum == 0) HAL_PCD_EP_Transmit(hpcd, 0x00, NULL, 0);
			if(epnum && this_callbacks->DataOutCmplt) (*this_callbacks->DataOutCmplt)(epnum);
		} else { // continue transfer
			uint16_t mps = hpcd->IN_ep[epnum].maxpacket;
			if(ep_state.length > mps){
				HAL_PCD_EP_Receive(hpcd, epnum, ep_state.buffer, mps);
				ep_state.buffer += mps;
				ep_state.length -= mps;
			} else {
				HAL_PCD_EP_Receive(hpcd, epnum, ep_state.buffer, ep_state.length);
				ep_state.length = 0;
			}
		}
	} else if(epnum && this_callbacks->DataOutCmplt) (*this_callbacks->DataOutCmplt)(epnum);
	#undef ep_state
}

void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum){
	#define ep_state SW_USBD_EP_States[1][epnum]
	if(ep_state.is_busy){
		if(ep_state.length < 0){ // ZLP
			HAL_PCD_EP_Transmit(hpcd, 0x00, NULL, 0);
			ep_state.length = 0;
		} else if(ep_state.length == 0) { //CMPLT
			ep_state.is_busy = 0;
			if(epnum == 0) HAL_PCD_EP_Receive(hpcd, 0x00, NULL, 0);
			if(epnum && this_callbacks->DataInCmplt) (*this_callbacks->DataInCmplt)(epnum);
		} else { // continue transfer
			uint16_t mps = hpcd->IN_ep[epnum].maxpacket;
			if(ep_state.length > mps){
				HAL_PCD_EP_Transmit(hpcd, epnum, ep_state.buffer, mps);
				ep_state.buffer += mps;
				ep_state.length -= mps;
				if(ep_state.length == 0 && epnum == 0) ep_state.length -= 1;
			} else {
				HAL_PCD_EP_Transmit(hpcd, epnum, ep_state.buffer, ep_state.length);
				ep_state.length = 0;
			}
		}
	} else if(epnum && this_callbacks->DataInCmplt) (*this_callbacks->DataInCmplt)(epnum);
	#undef ep_state
}

void SW_USBD_Transfer(uint8_t epnum, uint8_t* buf, uint32_t len){
	uint8_t dir = (epnum >> 7) & 0x01;
	epnum &= EP_ADDR_MSK;
	SW_USBD_EP_States[dir][epnum].is_busy = 1;
	SW_USBD_EP_States[dir][epnum].buffer = buf;
	SW_USBD_EP_States[dir][epnum].length = len>0?len:-1;
	if(dir) { // IN EP
		HAL_PCD_DataInStageCallback(this_hpcd, epnum);
	} else { // OUT EP
		HAL_PCD_DataOutStageCallback(this_hpcd, epnum);
	}
}

void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd){
	USB_Request_Packet_TypeDef* request = (USB_Request_Packet_TypeDef*)&(hpcd->Setup[0]);
	if(request->bmRequestType.recipient == USB_REQUEST_RECIPIENT_DEVICE && request->bmRequestType.type == USB_REQUEST_TYPE_STANDARD) {
		Get_Desc_Retval_TypeDef desc;
		switch (request->bRequest){
			case USB_STD_DEV_REQ_GET_STATUS:
				desc = (*this_callbacks->Get_Desc)(USB_DESC_TYPE_CONFIGURATION, 1);
				__ALIGN_BEGIN static struct __attribute__((packed)) {
					uint8_t  self_powered  :1;
					uint8_t  remote_wakeup :1;
					uint16_t reserved      :14;
				} retval __ALIGN_END;
				retval.self_powered = ((USB_Configuration_Descriptor_TypeDef*)(desc.desc))->bmAttributes.self_powered;
				retval.remote_wakeup = ((USB_Configuration_Descriptor_TypeDef*)(desc.desc))->bmAttributes.remote_wakeup;
				SW_USBD_Transfer(0x80, (uint8_t*)&retval, 2);
			break;
			case USB_STD_DEV_REQ_SET_ADDRESS:
				HAL_PCD_SetAddress(hpcd, request->wValue);
				HAL_PCD_EP_Transmit(hpcd, 0x00, NULL, 0);
			break;
			case USB_STD_DEV_REQ_GET_DESCRIPTOR:
				(void) hpcd;
				desc = (*this_callbacks->Get_Desc)(request->bDescType, request->iDescIdx);
				if(desc.desc){
					SW_USBD_Transfer(0x80, desc.desc, (request->wLength>desc.length)?desc.length:request->wLength);
				} else {
					HAL_PCD_EP_SetStall(hpcd, 0x00);
					HAL_PCD_EP_SetStall(hpcd, 0x80);
				}
			break;
			case USB_STD_DEV_REQ_GET_CONFIGURATION:
				(void) hpcd;
				__ALIGN_BEGIN static const uint8_t conf_num __ALIGN_END = 1;
				SW_USBD_Transfer(0x80, (uint8_t*)&conf_num, 1);
			break;
			case USB_STD_DEV_REQ_SET_CONFIGURATION:
				HAL_PCD_EP_Transmit(hpcd, 0x00, NULL, 0);
			break;
			default: 
				HAL_PCD_EP_SetStall(hpcd, 0x00);
				HAL_PCD_EP_SetStall(hpcd, 0x80);
			break;
		}
	} else {
		if(this_callbacks->Nonstnadard_Setup){
			(*this_callbacks->Nonstnadard_Setup)(request);
		} else {
			HAL_PCD_EP_SetStall(hpcd, 0x00);
			HAL_PCD_EP_SetStall(hpcd, 0x80);
		}
	}
}

void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd){
	
}

void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd){
	if(ignore_next_reset){
		ignore_next_reset = 0;
	} else {
		USBD_Connected = 1;
		HAL_PCD_EP_Open(hpcd, 0x00, 64, EP_TYPE_CTRL);
		HAL_PCD_EP_Open(hpcd, 0x80, 64, EP_TYPE_CTRL);
		HAL_PCDEx_SetTxFiFo(hpcd, 0x00, 128/4);
		HAL_PCDEx_SetRxFiFo(hpcd, 256/4);
		if(this_callbacks->Connected) (*this_callbacks->Connected)();
	}
}
