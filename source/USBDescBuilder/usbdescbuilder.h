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

  // //////////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////////
  // API
typedef struct usbdescbldr_ctx_s {
  unsigned char 	initialized;	// Have we been initialized? 0: no.

  unsigned char *	buffer;		    // Start of user-provided buffer (if any)
  size_t		      bufferSize;	  // Length in uint8_tas of any user-provided buffer
  unsigned char * append;		    // Address in buffer for next addition (append)
  unsigned char *	last_append;	// Start address of last completed 'level' (see stack)

  unsigned int  	i_string;	    // Next string index to be assigned
  unsigned int    i_devConfig;  // Next device configuration index to be assigned

  uint16_t(*fLittleShortToHost)(uint16_t s);
  uint16_t(*fHostToLittleShort)(uint16_t s);
  unsigned int(*fLittleIntToHost)(unsigned int s);
  unsigned int(*fHostToLittleInt)(unsigned int s);

  // ...
} usbdescbldr_ctx_t;

  typedef enum {
    USBDESCBLDR_OK,
    USBDESCBLDR_UNINITIALIZED,		// Never initialized
    USBDESCBLDR_UNSUPPORTED,		  // Never written...
    USBDESCBLDR_DRY_RUN,		      // Not actually creating the datum
    USBDESCBLDR_NO_SPACE,		      // Buffer exhausted
    USBDESCBLDR_LEVEL_VIOLATION,	// Can't do that here
    USBDESCBLDR_INVALID,		      // Null ptr, duplicate, range error..
    USBDESCBLDR_OVERSIZED,        // Exceeds size limit
    USBDESCBLDR_TOO_MANY,         // Exceeds instance count limit
    // ...
  } usbdescbldr_status_t;

  // The maker functions which accept a variable number of
  // arguments need a special value to terminate the list.
  // (It's this, or make people count their arguments manually.)
  // Since zero can be a common value to pass, zero makes a poor
  // terminator. We choose a value which cannot be represented as
  // a uint8_t or uint16_t and which should be unlikely
  // as a DWORD (uint32_t) value in a descriptor.
  static const uint32_t USBDESCBLDR_LIST_END = 0xee00eeee;

  // The 'handle' by which callers store the results of maker calls. Callers
  // 'know' this structure only to provide them as return (out) parameters;
  // never should they modify the contents.
