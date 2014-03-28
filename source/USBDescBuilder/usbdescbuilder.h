#pragma once

#ifdef __cplusplus
extern "C" {
#endif

int HelloWorld(void);

  // //////////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////////
  // API

  typedef enum {
    USBDESCBLDR_OK,
    USBDESCBLDR_UNINITIALIZED,		// Never initialized
    USBDESCBLDR_UNSUPPORTED,		// Never written...
    USBDESCBLDR_DRY_RUN,		// Not actually creating the datum
    USBDESCBLDR_NO_SPACE,		// Buffer exhausted
    USBDESCBLDR_LEVEL_VIOLATION,	// Can't do that here
    USBDESCBLDR_INVALID,		// Too many, duplicate, range error..
    USBDESCBLDR_OVERSIZED,
    // ...
  } usbdescbldr_status_t;

  // The 'handle' by which callers store the results of maker calls
#define USBDESCBLDR_MAX_CHILDREN 8
  typedef struct usbdescbldr_item_s {
    void *                      address;        // Where it is stored
    size_t                      size;           // size of item itself
    size_t                      totalSize;      // Size of item and all children (may be easier to compute this only when done)
    usbdescbldr_item_s *        item[USBDESCBLDR_MAX_CHILDREN];
    unsigned int                items;
  } usbdescbldr_item_t;

  // Setup and teardown

  // Reset internal state and begin a new descriptor.
  // Passing NULL for the buffer indicates a 'dry run' --
  // go through all the motions, checks, and size computations
  // but do not actually create a descriptor.

  usbdescbldr_status_t
  usbdescbldr_init(unsigned char *      buffer,
                   size_t               buffer_size);


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
  usbdescbldr_add_children(usbdescbldr_context_t *      ctx,
                           usbdescbldr_item_t *         parent, 
                           ...)

  // Create the language descriptor (actually string, index 0).
  // Pass the context, a result item, and the IDs.
  usbdescbldr_status_t
  usbdescbldr_make_languageIDs(usbdescbldr_context_t *  ctx,
                               usbdescbldr_item_t *     item,
                               ...);

  // Generate a USB Device Descriptor. This is one-shot and top-level.
  // Pass the context, a result item, and the completed Device Descriptor 
  // short-form structure.

  typedef struct {
    unsigned short	bcdUSB;		// USB Spec Version
    unsigned char	bDeviceClass;
    unsigned char	bDeviceSubClass;
    unsigned char	bDeviceProtocol;
    unsigned short	idVendor;
    unsigned short	idProduct;
    unsigned short	bcdDevice;	// BCD-encoded Release #
    unsigned char	iManufacturer;	// String index
    unsigned char	iProduct;	// String index
    unsigned char	iSerialNumber;	// String Index
    unsigned char	bNumConfigurations;
  } usbdescbldr_device_descriptor_short_form_t;

  usbdescbldr_status_t
  usbdescbldr_make_device_descriptor(usbdescbldr_context_t *ctx,
                                     usbdescbldr_item_t *item,
                                     usbdescbldr_device_descriptor_short_form_t *form);
    
  
  typedef struct {
    unsigned short	bcdUSB;		// USB Spec Version
    unsigned char	bDeviceClass;
    unsigned char	bDeviceSubClass;
    unsigned char	bDeviceProtocol;
    unsigned short	idVendor;
    unsigned short	idProduct;
    unsigned short	bcdDevice;	// BCD-encoded Release #
    unsigned char	iManufacturer;	// String index
    unsigned char	iProduct;	// String index
    unsigned char	iSerialNumber;	// String Index
    unsigned char	bNumConfigurations;
  } usbdescbldr_device_qualifier_short_form_t;

  // Generate a USB Device Qualifier Descriptor. This is one-shot and top-level.
  usbdescbldr_status_t
  usbdescbldr_make_device_qualifier_descriptor(usbdescbldr_context_t *ctx,
                                               usbdescbldr_item_t *item, 
                                               usbdescbldr_device_qualifier_short_form_t * form);
    

  // Define a new string and obtain its index. This is a one-shot top-level item.
  // Pass the string in ASCII and NULL-terminated (a classic C string).
  usbdescbldr_status_t
 usbdescbldr_make_string_descriptor(usbdescbldr_context_t *ctx,
                                    usbdescbldr_item_t *item,     // OUT
                                    unsigned char *index,         // OUT
                                    char *string);                // OUT


  // Generate a Binary Object Store. This is top-level.
  // Add the device Capabilities to it to complete the BOS.
  usbdescbldr_status_t 
  usbdescbldr_make_bos_descriptor(usbdescbldr_context_t *       ctx,
                                  usbdescbldr_item_t *          item);

  // Generate a Device Capability descriptor.
  usbdescbldr_status_t
  usbdescbldr_make_device_capability_descriptor(usbdescbldr_context_t * ctx,
                                                usbdescbldr_item_t *    item,
                                                unsigned char	          bDevCapabilityType,
                                                unsigned char *	        typeDependent,	// Anonymous byte data
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
    usbdescbldr_make_device_configuration_descriptor(usbdescbldr_context_t * ctx,
    usbdescbldr_item_t * item,
    unsigned char * index,
    usbdescbldr_device_qualifier_short_form_t * form);


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
    usbdescbldr_make_standard_interface_descriptor(usbdescbldr_context_t * ctx,
    usbdescbldr_item_t * item,
    usbdescbldr_standard_interface_short_form_t * form);


  typedef struct {
    uint8_t  bEndpointAddress;
    uint8_t  bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t  bInterval;
  } usbdescbldr_endpoint_short_form_t;


  usbdescbldr_status_t
    usbdescbldr_make_endpoint_descriptor(usbdescbldr_context_t * ctx,
    usbdescbldr_item_t * item,
    usbdescbldr_endpoint_short_form_t * form);



  typedef struct {
    uint8_t  bMaxBurst;
    uint8_t  bmAttributes;
    uint16_t wBytesPerInterval;
  } usbdescbldr_ss_ep_companion_short_form_t;

  usbdescbldr_status_t
    usbdescbldr_make_ss_ep_companion_descriptor(usbdescbldr_context_t * ctx,
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
    usbdescbldr_make_interface_association_descriptor(usbdescbldr_context_t * ctx,
    usbdescbldr_item_t * item,
    usbdescbldr_iad_short_form_t * form);

#ifdef __cplusplus
}
#endif
