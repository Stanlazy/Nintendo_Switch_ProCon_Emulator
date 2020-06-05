#ifndef __sw_usb_host_def_H
#define __sw_usb_host_def_H
#include <stdint.h>

#define  USB_DESC_TYPE_DEVICE                           0x01U
#define  USB_DESC_TYPE_CONFIGURATION                    0x02U
#define  USB_DESC_TYPE_STRING                           0x03U
#define  USB_DESC_TYPE_INTERFACE                        0x04U
#define  USB_DESC_TYPE_ENDPOINT                         0x05U
#define  USB_DESC_TYPE_DEVICE_QUALIFIER                 0x06U
#define  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION        0x07U
#define  USB_DESC_TYPE_BOS                              0x0FU
#define  USB_DESC_TYPE_HID_DESC                         0x21U
#define  USB_DESC_TYPE_HID_REPORT_DESC                  0x22U

typedef struct __attribute__((packed)) {
	uint8_t  bLength;            //18
	uint8_t  bDescriptorType;    //USB_DESC_TYPE_DEVICE
	uint16_t bcdUSB;             //0x0200
	uint8_t  bDeviceClass;
	uint8_t  bDeviceSubClass;
	uint8_t  bDeviceProtocol;
	uint8_t  bMaxPacketSize;
	uint16_t idVendor;
	uint16_t idProduct;
	uint16_t bcdDevice;
	uint8_t  iManufacturer;
	uint8_t  iProduct;
	uint8_t  iSerialNumber;
	uint8_t  bNumConfigurations;
} USB_Device_Descriptor_TypeDef;

typedef struct __attribute__((packed)) {
	uint8_t  bLength;            //9
	uint8_t  bDescriptorType;    //USB_DESC_TYPE_CONFIGURATION
	uint16_t wTotalLength;
	uint8_t  bNumInterfaces;
	uint8_t  bConfigurationValue;
	uint8_t  iConfiguration;
	struct __attribute__((packed)) {
		uint8_t reserved_as_0        :5;
		uint8_t remote_wakeup        :1;
		uint8_t self_powered         :1;
		uint8_t reserved_as_1        :1;
	}        bmAttributes;
	uint8_t  bMaxPower;          //per 2mA
} USB_Configuration_Descriptor_TypeDef;

typedef struct __attribute__((packed)) {
	uint8_t  bLength;            //9
	uint8_t  bDescriptorType;    //USB_DESC_TYPE_INTERFACE
	uint8_t  bInterfaceNumber;
	uint8_t  bAlternateSetting;
	uint8_t  bNumEndpoints;
	uint8_t  bInterfaceClass;
	uint8_t  bInterfaceSubClass;
	uint8_t  bInterfaceProtocol;
	uint8_t  iInterface;
} USB_Interface_Descriptor_TypeDef;

#define  USB_ENDPOINT_TYPE_CONTROL                      0x00U
#define  USB_ENDPOINT_TYPE_ISOCHRONOUS                  0x01U
#define  USB_ENDPOINT_TYPE_BULK                         0x02U
#define  USB_ENDPOINT_TYPE_INTERRUPT                    0x03U
#define  USB_ENDPOINT_SYNC_NOSYNC                       0x00U
#define  USB_ENDPOINT_SYNC_ASYNC                        0x01U
#define  USB_ENDPOINT_SYNC_ADAPTIVE                     0x02U
#define  USB_ENDPOINT_SYNC_SYNC                         0x03U
#define  USB_ENDPOINT_USAGE_DATA                        0x00U
#define  USB_ENDPOINT_USAGE_FEEDBACK                    0x01U
#define  USB_ENDPOINT_USAGE_EFEEDBACK                   0x02U

typedef struct __attribute__((packed)) {
	uint8_t  bLength;            //7
	uint8_t  bDescriptorType;    //USB_DESC_TYPE_ENDPOINT
	uint8_t  bEndpointAddress;
	struct __attribute__((packed)) {
		uint8_t transfer_type        :2;
		uint8_t synchronisation_type :2;
		uint8_t usage_type           :2;
		uint8_t unused               :2;
	}        bmAttributes;
	uint16_t wMaxPacketSize;
	uint8_t  bInterval;
} USB_Endpoint_Descriptor_TypeDef;

typedef struct __attribute__((packed)) {
	uint8_t  bLength;            //9
	uint8_t  bDescriptorType;    //USB_DESC_TYPE_HID_DESC
	uint16_t bcdHID;             //0x0111
	uint8_t  bCountryCode;       //0x00
	uint8_t  bNumDescriptors;
	uint8_t  bReportDescriptorType;//USB_DESC_TYPE_HID_REPORT_DESC
	uint16_t wDescriptorLength;
} USB_HID_Descriptor_TypeDef;

#define  USB_REQUEST_RECIPIENT_DEVICE                   0x00U
#define  USB_REQUEST_RECIPIENT_INTERFACE                0x01U
#define  USB_REQUEST_RECIPIENT_ENDPOINT                 0x02U
#define  USB_REQUEST_TYPE_STANDARD                      0x00U
#define  USB_REQUEST_TYPE_CLASS                         0x01U
#define  USB_REQUEST_TYPE_VENDOR                        0x02U
#define  USB_REQUEST_DIR_OUT                            0x00U
#define  USB_REQUEST_DIR_IN                             0x01U
#define  USB_STD_DEV_REQ_GET_STATUS                     0x00U
#define  USB_STD_DEV_REQ_CLEAR_FEATURE                  0x01U
#define  USB_STD_DEV_REQ_SET_FEATURE                    0x03U
#define  USB_STD_DEV_REQ_SET_ADDRESS                    0x05U
#define  USB_STD_DEV_REQ_GET_DESCRIPTOR                 0x06U
#define  USB_STD_DEV_REQ_SET_DESCRIPTOR                 0x07U
#define  USB_STD_DEV_REQ_GET_CONFIGURATION              0x08U
#define  USB_STD_DEV_REQ_SET_CONFIGURATION              0x09U

typedef struct __attribute__((packed)) {
	struct __attribute__((packed)) {
		uint8_t recipient            :5;
		uint8_t type                 :2;
		uint8_t direction            :1;
	}        bmRequestType;
	uint8_t  bRequest;
	union {
		struct {
			uint8_t iDescIdx;
			uint8_t bDescType;
		};
		uint16_t wValue;
	};
	uint16_t wIndex;
	uint16_t wLength;
} USB_Request_Packet_TypeDef;


#endif /* __sw_usb_host_def_H */
