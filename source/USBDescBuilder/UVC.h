#pragma once
#include "USB.h"
#include <boost/chrono/duration.hpp>

// Utility type representing a tenth of a microsecond, or one 100-ns interval
typedef boost::chrono::duration<uint32_t, boost::ratio<1, 1000 * 1000 * 10>> t_tenthUS;

// 1.0 control struct
#pragma pack(push, 1)
struct uvc_stream_ctrl_t {
	uint16_t bmHint;				//0
	uint8_t bFormatIndex;			//2
	uint8_t bFrameIndex;			//3
	uint32_t dwFrameInterval;		//4
	uint16_t wKeyFrameRate;			//8
	uint16_t wPFrameRate;			//10
	uint16_t wCompQuality;			//12
	uint16_t wCompWindowSize;		//14
	uint16_t wDelay;				//16
	uint32_t dwMaxVideoFrameSize;	//18
	uint32_t dwMaxPayloadTransferSize; //22
};

/** Color coding of stream, transport-independent
 * @ingroup streaming
 */
enum uvc_color_format {
	UVC_COLOR_FORMAT_UNKNOWN = 0,
	UVC_COLOR_FORMAT_UNCOMPRESSED,
	UVC_COLOR_FORMAT_COMPRESSED,
	/** YUVV/YUV2/YUV422: YUV encoding with one luminance value per pixel and
	 * one UV (chrominance) pair for every two pixels.
	 */
	UVC_COLOR_FORMAT_YUYV,
	UVC_COLOR_FORMAT_UYVY,
	/** 24-bit RGB */
	UVC_COLOR_FORMAT_RGB,
	UVC_COLOR_FORMAT_BGR,
	/** Motion-JPEG (or JPEG) encoded images */
	UVC_COLOR_FORMAT_MJPEG,
};

/** UVC request code (A.8) */
enum uvc_req_code {
	UVC_RC_UNDEFINED = 0x00,
	UVC_SET_CUR = 0x01,
	UVC_GET_CUR = 0x81,
	UVC_GET_MIN = 0x82,
	UVC_GET_MAX = 0x83,
	UVC_GET_RES = 0x84,
	UVC_GET_LEN = 0x85,
	UVC_GET_INFO = 0x86,
	UVC_GET_DEF = 0x87
};

enum uvc_device_power_mode {
	UVC_VC_VIDEO_POWER_MODE_FULL = 0x000b,
	UVC_VC_VIDEO_POWER_MODE_DEVICE_DEPENDENT = 0x001b,
};

/** Camera terminal control selector (A.9.4) */
enum uvc_ct_ctrl_selector {
	UVC_CT_CONTROL_UNDEFINED = 0x00,
	UVC_CT_SCANNING_MODE_CONTROL = 0x01,
	UVC_CT_AE_MODE_CONTROL = 0x02,
	UVC_CT_AE_PRIORITY_CONTROL = 0x03,
	UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL = 0x04,
	UVC_CT_EXPOSURE_TIME_RELATIVE_CONTROL = 0x05,
	UVC_CT_FOCUS_ABSOLUTE_CONTROL = 0x06,
	UVC_CT_FOCUS_RELATIVE_CONTROL = 0x07,
	UVC_CT_FOCUS_AUTO_CONTROL = 0x08,
	UVC_CT_IRIS_ABSOLUTE_CONTROL = 0x09,
	UVC_CT_IRIS_RELATIVE_CONTROL = 0x0a,
	UVC_CT_ZOOM_ABSOLUTE_CONTROL = 0x0b,
	UVC_CT_ZOOM_RELATIVE_CONTROL = 0x0c,
	UVC_CT_PANTILT_ABSOLUTE_CONTROL = 0x0d,
	UVC_CT_PANTILT_RELATIVE_CONTROL = 0x0e,
	UVC_CT_ROLL_ABSOLUTE_CONTROL = 0x0f,
	UVC_CT_ROLL_RELATIVE_CONTROL = 0x10,
	UVC_CT_PRIVACY_CONTROL = 0x11
};

enum uvc_scanning_mode: char {
  uvc_scanning_mode_interlaced,
  uvc_scanning_mode_progressive
};

