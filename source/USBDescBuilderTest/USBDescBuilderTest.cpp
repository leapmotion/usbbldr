#include "USBDescBuilderTest.h"
#include "USBDescBuilder/USBBldr.h"
#include "USBDescBuilder/usbdescbuilder.h"

static const uint8_t  CAMERA_OUTPUT_TERMINAL = 0x02;
static const uint8_t  STREAMING_OUTPUT_TERMINAL = 0x03;
static const uint8_t  VIDEO_PU_UNIT = 0x05;
static const uint8_t  VC_EXTENSION_UNIT = 0x06;
static const GUID     EXTENSION_GUID = {  0x8E9093EF, 0x97EA, 0x49E1, { 0x83, 0x06, 0x9F, 0x6B, 0x69, 0x6A, 0x1A, 0xEE } };
static const GUID     VS_FORMAT_UNCOMPRESSED = { 0x32595559, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 } }; // yuy2

static const uint32_t UNCOMPRESSED_FRAME_FORMATS = 7;


static void
emit(usbdescbldr_ctx_t *ctx, char *s, usbdescbldr_item_t *ip)
{
  uint16_t  t16, count, length;
  
  t16 = ip->size;
  if(ip->totalSize != NULL) {
    memcpy(&t16, ip->totalSize, sizeof(t16));
    t16 = ctx->fLittleShortToHost(t16);
  }
  if(t16 == 0) t16 = ip->size;
  length = t16;

  printf("uint8_t %s[] = {  // length %lu", s, length);

  for(count = 0; count < length; count++) {
    if(count > 0)
      printf(",");
    if(count % 8 == 0)
      printf("\n\t");
    else
      printf(" ");
    printf("0x%02X", ((uint8_t *) ip->address)[count]);
  }
  printf("};\n");
}

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
    
  // The string values we will make:
  enum {
    SI_LANGUAGES,     // Always zero'th

    SI_MANUFACTURER,
    SI_SERIALNO,
    SI_PRODUCT,
    SI_DEVICE,
    SI_CONFIG,

    SI_COUNT          // Always final
  };

  // What they'll say (special case 0 is for Language Support):
  static const char *ascii_strings[] = {
    NULL,

    "Leap Motion",
    "MH00007",
    "Meadow Hawk",
    "Leap Motion Test Device",
    "Leap Motion Test Config",
  };

  // .. and where they are held:
  usbdescbldr_item_t  it_string[SI_COUNT];
  
  usbdescbldr_ctx_t context, *c;

  usbdescbldr_status_t  s;

  uint8_t buffer[1024];

  // Make a shorthand for the context. The API is verbose enough, and this is typed a lot.
  c = & context;

  // Pair some working space with the context.
  s = usbdescbldr_init(c, buffer, sizeof(buffer));

  // Strings: LANGUAGES first - architected to be at index zero.
  if(IS_OK(s)) {
    s = usbdescbldr_make_languageIDs(c, &it_string[SI_LANGUAGES], 0x0409 /* EN */, USBDESCBLDR_LIST_END);
  } else {
    FLAG(s);
  }

  // Now, create other strings:
  for(uint32_t si = SI_MANUFACTURER; si < SI_COUNT; si++) {
    if(IS_OK(s)) {
      s = usbdescbldr_make_string_descriptor(c, &it_string[si], NULL, ascii_strings[si]);
    } else {
      FLAG(s);
      break;
    }
  }
  
  // Device Descriptor
  if(IS_OK(s)) {
    usbdescbldr_device_descriptor_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bDeviceClass = 0xef;         // Miscellaneous
    sf.bDeviceSubClass = 0x02;      // Common Class
    sf.bDeviceProtocol = 0x01;      // IAD
    sf.idVendor = 0xf182;
    sf.idProduct = 0x0004;
    sf.bcdDevice = 0x0108;
    sf.bcdUSB = 0x0300;
    sf.iManufacturer = SI_MANUFACTURER;
    sf.iSerialNumber = SI_SERIALNO;
    sf.iProduct = SI_PRODUCT;
    sf.bNumConfigurations = 1;
    s = usbdescbldr_make_device_descriptor(c, &it_device, &sf);
  } else {
    FLAG(s);
  }

  // Device Qualifier
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
  } else {
    FLAG(s);
  }

  // Configuration
  if(IS_OK(s)) {
    usbdescbldr_device_configuration_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bNumInterfaces = 2;
    sf.bConfigurationValue = 1;
    sf.iConfiguration = SI_CONFIG;
    sf.bmAttributes = 0x80;
    sf.bMaxPower = 100;
    s = usbdescbldr_make_device_configuration_descriptor(c, &it_config, &sf);
  } else {
    FLAG(s);
  }

  // Interface Association
  if(IS_OK(s)) {
    usbdescbldr_iad_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bFirstInterface = 0;
    sf.bInterfaceCount = 2;
    sf.bFunctionClass = USB_INTERFACE_CC_VIDEO;
    sf.bFunctionSubClass = USB_INTERFACE_VC_SC_VIDEO_INTERFACE_COLLECTION;  
    sf.bFunctionProtocol = USB_INTERFACE_VC_PC_PROTOCOL_UNDEFINED;
    s = usbdescbldr_make_interface_association_descriptor(c, &it_iad, &sf);
  } else { 
    FLAG(s);
  }

  // Interface 0 (control)
  if(IS_OK(s)) {
    usbdescbldr_vc_interface_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bInterfaceNumber = 0;
    sf.bAlternateSetting = 0;
    sf.bNumEndpoints = 0x01;
    s = usbdescbldr_make_vc_interface_descriptor(c, &it_vc_if, &sf);
  } else {
    FLAG(s);
  }

  // Video Control Interface Header Descriptor
  if(IS_OK(s)) {
    s = usbdescbldr_make_vc_interface_header(c, &it_vc_hdr, 0x000003e8, /* intf: */ 1, USBDESCBLDR_LIST_END);
  } else {
    FLAG(s);
  }

  // Camera Source
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

  // Processing Unit
  if(IS_OK(s)) {
    usbdescbldr_vc_processor_unit_short_form sf;
    memset(&sf, 0, sizeof(sf));
    sf.bUnitID = VIDEO_PU_UNIT;
    sf.bSourceID = CAMERA_OUTPUT_TERMINAL;
    sf.wMaxMultiplier = 0;
    sf.controls = (0x7b << 0) | (0x02 << 8) | (0x00 << 16); // XXX: rethink this
    s = usbdescbldr_make_vc_processor_unit(c, &it_vc_pu, &sf);
  } else {
    FLAG(s);
  }

  // Extension Unit
  if(IS_OK(s)) {
    usbdescbldr_vc_extension_unit_short_form_t sf;
    uint8_t bmControls[3];    // Taking 'three' from prior work.
    memset(&sf, 0, sizeof(sf));
    memset(bmControls, 0, sizeof(bmControls));
    bmControls[0] = 0x01;
    sf.bUnitID = VC_EXTENSION_UNIT;
    sf.guidExtensionCode = EXTENSION_GUID;
    sf.bNumControls = 0;
    sf.bControlSize = sizeof(bmControls);
    sf.bmControls = bmControls;
    // Pass in the list of input sources:
    s = usbdescbldr_make_extension_unit_descriptor(c, &it_vc_eu, &sf, VIDEO_PU_UNIT, USBDESCBLDR_LIST_END);
  } else {
    FLAG(s);
  }

  // VC Output Terminal 
  if(IS_OK(s)) {
    usbdescbldr_streaming_out_terminal_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bTerminalID = STREAMING_OUTPUT_TERMINAL;
    sf.bAssocTerminal = 0;
    sf.bSourceID = VC_EXTENSION_UNIT;
    s = usbdescbldr_make_streaming_out_terminal_descriptor(c, &it_vc_output, &sf);
  } else {
    FLAG(s);
  }

  // The Control Endpoint
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

  // Video Control super-speed EP companion
  if(IS_OK(s)) {
    usbdescbldr_ss_ep_companion_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bMaxBurst = 0;
    sf.bmAttributes = 0;
    sf.wBytesPerInterval = 1024;
    s = usbdescbldr_make_ss_ep_companion_descriptor(c, &it_vc_ss_companion, &sf);
  } else {
    FLAG(s);
  }

  // The Control Interrupt EP
  if(IS_OK(s)) {
    s = usbdescbldr_make_vc_interrupt_ep(c, &it_cs_ep_intr, 64);
  } else {
    FLAG(s);
  }

  // Interface 1 (streaming)
  if(IS_OK(s)) {
    usbdescbldr_vs_interface_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bInterfaceNumber = 1;
    sf.bAlternateSetting = 0;
    sf.bNumEndpoints = 0x01;
    s = usbdescbldr_make_vs_interface_descriptor(c, &it_vs_if, &sf);
  } else {
    FLAG(s);
  }

  // Video Streaming Header Descriptor
  if(IS_OK(s)) {
    usbdescbldr_vs_if_input_header_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bNumFormats = 1;
    sf.bEndpointAddress = 0x83;
    sf.bmInfo = 0;
    sf.bTerminalLink = STREAMING_OUTPUT_TERMINAL;
    sf.bStillCaptureMethod = 0;
    sf.bTriggerSupport = 0;
    sf.bTriggerUsage = 0;
    // The variable part that follows (the controls) is mystical. Size and count are both unclear. Rethink.
    s = usbdescbldr_make_vs_interface_header(c, &it_vs_header, &sf, 0 /* controls */, USBDESCBLDR_LIST_END);
  } else {
    FLAG(s);
  }

  // Streaming Format Descriptor
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

  // Helper table for building: Streaming Frame Descriptors
  typedef struct {
    uint16_t width;
    uint16_t height;
    uint32_t bitRate;
    uint32_t frameBufferSize;
    uint32_t frameInterval;
  } frame_parameter_t;

  static const frame_parameter_t frame_sheet[UNCOMPRESSED_FRAME_FORMATS] = {
    {  640, 480, 0x16E90000, 0x00096000, 0x0001F385 },
    {  640, 240, 0x16788000, 0x0004B000, 0x0000FEA5 },
    {  640, 120, 0x159B4000, 0x00025800, 0x0000846A },
    { 1280, 960, 0x34BC0000, 0x00258000, 0x0003640E },
    { 1280, 480, 0x34BC0000, 0x0012C000, 0x0001B207 },
    { 1280, 240, 0x31CE0000, 0x00096000, 0x0000E5C7 },
    { 1280, 120, 0x2E248000, 0x0004B000, 0x00007C02 }
  };

  // Construct all seven frame formats.
  for(uint32_t f = 0; f < UNCOMPRESSED_FRAME_FORMATS; f++) {
    if(IS_OK(s)) {
      usbdescbldr_uvc_vs_frame_uncompressed_short_form_t sf;
      memset(&sf, 0, sizeof(sf));
      sf.bFrameIndex = f + 1;
      sf.bFrameIntervalType = 1;    // Not continuous; a single interval.
      sf.bmCapabilities = 0x00;
      sf.wWidth = frame_sheet[f].width;
      sf.wHeight = frame_sheet[f].height;
      sf.dwMinBitRate = frame_sheet[f].bitRate;
      sf.dwMaxBitRate = frame_sheet[f].bitRate;
      sf.dwMaxVideoFrameBufferSize = frame_sheet[f].frameBufferSize;
      sf.dwDefaultFrameInterval = frame_sheet[f].frameInterval;
      s = usbdescbldr_make_uvc_vs_frame_uncompressed(c, &it_vs_frm_uncomp[f], &sf, sf.dwDefaultFrameInterval, USBDESCBLDR_LIST_END);
    } else {
      FLAG(s);
      break;
    }
  }

  // The Streaming Endpoint
  if(IS_OK(s)) {
    usbdescbldr_endpoint_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bEndpointAddress = 0x83;
    sf.bmAttributes = 0x02;
    sf.wMaxPacketSize = 1024;
    sf.bInterval = 0x01;
    s = usbdescbldr_make_endpoint_descriptor(c, &it_vs_ep, &sf);
  } else {
    FLAG(s);
  }
   
  // Video Stream super-speed EP companion
  if(IS_OK(s)) {
    usbdescbldr_ss_ep_companion_short_form_t sf;
    memset(&sf, 0, sizeof(sf));
    sf.bMaxBurst = 0x0f;
    sf.bmAttributes = 0;
    sf.wBytesPerInterval = 0;
    s = usbdescbldr_make_ss_ep_companion_descriptor(c, &it_vs_ss_companion, &sf);
  } else {
    FLAG(s);
  }


  // Now, the buffer is laid out and populated. Establish the structure
  // (hierarchy) within the descriptors. This is done 'bottom-up' to
  // allow proper percolation of lengths to higher and higher levels.

  // Streaming header envelops format descriptor
  if(IS_OK(s))
    s = usbdescbldr_add_children(c, &it_vs_header, &it_vs_fmt_uncomp, NULL);
  else {
    FLAG(s);
  }

  // Streaming header also envelops the frame descriptor(s)
  for(uint32_t f = 0; f < UNCOMPRESSED_FRAME_FORMATS; f++) {
    if(IS_OK(s))
      s = usbdescbldr_add_children(c, &it_vs_header, &it_vs_frm_uncomp[f], NULL);
    else {
      FLAG(s);
      break;
    }
  }

  // Control header envelops control units
  if(IS_OK(s))
    s = usbdescbldr_add_children(c, &it_vc_hdr, &it_vc_input, &it_vc_pu, &it_vc_eu, &it_vc_output, NULL);
  else {
    FLAG(s);
  }

  // Configuration envelops IAD, VC I/F and header, VS I/F and header
  if(IS_OK(s))
    s = usbdescbldr_add_children(c, &it_config, &it_iad, &it_vc_if, &it_vc_hdr, &it_vs_if, &it_vs_header, NULL);
  else {
    FLAG(s);
  }

  // Configuration also envelops endpoints
  if(IS_OK(s))
    s = usbdescbldr_add_children(c, &it_config, &it_cs_ep_intr, &it_vc_ep, &it_vc_ss_companion, &it_vs_ep, &it_vs_ss_companion, NULL);
  else {
    FLAG(s);
  }
 

  // Emit the buffer contents.
  emit(c, "deviceDesc", &it_device);
  emit(c, "deviceQualifier", &it_dev_qualifier);
  emit(c, "configurationDesc", &it_config);

  // Emit all strings
  for(uint32_t si = SI_LANGUAGES; si < SI_COUNT; si++) {
    char symbol[sizeof("stringDesc000")];
    uint8_t index = it_string[si].index;
    // XXX snprintf() vs. sprintf_s() .. not sure about platform portability.
    // without snprintf(), the check against 255 and the use of ..000" (above) must suffice
    // if(snprintf(symbol, sizeof(symbol), "stringDesc%03u", index) > sizeof(symbol)) {
    if(index > 255) {
      fprintf(stderr, "String index is out of range (%u)\n", index);
    }
    else {
      (void) sprintf(symbol, "stringDesc%03u", index); // Emits a compiler warning (valid warning)
      emit(c, symbol, &it_string[si]);
    }
  }

#undef IS_OK
#undef FLAG
}
