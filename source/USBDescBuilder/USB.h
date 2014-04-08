#pragma once
#include <stdint.h>

#ifndef __cplusplus
#define static_assert(cond,message) /* elide for now */
#endif // __cplusplus


//! Class-specific USB descriptor types.
enum
{
  USB_DEVICE        = 0x01,
  USB_CONFIGURATION = 0x02,
  USB_STRING        = 0x03,
	UVC_INTERFACE			= 0x04,
	UVC_ENDPOINT			= 0x05,
	USB_POWER   			= 0x08,
  USB_INTERFACE_ASSOCIATION = 11,
  USB_BOS           = 0x0f,
  USB_DEVICE_CAPABILITY = 0x10,
	UVC_CS_UNDEFINED		= 0x20,
	UVC_CS_DEVICE			= 0x21,
	UVC_CS_CONFIGURATION	= 0x22,
	UVC_CS_STRING			= 0x23,
	UVC_CS_INTERFACE		= 0x24,
	UVC_CS_ENDPOINT			= 0x25,
  UVC_COMPANION  = 0x30,
  USB_DESC_TYPE_LAST
} _USB_DESC_TYPE;
typedef unsigned char USB_DESC_TYPE;

//! bmRequest.Dir
typedef enum _BMREQUEST_DIR
{
    BMREQUEST_DIR_HOST_TO_DEVICE = 0,
    BMREQUEST_DIR_DEVICE_TO_HOST = 1,
} BMREQUEST_DIR;

//! bmRequest.Type
typedef enum _BMREQUEST_TYPE
{
    //! Standard request. See \ref USB_REQUEST_ENUM
    BMREQUEST_TYPE_STANDARD = 0,

    //! Class-specific request.
    BMREQUEST_TYPE_CLASS = 1,

    //! Vendor-specific request
    BMREQUEST_TYPE_VENDOR = 2,
} BMREQUEST_TYPE;

//! bmRequest.Recipient
typedef enum _BMREQUEST_RECIPIENT
{
    //! Request is for a device.
    BMREQUEST_RECIPIENT_DEVICE = 0,

    //! Request is for an interface of a device.
    BMREQUEST_RECIPIENT_INTERFACE = 1,

    //! Request is for an endpoint of a device.
    BMREQUEST_RECIPIENT_ENDPOINT = 2,

    //! Request is for a vendor-specific purpose.
    BMREQUEST_RECIPIENT_OTHER = 3,
} BMREQUEST_RECIPIENT;

//! Standard USB descriptor types. For more information, see section 9-5 of the USB 3.0 specifications.
enum _USB_DESCRIPTOR_TYPE
{
  //! Device descriptor type.
  USB_DESCRIPTOR_TYPE_DEVICE = 0x01,

  //! Configuration descriptor type.
  USB_DESCRIPTOR_TYPE_CONFIGURATION = 0x02,

  //! String descriptor type.
  USB_DESCRIPTOR_TYPE_STRING = 0x03,

  //! Interface descriptor type.
  USB_DESCRIPTOR_TYPE_INTERFACE = 0x04,

  //! Endpoint descriptor type.
  USB_DESCRIPTOR_TYPE_ENDPOINT = 0x05,

  //! Device qualifier descriptor type.
  USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER = 0x06,

  //! Config power descriptor type.
  USB_DESCRIPTOR_TYPE_CONFIG_POWER = 0x07,

  //! Interface power descriptor type.
  USB_DESCRIPTOR_TYPE_INTERFACE_POWER = 0x08,

  //! Interface association descriptor type.
  USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION = 0x0B,

  //! Binary Object Store descriptor type.
  USB_DESCRIPTOR_TYPE_BOS = 0x0f,

  //! Device Capability descriptor type.
  USB_DESCRIPTOR_TYPE_DEVICE_CAPABILITY = 0x10,
};
typedef unsigned char USB_DESCRIPTOR_TYPE;


#pragma pack(push, 1)
struct USB_SETUP_PACKET
{
  unsigned Recipient: 2;
  unsigned Reserved: 3;
  unsigned Type: 2;
  unsigned Dir: 1;

  unsigned Request:8;
  unsigned Value:16;
  unsigned Index:16;
  unsigned Length:16;
};

static_assert(sizeof(struct USB_SETUP_PACKET) == 8, "Setup packet is required to be exactly eight uint8_ts, by spec");

struct _usb_descriptor_header {
  uint8_t          bLength;
  USB_DESC_TYPE bDescriptorType;
};
typedef struct _usb_descriptor_header USB_DESCRIPTOR_HEADER;

static_assert(sizeof(USB_DESCRIPTOR_HEADER) == 2, "Invalid USB descriptor header size");

enum _USB_CC {
  USB_CC_VIDEO = 0x0E
};
typedef unsigned char USB_CC;

/** Video interface subclass code (A.2) */
enum _uvc_int_subclass_code {
	UVC_SC_UNDEFINED = 0x00,
	UVC_SC_VIDEOCONTROL = 0x01,
	UVC_SC_VIDEOSTREAMING = 0x02,
	UVC_SC_VIDEO_INTERFACE_COLLECTION = 0x03
};
typedef unsigned char uvc_int_subclass_code;

struct usb_if_desc
{
  USB_DESCRIPTOR_HEADER header;
  uint8_t bInterfaceNumber;
  uint8_t bAlternateSetting;
  uint8_t bNumEndpoints;
  USB_CC bInterfaceClass;
  union {
    uvc_int_subclass_code bUvcInterfaceSubClass;
  };
  uint8_t bInterfaceProtocol;
  uint8_t iInterface;
};
static_assert(sizeof(struct usb_if_desc) == 9, "VC Interface Descriptor size mismatch");

enum _EndpointTransferType {
  TransferTypeUnknown = 0,
  TransferTypeIso = 1,
  TransferTypeBulk = 2,
  TransferTypeInterrupt = 3
};
typedef unsigned char EndpointTransferType;

enum _EndpointSyncType {
  SyncTypeNone = 0,
  SyncTypeAsynch = 1,
  SyncTypeSync = 2
};
typedef unsigned char EndpointSyncType;

enum _EndpointDirection {
  EndpointDirectionOut = 0,
  EndpointDirectionIn = 1
};
typedef unsigned char EndpointDirection;

struct usb_endpoint
{
  USB_DESCRIPTOR_HEADER header;
  unsigned char EndpointNumber : 4;
  unsigned char:3;
  EndpointDirection EndpointDirection:1;
  EndpointTransferType TransferType:2;
  EndpointSyncType SyncType:2;
  unsigned char:0;
  uint16_t wMaxPacketSize;
  uint8_t bInterval;
};
static_assert(sizeof(struct usb_endpoint) == 7, "Endpoint descriptor size mismatch");

#pragma pack(pop)