/** Processing unit control selector (A.9.5) */
enum uvc_pu_ctrl_selector {
	UVC_PU_CONTROL_UNDEFINED = 0x00,
	UVC_PU_BACKLIGHT_COMPENSATION_CONTROL = 0x01,
	UVC_PU_BRIGHTNESS_CONTROL = 0x02,
	UVC_PU_CONTRAST_CONTROL = 0x03,
	UVC_PU_GAIN_CONTROL = 0x04,
	UVC_PU_POWER_LINE_FREQUENCY_CONTROL = 0x05,
	UVC_PU_HUE_CONTROL = 0x06,
	UVC_PU_SATURATION_CONTROL = 0x07,
	UVC_PU_SHARPNESS_CONTROL = 0x08,
	UVC_PU_GAMMA_CONTROL = 0x09,
	UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL = 0x0a,
	UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL = 0x0b,
	UVC_PU_WHITE_BALANCE_COMPONENT_CONTROL = 0x0c,
	UVC_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL = 0x0d,
	UVC_PU_DIGITAL_MULTIPLIER_CONTROL = 0x0e,
	UVC_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL = 0x0f,
	UVC_PU_HUE_AUTO_CONTROL = 0x10,
	UVC_PU_ANALOG_VIDEO_STANDARD_CONTROL = 0x11,
	UVC_PU_ANALOG_LOCK_STATUS_CONTROL = 0x12
};

enum uvc_status_class {
	UVC_STATUS_CLASS_CONTROL = 0x10,
	UVC_STATUS_CLASS_CONTROL_CAMERA = 0x11,
	UVC_STATUS_CLASS_CONTROL_PROCESSING = 0x12,
};

enum uvc_status_attribute {
	UVC_STATUS_ATTRIBUTE_VALUE_CHANGE = 0x00,
	UVC_STATUS_ATTRIBUTE_INFO_CHANGE = 0x01,
	UVC_STATUS_ATTRIBUTE_FAILURE_CHANGE = 0x02,
	UVC_STATUS_ATTRIBUTE_UNKNOWN = 0xff
};

/** Video interface protocol code (A.3) */
enum uvc_int_proto_code {
	UVC_PC_PROTOCOL_UNDEFINED = 0x00
};

/** UVC endpoint descriptor subtype (A.7) */
enum uvc_ep_desc_subtype {
	UVC_EP_UNDEFINED = 0x00,
	UVC_EP_GENERAL = 0x01,
	UVC_EP_ENDPOINT = 0x02,
	UVC_EP_INTERRUPT = 0x03
};

/** VideoControl interface control selector (A.9.1) */
enum uvc_vc_ctrl_selector {
	UVC_VC_CONTROL_UNDEFINED = 0x00,
	UVC_VC_VIDEO_POWER_MODE_CONTROL = 0x01,
	UVC_VC_REQUEST_ERROR_CODE_CONTROL = 0x02
};

/** Terminal control selector (A.9.2) */
enum uvc_term_ctrl_selector {
	UVC_TE_CONTROL_UNDEFINED = 0x00
};

/** Selector unit control selector (A.9.3) */
enum uvc_su_ctrl_selector {
	UVC_SU_CONTROL_UNDEFINED = 0x00,
	UVC_SU_INPUT_SELECT_CONTROL = 0x01
};

/** Extension unit control selector (A.9.6) */
enum uvc_xu_ctrl_selector {
	UVC_XU_CONTROL_UNDEFINED = 0x00
};

/** VideoStreaming interface control selector (A.9.7) */
enum uvc_vs_ctrl_selector {
	UVC_VS_CONTROL_UNDEFINED = 0x00,
	UVC_VS_PROBE_CONTROL = 0x01,
	UVC_VS_COMMIT_CONTROL = 0x02,
	UVC_VS_STILL_PROBE_CONTROL = 0x03,
	UVC_VS_STILL_COMMIT_CONTROL = 0x04,
	UVC_VS_STILL_IMAGE_TRIGGER_CONTROL = 0x05,
	UVC_VS_STREAM_ERROR_CODE_CONTROL = 0x06,
	UVC_VS_GENERATE_KEY_FRAME_CONTROL = 0x07,
	UVC_VS_UPDATE_FRAME_SEGMENT_CONTROL = 0x08,
	UVC_VS_SYNC_DELAY_CONTROL = 0x09
};


/** USB terminal type (B.1) */
enum uvc_term_type {
	UVC_TT_VENDOR_SPECIFIC = 0x0100,
	UVC_TT_STREAMING = 0x0101
};

/** Input terminal type (B.2) */
enum uvc_it_type {
	UVC_ITT_VENDOR_SPECIFIC = 0x0200,
	UVC_ITT_CAMERA = 0x0201,
	UVC_ITT_MEDIA_TRANSPORT_INPUT = 0x0202
};

/** Output terminal type (B.3) */
enum uvc_ot_type {
	UVC_OTT_VENDOR_SPECIFIC = 0x0300,
	UVC_OTT_DISPLAY = 0x0301,
	UVC_OTT_MEDIA_TRANSPORT_OUTPUT = 0x0302
};

