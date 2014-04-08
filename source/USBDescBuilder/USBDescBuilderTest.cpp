#include "USBDescBuilderTest.h"
#include "usbdescbuilder.h"

static const uint8_t  CAMERA_OUTPUT_TERMINAL = 0x02;
static const uint8_t  VIDEO_PU_UNIT = 0x05;
static const uint8_t  VC_EXTENSION_UNIT = 0x06;
static const GUID     EXTENSION_GUID = {  0x8E9093EF, 0x97EA, 0x49E1, { 0x83, 0x06, 0x9F, 0x6B, 0x69, 0x6A, 0x1A, 0xEE } };
static const GUID     VS_FORMAT_UNCOMPRESSED = { 0x32595559, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 } };

static const uint32_t UNCOMPRESSED_FRAME_FORMATS = 7;   // XXX this is only 6 in the project
static void
createUSB30Configuration(void)
{
#define IS_OK(s)  ((s) == USBDESCBLDR_OK)
#define FLAG(s)   do { fprintf(stderr, "%s:%u: failure (%u)\n", __FILE__, __LINE__, (s)); } while (0)
  usbdescbldr_item_t
    it_device,              // device descr.
    it_dev_qualifier,       // Device qualifier (SS)
    it_config,              // device configuration
    it_iad,                 // interface association descr.
    it_vc_if,               // Video Control: interface descr.
    it_vs_if,               // Video Streaming: interface descr.
    it_vc_hdr,              // vc: header descr.
    it_vc_input,            // vc: input terminal descr.
    it_vc_pu,               // vc: processing unit descr.
    it_vc_eu,               // vc: extension unit descr.
    it_vc_output,           // vc: output terminal descr.
    it_vc_ep,               // vc: endpoint descr.
    it_vs_ep,               // vs: endpoint descr.
    it_cs_ep_intr,          // class-specific endpoint, interrupt descr.
    it_vs_header,           // vs: interface 'header' interf. descr.
    it_vs_fmt_uncomp,       // vs: format (uncompressed) descr.
    it_vs_frm_uncomp[UNCOMPRESSED_FRAME_FORMATS],    // vs: frame (uncompressed) descr.
    it_vc_ss_companion,     // vc: superspeed companion EP descr.
    it_vs_ss_companion;     // vs: superspeed companion EP descr.
     
  usbdescbldr_item_t
    it_s_languages,         // string[0] - LANGs
    it_s_manufacturer,      // string, manuf.
    it_s_serialNo,          // string, serial number
    it_s_product,           // string, product
    it_s_device,            // string, device
    it_s_config;            // string, config

  uint8_t
    i_s_manufacturer,       // Generated string index values..
    i_s_serialNo,
    i_s_product,
    i_s_device,
    i_s_config;

  uint8_t
    i_config;             // Configuration value

  usbdescbldr_ctx_t context, *c;

  usbdescbldr_status_t  s;

  uint8_t buffer[512];        // 256, plus 256 for strings

  // Make a shorthand for the context. The API is verbose enough, and this is typed a lot.
  c = & context;

  // Pair some working space with the context.
  s = usbdescbldr_init(c, buffer, sizeof(buffer));

  // Strings first - we will need the string indices for other items.

  if(IS_OK(s)) {
    s = usbdescbldr_make_languageIDs(c, &it_s_languages, 0x0409 /* EN */);
  } else {
    FLAG(s);
  }

  if(IS_OK(s)) {
    s = usbdescbldr_make_string_descriptor(c, &it_s_manufacturer, &i_s_manufacturer, "Leap Motion");
  }
  else {
    FLAG(s);
  }

  if(IS_OK(s)) {
    s = usbdescbldr_make_string_descriptor(c, &it_s_serialNo, &i_s_serialNo, "MH00007");
  }

  if(IS_OK(s)) {
    s = usbdescbldr_make_string_descriptor(c, &it_s_device, &i_s_device, "Leap Motion Test Device");
  }

  if(IS_OK(s)) {
    s = usbdescbldr_make_string_descriptor(c, &it_s_config, &i_s_config, "Leap Motion Test Config");
  }

  if(IS_OK(s)) {
    s = usbdescbldr_make_string_descriptor(c, &it_s_product, &i_s_product, "Meadow Hawk");
  }

  if(IS_OK(s)) {
    usbdescbldr_device_descriptor_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bDeviceClass = 0xef;
    sf.bDeviceSubClass = 0x02;
    sf.bDeviceProtocol = 0x01;
    sf.idVendor = 0xf182;
    sf.idProduct = 0x0004;
    sf.bcdDevice = 0x0108;
    sf.bcdUSB = 0x0300;
    sf.iManufacturer = i_s_manufacturer;
    sf.iSerialNumber = i_s_serialNo;
    sf.iProduct = i_s_product;
    sf.bNumConfigurations = 1;
    s = usbdescbldr_make_device_descriptor(c, &it_device, &sf);
  }

  if(IS_OK(s)) {
    usbdescbldr_device_qualifier_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bcdUSB = 0x0200;
    sf.bDeviceClass = 0xef;
    sf.bDeviceClass = 0x02;
    sf.bDeviceProtocol = 0x01;
    sf.bMaxPacketSize0 = 64;
    sf.bNumConfigurations = 0x01;
    s = usbdescbldr_make_device_qualifier_descriptor(c, &it_dev_qualifier, &sf);
  }

  if(IS_OK(s)) {
    usbdescbldr_device_configuration_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bNumInterfaces = 2;
    // Here is where Bldr may go too far; it's going to assign the
    // configuration value.. and we probably want to be in control of that.
    sf.iConfiguration = i_s_config;
    sf.bmAttributes = 0x80;
    sf.bMaxPower = 100;
    s = usbdescbldr_make_device_configuration_descriptor(c, &it_config, & i_config, &sf);
  }

  if(IS_OK(s)) {
    usbdescbldr_iad_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bFirstInterface = 0;
    sf.bInterfaceCount = 2;
    sf.bFunctionClass = 0x0e;
    sf.bFunctionSubClass = 0x03;
    sf.bFunctionProtocol = 0x00;
    s = usbdescbldr_make_interface_association_descriptor(c, &it_iad, &sf);
  }

  if(IS_OK(s)) {
    usbdescbldr_standard_interface_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bInterfaceNumber = 0;
    sf.bAlternateSetting = 0;
    sf.bNumEndpoints = 0x01;
    sf.bInterfaceClass = 0x0e;
    sf.bInterfaceSubClass = 0x01;
    sf.bInterfaceProtocol = 0x00;
    s = usbdescbldr_make_standard_interface_descriptor(c, &it_vc_if, &sf);
  }

  if(IS_OK(s)) {
    usbdescbldr_standard_interface_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bInterfaceNumber = 1;
    sf.bAlternateSetting = 0;
    sf.bNumEndpoints = 0x01;
    sf.bInterfaceClass = 0x0e;
    sf.bInterfaceSubClass = 0x02;
    sf.bInterfaceProtocol = 0x00;
    s = usbdescbldr_make_standard_interface_descriptor(c, &it_vs_if, &sf);
  }

  if(IS_OK(s)) {
    // Parameters: pass interface 1 into the Video Control Header Descriptor maker
    s = usbdescbldr_make_vc_cs_interface_descriptor(c, &it_vc_hdr, 0x000003e8, 1, USBDESCBLDR_LIST_END);
  } else {
    FLAG(s);
  }

  if(IS_OK(s)) {
    usbdescbldr_camera_terminal_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bTerminalID = CAMERA_OUTPUT_TERMINAL;
    sf.bAssocTerminal = 0x00;
    sf.controls = (0x00 << 16) | (0x02 << 8) | (0x28 << 0);  // XXX: ok; this is ugly.
    s = usbdescbldr_make_camera_terminal_descriptor(c, &it_vc_input, &sf);
  } else {
    FLAG(s);
  }

  if(IS_OK(s)) {
    usbdescbldr_vc_processor_unit_short_form sf;
    memset(&sf, 0, sizeof(sf));
    sf.bUnitID = VIDEO_PU_UNIT;
    sf.bSourceID = CAMERA_OUTPUT_TERMINAL;
    sf.wMaxMultiplier = 0;
    s = usbdescbldr_make_vc_processor_unit(c, &it_vc_pu, &sf);
  } else {
    FLAG(s);
  }

  if(IS_OK(s)) {
    usbdescbldr_vc_extension_unit_short_form_t sf;
    uint8_t bmControls[3];    // Takig three from existing descriptor.
    memset(& sf, 0, sizeof(sf));
    memset(bmControls, 0, sizeof(bmControls));
    bmControls[0] = 0x01;
    sf.bUnitID = VC_EXTENSION_UNIT; // 0x06
    sf.guidExtensionCode = EXTENSION_GUID;
    sf.bNumControls = 0;
    sf.bmControls = bmControls;
    // Pass in the list of input sources:
    s = usbdescbldr_make_extension_unit_descriptor(c, &it_vc_eu, &sf, VIDEO_PU_UNIT, USBDESCBLDR_LIST_END);
  } else {
    FLAG(s);
  }

  if(IS_OK(s)) {
    // VC Output Terminal yet to be coded, so many flavors. Just code the one we need.
    s = USBDESCBLDR_UNSUPPORTED;
  } else {
    FLAG(s);
  }

  if(IS_OK(s)) {
    usbdescbldr_endpoint_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bEndpointAddress = 0x82;
    sf.bmAttributes = 0x03;
    sf.wMaxPacketSize = 1024;
    sf.bInterval = 0x01;
    s = usbdescbldr_make_endpoint_descriptor(c, &it_vc_ep, &sf);
  } else {
    FLAG(s);
  }

  if(IS_OK(s)) {
    usbdescbldr_endpoint_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bEndpointAddress = 0x82;
    sf.bmAttributes = 0x02;
    sf.wMaxPacketSize = 1024;
    sf.bInterval = 0x01;
    s = usbdescbldr_make_endpoint_descriptor(c, &it_vs_ep, &sf);
  } else {
    FLAG(s);
  }

  if(IS_OK(s)) {
    s = usbdescbldr_make_vc_interrupt_ep(c, &it_cs_ep_intr, 64);
  } else {
    FLAG(s);
  }

  if(IS_OK(s)) {
    usbdescbldr_vs_if_input_header_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bNumFormats = 1;
    sf.bEndpointAddress = 0x83;
    sf.bmInfo = 0;
    sf.bTerminalLink = 0x03; // find and symbolize
    sf.bStillCaptureMethod = 0;
    sf.bTriggerSupport = 0;
    sf.bTriggerUsage = 0;
    // The variable part that follows, the controls, is mystical. Rethink.
    s = usbdescbldr_make_uvc_vs_if_input_header(c, &it_vs_header, &sf, 0 /* controls */, USBDESCBLDR_LIST_END);
  } else {
    FLAG(s);
  }

  if(IS_OK(s)) {
    usbdescbldr_uvc_vs_format_uncompressed_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bFormatIndex = 1;
    sf.bNumFrameDescriptors = 1;
    sf.guidFormat = VS_FORMAT_UNCOMPRESSED;
    sf.bBitsPerPixel = 16;
    sf.bDefaultFrameIndex = 2;
    sf.bNumFrameDescriptors = UNCOMPRESSED_FRAME_FORMATS;
    sf.bAspectRatioX = 0;
    sf.bAspectRatioY = 0;
    sf.bmInterlaceFlags = 0;
    sf.bCopyProtect = 0;
    s = usbdescbldr_make_uvc_vs_format_uncompressed(c, &it_vs_fmt_uncomp, &sf);
  } else {
    FLAG(s);
  }

  if(IS_OK(s)) {
    usbdescbldr_uvc_vs_frame_uncompressed_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bFrameIndex = 1;
    sf.bFrameIntervalType = 1;    // Not continuous; a single interval.
    sf.bmCapabilities = 0x00;
    sf.wWidth = 640;
    sf.wHeight = 240;
    sf.dwMinBitRate = 0x16788000;
    sf.dwMaxBitRate = 0x16788000;
    sf.dwMaxVideoFrameBufferSize = 0x0004B000;
    sf.dwDefaultFrameInterval = 0x0000FEA5;
    s = usbdescbldr_make_uvc_vs_frame_uncompressed(c, &it_vs_frm_uncomp[sf.bFrameIndex - 1], &sf, sf.dwDefaultFrameInterval, USBDESCBLDR_LIST_END);
  } else {
    FLAG(s);
  }

  if(IS_OK(s)) {
    usbdescbldr_uvc_vs_frame_uncompressed_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bFrameIndex = 2;
    sf.bFrameIntervalType = 1;    // Not continuous; a single interval.
    sf.bmCapabilities = 0x00;
    sf.wWidth = 640;
    sf.wHeight = 120;
    sf.dwMinBitRate = 0x159B4000;
    sf.dwMaxBitRate = 0x159B4000;
    sf.dwMaxVideoFrameBufferSize = 0x00025800;
    sf.dwDefaultFrameInterval = 0x0000846A;
    s = usbdescbldr_make_uvc_vs_frame_uncompressed(c, &it_vs_frm_uncomp[sf.bFrameIndex - 1], &sf, sf.dwDefaultFrameInterval, USBDESCBLDR_LIST_END);
  } else {
    FLAG(s);
  }

  if(IS_OK(s)) {
    usbdescbldr_uvc_vs_frame_uncompressed_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bFrameIndex = 3;
    sf.bFrameIntervalType = 1;    // Not continuous; a single interval.
    sf.bmCapabilities = 0x00;
    sf.wWidth = 1280;
    sf.wHeight = 960;
    sf.dwMinBitRate = 0x34BC0000;
    sf.dwMaxBitRate = 0x34BC0000;
    sf.dwMaxVideoFrameBufferSize = 0x00258000;
    sf.dwDefaultFrameInterval = 0x0003640E;
    s = usbdescbldr_make_uvc_vs_frame_uncompressed(c, &it_vs_frm_uncomp[sf.bFrameIndex - 1], &sf, sf.dwDefaultFrameInterval, USBDESCBLDR_LIST_END);
  } else {
    FLAG(s);
  }

  if(IS_OK(s)) {
    usbdescbldr_uvc_vs_frame_uncompressed_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bFrameIndex = 4;
    sf.bFrameIntervalType = 1;    // Not continuous; a single interval.
    sf.bmCapabilities = 0x00;
    sf.wWidth = 1280;
    sf.wHeight = 480;
    sf.dwMinBitRate = 0x34BC0000;
    sf.dwMaxBitRate = 0x34BC0000;
    sf.dwMaxVideoFrameBufferSize = 0x0012C000;
    sf.dwDefaultFrameInterval = 0x0001B207;
    s = usbdescbldr_make_uvc_vs_frame_uncompressed(c, &it_vs_frm_uncomp[sf.bFrameIndex - 1], &sf, sf.dwDefaultFrameInterval, USBDESCBLDR_LIST_END);
  } else {
    FLAG(s);
  }

  if(IS_OK(s)) {
    usbdescbldr_uvc_vs_frame_uncompressed_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bFrameIndex = 5;
    sf.bFrameIntervalType = 1;    // Not continuous; a single interval.
    sf.bmCapabilities = 0x00;
    sf.wWidth = 1280;
    sf.wHeight = 240;
    sf.dwMinBitRate = 0x31CE0000;
    sf.dwMaxBitRate = 0x31CE0000;
    sf.dwMaxVideoFrameBufferSize = 0x00096000;
    sf.dwDefaultFrameInterval = 0x0000E5C7;
    s = usbdescbldr_make_uvc_vs_frame_uncompressed(c, &it_vs_frm_uncomp[sf.bFrameIndex - 1], &sf, sf.dwDefaultFrameInterval, USBDESCBLDR_LIST_END);
  } else {
    FLAG(s);
  }

  if(IS_OK(s)) {
    usbdescbldr_uvc_vs_frame_uncompressed_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bFrameIndex = 6;
    sf.bFrameIntervalType = 1;    // Not continuous; a single interval.
    sf.bmCapabilities = 0x00;
    sf.wWidth = 1280;
    sf.wHeight = 120;
    sf.dwMinBitRate = 0x2E248000;
    sf.dwMaxBitRate = 0x2E248000;
    sf.dwMaxVideoFrameBufferSize = 0x0004B000;
    sf.dwDefaultFrameInterval = 0x00007C02;
    s = usbdescbldr_make_uvc_vs_frame_uncompressed(c, &it_vs_frm_uncomp[sf.bFrameIndex - 1], &sf, sf.dwDefaultFrameInterval, USBDESCBLDR_LIST_END);
  } else {
    FLAG(s);
  }

  if(IS_OK(s)) {
  }
  else {
    FLAG(s);
  }

  if(IS_OK(s)) {
  }
  else {
    FLAG(s);
  }

  if(IS_OK(s)) {
  }
  else {
    FLAG(s);
  }







#undef IS_OK
#undef FLAG
}

TEST_F(USBDescBuilderTest, HelloWorldTest) {
  ASSERT_EQ(1, HelloWorld()) << "Hello world did not return 1!";
  createUSB30Configuration();
}