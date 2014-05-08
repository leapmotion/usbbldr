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
///
/// The context must be initialized before making any other use of the API.
///
/// Callers should treat the context as 'read only'.

typedef struct usbdescbldr_ctx_s {
  unsigned char   initialized;  // Have we been initialized? 0: no.

  unsigned char * buffer;       // Start of user-provided buffer (if any)
  size_t          bufferSize;   // Length in bytes of any user-provided buffer
  unsigned char * append;       // Address in buffer for next addition (append)

  unsigned int    i_string;     // Next string index to be assigned

  // Conversion functions for little-endian support across platforms.
  uint16_t(*fLittleShortToHost)(uint16_t s);
  uint16_t(*fHostToLittleShort)(uint16_t s);
  unsigned int(*fLittleIntToHost)(unsigned int s);
  unsigned int(*fHostToLittleInt)(unsigned int s);
} usbdescbldr_ctx_t;

/// Error values returned by the API.
  typedef enum {
    USBDESCBLDR_OK,               ///< Success
    USBDESCBLDR_UNINITIALIZED,    ///< Never initialized
    USBDESCBLDR_UNSUPPORTED,      ///< Never written...
    USBDESCBLDR_DRY_RUN,          ///< Not actually creating the datum
    USBDESCBLDR_NO_SPACE,         ///< Buffer exhausted
    USBDESCBLDR_INVALID,          ///< Null ptr, duplicate, range error..
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

  /// API members which accept a variable number of arguments
  /// typically will use this constant to terminate the list.
  /// API members which accept a variable number of pointers
  /// will use NULL to terminate their lists.
  static const uint32_t USBDESCBLDR_LIST_END = 0xee00eeef;


  // ITEM
  // The 'handle' by which callers store the results of maker calls. Callers
  // 'know' this structure only to provide them as return (inout) parameters;
  // never should they modify the contents.

  /// Items (the handles) contain a list of the items they enclose
  /// (items tha contribute to their total length).
#define USBDESCBLDR_MAX_CHILDREN 16

  /// Items are filled in as results from each of the API maker
  /// calls. They will be used after the maker calls have been
  /// finished to layer the descriptors and build up the lengths
  /// of layered descriptors, and to access the binary results
  /// for each descriptor.
  /// 
  /// Callers should treat items as 'read only'.
  typedef struct usbdescbldr_item_s {
    void *                      address;        ///< Where it is stored
    unsigned int                index;          ///< Index, interface number, endpoint number, anything of this nature
    uint16_t                    size;           ///< Size of item itself
    uint8_t *                   totalSize;      ///< Unaligned uint16_t *; Size of item and all children, or NULL if not kept
    unsigned int                items;          ///< Number of sub-items ('children')
    struct usbdescbldr_item_s * item[USBDESCBLDR_MAX_CHILDREN];
  } usbdescbldr_item_t;

 // The standard GUID:
  typedef struct usbdescbldr_guid_s
  {
    uint32_t dwData1;
    uint16_t dwData2;
    uint16_t dwData3;
    uint8_t  dwData4[8];
  } usbdescbldr_guid_t;


  // Setup and teardown

  /// Reset internal state and begin a new descriptor.
  /// Passing NULL for the buffer indicates a 'dry run' --
  /// go through all the motions, checks, and size computations
  /// but do not actually create a descriptor.
  ///\param [in] ctx A context to be initialized for the session.
  ///\param [in] buffer An optional buffer for the session to use to actually assemble the
  /// results into final forms. If NULL is provided, the API 
  /// will simply use the items to try and come up with a scaffolding of
  /// the final results, which may provide sanity checking and sizes.
  ///\param [in] bufferSize The size in bytes of the buffer.
  usbdescbldr_status_t
  usbdescbldr_init(usbdescbldr_ctx_t *  ctx,
                   unsigned char *      buffer,
                   size_t               bufferSize);

  /// Commit (complete/finish) the build session in progress.
  usbdescbldr_status_t
    usbdescbldr_close(usbdescbldr_ctx_t * ctx);

  /// Terminate use of the builder. Release any resources.
  /// Once this is complete, the API requires an _init() before
  /// anything will perform again.
  usbdescbldr_status_t
    usbdescbldr_end(usbdescbldr_ctx_t * ctx);

  // //////////////////////////////////////////////////////////////////
  // Constructions

  /// Make a set of items subordinate to one parent item. This
  /// is used to allow the parent item to account for their accumulated lengths.
  /// Pass the context, a result item, and the subordinate items.
  /// Terminate this list with NULL.
  /// This call can be used as many times as necessary (its effects
  /// are cumulative).
  ///\param [in] ctx The context for the session.
  ///\param [in] parent The item to accept new subordinates.
  ///\param [in] ... The items for the parent to accept, NULL-terminated.
  usbdescbldr_status_t
    usbdescbldr_add_children(usbdescbldr_ctx_t *  ctx,
                             usbdescbldr_item_t * parent,
                             ...);

  // //////////////////////////////////////////////////////////////////

  /// Create the language descriptor (actually string, index 0).
  /// Pass the context, a result item, and the language IDs.
  ///\param [in] ctx The context for the session.
  ///\param [in] item The item to receive the result.
  ///\param [in] ... The language specifiers to be included in the descriptor. 
  /// Terminate this list with USBDESCBLDR_LIST_END .
  usbdescbldr_status_t
    usbdescbldr_make_languageIDs(usbdescbldr_ctx_t * ctx,
                                 usbdescbldr_item_t * item,
                                 ...);

  // //////////////////////////////////////////////////////////////////
  // Maker calls.
  //
  // Maker actions that require several inputs take a 'short form' structure,
  // essentially the values for the descriptor-to-be which the builder cannot
  // immediately derive itself (or which the caller probably wants to control).
  //
  // By and large, the short-form members are named directly and obviously
  // to clearly 'be' the USB specification values they provide, and are not
  // described here in any greater detail.
  //
  // Maker actions return an item containing the result. As a side-effect,
  // the buffer (if any) has been updated with the scaffold of the requested
  // descriptor, completely excluding any layering which will be performed once all the
  // subordinate descriptors are also available.
  //
  // For the time being, and perhaps for all time, the buffer contents are defined
  // by the order of the maker calls which filled it. This imposes some requirements on
  // the order of maker calls -- in a word, maker calls should be made 'top-down'
  // to place each descriptor in the order expected for the completed, flat,
  // buffered result(s).

  /// The Device Descriptor short-form.
  /// The content of the short forms is intended to precisely mimic the descriptor each one
  /// creates. Please refer to the USB and UVC specifications for details on short form members.
   typedef struct {
    uint16_t  bcdUSB;           // USB Spec Version
    uint8_t   bDeviceClass;
    uint8_t   bDeviceSubClass;
    uint8_t   bDeviceProtocol;
    uint16_t  idVendor;
    uint16_t  idProduct;
    uint16_t  bcdDevice;        // BCD-encoded Release #
    uint8_t   iManufacturer;    // String index
    uint8_t   iProduct;         // String index
    uint8_t   iSerialNumber;    // String Index
    uint8_t   bNumConfigurations;
  } usbdescbldr_device_descriptor_short_form_t;

   /// Build a device descriptor based upon those values given in the short form by the
   /// caller.
   ///\param [in] ctx The Builder context.
   ///\param [in] ctx The Builder context.
   ///\param [in,out] item The item to receive the results.
   ///\param [in] form Descriptor-specific values which must be specified by the caller.
  usbdescbldr_status_t
    usbdescbldr_make_device_descriptor(usbdescbldr_ctx_t *ctx,
                                       usbdescbldr_item_t *item,
                                       const usbdescbldr_device_descriptor_short_form_t *form);
    
  // //////////////////////////////////////////////////////////////////

  /// The Make Device Qualifier short-form.
  /// The content of the short forms is intended to precisely mimic the descriptor each one
  /// creates. Please refer to the USB and UVC specifications for details on short form members.
  typedef struct {
    uint16_t  bcdUSB;   // USB Spec Version
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint8_t bNumConfigurations;
  } usbdescbldr_device_qualifier_short_form_t;

  // Generate a USB Device Qualifier Descriptor.
  ///\param [in] ctx The context for the session.
  ///\param [in] item The result for the make.
  ///\param [in] form The values to be used to populate the descriptor.
  usbdescbldr_status_t
    usbdescbldr_make_device_qualifier_descriptor(usbdescbldr_ctx_t *ctx,
                                                 usbdescbldr_item_t *item, 
                                                 const usbdescbldr_device_qualifier_short_form_t * form);
    
  // //////////////////////////////////////////////////////////////////

  /// Define a new string and obtain its index. The string is passed as a flat C-style
  /// ASCII string. Builder assigns string indices automatically, and so the result
  /// is passed back to the caller through the (optional) out parameter index .
  /// (This is also stored in the item; should the caller wish to 'worry about it' later, it can be
  /// accessed there.)
  ///\param [in] ctx The context for the session.
  ///\param [in] item The result for the make.
  ///\param [out] index If non-NULL, the assigned string index will be returned here.
  ///\param [in] string The string to be stored in the descriptor.
  // Pass the string in ASCII and NULL-terminated (a classic C string).
  usbdescbldr_status_t
    usbdescbldr_make_string_descriptor(usbdescbldr_ctx_t *     ctx,
                                       usbdescbldr_item_t *    item,
                                       uint8_t *               index,    // OUT
                                       const char *            string);  // IN

  // //////////////////////////////////////////////////////////////////

  /// Generate a Binary Object Store.
  /// Once constructed, the caller can add Device Capabilities to a BOS to complete the BOS.
  ///\param [in] ctx The Builder context.
  ///\param [in,out] item The item to receive the results.
  ///\param [in] capabilities The number of device capabilities to follow.
 usbdescbldr_status_t
    usbdescbldr_make_bos_descriptor(usbdescbldr_ctx_t *  ctx,
                                    usbdescbldr_item_t * item,
                                    uint8_t              capabilities);


  /// Generate a Device Capability descriptor.
  ///\param [in] ctx The Builder context.
  ///\param [in,out] item The item to receive the results.
  ///\param [in] bDevCapabilityType The capability identifier for this device capability.
  ///\param [in] typeDependent Anonymous, type-specific data to be represented by this descriptor.
  ///\param [in] typeDependentSize The sizes in bytes of the typeDependent data.
  usbdescbldr_status_t
    usbdescbldr_make_device_capability_descriptor(usbdescbldr_ctx_t *  ctx,
                                                  usbdescbldr_item_t * item,
                                                  uint8_t              bDevCapabilityType,
                                                  const uint8_t *      typeDependent, // Anonymous byte data
                                                  size_t               typeDependentSize);

 
  // //////////////////////////////////////////////////////////////////

  /// The Configuration short-form.
  /// The content of the short forms is intended to precisely mimic the descriptor each one
  /// creates. Please refer to the USB and UVC specifications for details on short form members.
  typedef struct {
    uint8_t  bNumInterfaces;
    uint8_t  bConfigurationValue;
    uint8_t  iConfiguration;
    uint8_t  bmAttributes;
    uint8_t  bMaxPower;
  } usbdescbldr_device_configuration_short_form_t;

  /// Generate a Device Configuration descriptor. 
  /// The string index for
  /// the iConfiguration must be provided, but also the string (if any)
  /// must be added as a child of the configuration.
  /// Likewise, the short form takes the number of interfaces, but these too
  /// must be added to the children.
  ///\param [in] ctx The Builder context.
  ///\param [in,out] item The item to receive the results.
  ///\param [in] form Descriptor-specific values which must be specified by the caller.
  usbdescbldr_status_t
    usbdescbldr_make_device_configuration_descriptor(usbdescbldr_ctx_t * ctx,
                                                     usbdescbldr_item_t * item,
                                                     const usbdescbldr_device_configuration_short_form_t * form);


  // //////////////////////////////////////////////////////////////////

  /// The standard Interface short-form.
  /// The content of the short forms is intended to precisely mimic the descriptor each one
  /// creates. Please refer to the USB and UVC specifications for details on short form members.
  typedef struct {
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
  } usbdescbldr_standard_interface_short_form_t;

  /// Generate a standard (as opposed to class-specific) interface descriptor. By varying
  /// the class, subclass, and protocol values in the request, several USB descriptor needs
  /// may be met by this call.
  /// Pass the context, a result item, and the completed Interface Descriptor 
  /// short-form structure. Note that some of the values needed for this descriptor must be obtained from other items
  /// (e.g. the interface string index), and as such those should be made before this descriptor is made.
  ///\param [in] ctx The context for the session.
  ///\param [in,out] item The result for the make.
  ///\param [in] form The values to be used to populate the descriptor.
  usbdescbldr_status_t
    usbdescbldr_make_standard_interface_descriptor(usbdescbldr_ctx_t * ctx,
                                                   usbdescbldr_item_t * item,
                                                   const usbdescbldr_standard_interface_short_form_t * form);


  /// The Make UVC Video Control Interface short-form.
  /// The content of the short forms is intended to precisely mimic the descriptor each one
  /// creates. Please refer to the USB and UVC specifications for details on short form members.
  typedef struct {
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t iInterface;
  } usbdescbldr_vc_interface_short_form_t;

  /// Generate a UVC Video Control interface descriptor.
  /// Pass the context, a result item, and the completed Interface Descriptor 
  /// short-form structure.
  /// Note that some of the values needed for this descriptor must be obtained from other items
  /// (e.g. the string index), and as such those should be made before this descriptor is made.
  ///\param [in] ctx The context for the session.
  ///\param [in,out] item The result for the make.
  ///\param [in] form The values to be used to populate the descriptor.
  usbdescbldr_status_t
    usbdescbldr_make_vc_interface_descriptor(usbdescbldr_ctx_t * ctx,
                                             usbdescbldr_item_t * item,
                                             const usbdescbldr_vc_interface_short_form_t * form);


  /// The Make UVC Video Streaming Interface short-form.
  /// The content of the short forms is intended to precisely mimic the descriptor each one
  /// creates. Please refer to the USB and UVC specifications for details on short form members.
  typedef struct {
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t iInterface;
  } usbdescbldr_vs_interface_short_form_t;

  /// Generate a UVC Video Streaming interface descriptor.
  /// Pass the context, a result item, and the completed Interface Descriptor 
  /// short-form structure.
  /// Note that some of the values needed for this descriptor must be obtained from other items
  /// (e.g. the string index), and as such those should be made before this descriptor is made.
  ///\param [in] ctx The context for the session.
  ///\param [in,out] item The result for the make.
  ///\param [in] form The values to be used to populate the descriptor.
  usbdescbldr_status_t
    usbdescbldr_make_vs_interface_descriptor(usbdescbldr_ctx_t * ctx,
                                             usbdescbldr_item_t * item,
                                             const usbdescbldr_vs_interface_short_form_t * form);


  // //////////////////////////////////////////////////////////////////

  /// The content of the short forms is intended to precisely mimic the descriptor each one
  /// creates. Please refer to the USB and UVC specifications for details on short form members.

  typedef struct {
    uint8_t  bEndpointAddress;
    uint8_t  bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t  bInterval;
  } usbdescbldr_endpoint_short_form_t;

  /// Generate an Endpoint descriptor.
  /// Pass the context, a result item, and the completed 
  /// short-form structure.
  ///\param [in] ctx The context for the session.
  ///\param [in,out] item The result for the make.
  ///\param [in] form The values to be used to populate the descriptor.
  usbdescbldr_status_t
    usbdescbldr_make_endpoint_descriptor(usbdescbldr_ctx_t * ctx,
                                         usbdescbldr_item_t * item,
                                         const usbdescbldr_endpoint_short_form_t * form);


  // //////////////////////////////////////////////////////////////////

  /// The content of the short forms is intended to precisely mimic the descriptor each one
  /// creates. Please refer to the USB and UVC specifications for details on short form members.

  typedef struct {
    uint8_t  bMaxBurst;
    uint8_t  bmAttributes;
    uint16_t wBytesPerInterval;
  } usbdescbldr_ss_ep_companion_short_form_t;

  /// Generate a SuperSpeed Endpoint Companion descriptor.
  /// Pass the context, a result item, and the completed short-form structure.
  ///\param [in] ctx The context for the session.
  ///\param [in,out] item The result for the make.
  ///\param [in] form The values to be used to populate the descriptor.
  usbdescbldr_status_t
    usbdescbldr_make_ss_ep_companion_descriptor(usbdescbldr_ctx_t * ctx,
                                                usbdescbldr_item_t * item,
                                                const usbdescbldr_ss_ep_companion_short_form_t * form);


  // //////////////////////////////////////////////////////////////////

  /// The content of the short forms is intended to precisely mimic the descriptor each one
  /// creates. Please refer to the USB and UVC specifications for details on short form members.

  typedef struct {
    uint8_t bFirstInterface;
    uint8_t bInterfaceCount;
    uint8_t bFunctionClass;
    uint8_t bFunctionSubClass;
    uint8_t bFunctionProtocol;
    uint8_t iFunction;
  } usbdescbldr_iad_short_form_t;

  /// Generate an IAD (interface association descriptor).
  /// Pass the context, a result item, and the completed Interface Descriptor 
  /// short-form structure.
  ///\param [in] ctx The context for the session.
  ///\param [in,out] item The result for the make.
  ///\param [in] form The values to be used to populate the descriptor.
  usbdescbldr_status_t
    usbdescbldr_make_interface_association_descriptor(usbdescbldr_ctx_t * ctx,
                                                      usbdescbldr_item_t * item,
                                                      const usbdescbldr_iad_short_form_t * form);

  // //////////////////////////////////////////////////////////////////

  /// Generate a UVC Video Control Interface Header descriptor, variadic style.
  /// Pass the context, a result item, and the completed short-form structure.
  /// Note that the interfaces to be associated are provded (by number) at this
  /// point. The caller must be responsible for assigning these numbers to the
  /// interfaces which this header will enclose.
  ///\param [in] ctx The context for the session.
  ///\param [in,out] item The result for the make.
  ///\param [in] dwClockFrequency The clock frequency, in Hertz (deprecated; included for completeness).
  ///\param [in] ... The interfaces to follow this header. This list is terminated with USBDESCBLDR_END_LIST .
  usbdescbldr_status_t
    usbdescbldr_make_vc_interface_header(usbdescbldr_ctx_t *  ctx,
                                         usbdescbldr_item_t * item,
                                         uint32_t             dwClockFrequency,
                                         ...); // Terminated List of Interface Numbers 
    


  /// Generate a UVC Video Control Interface Header descriptor, fixed parameter style.
  /// Pass the context, a result item, and the completed short-form structure.
  /// Note that the interfaces to be associated are provded (by number) at this
  /// point. The caller must be responsible for assigning these numbers to the
  /// interfaces which this header will enclose.
  ///\param [in] ctx The context for the session.
  ///\param [in,out] item The result for the make.
  ///\param [in] dwClockFrequency The clock frequency, in Hertz (deprecated; included for completeness).
  ///\param [in] interfaceList The interfaces to follow this header.
  ///\param [in] interfaceListLength The number of interfaces in the interfaceList.
  usbdescbldr_status_t
    usbdescbldr_make_vc_interface_header_fixed(usbdescbldr_ctx_t *  ctx,
                                               usbdescbldr_item_t * item,
                                               uint32_t             dwClockFrequency,
                                               const uint8_t *      interfaceList,
                                               size_t               interfaceListLength);
    
  // //////////////////////////////////////////////////////////////////

  // Video Control Units

  /// The content of the short forms is intended to precisely mimic the descriptor each one
  /// creates. Please refer to the USB and UVC specifications for details on short form members.

  typedef struct {
    uint8_t  bTerminalID;
    uint8_t  bAssocTerminal;
    uint8_t  iTerminal;
    uint16_t wObjectiveFocalLengthMin;
    uint16_t wObjectiveFocalLengthMax;
    uint16_t wOcularFocalLength;
    uint32_t controls;
  } usbdescbldr_camera_terminal_short_form_t;


  /// Generate a VC Camera Terminal (an Input) Descriptor.
  /// Pass the context, a result item, and the completed short-form structure.
  ///\param [in] ctx The context for the session.
  ///\param [in,out] item The result for the make.
  ///\param [in] form The values to be used to populate the descriptor.

  usbdescbldr_status_t
    usbdescbldr_make_camera_terminal_descriptor(usbdescbldr_ctx_t *  ctx,
                                                usbdescbldr_item_t * item,
                                                const usbdescbldr_camera_terminal_short_form_t * form);

 
  // //////////////////////////////////////////////////////////////////

  /// The content of the short forms is intended to precisely mimic the descriptor each one
  /// creates. Please refer to the USB and UVC specifications for details on short form members.

  typedef struct {
    uint8_t  bTerminalID;
    uint8_t  bAssocTerminal;
    uint8_t  bSourceID;
    uint8_t  iTerminal;
  } usbdescbldr_streaming_out_terminal_short_form_t;

  /// Generate a VC Streaming Terminal (an Output) Descriptor.
  /// Pass the context, a result item, and the completed short-form structure.
  ///\param [in] ctx The context for the session.
  ///\param [in,out] item The result for the make.
  ///\param [in] form The values to be used to populate the descriptor.

  usbdescbldr_status_t
    usbdescbldr_make_streaming_out_terminal_descriptor(usbdescbldr_ctx_t *  ctx,
                                                       usbdescbldr_item_t * item,
                                                       const usbdescbldr_streaming_out_terminal_short_form_t * form);


  // //////////////////////////////////////////////////////////////////

  /// Generate a VC Selector Unit Descriptor, variadic style.
  /// Pass the context, a result item, and the completed short-form structure.
  ///\param [in] ctx The context for the session.
  ///\param [in,out] item The result for the make.
  ///\param [in] iSelector The string index to describe the selector.
  ///\param [in] bUnitID The unit identifier for this selector.
  ///\param [in] ... The inputs for the pins, input[1]..input[n]. This list is terminated with USBDESCBLDR_LIST_END .

  usbdescbldr_status_t
    usbdescbldr_make_vc_selector_unit(usbdescbldr_ctx_t * ctx,
                                      usbdescbldr_item_t * item,
                                      uint8_t              iSelector, // string index
                                      uint8_t              bUnitID,
                                      ...); // Terminated List of Input (Source) Pin(s)
   

  /// Generate a VC Selector Unit Descriptor, fixed parameters style.
  /// Pass the context, a result item, and the completed short-form structure.
  ///\param [in] ctx The context for the session.
  ///\param [in,out] item The result for the make.
  ///\param [in] iSelector The string index to describe the selector.
  ///\param [in] bUnitID The unit identifier for this selector.
  ///\param [in] inputs The inputs for the pins, input[1]..input[n].
  ///\param [in] inputsLength The number of inputs in the list (n - 1).

  usbdescbldr_status_t
    usbdescbldr_make_vc_selector_unit_fixed(usbdescbldr_ctx_t *  ctx,
                                            usbdescbldr_item_t * item,
                                            uint8_t              iSelector,     // string index
                                            uint8_t              bUnitID,
                                            const uint8_t *      inputs,        // List of Input (Source) Pin(s)
                                            size_t               inputsLength); // Length of the above
   

  // //////////////////////////////////////////////////////////////////

  /// The content of the short forms is intended to precisely mimic the descriptor each one
  /// creates. Please refer to the USB and UVC specifications for details on short form members.

  typedef struct
  {
    uint8_t  bUnitID;
    uint8_t  bSourceID;
    uint16_t wMaxMultiplier;
    uint32_t controls;
    uint8_t  iProcessing;
    uint8_t  bmVideoStandards;  // UVC 1.1 and above; otherwise, ignored.
  } usbdescbldr_vc_processor_unit_short_form;

  /// Generate a VC Processing Uint Descriptor.
  /// Pass the context, a result item, and the completed short-form structure.
  ///\param [in] ctx The context for the session.
  ///\param [in,out] item The result for the make.
  ///\param [in] form The values to be used to populate the descriptor.

  usbdescbldr_status_t
    usbdescbldr_make_vc_processor_unit(usbdescbldr_ctx_t * ctx,
                                       usbdescbldr_item_t * item,
                                       const usbdescbldr_vc_processor_unit_short_form * form);

  
  // //////////////////////////////////////////////////////////////////

  /// The content of the short forms is intended to precisely mimic the descriptor each one
  /// creates. Please refer to the USB and UVC specifications for details on short form members.

  typedef struct
  {
    uint8_t   bUnitID;
    usbdescbldr_guid_t guidExtensionCode;
    uint8_t   bNumControls;
    uint8_t   bControlSize;
    uint8_t * bmControls;
    uint8_t   iExtension;
  } usbdescbldr_vc_extension_unit_short_form_t;

  /// Generate a VC Extension Unit Descriptor, variadic style.
  /// Pass the context, a result item, and the completed short-form structure.
  ///\param [in] ctx The context for the session.
  ///\param [in,out] item The result for the make.
  ///\param [in] form The values to be used to populate the descriptor.
  ///\param [in] ... The sources for the unit, source[1]..source[n]. This list is terminated with USBDESCBLDR_LIST_END .

  usbdescbldr_status_t
    usbdescbldr_make_extension_unit_descriptor(usbdescbldr_ctx_t *  ctx,
                                               usbdescbldr_item_t * item,
                                               const usbdescbldr_vc_extension_unit_short_form_t * form,
                                               ...);

  /// Generate a VC Extension Unit Descriptor, fixed parameters style.
  /// Pass the context, a result item, and the completed short-form structure.
  ///\param [in] ctx The context for the session.
  ///\param [in,out] item The result for the make.
  ///\param [in] form The values to be used to populate the descriptor.
  ///\param [in] sources The sources for the unit, source[1]..source[n].
  ///\param [in] sourcesLength The number of sources for the unit (n - 1) .

  usbdescbldr_status_t
    usbdescbldr_make_extension_unit_descriptor_fixed(usbdescbldr_ctx_t *  ctx,
                                                     usbdescbldr_item_t * item,
                                                     const usbdescbldr_vc_extension_unit_short_form_t * form,
                                                     const uint8_t *      sources,
                                                     size_t               sourcesLength);


  // //////////////////////////////////////////////////////////////////

  // UVC Class-Specific VC interrupt endpoint:

  usbdescbldr_status_t
    usbdescbldr_make_vc_interrupt_ep(usbdescbldr_ctx_t *  ctx,
                                     usbdescbldr_item_t * item,
                                     uint16_t             wMaxTransferSize);


  // //////////////////////////////////////////////////////////////////
  // UVC Video Stream Interface Input Header

  /// The content of the short forms is intended to precisely mimic the descriptor each one
  /// creates. Please refer to the USB and UVC specifications for details on short form members.

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

  /// Generate a UVC Video Stream Interface Input Header descriptor.
  /// Pass the context, a result item, and the completed short-form structure.
  ///\param [in] ctx The context for the session.
  ///\param [in,out] item The result for the make.
  ///\param [in] form The values to be used to populate the descriptor.
  ///\param [in] ... bmaControls, eight-bit bitmasks as specified in the UVC specification, passed in order of increasing significance.
  /// Terminate this list with USBDESCBLDR_LIST_END .
  usbdescbldr_status_t
    usbdescbldr_make_vs_interface_header(usbdescbldr_ctx_t *  ctx,
                                         usbdescbldr_item_t * item,
                                         const usbdescbldr_vs_if_input_header_short_form_t * form,
                                         ...);

  /// Generate a UVC Video Stream Interface Input Header descriptor, fixed parameter style.
  /// Pass the context, a result item, and the completed short-form structure.
  ///\param [in] ctx The context for the session.
  ///\param [in,out] item The result for the make.
  ///\param [in] form The values to be used to populate the descriptor.
  ///\param [in] ... bmaControls[0] .. bmaControls[j] 
  ///\param [in] The number of bmaControl bytes above (j - 1).
  /// Terminate this list with USBDESCBLDR_LIST_END .
  usbdescbldr_status_t
    usbdescbldr_make_vs_interface_header_fixed(usbdescbldr_ctx_t *  ctx,
                                               usbdescbldr_item_t * item,
                                               const usbdescbldr_vs_if_input_header_short_form_t * form,
                                               const uint8_t *      bmaControls,
                                               size_t               bmaControlsLength);


  // //////////////////////////////////////////////////////////////////
  // UVC Video Stream Interface Output Header

  /// The content of the short forms is intended to precisely mimic the descriptor each one
  /// creates. Please refer to the USB and UVC specifications for details on short form members.

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

    /// Generate a UVC Video Stream Interface Output Header descriptor, varadic style.
    /// Pass the context, a result item, and the completed short-form structure.
    ///\param [in] ctx The context for the session.
    ///\param [in,out] item The result for the make.
    ///\param [in] form The values to be used to populate the descriptor.
    ///\param [in] ... bmaControls, eight-bit bitmasks as specified in the UVC specification, passed in order of increasing significance.
    /// Terminate this list with USBDESCBLDR_LIST_END .
    usbdescbldr_status_t
      usbdescbldr_make_uvc_vs_if_output_header(usbdescbldr_ctx_t *  ctx,
                                               usbdescbldr_item_t * item,
                                               const usbdescbldr_vs_if_output_header_short_form_t * form,
                                               ...);


    /// Generate a UVC Video Stream Interface Output Header descriptor, fixed style.
    /// Pass the context, a result item, and the completed short-form structure.
    ///\param [in] ctx The context for the session.
    ///\param [in,out] item The result for the make.
    ///\param [in] form The values to be used to populate the descriptor.
    ///\param [in] bmaControls, eight-bit bitmasks as specified in the UVC specification
    ///\param [in] bmaControlsLength, eight-bit bitmasks as specified in the UVC specification
    /// Terminate this list with USBDESCBLDR_LIST_END .
    usbdescbldr_status_t
      usbdescbldr_make_uvc_vs_if_output_header_fixed(usbdescbldr_ctx_t *  ctx,
																										 usbdescbldr_item_t * item,
																										 const usbdescbldr_vs_if_output_header_short_form_t * form,
																										 const  uint8_t *     bmaControls,
																										 size_t               bmaControlsLength);


    // //////////////////////////////////////////////////////////////////

    /// The content of the short forms is intended to precisely mimic the descriptor each one
    /// creates. Please refer to the USB and UVC specifications for details on short form members.

    typedef struct {
      uint8_t bFormatIndex;
      uint8_t bNumFrameDescriptors;
      usbdescbldr_guid_t guidFormat;
      uint8_t bBitsPerPixel;
      uint8_t bDefaultFrameIndex;
      uint8_t bAspectRatioX;
      uint8_t bAspectRatioY;
      uint8_t bmInterlaceFlags;
      uint8_t bCopyProtect;
      uint8_t bVariableSize;
    } usbdescbldr_uvc_vs_format_frame_based_short_form_t;


    /// Generate a UVC Video Stream Format descriptor for Frame-Based payloads.
    /// Pass the context, a result item, and the completed short-form structure.
    ///\param [in] ctx The context for the session.
    ///\param [in,out] item The result for the make.
    ///\param [in] form The values to be used to populate the descriptor.
    usbdescbldr_status_t
      usbdescbldr_make_uvc_vs_format_frame(usbdescbldr_ctx_t * ctx,
                                           usbdescbldr_item_t * item,
                                           const usbdescbldr_uvc_vs_format_frame_based_short_form_t * form);


    // //////////////////////////////////////////////////////////////////

    /// The content of the short forms is intended to precisely mimic the descriptor each one
    /// creates. Please refer to the USB and UVC specifications for details on short form members.

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

    /// Generate a UVC Video Stream Frame descriptor for Frame-Based payloads, variadic form.
    /// Pass the context, a result item, and the completed short-form structure.
    ///\param [in] ctx The context for the session.
    ///\param [in,out] item The result for the make.
    ///\param [in] form The values to be used to populate the descriptor.
    ///\param [in] ... The frame intervals. Depending on the setting of bFrameIntervalType in the
    /// short form, the number (and meaning) of these will vary.

    usbdescbldr_status_t
      usbdescbldr_make_uvc_vs_frame_frame(usbdescbldr_ctx_t * ctx,
                                          usbdescbldr_item_t * item,
                                          const usbdescbldr_uvc_vs_frame_frame_based_short_form_t * form,
                                          ...);


    /// Generate a UVC Video Stream Frame descriptor for Frame-Based payloads, fixed form.
    /// Pass the context, a result item, and the completed short-form structure.
    ///\param [in] ctx The context for the session.
    ///\param [in,out] item The result for the make.
    ///\param [in] form The values to be used to populate the descriptor.
    ///\param [in] dwIntervals The frame intervals. Depending on the setting of bFrameIntervalType in the
    /// short form, the number (and meaning) of these will vary.
    ///\param [in] dwIntervalsLength The number of frame intervals. (This is equal to bFrameIntervalType.)

    usbdescbldr_status_t
      usbdescbldr_make_uvc_vs_frame_frame_fixed(usbdescbldr_ctx_t *  ctx,
                                                usbdescbldr_item_t * item,
                                                const usbdescbldr_uvc_vs_frame_frame_based_short_form_t * form,
                                                const uint32_t *     dwIntervals,
                                                size_t               dwIntervalsLength);


    // //////////////////////////////////////////////////////////////////
    // Streaming Format Descriptors

    /// The content of the short forms is intended to precisely mimic the descriptor each one
    /// creates. Please refer to the USB and UVC specifications for details on short form members.
    typedef struct
    {
      uint8_t bFormatIndex;
      uint8_t bNumFrameDescriptors;
      usbdescbldr_guid_t guidFormat;
      uint8_t bBitsPerPixel;
      uint8_t bDefaultFrameIndex;
      uint8_t bAspectRatioX;
      uint8_t bAspectRatioY;
      uint8_t bmInterlaceFlags;
      uint8_t bCopyProtect;
    } usbdescbldr_uvc_vs_format_uncompressed_short_form_t;

    // UVC Video Stream Format (Uncompressed)

    /// Generate a UVC Video Stream Format descriptor for Uncompressed payloads.
    /// Pass the context, a result item, and the completed short-form structure.
    ///\param [in] ctx The context for the session.
    ///\param [in,out] item The result for the make.
    ///\param [in] form The values to be used to populate the descriptor.

    usbdescbldr_status_t
      usbdescbldr_make_uvc_vs_format_uncompressed(usbdescbldr_ctx_t *  ctx,
                                                  usbdescbldr_item_t * item,
                                                  const usbdescbldr_uvc_vs_format_uncompressed_short_form_t * form);


    // //////////////////////////////////////////////////////////////////
    // Streaming Frame Descriptors

    /// The content of the short forms is intended to precisely mimic the descriptor each one
    /// creates. Please refer to the USB and UVC specifications for details on short form members.

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
    } usbdescbldr_uvc_vs_frame_uncompressed_short_form_t;

    /// Generate a UVC Video Stream Frame descriptor for Uncompressed payloads, varadic form.
    /// Pass the context, a result item, and the completed short-form structure.
    ///\param [in] ctx The context for the session.
    ///\param [in,out] item The result for the make.
    ///\param [in] form The values to be used to populate the descriptor.
    ///\param [in] ... The frame intervals. Depending on the setting of bFrameIntervalType in the
    ///\param [in] dwIntervals The frame intervals. Depending on the setting of bFrameIntervalType in the
    usbdescbldr_status_t
      usbdescbldr_make_uvc_vs_frame_uncompressed(usbdescbldr_ctx_t *  ctx,
                                                 usbdescbldr_item_t * item,
                                                 const usbdescbldr_uvc_vs_frame_uncompressed_short_form_t * form,
                                                 ...);

    /// Generate a UVC Video Stream Frame descriptor for Uncompressed payloads, fixed form.
    /// Pass the context, a result item, and the completed short-form structure.
    ///\param [in] ctx The context for the session.
    ///\param [in,out] item The result for the make.
    ///\param [in] form The values to be used to populate the descriptor.
    ///\param [in] dwIntervals The frame intervals. Depending on the setting of bFrameIntervalType in the
    /// short form, the number (and meaning) of these will vary.
    ///\param [in] dwIntervalsLength The number of frame intervals. (This is equal to bFrameIntervalType.)
    usbdescbldr_status_t
      usbdescbldr_make_uvc_vs_frame_uncompressed_fixed(usbdescbldr_ctx_t *  ctx,
                                                       usbdescbldr_item_t * item,
                                                       const usbdescbldr_uvc_vs_frame_uncompressed_short_form_t * form,
                                                       const  uint32_t *    dwIntervals,
                                                       size_t               dwIntervalsLength);


#ifdef __cplusplus
}
#endif
