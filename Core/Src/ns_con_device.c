#include "ns_con_device.h"
#include "gpio.h"
#include "tim.h"

//220hz
#define NS_TIMER_1 htim10
//100hz
#define NS_TIMER_2 htim11

__ALIGN_BEGIN static const uint8_t NSCON_CfgDesc[0x29] __ALIGN_END = {0x09,0x02,0x29,0x00,0x01,0x01,0x00,0xa0,0xfa,0x09,0x04,0x00,0x00,0x02,0x03,0x00,0x00,0x00,0x09,0x21,0x11,0x01,0x00,0x01,0x22,0xcb,0x00,0x07,0x05,0x81,0x03,0x40,0x00,0x08,0x07,0x05,0x01,0x03,0x40,0x00,0x08};
__ALIGN_BEGIN static const uint8_t NSCON_HID_Desc[9] __ALIGN_END  = {0x09,0x21,0x11,0x01,0x00,0x01,0x22,203,0x00};
__ALIGN_BEGIN static const uint8_t NSCON_ReportDesc[203] __ALIGN_END = {0x05,0x01,0x15,0x00,0x09,0x04,0xa1,0x01,0x85,0x30,0x05,0x01,0x05,0x09,0x19,0x01,0x29,0x0a,0x15,0x00,0x25,0x01,0x75,0x01,0x95,0x0a,0x55,0x00,0x65,0x00,0x81,0x02,0x05,0x09,0x19,0x0b,0x29,0x0e,0x15,0x00,0x25,0x01,0x75,0x01,0x95,0x04,0x81,0x02,0x75,0x01,0x95,0x02,0x81,0x03,0x0b,0x01,0x00,0x01,0x00,0xa1,0x00,0x0b,0x30,0x00,0x01,0x00,0x0b,0x31,0x00,0x01,0x00,0x0b,0x32,0x00,0x01,0x00,0x0b,0x35,0x00,0x01,0x00,0x15,0x00,0x27,0xff,0xff,0x00,0x00,0x75,0x10,0x95,0x04,0x81,0x02,0xc0,0x0b,0x39,0x00,0x01,0x00,0x15,0x00,0x25,0x07,0x35,0x00,0x46,0x3b,0x01,0x65,0x14,0x75,0x04,0x95,0x01,0x81,0x02,0x05,0x09,0x19,0x0f,0x29,0x12,0x15,0x00,0x25,0x01,0x75,0x01,0x95,0x04,0x81,0x02,0x75,0x08,0x95,0x34,0x81,0x03,0x06,0x00,0xff,0x85,0x21,0x09,0x01,0x75,0x08,0x95,0x3f,0x81,0x03,0x85,0x81,0x09,0x02,0x75,0x08,0x95,0x3f,0x81,0x03,0x85,0x01,0x09,0x03,0x75,0x08,0x95,0x3f,0x91,0x83,0x85,0x10,0x09,0x04,0x75,0x08,0x95,0x3f,0x91,0x83,0x85,0x80,0x09,0x05,0x75,0x08,0x95,0x3f,0x91,0x83,0x85,0x82,0x09,0x06,0x75,0x08,0x95,0x3f,0x91,0x83,0xc0};
__ALIGN_BEGIN static const uint8_t NSCON_DeviceDesc[0x12] __ALIGN_END =  {0x12,0x01,0x00,0x02,0x00,0x00,0x00,64,0x7e,0x05,0x09,0x20,0x00,0x02,1,2,3,0x01};
__ALIGN_BEGIN static const uint8_t NSCON_LangIDDesc[0x04] __ALIGN_END = {0x04,0x03,0x33,0x10};
__ALIGN_BEGIN static const uint8_t NSCON_StringSerial[0x1a] __ALIGN_END = {0x1a,0x03,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x31,0x00};
__ALIGN_BEGIN uint8_t NSCON_StrDesc[64] __ALIGN_END;

