/* Copyright (c) 2010 - 2013 LEAP Motion. All rights reserved.
 *
 * The intellectual and technical concepts contained herein are proprietary and
 * confidential to Leap Motion, and are protected by trade secret or copyright
 * law. Dissemination of this information or reproduction of this material is
 * strictly forbidden unless prior written permission is obtained from LEAP
 * Motion.
 *
 * File created         Feb. 4, 2014           Chen Zheng
 */

#ifndef USB_DSCR_H_
#define USB_DSCR_H_

#include <cyu3types.h>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Define
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define bMS_VendorCode 1
#ifdef MT9M021
	#define NUM_FRAME_DESCRIPTOR	7
#endif
#ifdef MT9V024
	#define NUM_FRAME_DESCRIPTOR	6
#endif
//structs
typedef struct _USB_DEVICE_DESCRIPTOR
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
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
}__attribute__ ((packed, aligned(1))) USB_DEVICE_DESCRIPTOR;

typedef struct _USB_DEVICE_QUALIFIER_DESCRIPTOR {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t bcdUSB;
    uint8_t  bDeviceClass;
    uint8_t  bDeviceSubClass;
    uint8_t  bDeviceProtocol;
    uint8_t  bMaxPacketSize0;
    uint8_t  bNumConfigurations;
    uint8_t  bReserved;
}__attribute__ ((packed, aligned(1))) USB_DEVICE_QUALIFIER_DESCRIPTOR;

typedef struct _USB_CONFIG_DESCRIPTOR
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t wTotalLength;
    uint8_t  bNumInterfaces;
    uint8_t  bConfigurationValue;
    uint8_t  iConfiguration;
    uint8_t  bmAttributes;
    uint8_t  bMaxPower;
}__attribute__ ((packed, aligned(1)))USB_CONFIG_DESCRIPTOR;

typedef struct _USB_INTERFACE_ASSOCIATION_DESCRIPTOR
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bFirstInterface;
    uint8_t bInterfaceCount;
    uint8_t bFunctionClass;
    uint8_t bFunctionSubClass;
    uint8_t bFunctionProtocol;
    uint8_t iFunction;
}__attribute__ ((packed, aligned(1)))USB_INTERFACE_ASSOCIATION_DESCRIPTOR;

typedef struct _USB_INTERFACE_DESCRIPTOR
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
}__attribute__ ((packed, aligned(1)))USB_INTERFACE_DESCRIPTOR;

typedef struct _USB_SS_EP_COMPANION_DESCRIPTOR          // Question: Should this be used?????
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;       //0x30
    uint8_t  bMaxBurst;
    uint8_t  bmAttributes;
    uint16_t wBytesPerInterval;
}__attribute__ ((packed))USB_SS_EP_COMPANION_DESCRIPTOR;

typedef struct _GUID {
    uint32_t dwData1;
    uint16_t dwData2;
    uint16_t dwData3;
    uint8_t  dwData4[8];
}__attribute__ ((packed, aligned(1)))GUID;

typedef struct _USB_ENDPOINT_DESCRIPTOR
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bEndpointAddress;
    uint8_t  bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t  bInterval;
}__attribute__ ((packed, aligned(1)))USB_ENDPOINT_DESCRIPTOR;

typedef struct _USB_CLASS_SPECIFIC_INTERRUPT_ENDPOINT_DESCRIPTOR
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubType;
    uint16_t wMaxTransferSize;
}__attribute__ ((packed, aligned(1)))USB_CLASS_SPECIFIC_INTERRUPT_ENDPOINT_DESCRIPTOR;

typedef struct _USB_UVC_VC_HEADER_DESCRIPTOR
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;            //0x24
    uint8_t  bDescriptorSubType;       //0x01
    uint16_t bcdUVC;
    uint16_t wTotalLength;
    uint32_t dwClockFrequency;
    uint8_t  bInCollection;
    uint8_t  baInterfaceNr;
}__attribute__ ((packed, aligned(1)))USB_UVC_VC_HEADER_DESCRIPTOR;

