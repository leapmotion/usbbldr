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


  // Setup and teardown

  // Reset internal state and begin a new descriptor.
  // Passing NULL for the buffer indicates a 'dry run' --
  // go through all the motions, checks, and size computations
  // but do not actually create a descriptor.

  usbdescbldr_status_t
  usbdescbldr_init(unsigned char *	buffer,
		   size_t		buffer_size);


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

  // Generate a USB Device Descriptor. This is one-shot and top-level.
  usbdescbldr_status_t
  usbdescbldr_usb_device_descriptor(unsigned short	bcdUSB,		// USB Spec Version
				    unsigned char	bDeviceClass,
				    unsigned char	bDeviceSubClass,
				    unsigned char	bDeviceProtocol,
				    unsigned short	idVendor,
				    unsigned short	idProduct,
				    unsigned short	bcdDevice,	// BCD-encoded Release #
				    unsigned char	iManufacturer,	// String index
				    unsigned char	iProduct,	// String index
				    unsigned char	iSerialNumber,	// String Index
				    unsigned char	bNumConfigurations);

  // Generate a USB Device Qualifier Descriptor. This is one-shot and top-level.
  usbdescbldr_status_t
  usbdescbldr_usb_device_qualifier_descriptor(unsigned short	bcdUSB,		// USB Spec Version
					      unsigned char	bDeviceClass,
					      unsigned char	bDeviceSubClass,
					      unsigned char	bDeviceProtocol,
					      unsigned char	bNumConfigurations);

  // Begin construction of the Supported Languages String (String Index 0).
  // This is a top-level item.
  usbdescbldr_status_t
  usbdescbldr_open_stringIndex0(void);

  // Add a language ID to the Supported Language String
  usbdescbldr_status_t
  usbdescbldr_add_languageID(unsigned short wLangID);

  // Close (and verify) stringIndex0
  usbdescbldr_status_t 
  usbdescbldr_close_stringIndex0(void);

  // Define a new string and obtain its index. This is a one-shot top-level item.
  // Pass the string in ASCII and NULL-terminated (a classic C string).
  usbdescbldr_status_t 
  usbdescbldr_string_descriptor(char *string);

  // Begin the Binary Object Store. This is top-level.
  usbdescbldr_status_t 
  usbdescbldr_open_bos(void);

  // Add a Device Capability to the BOS. This is within a BOS, only.
  // The typeDependent data can be NULL.
  usbdescbldr_status_t 
  usbdescbldr_add_device_capability(unsigned char	bDevCapabilityType,
				    unsigned char *	typeDependent,	// Anonymous byte data
				    size_t		typeDependentSize);	

  // Close off the Binary Object Store. This is top-level.
  usbdescbldr_status_t 
  usbdescbldr_close_bos(void);




#ifdef __cplusplus
}
#endif