static const uint8_t NSCON_initcmd[22][64] = {
	{0x81,0x01,0x00, 0x03,0xa3,0x95, 0xf0,0xcb,0x58,0x94,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x81,0x02,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x21,0x00,0x91, 0x00,0x80,0x00, 0x00,0x08,0x80,0x00,0x08,0x80, 0x00,0x80,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x21,0x00,0x91, 0x00,0x80,0x00, 0x00,0x08,0x80,0x00,0x08,0x80, 0x00,0x80,0x48,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x21,0x00,0x91, 0x00,0x80,0x00, 0x00,0x08,0x80,0x00,0x08,0x80, 0x00,0x82,0x02,0x03,0x48,0x03,0x02,0x94,0x58,0xcb,0xf0,0x95,0xa3,0x03,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x21,0x00,0x91, 0x00,0x80,0x00, 0x00,0x08,0x80,0x00,0x08,0x80, 0x00,0x80,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x21,0x00,0x91, 0x00,0x80,0x00, 0x00,0x08,0x80,0x00,0x08,0x80, 0x00,0x90,0x10,0x00,0x60,0x00,0x00,0x10,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x21,0x00,0x91, 0x00,0x80,0x00, 0x00,0x08,0x80,0x00,0x08,0x80, 0x00,0x90,0x10,0x50,0x60,0x00,0x00,0x0d,0x32,0x32,0x32,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x21,0x00,0x91, 0x00,0x80,0x00, 0x00,0x08,0x80,0x00,0x08,0x80, 0x00,0x81,0x01,0x01,0xa3,0x95,0xf0,0xcb,0x58,0x94,0x00,0x25,0x08,0x50,0x72,0x6f,0x20,0x43,0x6f,0x6e,0x74,0x72,0x6f,0x6c,0x6c,0x65,0x72,0x00,0x00,0x00,0x00,0x00,0x00,0x68,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x21,0x00,0x91, 0x00,0x80,0x00, 0x00,0x08,0x80,0x00,0x08,0x80, 0x00,0x81,0x01,0x02,0x59,0x6d,0x18,0xd6,0x0d,0xfa,0x9c,0xb2,0x72,0xe8,0xa6,0x21,0xd3,0xe0,0x50,0x63,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x21,0x00,0x91, 0x00,0x80,0x00, 0x00,0x08,0x80,0x00,0x08,0x80, 0x00,0x81,0x01,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x21,0x00,0x91, 0x00,0x80,0x00, 0x00,0x08,0x80,0x00,0x08,0x80, 0x00,0x80,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x21,0x00,0x91, 0x00,0x80,0x00, 0x00,0x08,0x80,0x00,0x08,0x80, 0x00,0x83,0x04,0x48,0x00,0x48,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x21,0x00,0x91, 0x00,0x80,0x00, 0x00,0x08,0x80,0x00,0x08,0x80, 0x00,0x90,0x10,0x80,0x60,0x00,0x00,0x18,0x50,0xfd,0x00,0x00,0xc6,0x0f,0x0f,0x30,0x61,0xae,0x90,0xd9,0xd4,0x14,0x54,0x41,0x15,0x54,0xc7,0x79,0x9c,0x33,0x36,0x63,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x21,0x00,0x91, 0x00,0x80,0x00, 0x00,0x08,0x80,0x00,0x08,0x80, 0x09,0x90,0x10,0x98,0x60,0x00,0x00,0x12,0x0f,0x30,0x61,0xae,0x90,0xd9,0xd4,0x14,0x54,0x41,0x15,0x54,0xc7,0x79,0x9c,0x33,0x36,0x63,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x21,0x00,0x91, 0x00,0x80,0x00, 0x00,0x08,0x80,0x00,0x08,0x80, 0x09,0x90,0x10,0x10,0x80,0x00,0x00,0x18,0xb2,0xa1,0xff,0xf7,0x7f,0x00,0x08,0x80,0xff,0xf7,0x7f,0xb2,0xa1,0xff,0xf7,0x7f,0x00,0x08,0x80,0xff,0xf7,0x7f,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x21,0x00,0x91, 0x00,0x80,0x00, 0x00,0x08,0x80,0x00,0x08,0x80, 0x09,0x90,0x10,0x3d,0x60,0x00,0x00,0x19,0x0c,0x26,0x6c,0x59,0xb8,0x7f,0x33,0xb6,0x5e,0x51,0x98,0x81,0x36,0x16,0x63,0xf9,0xb5,0x63,0xff,0x32,0x32,0x32,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x21,0x00,0x91, 0x00,0x80,0x00, 0x00,0x08,0x80,0x00,0x08,0x80, 0x0a,0x90,0x10,0x20,0x60,0x00,0x00,0x18,0xa5,0xff,0x81,0x00,0xb7,0x01,0x00,0x40,0x00,0x40,0x00,0x40,0x08,0x00,0x00,0x00,0x0c,0x00,0xe7,0x3b,0xe7,0x3b,0xe7,0x3b,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x21,0x00,0x91, 0x00,0x80,0x00, 0x00,0x08,0x80,0x00,0x08,0x80, 0x0a,0x80,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x21,0x00,0x91, 0x00,0x80,0x00, 0x00,0x08,0x80,0x00,0x08,0x80, 0x09,0x80,0x48,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x21,0x00,0x91, 0x00,0x80,0x00, 0x00,0x08,0x80,0x00,0x08,0x80, 0x00,0xa0,0x21,0x01,0x00,0xff,0x00,0x03,0x00,0x05,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x21,0x00,0x91, 0x00,0x80,0x00, 0x00,0x08,0x80,0x00,0x08,0x80, 0x0a,0x80,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
};

uint8_t NSCON_Report[64] = {0x30,0x00,0x91, 0x00,0x80,0x00, 0x00,0x08,0x80,0x00,0x08,0x80, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

uint8_t dataout_buffer[64];

USBD_HandleTypeDef NSCON_USB_Handle;

uint8_t init_cmdptr = 0;
uint8_t init_state = 0;
volatile uint32_t dataout_last_length = 0;

uint8_t NSCON_SendReport (USBD_HandleTypeDef  *pdev, uint8_t *report, uint16_t len);
uint8_t NSCON_USBHID_Init (USBD_HandleTypeDef *pdev, uint8_t cfgidx);
uint8_t NSCON_USBHID_DeInit (USBD_HandleTypeDef *pdev, uint8_t cfgidx);
uint8_t NSCON_USBHID_Setup (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
uint8_t* NSCON_USBHID_GetFSCfgDesc (uint16_t *length);
uint8_t NSCON_USBHID_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum);
uint8_t NSCON_USBHID_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum);
uint8_t* NSCON_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t* NSCON_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t* NSCON_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t* NSCON_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t* NSCON_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t* NSCON_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t* NSCON_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);