// Input terminal
typedef struct _USB_UVC_VC_INPUT_TERMINAL
{
    uint8_t  bLength;
    uint8_t  bDescriptorTypr;            //0x24
    uint8_t  bDescriptorSubType;       //0x02
    uint8_t  bTerminalID;
    uint16_t wTerminalType;
    uint8_t  bAssocTerminal;
    uint8_t  iTerminal;
    uint16_t wOpticalZoom0;             // Not used?
    uint16_t wOpticalZoom1;             // Not used?
    uint16_t wOpticalZoom2;             // Not used?
    uint8_t  bControlBitfieldSize;      // Not used?
    uint8_t  bmControls[3];              // Not used?
}__attribute__ ((packed, aligned(1)))USB_UVC_VC_INPUT_TERMINAL;

// Processing unit
typedef struct _USB_UVC_VC_PROCESSING_UNIT           //TODO: does not parse??? Chen
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;            //0x24
    uint8_t  bDescriptorSubType;       //0x05
    uint8_t  bUnitID;
    uint8_t  bSourceID;
    uint16_t wMaxMultiplier;
    uint8_t  bControlSize;
    uint8_t  bmControls[3];
    uint8_t  iTerminal;
}__attribute__ ((packed, aligned(1)))USB_UVC_VC_PROCESSING_UNIT;

// Extension unit
typedef struct _USB_UVC_VC_EXTENSION_UNIT
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;            //0x24
    uint8_t  bDescriptorSubType;       //0x06
    uint8_t  bUnitID;
    GUID     guidExtensionCode;
    uint8_t  bNumControls;
    uint8_t  bNrInPins;
    uint8_t  baSourceID;
    uint8_t  bControlSize;
    uint8_t  bmControls[3];
    uint8_t  iExtension;
}__attribute__ ((packed, aligned(1)))USB_UVC_VC_EXTENSION_UNIT;

// Output terminal
typedef struct _USB_UVC_VC_OUTPUT_TERMINAL
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;          //0x24
    uint8_t  bDescriptorSubType;       //0x03
    uint8_t  bTerminalID;
    uint16_t wTerminalType;
    uint8_t  bAssocTerminal;
    uint8_t  bSourceID;
    uint8_t  iTerminal;
}__attribute__ ((packed, aligned(1)))USB_UVC_VC_OUTPUT_TERMINAL;

typedef struct _USB_UVC_VS_HEADER_DESCRIPTOR
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;            //0x24
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
    uint8_t  bmaControls;
}__attribute__ ((packed, aligned(1)))USB_UVC_VS_HEADER_DESCRIPTOR;

typedef struct _USB_UVC_VS_FORMAT_UNCOMPRESSED_DESCRIPTOR
{
    uint8_t  bLength;                   //0x1B
    uint8_t  bDescriptorType;           //0x24
    uint8_t  bDescriptorSubType;        //0x04
    uint8_t  bFrameIndex;
    uint8_t  bNumFrameDescriptors;
    GUID     guidFormat;
    uint8_t  bBitsPerPixel;
    uint8_t  bDefaultFrameIndex;
    uint8_t  bAspectRatioX;
    uint8_t  bAspectRatioY;
    uint8_t  bmInterlaceFlags;
    uint8_t  bCopyProtect;
}__attribute__ ((packed, aligned(1)))USB_UVC_VS_FORMAT_UNCOMPRESSED_DESCRIPTOR;

typedef struct _UVC_VS_FRAME_UNCOMPRESSED_DESCRIPTOR
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;           //0x24
    uint8_t  bDescriptorSubType;        //0x05
    uint8_t  bFrameIndex;
    uint8_t  bmCapabilities;
    uint16_t wWidth;
    uint16_t wHeight;
    uint32_t dwMinBitRate;
    uint32_t dwMaxBitRate;
    uint32_t dwMaxVideoFrameBufferSize;
    uint32_t dwDefaultFrameInterval;
    uint8_t  bFrameIntervalType;
    uint32_t dwFrameInterval;
}__attribute__ ((packed, aligned(1)))UVC_VS_FRAME_UNCOMPRESSED_DESCRIPTOR;