#define USBDESCBLDR_MAX_CHILDREN 8
  typedef struct usbdescbldr_item_s {
    void *                      address;        // Where it is stored
    unsigned int                index;          // Index, interface number, endpoint number, anything of this nature
    size_t                      size;           // Size of item itself
    size_t                      totalSize;      // Size of item and all children (may be easier to compute this only when done)
    unsigned int                items;          // Number iof sub-items ('children')
    struct usbdescbldr_item_s * item[USBDESCBLDR_MAX_CHILDREN];
  } usbdescbldr_item_t;

  // Setup and teardown

  // Reset internal state and begin a new descriptor.
  // Passing NULL for the buffer indicates a 'dry run' --
  // go through all the motions, checks, and size computations
  // but do not actually create a descriptor.

  usbdescbldr_status_t
    usbdescbldr_init(usbdescbldr_ctx_t *  ctx,
    unsigned char *      buffer,
    size_t               bufferSize);


  // Commit (complete/finish) the descriptor in progress.
  // Collections, interfaces, etc left open will be tidied up
  // (if possible..?)
  usbdescbldr_status_t
  usbdescbldr_close(void);

  // Size of descriptor 'so far'
  usbdescbldr_status_t
  usbdescbldr_descriptor_size(void);

  // Address (within buffer) of the last completed level/item.
  // Returns NULL when called in 'dry-run' mode.
  usbdescbldr_status_t
  usbdescbldr_last_address(unsigned char **base_address);

  // Terminate use of the builder. Release any resources.
  // Once this is complete, the API requires an _init() before
  // anything will perform again.
  usbdescbldr_status_t
  usbdescbldr_end(void);


  // Constructions

  // Make a set of items subordinate to one parent item. This
  // is used to both place the items contiguously, in order,
  // in memory after the parent item and to allow the parent
  // item to account for their accumulated lengths.
  // Pass the context, a result item, and the subordinate items.

  usbdescbldr_status_t
    usbdescbldr_add_children(usbdescbldr_ctx_t *  ctx,
    usbdescbldr_item_t * parent,
    ...);

  // Create the language descriptor (actually string, index 0).
  // Pass the context, a result item, and the IDs.
  usbdescbldr_status_t
    usbdescbldr_make_languageIDs(usbdescbldr_ctx_t * ctx,
                               usbdescbldr_item_t * item,
                               ...);

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
    
  
  typedef struct {
    uint16_t	bcdUSB;		// USB Spec Version
    uint8_t	bDeviceClass;
    uint8_t	bDeviceSubClass;
    uint8_t	bDeviceProtocol;
    uint16_t	idVendor;
    uint16_t	idProduct;
    uint16_t	bcdDevice;	// BCD-encoded Release #
    uint8_t	iManufacturer;	// String index
    uint8_t	iProduct;	// String index
    uint8_t	iSerialNumber;	// String Index
    uint8_t	bNumConfigurations;
  } usbdescbldr_device_qualifier_short_form_t;

  // Generate a USB Device Qualifier Descriptor. This is one-shot and top-level.
  usbdescbldr_status_t
    usbdescbldr_make_device_qualifier_descriptor(usbdescbldr_ctx_t *ctx,
                                               usbdescbldr_item_t *item, 
                                               usbdescbldr_device_qualifier_short_form_t * form);
    

  // Define a new string and obtain its index. This is a one-shot top-level item.
  // Pass the string in ASCII and NULL-terminated (a classic C string).
  usbdescbldr_status_t
    usbdescbldr_make_string_descriptor(usbdescbldr_ctx_t *ctx,
                                    usbdescbldr_item_t *item,     // OUT
                                    uint8_t *index,         // OUT
                                    char *string);                // OUT


  // Generate a Binary Object Store. This is top-level.
  // Add the device Capabilities to it to complete the BOS.
  usbdescbldr_status_t 
    usbdescbldr_make_bos_descriptor(usbdescbldr_ctx_t *       ctx,
                                  usbdescbldr_item_t *          item);

  // Generate a Device Capability descriptor.
  usbdescbldr_status_t
    usbdescbldr_make_device_capability_descriptor(usbdescbldr_ctx_t * ctx,
                                                usbdescbldr_item_t *    item,
                                                uint8_t	          bDevCapabilityType,
                                                uint8_t *	        typeDependent,	// Anonymous uint8_ta data
                                                size_t		              typeDependentSize);

 

  // Generate a Device Configuration descriptor. 
  // The string index for
  // the iConfiguration must be provided, but also the string (if any)
  // must be added as a child of the configuration.
  // Likewise, the short form takes the number of interfaces, but these too
  // must be added to the children.
  // The assigned configuration
  // index is returned -- this is probably presumed by other code,
  // and should probably be specified explicitly.. we'll see. Currently 
  // if that is the case, I assume the caller will perform an assert() 
  // or other sanity check on the returned value.

  typedef struct {
    uint8_t  bNumInterfaces;
    uint8_t  iConfiguration;
    uint8_t  bmAttributes;
    uint8_t  bMaxPower;
  } usbdescbldr_device_configuration_short_form_t;

  usbdescbldr_status_t
    usbdescbldr_make_device_configuration_descriptor(usbdescbldr_ctx_t * ctx,
    usbdescbldr_item_t * item,
    uint8_t * index,
    usbdescbldr_device_configuration_short_form_t * form);


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



  typedef struct {
    uint8_t  bMaxBurst;
    uint8_t  bmAttributes;
    uint16_t wBytesPerInterval;
  } usbdescbldr_ss_ep_companion_short_form_t;

  usbdescbldr_status_t
    usbdescbldr_make_ss_ep_companion_descriptor(usbdescbldr_ctx_t * ctx,
    usbdescbldr_item_t * item,
    usbdescbldr_ss_ep_companion_short_form_t * form);



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


  usbdescbldr_status_t
    usbdescbldr_make_video_control_interface_descriptor(usbdescbldr_ctx_t * ctx,
    usbdescbldr_item_t * item,
    usbdescbldr_standard_interface_short_form_t * form);


  usbdescbldr_status_t
    usbdescbldr_make_vc_cs_interface_descriptor(usbdescbldr_ctx_t * ctx,
    usbdescbldr_item_t * item,
    unsigned int dwClockFrequency,
    ... // Terminated List of Interface Numbers 
    );


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




  usbdescbldr_status_t
    usbdescbldr_make_vc_selector_unit(usbdescbldr_ctx_t * ctx,
    usbdescbldr_item_t * item,
    unsigned int bUnitID,
    ... // Terminated List of Input (Source) Pin(s)
    );




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

  
  typedef struct
  {
    uint8_t  bUnitID;
    GUID     guidExtensionCode;
    uint8_t  bNumControls;
    uint8_t  bNrInPins;
    uint8_t  baSourceID;
    uint8_t  bControlSize;
    uint8_t * bmControls;
    uint8_t  iExtension;
  } usbdescbldr_vc_extension_unit_short_form_t;

  usbdescbldr_status_t
    usbdescbldr_make_extension_unit_descriptor(usbdescbldr_ctx_t * ctx,
    usbdescbldr_item_t * item,
    usbdescbldr_vc_extension_unit_short_form_t * form,
    ...);


#ifdef __cplusplus
}
#endif