static const USBD_ClassTypeDef NSCON_class = {
	NSCON_USBHID_Init,
	NSCON_USBHID_DeInit,
	NSCON_USBHID_Setup,
	NULL, /*EP0_TxSent*/
	NULL, /*EP0_RxReady*/
	NSCON_USBHID_DataIn, /*DataIn*/
	NSCON_USBHID_DataOut, /*DataOut*/
	NULL, /*SOF */
	NULL,
	NULL,
	NULL,
	NSCON_USBHID_GetFSCfgDesc,
	NULL,
	NULL,
};

static const USBD_DescriptorsTypeDef NSCON_FS_Desc = {
	NSCON_DeviceDescriptor,
	NSCON_LangIDStrDescriptor,
	NSCON_ManufacturerStrDescriptor,
	NSCON_ProductStrDescriptor,
	NSCON_SerialStrDescriptor,
	NSCON_ConfigStrDescriptor,
	NSCON_InterfaceStrDescriptor
};

//replace MX_USB_DEVICE_Init
void NSCON_init(){
	USBD_Init(&NSCON_USB_Handle, (USBD_DescriptorsTypeDef*)&NSCON_FS_Desc, 0);
	USBD_RegisterClass(&NSCON_USB_Handle, (USBD_ClassTypeDef*)&NSCON_class);
	USBD_Start(&NSCON_USB_Handle);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);	//PullUp
}