// Configuration processes
typedef struct _USB_HS_CONFIGURATION_PROCESS
{
    USB_CONFIG_DESCRIPTOR                               configDesc;
    USB_INTERFACE_ASSOCIATION_DESCRIPTOR                interfaceAssocDesc;
    USB_INTERFACE_DESCRIPTOR                            vcInterfaceDesc;
    USB_UVC_VC_HEADER_DESCRIPTOR                        vcHeaderDesc;
    USB_UVC_VC_INPUT_TERMINAL                           vcITDesc;
    USB_UVC_VC_PROCESSING_UNIT                          vcPUDesc;
    USB_UVC_VC_EXTENSION_UNIT                           vcEUDesc;
    USB_UVC_VC_OUTPUT_TERMINAL                          vcOTDesc;
    USB_ENDPOINT_DESCRIPTOR                             vcEndpointDesc;
    USB_CLASS_SPECIFIC_INTERRUPT_ENDPOINT_DESCRIPTOR    csInterruptEndpointDesc;
    USB_INTERFACE_DESCRIPTOR                            vsInterfaceDesc;
    USB_UVC_VS_HEADER_DESCRIPTOR                        vsHeaderDesc;
    USB_UVC_VS_FORMAT_UNCOMPRESSED_DESCRIPTOR           vsFormatUncompressedDesc;
    UVC_VS_FRAME_UNCOMPRESSED_DESCRIPTOR                vsFrameUncompressedDesc[NUM_FRAME_DESCRIPTOR];
    USB_ENDPOINT_DESCRIPTOR                             vsEndpointDesc;
}__attribute__ ((packed, aligned(1))) USB_HS_CONFIGURATION_PROCESS;

typedef struct _USB_SS_CONFIGURATION_PROCESS
{
    USB_CONFIG_DESCRIPTOR                               configDesc;
    USB_INTERFACE_ASSOCIATION_DESCRIPTOR                interfaceAssocDesc;
    USB_INTERFACE_DESCRIPTOR                            vcInterfaceDesc;
    USB_UVC_VC_HEADER_DESCRIPTOR                        vcHeaderDesc;
    USB_UVC_VC_INPUT_TERMINAL                           vcITDesc;
    USB_UVC_VC_PROCESSING_UNIT                          vcPUDesc;
    USB_UVC_VC_EXTENSION_UNIT                           vcEUDesc;
    USB_UVC_VC_OUTPUT_TERMINAL                          vcOTDesc;
    USB_ENDPOINT_DESCRIPTOR                             vcEndpointDesc;
    USB_SS_EP_COMPANION_DESCRIPTOR                      vcSSCompanionDesc;
    USB_CLASS_SPECIFIC_INTERRUPT_ENDPOINT_DESCRIPTOR    csInterruptEndpointDesc;
    USB_INTERFACE_DESCRIPTOR                            vsInterfaceDesc;
    USB_UVC_VS_HEADER_DESCRIPTOR                        vsHeaderDesc;
    USB_UVC_VS_FORMAT_UNCOMPRESSED_DESCRIPTOR           vsFormatUncompressedDesc;
    UVC_VS_FRAME_UNCOMPRESSED_DESCRIPTOR                vsFrameUncompressedDesc[NUM_FRAME_DESCRIPTOR];
    USB_ENDPOINT_DESCRIPTOR                             vsEndpointDesc;
    USB_SS_EP_COMPANION_DESCRIPTOR                      vsSSCompanionDesc;
}__attribute__ ((packed, aligned(1))) USB_SS_CONFIGURATION_PROCESS;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Global Variables
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
extern const uint8_t CyFxUsbOSDscrWinUSB[];
extern const uint8_t CyFxUsbOSDscrWinUSB_ExtendedCompatID[];
extern const uint8_t CyFxUsbOSDscrWinUSB_ExtendedProperties[];
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Function Declaration
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Set_Enumeration_Descriptors();
void Descriptor_Data_Init();

#endif
