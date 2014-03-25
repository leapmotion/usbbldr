#pragma once
#include "UVC.h"
#include <stdlib.h>
#pragma pack(push, 1)

/// <summary>
/// Association descriptor
/// </summary>
struct uvc_if_assoc_desc:
  usb_descriptor_header
{
  BYTE bFirstInterface;
  BYTE bInterfaceCount;
  BYTE bFunctionClass;
  BYTE bFunctionSubClass;
  BYTE bFunctionProtocol;
  BYTE iFunction;
};

/// <summary>
/// Standard VC interface descriptor
/// </summary>
struct uvc_vc_if_desc:
  usb_if_desc
{
};

/// <summary>
/// Base part of all Video Control descriptors, detailed in A.5:
/// </summary>
enum uvc_vc_desc_subtype : char {
	UVC_VC_DESCRIPTOR_UNDEFINED = 0x00,
	UVC_VC_HEADER = 0x01,
	UVC_VC_INPUT_TERMINAL = 0x02,
	UVC_VC_OUTPUT_TERMINAL = 0x03,
	UVC_VC_SELECTOR_UNIT = 0x04,
	UVC_VC_PROCESSING_UNIT = 0x05,
	UVC_VC_EXTENSION_UNIT = 0x06
};
struct uvc_vc_descriptor_header:
  usb_descriptor_header
{
  uvc_vc_desc_subtype bDescriptorSubType;
};
static_assert(sizeof(uvc_vc_descriptor_header) == 3, "VC interface descriptor header size mismatch");

struct uvc_vc_if_hdr_desc:
  uvc_vc_descriptor_header
{
  WORD bcdUVC;
  WORD wTotalLength;
  DWORD dwClockFrequency;
  BYTE bInCollection;
  BYTE baInterfaceNr[1];
};

struct uvc_input_terminal_desc:
  uvc_vc_descriptor_header
{
  BYTE bTerminalID;
  WORD wTerminalType;
  BYTE bAssocTerminal;
  BYTE iTerminal;
};

struct uvc_input_terminal_uncompressed_desc:
  uvc_input_terminal_desc
{
};

struct uvc_processing_unit_desc:
  uvc_vc_descriptor_header
{
  BYTE bUnitID;
  BYTE bSourceID;
  WORD wMaxMultiplier;
  BYTE bControlSize;
  struct {
    unsigned char Brightness:1;
    unsigned char Contrast:1;
    unsigned char Hue:1;
    unsigned char Saturation:1;
    unsigned char Sharpness:1;
    unsigned char Gamma:1;
    unsigned char WBT:1;
    unsigned char WBC:1;
    unsigned char BacklightCompensation:1;
    unsigned char Gain:1;
    unsigned char PowerLineFreq:1;
    unsigned char HueAuto:1;
    unsigned char WBTAuto:1;
    unsigned char WBCAuto:1;
    unsigned char DigitalMultipler:1;
    unsigned char DigitalMultiplerLimit:1;
    unsigned char AnalogVideoStandard:1;
    unsigned char AnalogVideoLockStatus:1;
    unsigned char ContrastAuto:1;
    unsigned char:5;
  } bmControls;
};
static_assert(sizeof(uvc_processing_unit_desc) == 11, "Short unit descriptor not short enough for typical use");

struct uvc_processing_unit_desc_full:
  uvc_processing_unit_desc
{
  BYTE iProcessing;
  struct {
    unsigned char None:1;
    unsigned char NTSC525:1;
    unsigned char PAL625:1;
    unsigned char SECAM:1;
    unsigned char NTSC625:1;
    unsigned char PAL525:1;
    unsigned char:2;
  } bmVideoStandards;
};
static_assert(sizeof(uvc_processing_unit_desc_full) == 13, "Processing unit descriptor not compliant with section 3.7.2.5");