//~220hz seq generator
void NSCON_Timer1_Callback(){
	NSCON_Report[1]++;
}

//100hz report timer
void NSCON_Timer2_Callback(){
	if(dataout_last_length == 49 && init_cmdptr < 22){
		dataout_last_length = 0;
		NSCON_SendReport(&NSCON_USB_Handle, (uint8_t*)NSCON_initcmd[init_cmdptr], sizeof(NSCON_initcmd[0]));
		init_cmdptr++;
	} else {
		NSCON_SendReport(&NSCON_USB_Handle, NSCON_Report, sizeof(NSCON_Report));
	}
}

void NSCON_Parse_Report(NSCON_ButtonList_TypeDef* list){
	NSCON_Report[3] = 0;
	NSCON_Report[4] = 0;
	NSCON_Report[5] = 0;
	
	if(list->buttA)      {NSCON_Report[3] |= 0x08;}
	if(list->buttB)      {NSCON_Report[3] |= 0x04;}
	if(list->buttX)      {NSCON_Report[3] |= 0x02;}
	if(list->buttY)      {NSCON_Report[3] |= 0x01;}
	if(list->buttLPadL)  {NSCON_Report[5] |= 0x08;}
	if(list->buttLPadR)  {NSCON_Report[5] |= 0x04;}
	if(list->buttLPadU)  {NSCON_Report[5] |= 0x02;}
	if(list->buttLPadD)  {NSCON_Report[5] |= 0x01;}
	if(list->buttMinus)  {NSCON_Report[4] |= 0x01;}
	if(list->buttPlus)   {NSCON_Report[4] |= 0x02;}
	if(list->buttHome)   {NSCON_Report[4] |= 0x10;}
	if(list->buttShare)  {NSCON_Report[4] |= 0x20;}
	if(list->buttL1)     {NSCON_Report[5] |= 0x40;}
	if(list->buttL2)     {NSCON_Report[5] |= 0x80;}
	if(list->buttR1)     {NSCON_Report[3] |= 0x40;}
	if(list->buttR2)     {NSCON_Report[3] |= 0x80;}
	if(list->buttLStickM){NSCON_Report[4] |= 0x08;}
	if(list->buttRStickM){NSCON_Report[4] |= 0x04;}
	
	int16_t temp[4];
	temp[0] = list->analLStickX;
	temp[1] = list->analLStickY;
	temp[2] = list->analRStickX;
	temp[3] = list->analRStickY;
	for(uint8_t i = 0; i < 4; i++){
		temp[i] += 0x800;
		if(temp[i] < 0x001) temp[i] = 0x001;
		if(temp[i] > 0xfff) temp[i] = 0xfff;
	}
	NSCON_Report[6] = temp[0] & 0xff;
	NSCON_Report[7] = ((temp[0] >> 8) & 0x0f) | ((temp[1] << 4) & 0xf0);
	NSCON_Report[8] = (temp[1] >> 4) & 0xff;
	NSCON_Report[9] = temp[2] & 0xff;
	NSCON_Report[10] = ((temp[2] >> 8) & 0x0f) | ((temp[3] << 4) & 0xf0);
	NSCON_Report[11] = (temp[3] >> 4) & 0xff;
}

uint8_t NSCON_SendReport (USBD_HandleTypeDef  *pdev, uint8_t *report, uint16_t len){
	USBD_HID_HandleTypeDef *hhid = (USBD_HID_HandleTypeDef*)pdev->pClassData;
	if (pdev->dev_state == USBD_STATE_CONFIGURED ) {
		if(hhid->state == HID_IDLE) {
			hhid->state = HID_BUSY;
			USBD_LL_Transmit (pdev, 0x81, report, len);
		}
	}
	return USBD_OK;
}

