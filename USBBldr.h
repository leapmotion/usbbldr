/* Copyright (c) 2014 LEAP Motion. All rights reserved.
 *
 * The intellectual and technical concepts contained herein are proprietary and
 * confidential to Leap Motion, and are protected by trade secret or copyright
 * law. Dissemination of this information or reproduction of this material is
 * strictly forbidden unless prior written permission is obtained from LEAP
 * Motion.
 */

#pragma once

#ifndef __cplusplus
#define static_assert(cond,message) /* elide for now */
#endif // __cplusplus

#include <stdint.h>

// Select the class of UVC desired: 1.0, 1.1, 1.5
#define UVC_CLASS_SELECT 100
#if     UVC_CLASS_SELECT == 150
#define UVC_CLASS 0x0150    // 1.5, conveniently in BCD
#elif   UVC_CLASS_SELECT == 110
#define UVC_CLASS 0x0110    // 1.1, conveniently in BCD
#else
#define UVC_CLASS 0x0100    // 1.0, conveniently in BCD
#endif

// The following MUST mimic the exported, API-visible usbdescbldr_guid_t :
typedef struct
{
  uint32_t dwData1;
  uint16_t dwData2;
  uint16_t dwData3;
  uint8_t  dwData4[8];
} USB_GUID;



//! Class-specific USB descriptor types.
enum
{
  USB_DEVICE = 0x01,
  USB_CONFIGURATION = 0x02,
  USB_STRING = 0x03,
  UVC_INTERFACE = 0x04,
  UVC_ENDPOINT = 0x05,
  USB_POWER = 0x08,
  USB_INTERFACE_ASSOCIATION = 0x0B,
  USB_BOS = 0x0F,
  USB_DEVICE_CAPABILITY = 0x10,
  UVC_CS_UNDEFINED = 0x20,
  UVC_CS_DEVICE = 0x21,
  UVC_CS_CONFIGURATION = 0x22,
  UVC_CS_STRING = 0x23,
  UVC_CS_INTERFACE = 0x24,
  UVC_CS_ENDPOINT = 0x25,
  UVC_COMPANION = 0x30,
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
  //! Standard request
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
  USB_DESCRIPTOR_TYPE_BOS = 0x0F,

  //! Device Capability descriptor type.
  USB_DESCRIPTOR_TYPE_DEVICE_CAPABILITY = 0x10,

  //! SuperSpeed Companion Endpoint descriptor type.
  USB_DESCRIPTOR_TYPE_SS_EP_COMPANION = 0x30,

};
typedef unsigned char USB_DESCRIPTOR_TYPE;

#pragma pack(push, 1)

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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Structures / Descriptors
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct _usb_descriptor_header {
  uint8_t          bLength;
  USB_DESC_TYPE bDescriptorType;
};
typedef struct _usb_descriptor_header USB_DESCRIPTOR_HEADER;

static_assert(sizeof(USB_DESCRIPTOR_HEADER) == 2, "Invalid USB descriptor header size");

// Class-specific headers take the extra subtype:
typedef struct _USB_CS_DESCRIPTOR_HEADER {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bDescriptorSubtype;
} USB_CS_DESCRIPTOR_HEADER;


typedef struct _USB_DEVICE_DESCRIPTOR
{
  USB_DESCRIPTOR_HEADER header;
  uint16_t bcdUSB;
  uint8_t  bDeviceClass;
  uint8_t  bDeviceSubClass;
  uint8_t  bDeviceProtocol;
  uint8_t  bMaxPacketSize0;
  uint16_t idVendor;
  uint16_t idProduct;
  uint16_t bcdDevice;
  uint8_t  iManufacturer;
  uint8_t  iProduct;
  uint8_t  iSerialNumber;
  uint8_t  bNumConfigurations;
} USB_DEVICE_DESCRIPTOR;

typedef struct _USB_DEVICE_QUALIFIER_DESCRIPTOR
{
  USB_DESCRIPTOR_HEADER header;
  uint16_t bcdUSB;
  uint8_t  bDeviceClass;
  uint8_t  bDeviceSubClass;
  uint8_t  bDeviceProtocol;
  uint8_t  bMaxPacketSize0;
  uint8_t  bNumConfigurations;
  uint8_t  bReserved;
}  USB_DEVICE_QUALIFIER_DESCRIPTOR;