enum uvc_vc_frame_interval_type: char {
  uvc_vc_frame_interval_continuous
};
struct uvc_vc_frame_uncompressed:
  uvc_vc_descriptor_header
{
  BYTE bFrameIndex;
  struct {
    unsigned char IsStillImageSupported:1;
    unsigned char IsFixedFrameRate:1;
    unsigned char:6;
  } bmCapabilities;
  WORD wWidth;
  WORD wHeight;
  DWORD dwMinBitRate;
  DWORD dwMaxBitRate;
  unsigned int:32;
  t_tenthUS dwDefaultFrameInterval;
  uvc_vc_frame_interval_type bFrameIntervalType;
};
static_assert(sizeof(uvc_vc_frame_uncompressed) == 26, "Frame structure size mismatch");


/// <summary>
/// Base part of all Video Streaming descriptors, detailed in A.6:
/// </summary>
enum uvc_vs_desc_subtype : char {
	UVC_VS_UNDEFINED = 0x00,
	UVC_VS_INPUT_HEADER = 0x01,
	UVC_VS_OUTPUT_HEADER = 0x02,
	UVC_VS_STILL_IMAGE_FRAME = 0x03,
	UVC_VS_FORMAT_UNCOMPRESSED = 0x04,
	UVC_VS_FRAME_UNCOMPRESSED = 0x05,
	UVC_VS_FORMAT_MJPEG = 0x06,
	UVC_VS_FRAME_MJPEG = 0x07,
	UVC_VS_FORMAT_MPEG2TS = 0x0a,
	UVC_VS_FORMAT_DV = 0x0c,
	UVC_VS_COLORFORMAT = 0x0d,
	UVC_VS_FORMAT_FRAME_BASED = 0x10,
	UVC_VS_FRAME_FRAME_BASED = 0x11,
	UVC_VS_FORMAT_STREAM_BASED = 0x12
};

/// <summary>
/// Standard VC interface descriptor
/// </summary>
struct uvc_vs_if_desc:
  usb_if_desc
{
};

/// <summary>
/// All subsequent video streaming descriptor headers
/// </summary>
struct uvc_vs_descriptor_header:
  usb_descriptor_header
{
  uvc_vs_desc_subtype bDescriptorSubType;
};
static_assert(sizeof(uvc_vs_descriptor_header) == 3, "VS interface descriptor header size mismatch");

struct uvc_vs_input_header:
  uvc_vs_descriptor_header
{
  BYTE bNumFormats;
  WORD wTotalLength;
  BYTE bEndpointAddress;
  BYTE bmInfo;
  BYTE bTerminalLink;
  BYTE bStillCaptureMethod;
  BYTE bTriggerSupport;
  BYTE bTriggerUsage;
};

enum uvc_vc_uncompressed_frame_pattern: char {
  frame_pattern_field1 = 0,
  frame_pattern_field2 = 1,
  frame_pattern_regular = 2,
  frame_pattern_random = 3
};
struct uvc_vs_format_uncompressed:
  uvc_vs_descriptor_header
{
  BYTE bFormatIndex;
  BYTE bNumFrameDescriptors;
  GUID guidFormat;
  BYTE bBitsPerPixel;
  BYTE bDefaultFrameIndex;
  BYTE bAspectRatioX;
  BYTE bAspectRatioY;
  struct {
    unsigned char IsInterlaced:1;
    unsigned char UseTwoFieldsPerFrame:1;
    unsigned char IsFieldOneFirst:1;
    unsigned char:1;
    uvc_vc_uncompressed_frame_pattern FieldPattern:2;
    unsigned char:2;
  } bmInterlaceFlags;
  BYTE bCopyProtect;
};
static_assert(sizeof(uvc_vs_format_uncompressed) == 27, "Format structure size mismatch");

struct uvc_xu_descriptor_header:
  uvc_vc_descriptor_header
{
  // Unique identifier for this unit
  BYTE bUnitID;

  // Extension unit GUID
  GUID guidExtensionCode;

  // Number of controls in this unit
  BYTE bNumControls;

  // Number of input pins
  BYTE bNrInPins;

  // Input terminal identifiers
  BYTE inTerminalIDs[1];
};
static_assert(offsetof(uvc_xu_descriptor_header, bUnitID) == 3, "Unit identifer offset incorrect");

#pragma pack(pop)