uint8_t NSCON_USBHID_Init (USBD_HandleTypeDef *pdev, uint8_t cfgidx){
	(void) cfgidx;
	USBD_LL_OpenEP(pdev, 0x81, USBD_EP_TYPE_INTR, 64);
	USBD_LL_OpenEP(pdev, 0x01, USBD_EP_TYPE_INTR, 64);
	pdev->ep_in[0x81 & 0xFU].is_used = 1U;
	pdev->ep_out[0x01 & 0xFU].is_used = 1U;
	
	pdev->pClassData = USBD_malloc(sizeof (USBD_HID_HandleTypeDef));
	if (pdev->pClassData == NULL) return USBD_FAIL;
	((USBD_HID_HandleTypeDef *)pdev->pClassData)->state = HID_IDLE;
	
	USBD_LL_PrepareReceive(pdev, 0x01, dataout_buffer, sizeof(dataout_buffer));
	init_cmdptr = 0;
	NSCON_SendReport(&NSCON_USB_Handle, (uint8_t*)NSCON_initcmd[init_cmdptr], sizeof(NSCON_initcmd[0]));
	init_cmdptr++;
	init_state = 1;
	
	return USBD_OK;
}

uint8_t NSCON_USBHID_DeInit (USBD_HandleTypeDef *pdev, uint8_t cfgidx){
	(void) cfgidx;
	USBD_LL_CloseEP(pdev, 0x81);
	USBD_LL_CloseEP(pdev, 0x01);
	pdev->ep_in[0x81 & 0xFU].is_used = 0U;
	pdev->ep_out[0x01 & 0xFU].is_used = 0U;
	
	if(pdev->pClassData != NULL) {
		USBD_free(pdev->pClassData);
		pdev->pClassData = NULL;
	}
	
	return USBD_OK;
}

uint8_t NSCON_USBHID_Setup (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req){
	USBD_HID_HandleTypeDef *hhid = (USBD_HID_HandleTypeDef*) pdev->pClassData;
	uint16_t len = 0U;
	uint8_t *pbuf = NULL;
	uint16_t status_info = 0U;
	USBD_StatusTypeDef ret = USBD_OK;

	switch (req->bmRequest & USB_REQ_TYPE_MASK)
	{
	case USB_REQ_TYPE_CLASS :
		switch (req->bRequest)
		{
		case 0x0b:
			hhid->Protocol = (uint8_t)(req->wValue);
			break;

		case 0x03:
			USBD_CtlSendData (pdev, (uint8_t *)(void *)&hhid->Protocol, 1U);
			break;

		case 0x0a:
			hhid->IdleState = (uint8_t)(req->wValue >> 8);
			break;

		case 0x02:
			USBD_CtlSendData (pdev, (uint8_t *)(void *)&hhid->IdleState, 1U);
			break;

		default:
			USBD_CtlError (pdev, req);
			ret = USBD_FAIL;
			break;
		}
		break;
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest)
		{
		case USB_REQ_GET_STATUS:
			if (pdev->dev_state == USBD_STATE_CONFIGURED)
			{
				USBD_CtlSendData (pdev, (uint8_t *)(void *)&status_info, 2U);
			}
			else
			{
				USBD_CtlError (pdev, req);
				ret = USBD_FAIL;
			}
			break;

		case USB_REQ_GET_DESCRIPTOR:
			if(req->wValue >> 8 == 0x22)
			{
				len = MIN(203 , req->wLength);
				pbuf = (uint8_t*)NSCON_ReportDesc;
			}
			else if(req->wValue >> 8 == 0x21)
			{
				pbuf = (uint8_t*)NSCON_HID_Desc;
				len = MIN(9, req->wLength);
			}
			else
			{
				USBD_CtlError (pdev, req);
				ret = USBD_FAIL;
				break;
			}
			USBD_CtlSendData (pdev, pbuf, len);
			break;

		case USB_REQ_GET_INTERFACE :
			if (pdev->dev_state == USBD_STATE_CONFIGURED)
			{
				USBD_CtlSendData (pdev, (uint8_t *)(void *)&hhid->AltSetting, 1U);
			}
			else
			{
				USBD_CtlError (pdev, req);
				ret = USBD_FAIL;
			}
			break;

		case USB_REQ_SET_INTERFACE :
			if (pdev->dev_state == USBD_STATE_CONFIGURED)
			{
				hhid->AltSetting = (uint8_t)(req->wValue);
			}
			else
			{
				USBD_CtlError (pdev, req);
				ret = USBD_FAIL;
			}
			break;

		default:
			USBD_CtlError (pdev, req);
			ret = USBD_FAIL;
			break;
		}
		break;

	default:
		USBD_CtlError (pdev, req);
		ret = USBD_FAIL;
		break;
	}

	return ret;
}