typedef struct _USB_CONFIG_DESCRIPTOR
{
  USB_DESCRIPTOR_HEADER header;
  uint16_t wTotalLength;
  uint8_t  bNumInterfaces;
  uint8_t  bConfigurationValue;
  uint8_t  iConfiguration;
  uint8_t  bmAttributes;
  uint8_t  bMaxPower;
} USB_CONFIG_DESCRIPTOR;

typedef struct _USB_INTERFACE_ASSOCIATION_DESCRIPTOR
{
  USB_DESCRIPTOR_HEADER header;
  uint8_t bFirstInterface;
  uint8_t bInterfaceCount;
  uint8_t bFunctionClass;
  uint8_t bFunctionSubClass;
  uint8_t bFunctionProtocol;
  uint8_t iFunction;
} USB_INTERFACE_ASSOCIATION_DESCRIPTOR;

typedef struct _USB_INTERFACE_DESCRIPTOR
{
  USB_DESCRIPTOR_HEADER header;
  uint8_t bInterfaceNumber;
  uint8_t bAlternateSetting;
  uint8_t bNumEndpoints;
  uint8_t bInterfaceClass;
  uint8_t bInterfaceSubClass;
  uint8_t bInterfaceProtocol;
  uint8_t iInterface;
} USB_INTERFACE_DESCRIPTOR;

typedef struct _USB_SS_EP_COMPANION_DESCRIPTOR 
{
  USB_DESCRIPTOR_HEADER header;
  uint8_t  bMaxBurst;
  uint8_t  bmAttributes;
  uint16_t wBytesPerInterval;
} USB_SS_EP_COMPANION_DESCRIPTOR;

typedef struct _USB_ENDPOINT_DESCRIPTOR
{
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bEndpointAddress;
  uint8_t  bmAttributes;
  uint16_t wMaxPacketSize;
  uint8_t  bInterval;
} USB_ENDPOINT_DESCRIPTOR;

typedef struct _USB_CLASS_SPECIFIC_INTERRUPT_ENDPOINT_DESCRIPTOR
{
  USB_DESCRIPTOR_HEADER header;
  uint8_t  bDescriptorSubType;
  uint16_t wMaxTransferSize;
} USB_CLASS_SPECIFIC_INTERRUPT_ENDPOINT_DESCRIPTOR;

typedef struct _USB_UVC_VC_HEADER_DESCRIPTOR
{
  USB_DESCRIPTOR_HEADER header;
  uint8_t  bDescriptorSubType;       //0x01
  uint16_t bcdUVC;
  uint16_t wTotalLength;
  uint32_t dwClockFrequency;
  uint8_t  bInCollection;
  uint8_t  baInterfaceNr;
} USB_UVC_VC_HEADER_DESCRIPTOR;

// Input terminal
typedef struct _USB_UVC_VC_INPUT_TERMINAL
{
  USB_DESCRIPTOR_HEADER header;
  uint8_t  bDescriptorSubType;       //0x02
  uint8_t  bTerminalID;
  uint16_t wTerminalType;
  uint8_t  bAssocTerminal;
  uint8_t  iTerminal;
  uint16_t wOpticalZoom0;
  uint16_t wOpticalZoom1;
  uint16_t wOpticalZoom2;
  uint8_t  bControlBitfieldSize;
  uint8_t  bmControls[3];
} USB_UVC_VC_INPUT_TERMINAL;

// Camera terminal
typedef struct _USB_UVC_CAMERA_TERMINAL
{
  USB_DESCRIPTOR_HEADER header;
  uint8_t  bDescriptorSubType;       //0x02
  uint8_t  bTerminalID;
  uint16_t wTerminalType;
  uint8_t  bAssocTerminal;
  uint8_t  iTerminal;
  uint16_t wObjectiveFocalLengthMin;
  uint16_t wObjectiveFocalLengthMax;
  uint16_t wOcularFocalLength;
  uint8_t  bControlBitfieldSize;
  uint8_t  bmControls[3];
} USB_UVC_CAMERA_TERMINAL;

static const uint16_t USB_UVC_ITT_CAMERA = 0x0201;

// Streaming Output terminal
typedef struct _USB_UVC_STREAMING_OUT_TERMINAL
{
  USB_DESCRIPTOR_HEADER header;
  uint8_t  bDescriptorSubType;       //0x03
  uint8_t  bTerminalID;
  uint16_t wTerminalType;
  uint8_t  bAssocTerminal;
  uint8_t  bSourceID;
  uint8_t  iTerminal;
} USB_UVC_STREAMING_OUT_TERMINAL;

static const uint16_t USB_UVC_OTT_STREAMING = 0x0101;