/** External terminal type (B.4) */
enum uvc_et_type {
	UVC_EXTERNAL_VENDOR_SPECIFIC = 0x0400,
	UVC_COMPOSITE_CONNECTOR = 0x0401,
	UVC_SVIDEO_CONNECTOR = 0x0402,
	UVC_COMPONENT_CONNECTOR = 0x0403
};

/** Status packet type (2.4.2.2) */
enum uvc_status_type {
	UVC_STATUS_TYPE_CONTROL = 1,
	UVC_STATUS_TYPE_STREAMING = 2
};

/** Payload header flags (2.4.3.3) */
#define UVC_STREAM_EOH (1 << 7)
#define UVC_STREAM_ERR (1 << 6)
#define UVC_STREAM_STI (1 << 5)
#define UVC_STREAM_RES (1 << 4)
#define UVC_STREAM_SCR (1 << 3)
#define UVC_STREAM_PTS (1 << 2)
#define UVC_STREAM_EOF (1 << 1)
#define UVC_STREAM_FID (1 << 0)

/** Control capabilities (4.1.2) */
#define UVC_CONTROL_CAP_GET (1 << 0)
#define UVC_CONTROL_CAP_SET (1 << 1)
#define UVC_CONTROL_CAP_DISABLED (1 << 2)
#define UVC_CONTROL_CAP_AUTOUPDATE (1 << 3)
#define UVC_CONTROL_CAP_ASYNCHRONOUS (1 << 4)

struct uvc_format_desc;
struct uvc_frame_desc;
struct uvc_streaming_interface;
struct uvc_device_info;

/** Format descriptor
 *
 * A "format" determines a stream's image type (e.g., raw YUYV or JPEG)
 * and includes many "frame" configurations.
 */
typedef struct uvc_format_desc {
	struct uvc_streaming_interface *parent;
	struct uvc_format_desc *prev, *next;
	/** Type of image stream, such as JPEG or uncompressed. */
	enum uvc_vs_desc_subtype bDescriptorSubtype;
	/** Identifier of this format within the VS interface's format list */
	unsigned char bFormatIndex;
	union {
		/** Format and depth specification for uncompressed stream */
		struct {
			unsigned char guidFormat[16];
			unsigned char bBitsPerPixel;
		};
		/** Flags for JPEG stream */
		unsigned char bmFlags;
	};
	/** Default {uvc_frame_desc} to choose given this format */
	unsigned char bDefaultFrameIndex;
	unsigned char bAspectRatioX;
	unsigned char bAspectRatioY;
	unsigned char bmInterlaceFlags;
	unsigned char bCopyProtect;
	/** Available frame specifications for this format */
	struct uvc_frame_desc *frame_descs;
} uvc_format_desc_t;


/** VideoStream interface */
typedef struct uvc_streaming_interface {
	struct uvc_device_info *parent;
	struct uvc_streaming_interface *prev, *next;
	/** Interface number */
	unsigned char bInterfaceNumber;
	/** Video formats that this interface provides */
	struct uvc_format_desc *format_descs;
	/** USB endpoint to use when communicating with this interface */
	unsigned char bEndpointAddress;
	unsigned char bTerminalLink;
} uvc_streaming_interface_t;

/** Representation of the interface that brings data into the UVC device */
typedef struct uvc_input_terminal {
	struct uvc_input_terminal *prev, *next;
	/** Index of the terminal within the device */
	unsigned char bTerminalID;
	/** Type of terminal (e.g., camera) */
	enum uvc_it_type wTerminalType;
	unsigned short wObjectiveFocalLengthMin;
	unsigned short wObjectiveFocalLengthMax;
	unsigned short wOcularFocalLength;
	/** Camera controls (meaning of bits given in {uvc_ct_ctrl_selector}) */
	unsigned long bmControls;
} uvc_input_terminal_t;

typedef struct uvc_output_terminal {
	struct uvc_output_terminal *prev, *next;
} uvc_output_terminal_t;

/** Represents post-capture processing functions */
typedef struct uvc_processing_unit {
	struct uvc_processing_unit *prev, *next;
	/** Index of the processing unit within the device */
	unsigned char bUnitID;
	/** Index of the terminal from which the device accepts images */
	unsigned char bSourceID;
	/** Processing controls (meaning of bits given in {uvc_pu_ctrl_selector}) */
	unsigned long bmControls;
} uvc_processing_unit_t;

typedef struct uvc_control_interface {
	struct uvc_device_info *parent;
	struct uvc_input_terminal *input_term_descs;
	// struct uvc_output_terminal *output_term_descs;
	struct uvc_processing_unit *processing_unit_descs;
	struct uvc_extension_unit *extension_unit_descs;
	unsigned short bcdUVC;
	unsigned char bEndpointAddress;
} uvc_control_interface_t;