uint8_t* NSCON_USBHID_GetFSCfgDesc (uint16_t *length){
	*length = sizeof (NSCON_CfgDesc);
	return (uint8_t*)NSCON_CfgDesc;
}

uint8_t NSCON_USBHID_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum){
	((USBD_HID_HandleTypeDef *)pdev->pClassData)->state = HID_IDLE;
	switch(init_state){
		case 1: init_state = 2; break; //8101
		case 3: init_state = 4; break; //8102
		case 5: init_state = 6; break; //init[2]
		default:break;
	}
	return USBD_OK;
}

uint8_t NSCON_USBHID_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum){
	switch(init_state){
		case 2: //8002
			init_state = 3;
			NSCON_SendReport(&NSCON_USB_Handle, (uint8_t*)NSCON_initcmd[init_cmdptr], sizeof(NSCON_initcmd[0])); //8102
			init_cmdptr++;
		break;
		case 4: //0100000033
			init_state = 5;
			NSCON_SendReport(&NSCON_USB_Handle, (uint8_t*)NSCON_initcmd[init_cmdptr], sizeof(NSCON_initcmd[0]));
			init_cmdptr++;
		break;
		case 6: //8004
			init_state = 7;
			NSCON_SendReport(&NSCON_USB_Handle, (uint8_t*)NSCON_initcmd[init_cmdptr], sizeof(NSCON_initcmd[0]));
			init_cmdptr++;
			HAL_TIM_Base_Start_IT(&NS_TIMER_1);
			HAL_TIM_Base_Start_IT(&NS_TIMER_2);
		break;
		case 7:
			dataout_last_length = USBD_LL_GetRxDataSize(pdev, epnum);
		break;
		default:break;
	}
	USBD_LL_PrepareReceive (pdev, 0x01, dataout_buffer, sizeof(dataout_buffer));
	return USBD_OK;
}

uint8_t* NSCON_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length){
	(void) speed;
	*length = sizeof(NSCON_DeviceDesc);
	return (uint8_t*)NSCON_DeviceDesc;
}

uint8_t* NSCON_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length){
	(void) speed;
	*length = sizeof(NSCON_LangIDDesc);
	return (uint8_t*)NSCON_LangIDDesc;
}

uint8_t* NSCON_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length){
	(void) speed;
	USBD_GetString((uint8_t *)"Nintendo Co., Ltd.", NSCON_StrDesc, length);
	return NSCON_StrDesc;
}

uint8_t* NSCON_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length){
	(void) speed;
	USBD_GetString((uint8_t *)"Pro Controller", NSCON_StrDesc, length);
	return NSCON_StrDesc;
}

uint8_t* NSCON_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length){
	(void) speed;
	*length = 0x1a;
	return (uint8_t*)NSCON_StringSerial;
}

uint8_t* NSCON_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length){
	(void) speed;
	USBD_GetString((uint8_t *)"HID Config", NSCON_StrDesc, length);
	return NSCON_StrDesc;
}

uint8_t* NSCON_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length){
	(void) speed;
	USBD_GetString((uint8_t *)"HID Interface", NSCON_StrDesc, length);
	return NSCON_StrDesc;
}