// Selector unit
typedef struct _USB_UVC_VC_SELECTOR_UNIT
{
  USB_DESCRIPTOR_HEADER header;
  uint8_t  bDescriptorSubType;
  uint8_t  bUnitID;
  uint8_t  bSourceID;
  uint16_t wMaxMultiplier;
  uint8_t  bControlSize;
  uint8_t  bmControls[3];
  uint8_t  iTerminal;
} USB_UVC_VC_SELECTOR_UNIT;


// Processing unit
typedef struct _USB_UVC_VC_PROCESSING_UNIT
{
  USB_DESCRIPTOR_HEADER header;
  uint8_t  bDescriptorSubType;       //0x05
  uint8_t  bUnitID;
  uint8_t  bSourceID;
  uint16_t wMaxMultiplier;
  uint8_t  bControlSize;
  uint8_t  bmControls[3];
  uint8_t  iProcessing;
#if     UVC_CLASS_SELECT >= 110
  uint8_t   bmVideoStandards;
#endif
} USB_UVC_VC_PROCESSING_UNIT;

// Extension unit
typedef struct _USB_UVC_VC_EXTENSION_UNIT
{
  USB_DESCRIPTOR_HEADER header;
  uint8_t  bDescriptorSubType;       //0x06
  uint8_t  bUnitID;
  USB_GUID guidExtensionCode;
  uint8_t  bNumControls;
  uint8_t  bNrInPins;
  //uint8_t  baSourceID;      // Nothing from here on is at a fixed offset.
  //uint8_t  bControlSize;
  //uint8_t  bmControls[3];
  //uint8_t  iExtension;
} USB_UVC_VC_EXTENSION_UNIT;

// Output terminal
typedef struct _USB_UVC_VC_OUTPUT_TERMINAL
{
  USB_DESCRIPTOR_HEADER header;
  uint8_t  bDescriptorSubType;       //0x03
  uint8_t  bTerminalID;
  uint16_t wTerminalType;
  uint8_t  bAssocTerminal;
  uint8_t  bSourceID;
  uint8_t  iTerminal;
} USB_UVC_VC_OUTPUT_TERMINAL;

typedef struct _USB_UVC_VS_INPUT_HEADER_DESCRIPTOR
{
  USB_DESCRIPTOR_HEADER header;
  uint8_t  bDescriptorSubType;        //0x01
  uint8_t  bNumFormats;
  uint16_t wTotalLength;
  uint8_t  bEndpointAddress;
  uint8_t  bmInfo;
  uint8_t  bTerminalLink;
  uint8_t  bStillCaptureMethod;
  uint8_t  bTriggerSupport;
  uint8_t  bTriggerUsage;
  uint8_t  bControlSize;
//  uint8_t  bmaControls;       // Added at build time
} USB_UVC_VS_INPUT_HEADER_DESCRIPTOR;

typedef struct _USB_UVC_VS_OUTPUT_HEADER_DESCRIPTOR
{
  USB_DESCRIPTOR_HEADER header;
  uint8_t  bDescriptorSubType;        //0x01
  uint8_t  bNumFormats;
  uint16_t wTotalLength;
  uint8_t  bEndpointAddress;
  uint8_t  bTerminalLink;
  uint8_t  bControlSize;
  //  uint8_t  bmaControls;       // Added at build time
} USB_UVC_VS_OUTPUT_HEADER_DESCRIPTOR;

typedef struct _USB_UVC_VS_FORMAT_UNCOMPRESSED_DESCRIPTOR
{
  USB_DESCRIPTOR_HEADER header;
  uint8_t  bDescriptorSubType;        //0x04
  uint8_t  bFrameIndex;
  uint8_t  bNumFrameDescriptors;
  USB_GUID guidFormat;
  uint8_t  bBitsPerPixel;
  uint8_t  bDefaultFrameIndex;
  uint8_t  bAspectRatioX;
  uint8_t  bAspectRatioY;
  uint8_t  bmInterlaceFlags;
  uint8_t  bCopyProtect;
} USB_UVC_VS_FORMAT_UNCOMPRESSED_DESCRIPTOR;

typedef struct _USB_CONFIGURATION_DESCRIPTOR {
  USB_DESCRIPTOR_HEADER header;
  uint16_t wTotalLength;
  uint8_t bNumInterfaces;
  uint8_t bConfigurationValue;
  uint8_t iConfiguration;
  uint8_t bmAttributes;
  uint8_t MaxPower;
} USB_CONFIGURATION_DESCRIPTOR, *PUSB_CONFIGURATION_DESCRIPTOR;

