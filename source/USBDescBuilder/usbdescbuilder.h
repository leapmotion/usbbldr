/* Copyright (c) 2014 LEAP Motion. All rights reserved.
*
* The intellectual and technical concepts contained herein are proprietary and
* confidential to Leap Motion, and are protected by trade secret or copyright
* law. Dissemination of this information or reproduction of this material is
* strictly forbidden unless prior written permission is obtained from LEAP
* Motion.
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "USBExtras.h"

int HelloWorld(void);

// TODO
// -- Dry run mode won't work right now; the wTotalSize pointers in the items
// all point to descriptor fields in the buffer. Remedy will be to shadow the total
// lengths within the items themselves.

  // //////////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////////
  // API

/// The API is based around a context which is used to collect and maintain
/// state as the API is used. It is not opaque (e.g. void *) as the caller
/// must provide one to the API; the API does not create structures dynamically.
/// The context must be initialized before making any other use of the API.
/// Callers should treat the context as 'read only'.

typedef struct usbdescbldr_ctx_s {
  unsigned char 	initialized;	// Have we been initialized? 0: no.

  unsigned char *	buffer;		    // Start of user-provided buffer (if any)
  size_t		      bufferSize;	  // Length in bytes of any user-provided buffer
  unsigned char * append;		    // Address in buffer for next addition (append)

  unsigned int  	i_string;	    // Next string index to be assigned
  unsigned int    i_devConfig;  // Next device configuration index to be assigned

  // Conversion functions for little-endian support across platforms.
  uint16_t(*fLittleShortToHost)(uint16_t s);
  uint16_t(*fHostToLittleShort)(uint16_t s);
  unsigned int(*fLittleIntToHost)(unsigned int s);
  unsigned int(*fHostToLittleInt)(unsigned int s);
} usbdescbldr_ctx_t;

/// Errors returned by the API.
  typedef enum {
    USBDESCBLDR_OK,
    USBDESCBLDR_UNINITIALIZED,		///< Never initialized
    USBDESCBLDR_UNSUPPORTED,		  ///< Never written...
    USBDESCBLDR_DRY_RUN,		      ///< Not actually creating the datum
    USBDESCBLDR_NO_SPACE,		      ///< Buffer exhausted
    USBDESCBLDR_INVALID,		      ///< Null ptr, duplicate, range error..
    USBDESCBLDR_OVERSIZED,        ///< Exceeds size limit
    USBDESCBLDR_TOO_MANY,         ///< Exceeds instance count limit
    // ...
  } usbdescbldr_status_t;

  // The maker functions which accept a variable number of
  // arguments need a special value to terminate the list.
  // (It's this, or make people count their arguments manually.)
  // Since zero can be a common value to pass, zero makes a poor
  // terminator. We choose a value which cannot be represented as
  // a uint8_t or uint16_t, isn't a legit pointer, and should be unlikely
  // as a DWORD (uint32_t) value in a descriptor.

  // API memebers which accept a variable number of arguments
  // typically will use this constant to terminate the list.
  // API memebers which accept a variable number of pointers
  // will use NULL to terminate their lists.
  static const uint32_t USBDESCBLDR_LIST_END = 0xee00eeef;


  // The 'handle' by which callers store the results of maker calls. Callers
  // 'know' this structure only to provide them as return (out) parameters;
  // never should they modify the contents.
#define USBDESCBLDR_MAX_CHILDREN 16

  /// Items are filled in as results from all of the API maker
  /// calls. They will be used after the make calls have been
  // finished to layer the descriptors and build up the lengths
  // of layered descriptors, and to access the binary results
  // for each descriptor. Callers should trest items as 'read only'.

  typedef struct usbdescbldr_item_s {
    void *                      address;        ///< Where it is stored
    unsigned int                index;          ///< Index, interface number, endpoint number, anything of this nature
    uint16_t                    size;           ///< Size of item itself
    uint16_t *                  totalSize;      ///< Size of item and all children, or NULL if not kept
    unsigned int                items;          ///< Number iof sub-items ('children')
    struct usbdescbldr_item_s * item[USBDESCBLDR_MAX_CHILDREN];
  } usbdescbldr_item_t;

 
  // Setup and teardown

  /// Reset internal state and begin a new descriptor.
  /// Passing NULL for the buffer indicates a 'dry run' --
  /// go through all the motions, checks, and size computations
  /// but do not actually create a descriptor.
  ///\param [in] ctx A context to be initialized for the session.
  ///\param [in] buffer An optional buffer for the session to use to actually assemble the results into final forms. If NULL is provided, the API 
  /// will simply use the items to try and come up with a scaffolding of the final results, which may provide sanity checking and sizes.
  ///\param [in] bufferSize The size in bytes of the buffer.
  usbdescbldr_status_t
  usbdescbldr_init(usbdescbldr_ctx_t *  ctx,
                   unsigned char *      buffer,
                   size_t               bufferSize);


  // Commit (complete/finish) the descriptor in progress.
  // Collections, interfaces, etc left open will be tidied up
  // (if possible..?)
  usbdescbldr_status_t
    usbdescbldr_close(usbdescbldr_ctx_t * ctx);

  // Terminate use of the builder. Release any resources.
  // Once this is complete, the API requires an _init() before
  // anything will perform again.
  usbdescbldr_status_t
    usbdescbldr_end(usbdescbldr_ctx_t * ctx);

  // //////////////////////////////////////////////////////////////////
  // Constructions

  // Make a set of items subordinate to one parent item. This
  // is used to allow the parent item to account for their accumulated lengths.
  // Pass the context, a result item, and the subordinate items.
  // Terminate the list with NULL.

  usbdescbldr_status_t
    usbdescbldr_add_children(usbdescbldr_ctx_t *  ctx,
    usbdescbldr_item_t * parent,
    ...);

  // //////////////////////////////////////////////////////////////////

  // Create the language descriptor (actually string, index 0).
  // Pass the context, a result item, and the IDs.
  usbdescbldr_status_t
    usbdescbldr_make_languageIDs(usbdescbldr_ctx_t * ctx,
                                 usbdescbldr_item_t * item,
                                 ...);

  // //////////////////////////////////////////////////////////////////

  // Generate a USB Device Descriptor. This is one-shot and top-level.
  // Pass the context, a result item, and the completed Device Descriptor 
  // short-form structure.

  typedef struct {
    uint16_t	bcdUSB;		        // USB Spec Version
    uint8_t	  bDeviceClass;
    uint8_t	  bDeviceSubClass;
    uint8_t	  bDeviceProtocol;
    uint16_t	idVendor;
    uint16_t	idProduct;
    uint16_t	bcdDevice;	      // BCD-encoded Release #
    uint8_t	  iManufacturer;	  // String index
    uint8_t	  iProduct;	        // String index
    uint8_t	  iSerialNumber;	  // String Index
    uint8_t	  bNumConfigurations;
  } usbdescbldr_device_descriptor_short_form_t;

  usbdescbldr_status_t
    usbdescbldr_make_device_descriptor(usbdescbldr_ctx_t *ctx,
                                       usbdescbldr_item_t *item,
                                       usbdescbldr_device_descriptor_short_form_t *form);
    
  // //////////////////////////////////////////////////////////////////

  typedef struct {
    uint16_t	bcdUSB;		// USB Spec Version
    uint8_t	bDeviceClass;
    uint8_t	bDeviceSubClass;
    uint8_t	bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint8_t	bNumConfigurations;
  } usbdescbldr_device_qualifier_short_form_t;

  // Generate a USB Device Qualifier Descriptor. This is one-shot and top-level.
  usbdescbldr_status_t
    usbdescbldr_make_device_qualifier_descriptor(usbdescbldr_ctx_t *ctx,
                                               usbdescbldr_item_t *item, 
                                               usbdescbldr_device_qualifier_short_form_t * form);
    
  // //////////////////////////////////////////////////////////////////

  // Define a new string and obtain its index. This is a one-shot top-level item.
  // Pass the string in ASCII and NULL-terminated (a classic C string).
  usbdescbldr_status_t
    usbdescbldr_make_string_descriptor(usbdescbldr_ctx_t *  ctx,
                                    usbdescbldr_item_t *    item,     // OUT
                                    uint8_t *               index,    // OUT
                                    const char *            string);  // IN

  // //////////////////////////////////////////////////////////////////

  // Generate a Binary Object Store. This is top-level.
  // Add the device Capabilities to it to complete the BOS.
  usbdescbldr_status_t 
    usbdescbldr_make_bos_descriptor(usbdescbldr_ctx_t * ctx,
                                  usbdescbldr_item_t *  item);


  // Generate a Device Capability descriptor.
  usbdescbldr_status_t
    usbdescbldr_make_device_capability_descriptor(usbdescbldr_ctx_t * ctx,
                                                usbdescbldr_item_t *  item,
                                                uint8_t               bDevCapabilityType,
                                                uint8_t *             typeDependent,	// Anonymous byte data
                                                size_t                typeDependentSize);

 
  // //////////////////////////////////////////////////////////////////

  // Generate a Device Configuration descriptor. 
  // Likewise, the short form takes the number of interfaces, but these too
  // must be added to the children.
  // The assigned configuration
  // index is returned -- this is probably presumed by other code,
  // and should probably be specified explicitly.. we'll see. Currently 
  // if that is the case, I assume the caller will perform an assert() 
  // or other sanity check on the returned value.

  typedef struct {
    uint8_t  bNumInterfaces;
    uint8_t  bConfigurationValue;
    uint8_t  iConfiguration;
    uint8_t  bmAttributes;
    uint8_t  bMaxPower;
  } usbdescbldr_device_configuration_short_form_t;

  usbdescbldr_status_t
    usbdescbldr_make_device_configuration_descriptor(usbdescbldr_ctx_t * ctx,
                                                     usbdescbldr_item_t * item,
                                                     uint8_t * index,
                                                     usbdescbldr_device_configuration_short_form_t * form);


  // //////////////////////////////////////////////////////////////////

  typedef struct {
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
  } usbdescbldr_standard_interface_short_form_t;


  usbdescbldr_status_t
    usbdescbldr_make_standard_interface_descriptor(usbdescbldr_ctx_t * ctx,
                                                   usbdescbldr_item_t * item,
                                                   usbdescbldr_standard_interface_short_form_t * form);


  usbdescbldr_status_t
    usbdescbldr_make_vc_interface_descriptor(usbdescbldr_ctx_t * ctx,
                                             usbdescbldr_item_t * item,
                                             usbdescbldr_standard_interface_short_form_t * form);


  usbdescbldr_status_t
    usbdescbldr_make_vs_interface_descriptor(usbdescbldr_ctx_t * ctx,
                                             usbdescbldr_item_t * item,
                                             usbdescbldr_standard_interface_short_form_t * form);



  // //////////////////////////////////////////////////////////////////

  typedef struct {
    uint8_t  bEndpointAddress;
    uint8_t  bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t  bInterval;
  } usbdescbldr_endpoint_short_form_t;


  usbdescbldr_status_t
    usbdescbldr_make_endpoint_descriptor(usbdescbldr_ctx_t * ctx,
                                         usbdescbldr_item_t * item,
                                         usbdescbldr_endpoint_short_form_t * form);


  // //////////////////////////////////////////////////////////////////

  typedef struct {
    uint8_t  bMaxBurst;
    uint8_t  bmAttributes;
    uint16_t wBytesPerInterval;
  } usbdescbldr_ss_ep_companion_short_form_t;

  usbdescbldr_status_t
    usbdescbldr_make_ss_ep_companion_descriptor(usbdescbldr_ctx_t * ctx,
                                                usbdescbldr_item_t * item,
                                                usbdescbldr_ss_ep_companion_short_form_t * form);


  // //////////////////////////////////////////////////////////////////

  typedef struct {
    uint8_t bFirstInterface;
    uint8_t bInterfaceCount;
    uint8_t bFunctionClass;
    uint8_t bFunctionSubClass;
    uint8_t bFunctionProtocol;
    uint8_t iFunction;
  } usbdescbldr_iad_short_form_t;

  usbdescbldr_status_t
    usbdescbldr_make_interface_association_descriptor(usbdescbldr_ctx_t * ctx,
                                                      usbdescbldr_item_t * item,
                                                      usbdescbldr_iad_short_form_t * form);

  // //////////////////////////////////////////////////////////////////

  usbdescbldr_status_t
    usbdescbldr_make_vc_interface_header(usbdescbldr_ctx_t * ctx,
                                         usbdescbldr_item_t * item,
                                         unsigned int dwClockFrequency,
                                         ...); // Terminated List of Interface Numbers 
    


  // //////////////////////////////////////////////////////////////////

  typedef struct {
    uint8_t  bTerminalID;
    uint8_t  bAssocTerminal;
    uint8_t  iTerminal;
    uint16_t wObjectiveFocalLengthMin;
    uint16_t wObjectiveFocalLengthMax;
    uint16_t wOcularFocalLength;
    uint32_t controls;
  } usbdescbldr_camera_terminal_short_form_t;

  // The VC Camera Terminal (an Input) Descriptor.

  usbdescbldr_status_t
    usbdescbldr_make_camera_terminal_descriptor(usbdescbldr_ctx_t * ctx,
                                                usbdescbldr_item_t * item,
                                                usbdescbldr_camera_terminal_short_form_t * form);

 
  // //////////////////////////////////////////////////////////////////


  typedef struct {
    uint8_t  bTerminalID;
    uint8_t  bAssocTerminal;
    uint8_t   bSourceID;
    uint8_t  iTerminal;
  } usbdescbldr_streaming_out_terminal_short_form_t;

  // The VC Streaming Terminal (an Output) Descriptor.

  usbdescbldr_status_t
    usbdescbldr_make_streaming_out_terminal_descriptor(usbdescbldr_ctx_t * ctx,
                                                       usbdescbldr_item_t * item,
                                                       usbdescbldr_streaming_out_terminal_short_form_t * form);


  // //////////////////////////////////////////////////////////////////


  usbdescbldr_status_t
    usbdescbldr_make_vc_selector_unit(usbdescbldr_ctx_t * ctx,
                                      usbdescbldr_item_t * item,
                                      uint8_t iSelector, // string index
                                      uint8_t bUnitID,
                                      ...); // Terminated List of Input (Source) Pin(s)
   

  // //////////////////////////////////////////////////////////////////


  typedef struct
  {
    uint8_t  bUnitID;
    uint8_t  bSourceID;
    uint16_t wMaxMultiplier;
    uint32_t  controls;
    uint8_t   iProcessing;
    uint8_t   bmVideoStandards;
  } usbdescbldr_vc_processor_unit_short_form;

  usbdescbldr_status_t
    usbdescbldr_make_vc_processor_unit(usbdescbldr_ctx_t * ctx,
                                       usbdescbldr_item_t * item,
                                       usbdescbldr_vc_processor_unit_short_form * form);

  
  // //////////////////////////////////////////////////////////////////

  typedef struct
  {
    uint8_t  bUnitID;
    GUID     guidExtensionCode;
    uint8_t  bNumControls;
    uint8_t  bControlSize;
    uint8_t * bmControls;
    uint8_t  iExtension;
  } usbdescbldr_vc_extension_unit_short_form_t;

  usbdescbldr_status_t
    usbdescbldr_make_extension_unit_descriptor(usbdescbldr_ctx_t * ctx,
                                               usbdescbldr_item_t * item,
                                               usbdescbldr_vc_extension_unit_short_form_t * form,
                                               ...); // Varying number of SourceIDs.


  // //////////////////////////////////////////////////////////////////

  // UVC Class-Specific VC interrupt endpoint:

  usbdescbldr_status_t
    usbdescbldr_make_vc_interrupt_ep(usbdescbldr_ctx_t * ctx,
                                     usbdescbldr_item_t * item,
                                     uint16_t wMaxTransferSize);


  // //////////////////////////////////////////////////////////////////
  // UVC Video Stream Interface Input Header


  typedef struct
  {
    uint8_t  bNumFormats;
    uint8_t  bEndpointAddress;
    uint8_t  bmInfo;
    uint8_t  bTerminalLink;
    uint8_t  bStillCaptureMethod;
    uint8_t  bTriggerSupport;
    uint8_t  bTriggerUsage;
  } usbdescbldr_vs_if_input_header_short_form_t;

  usbdescbldr_status_t
    usbdescbldr_make_vs_interface_header(usbdescbldr_ctx_t * ctx,
                                         usbdescbldr_item_t * item,
                                         usbdescbldr_vs_if_input_header_short_form_t * form,
                                         ...); // Varying: bmaControls (which are passed as int32s), terminated by USBDESCBLDR_LIST_END


  // //////////////////////////////////////////////////////////////////
  // UVC Video Stream Interface Output Header

    typedef struct
  {
      uint8_t  bNumFormats;
      uint8_t  bEndpointAddress;
      uint8_t  bmInfo;
      uint8_t  bTerminalLink;
      uint8_t  bStillCaptureMethod;
      uint8_t  bTriggerSupport;
      uint8_t  bTriggerUsage;
    } usbdescbldr_vs_if_output_header_short_form_t;

    usbdescbldr_status_t
      usbdescbldr_make_uvc_vs_if_output_header(usbdescbldr_ctx_t * ctx,
                                               usbdescbldr_item_t * item,
                                               usbdescbldr_vs_if_output_header_short_form_t * form,
                                               ...); // Varying: bmaControls (which are passed as int32s), terminated by USBDESCBLDR_LIST_END


    // //////////////////////////////////////////////////////////////////


    typedef struct {
      uint8_t bFormatIndex;
      uint8_t bNumFrameDescriptors;
      GUID    guidFormat;
      uint8_t bBitsPerPixel;
      uint8_t bDefaultFrameIndex;
      uint8_t bAspectRatioX;
      uint8_t bAspectRatioY;
      uint8_t bmInterlaceFlags;
      uint8_t bCopyProtect;
      uint8_t bVariableSize;
    } usbdescbldr_uvc_vs_format_frame_based_short_form_t;

    usbdescbldr_status_t
      usbdescbldr_make_uvc_vs_format_frame(usbdescbldr_ctx_t * ctx,
                                           usbdescbldr_item_t * item,
                                           usbdescbldr_uvc_vs_format_frame_based_short_form_t * form);


    // //////////////////////////////////////////////////////////////////

    typedef struct
    {
      uint8_t  bFrameIndex;
      uint8_t  bmCapabilities;
      uint16_t wWidth;
      uint16_t wHeight;
      uint32_t dwMinBitRate;
      uint32_t dwMaxBitRate;
      uint32_t dwMaxVideoFrameBufferSize;
      uint32_t dwDefaultFrameInterval;
      uint8_t  bFrameIntervalType;
      uint32_t dwBytesPerLine;
    } usbdescbldr_uvc_vs_frame_frame_based_short_form_t;


    usbdescbldr_status_t
      usbdescbldr_make_uvc_vs_frame_frame(usbdescbldr_ctx_t * ctx,
                                          usbdescbldr_item_t * item,
                                          usbdescbldr_uvc_vs_frame_frame_based_short_form_t * form,
                                          ...);


    // //////////////////////////////////////////////////////////////////
    // Streaming Format Descriptors

    typedef struct
    {
      uint8_t bFormatIndex;
      uint8_t bNumFrameDescriptors;
      GUID    guidFormat;
      uint8_t bBitsPerPixel;
      uint8_t bDefaultFrameIndex;
      uint8_t bAspectRatioX;
      uint8_t bAspectRatioY;
      uint8_t bmInterlaceFlags;
      uint8_t bCopyProtect;
    } usbdescbldr_uvc_vs_format_uncompressed_short_form_t;

    // UVC Video Stream Format (Uncompressed)

    usbdescbldr_status_t
      usbdescbldr_make_uvc_vs_format_uncompressed(usbdescbldr_ctx_t * ctx,
                                                  usbdescbldr_item_t * item,
                                                  usbdescbldr_uvc_vs_format_uncompressed_short_form_t * form);


    // //////////////////////////////////////////////////////////////////
    // Streaming Frame Descriptors

    typedef struct
    {
      uint8_t   bFrameIndex;
      uint8_t   bmCapabilities;
      uint16_t wWidth;
      uint16_t wHeight;
      uint32_t dwMinBitRate;
      uint32_t dwMaxBitRate;
      uint32_t dwMaxVideoFrameBufferSize;
      uint32_t dwDefaultFrameInterval;
      uint8_t bFrameIntervalType;
    } usbdescbldr_uvc_vs_frame_uncompressed_short_form_t;

    usbdescbldr_status_t
      usbdescbldr_make_uvc_vs_frame_uncompressed(usbdescbldr_ctx_t * ctx,
                                                 usbdescbldr_item_t * item,
                                                 usbdescbldr_uvc_vs_frame_uncompressed_short_form_t * form,
      ... /* interval data */);


#ifdef __cplusplus
}
#endif