struct uvc_vs_probe_control {
  struct {
    unsigned char bdwFrameInterval:1;
    unsigned char bwKeyFrameRate:1;
    unsigned char bwPFrameRate:1;
    unsigned char bwCompQuality:1;
    unsigned char bwCompWindowSize:1;
    unsigned char:0;
    unsigned char:8;
  } bmHint;
  uint8_t bFormatIndex;
  uint8_t bFrameIndex;

  t_tenthUS dwFrameInterval;
  uint16_t wKeyFrameRate;
  uint16_t wPFrameRate;
  uint16_t wCompQuality;
  uint16_t wCompWindowSize;
  uint16_t wDelay;
  uint32_t dwMaxVideoFrameSize;
  uint32_t dwMaxPayloadTransferSize;
};
static_assert(sizeof(uvc_vs_probe_control) == 26, "UVC probe control is incorrectly sized");

struct uvc_vs_probe_control_full:
  uvc_vs_probe_control
{
  struct {
    unsigned char FIDRequired:1;
    unsigned char EndOfFrame:1;
    unsigned char EndOfSlice:1;
    unsigned char:0;
  } bmFramingInfo;
  
  uint8_t bPreferredVersion;
  uint8_t bMinVersion;
  uint8_t bMaxVersion;
  uint8_t bUsage;
  uint8_t bBitDepthLuma;
  uint8_t bmSettings;
  uint8_t bMaxNumberOfRefFramesPlus1;
  struct {
    unsigned char s1Rate:4;
    unsigned char s2Rate:4;
    unsigned char s3Rate:4;
    unsigned char s4Rate:4;
  } bmRateControlModes;
  struct {
    unsigned s1Structure:16;
    unsigned s2Structure:16;
    unsigned s3Structure:16;
    unsigned s4Structure:16;
  } bmLayoutPerStream;
  uint32_t dwClockFrequency;
};

static_assert(sizeof(uvc_vs_probe_control_full) == 48, "UVC probe control is incorrectly sized");

union uvc_ae_mode_control {
  struct {
    unsigned bManualMode:1;
    unsigned bAutoMode:1;
    unsigned shutterPriority:1;
    unsigned aperturePriority:1;
    unsigned reserved:4;
  };
  unsigned char data;
};

/// <summary>
/// Composes a default SET_CUR setup packet on the specified type
/// </summary>
template<class T>
struct USB_UVC_SET_CUR:
  USB_SETUP_PACKET
{
  USB_UVC_SET_CUR(void) {
    Dir = BMREQUEST_DIR_HOST_TO_DEVICE;
    Type = BMREQUEST_TYPE_CLASS;
    Recipient = BMREQUEST_RECIPIENT_INTERFACE;
    Reserved = 0;
    Request = UVC_SET_CUR;
    Length = sizeof(T);
  }
};

template<class T, int REQUEST> struct USB_UVC_SETUP_PACKET;

template<class T>
struct USB_UVC_SETUP_PACKET<T, UVC_RC_UNDEFINED>:
  USB_SETUP_PACKET
{
  USB_UVC_SETUP_PACKET(void) {
    Type = BMREQUEST_TYPE_CLASS;
    Recipient = BMREQUEST_RECIPIENT_INTERFACE;
    Reserved = 0;
    Length = sizeof(T);
  }
};

/// <summary>
/// Composes a default SET_CUR setup packet on the specified type
/// </summary>
template<class T>
struct USB_UVC_SETUP_PACKET<T, UVC_SET_CUR>:
  USB_UVC_SETUP_PACKET<T, UVC_RC_UNDEFINED>
{
  USB_UVC_SETUP_PACKET(int prop, int terminal) {
    Dir = BMREQUEST_DIR_HOST_TO_DEVICE;
    Request = UVC_SET_CUR;
    Value = prop << 8;
    Index = terminal << 8;
  }
};

/// <summary>
/// Composes a default GET_CUR setup packet on the specified type
/// </summary>
template<class T>
struct USB_UVC_SETUP_PACKET<T, UVC_GET_CUR>:
  USB_UVC_SETUP_PACKET<T, UVC_RC_UNDEFINED>
{
  USB_UVC_SETUP_PACKET(int prop, int terminal) {
    Dir = BMREQUEST_DIR_DEVICE_TO_HOST;
    Request = UVC_GET_CUR;
    Value = prop << 8;
    Index = terminal << 8;
  }
};

#pragma pack(pop)