typedef struct _USB_STRING_DESCRIPTOR {
  USB_DESCRIPTOR_HEADER header;
  //uint16_t bString[1];
} USB_STRING_DESCRIPTOR, *PUSB_STRING_DESCRIPTOR;

typedef struct _USB_BOS_DESCRIPTOR {
  USB_DESCRIPTOR_HEADER header;
  uint16_t  wTotalLength;
  uint8_t bNumDeviceCaps;
} USB_BOS_DESCRIPTOR;

typedef struct _USB_DEVICE_CAPABILITY_DESCRIPTOR {
  USB_DESCRIPTOR_HEADER header;
  uint8_t bDevCapabilityType;
  // uint8_t baCapability[0];
} USB_DEVICE_CAPABILITY_DESCRIPTOR;

typedef struct _UVC_VS_FORMAT_FRAME_DESCRIPTOR
{
  USB_CS_DESCRIPTOR_HEADER header;
  uint8_t bFormatIndex;
  uint8_t bNumFrameDescriptors;
  USB_GUID guidFormat;
  uint8_t bBitsPerPixel;
  uint8_t bDefaultFrameIndex;
  uint8_t bAspectRatioX;
  uint8_t bAspectRatioY;
  uint8_t bmInterlaceFlags;
  uint8_t bCopyProtect;
  uint8_t bVariableSize;
} UVC_VS_FORMAT_FRAME_DESCRIPTOR;

typedef struct _UVC_VS_FORMAT_UNCOMPRESSED_DESCRIPTOR
{
  USB_CS_DESCRIPTOR_HEADER header;
  uint8_t bFormatIndex;
  uint8_t bNumFrameDescriptors;
  USB_GUID guidFormat;
  uint8_t bBitsPerPixel;
  uint8_t bDefaultFrameIndex;
  uint8_t bAspectRatioX;
  uint8_t bAspectRatioY;
  uint8_t bmInterlaceFlags;
  uint8_t bCopyProtect;
} UVC_VS_FORMAT_UNCOMPRESSED_DESCRIPTOR;

typedef struct _UVC_VS_FRAME_FRAME_DESCRIPTOR
{
  USB_CS_DESCRIPTOR_HEADER header;
  uint8_t bFrameIndex;
  uint8_t  bmCapabilities;
  uint16_t wWidth;
  uint16_t wHeight;
  uint32_t dwMinBitRate;
  uint32_t dwMaxBitRate;
  uint32_t dwDefaultFrameInterval;
  uint8_t  bFrameIntervalType;
  uint32_t dwBytesPerLine;

  // .. varies depending on the value of bFrameIntervalType
} UVC_VS_FRAME_FRAME_DESCRIPTOR;

typedef struct _UVC_VS_FRAME_UNCOMPRESSED_DESCRIPTOR
{
  USB_CS_DESCRIPTOR_HEADER header;
  uint8_t   bFrameIndex;
  uint8_t   bmCapabilities;
  uint16_t wWidth;
  uint16_t wHeight;
  uint32_t dwMinBitRate;
  uint32_t dwMaxBitRate;
  uint32_t dwMaxVideoFrameBufferSize;
  uint32_t dwDefaultFrameInterval;
  uint8_t bFrameIntervalType;
  // .. varies depending on the value of bFrameIntervalType
} UVC_VS_FRAME_UNCOMPRESSED_DESCRIPTOR;



typedef struct _USB_VC_CS_INTERFACE_DESCRIPTOR
{
  USB_CS_DESCRIPTOR_HEADER header;
  uint16_t  bcdUVC;
  uint16_t  wTotalLength;
  uint32_t  dwClockFrequency;
  uint8_t   bInCollection;
  // uint8_t   baInterfaceNr[0];               // Must be 'grown' at build time
} USB_VC_CS_INTERFACE_DESCRIPTOR;


typedef struct _USB_VC_CS_INTR_EP_DESCRIPTOR
{
  USB_CS_DESCRIPTOR_HEADER header;
  uint16_t  wMaxTransferSize;
} USB_VC_CS_INTR_EP_DESCRIPTOR;

// UVC Class and Class-specifics:
// A.1
static const uint8_t USB_INTERFACE_CC_VIDEO = 0x0E;

// A.2
static const uint8_t USB_INTERFACE_VC_SC_UNDEFINED = 0x00;
static const uint8_t USB_INTERFACE_VC_SC_VIDEOCONTROL = 0x01;
static const uint8_t USB_INTERFACE_VC_SC_VIDEOSTREAMING = 0x02;
static const uint8_t USB_INTERFACE_VC_SC_VIDEO_INTERFACE_COLLECTION = 0x03;

// A.3
static const uint8_t USB_INTERFACE_VC_PC_PROTOCOL_UNDEFINED = 0x00;
static const uint8_t USB_INTERFACE_VC_PC_PROTOCOL_15 = 0x01;

// A.4
static const uint8_t USB_DESCRIPTOR_TYPE_VC_CS_UNDEFINED = 0x20;
static const uint8_t USB_DESCRIPTOR_TYPE_VC_CS_DEVICE = 0x21;
static const uint8_t USB_DESCRIPTOR_TYPE_VC_CS_CONFIGURATION = 0x22;
static const uint8_t USB_DESCRIPTOR_TYPE_VC_CS_STRING = 0x23;
static const uint8_t USB_DESCRIPTOR_TYPE_VC_CS_INTERFACE = 0x24;
static const uint8_t USB_DESCRIPTOR_TYPE_VC_CS_ENDPOINT = 0x25;

// A.5
static const uint8_t USB_INTERFACE_SUBTYPE_VC_DESCRIPTOR_UNDEFINED = 0x00;
static const uint8_t USB_INTERFACE_SUBTYPE_VC_HEADER = 0x01;
static const uint8_t USB_INTERFACE_SUBTYPE_VC_INPUT_TERMINAL = 0x02;
static const uint8_t USB_INTERFACE_SUBTYPE_VC_OUTPUT_TERMINAL = 0x03;
static const uint8_t USB_INTERFACE_SUBTYPE_VC_SELECTOR_UNIT = 0x04;
static const uint8_t USB_INTERFACE_SUBTYPE_VC_PROCESSING_UNIT = 0x05;
static const uint8_t USB_INTERFACE_SUBTYPE_VC_EXTENSION_UNIT = 0x06;
static const uint8_t USB_INTERFACE_SUBTYPE_VC_ENCODING_UNIT = 0x07;

// A.6
static const uint8_t USB_INTERFACE_SUBTYPE_VS_UNDEFINED = 0x00;
static const uint8_t USB_INTERFACE_SUBTYPE_VS_INPUT_HEADER = 0x01;
static const uint8_t USB_INTERFACE_SUBTYPE_VS_OUTPUT_HEADER = 0x02;
static const uint8_t USB_INTERFACE_SUBTYPE_VS_STILL_IMAGE_FRAME = 0x03;
static const uint8_t USB_INTERFACE_SUBTYPE_VS_FORMAT_UNCOMPRESSED = 0x04;
static const uint8_t USB_INTERFACE_SUBTYPE_VS_FRAME_UNCOMPRESSED = 0x05;
static const uint8_t USB_INTERFACE_SUBTYPE_VS_FORMAT_MJPEG = 0x06;
static const uint8_t USB_INTERFACE_SUBTYPE_VS_FRAME_MJPEG = 0x07;
static const uint8_t USB_INTERFACE_SUBTYPE_VS_FORMAT_MPEG2TS = 0x0A;
static const uint8_t USB_INTERFACE_SUBTYPE_VS_FORMAT_DV = 0x0C;
static const uint8_t USB_INTERFACE_SUBTYPE_VS_COLORFORMAT = 0x0D;
static const uint8_t USB_INTERFACE_SUBTYPE_VS_FORMAT_FRAME_BASED = 0x10;
static const uint8_t USB_INTERFACE_SUBTYPE_VS_FRAME_FRAME_BASED = 0x11;
static const uint8_t USB_INTERFACE_SUBTYPE_VS_FORMAT_STREAM_BASED = 0x12;
static const uint8_t USB_INTERFACE_SUBTYPE_VS_FORMAT_H264 = 0x13;
static const uint8_t USB_INTERFACE_SUBTYPE_VS_FRAME_H264 = 0x14;
static const uint8_t USB_INTERFACE_SUBTYPE_VS_FORMAT_H264_SIMULCAST = 0x15;

// A.7
static const uint8_t USB_VC_SUBTYPE_EP_UNDEFINED = 0x00;
static const uint8_t USB_VC_SUBTYPE_EP_GENERAL = 0x01;
static const uint8_t USB_VC_SUBTYPE_EP_ENDPOINT = 0x02;
static const uint8_t USB_VC_SUBTYPE_EP_INTERRUPT = 0x03;

#pragma pack(pop)
