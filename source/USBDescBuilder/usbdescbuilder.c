#include "usbdescbuilder.h"

// Top concerns: not sure how I can make this emit C code, but it's gotta do that. Shouldn't
// be impossible.

// Next: do some of the basics and make sure that the ideas, when detailed into code,
// actually are decent ideas. If so, complete the API.

// Ongoing: add the remaining descriptor types (not TOO many really)
// Ongoing: add all the primitives to copy (unaligned) shorts and words into the buffer
// Ongoing: ASCII->wchar primitive for building string descrs
// Ongoing: "auto-close" code




  // //////////////////////////////////////////////////////////////////
  // //////////////////////////////////////////////////////////////////
  // Internals

  // The context used across calls into the builder

  typedef enum {
    USBDESCBLDR_USB20_DEVICE_CONFIG,
    USBDESCBLDR_USB20_DEVICE_QUALIFIER_CONFIG,
    USBDESCBLDR_BOS,
    USBDESCBLDR_INTERFACE,
    USBDESCBLDR_ENDPOINT,
    USBDESCBLDR_ENDPOINT_COMPANION,
    USBDESCBLDR_STRING,
    // ...
  } usbdescbldr_level_t;


  typedef struct usbdescbldr_ctx_s {
    unsigned char	initialized;	// Have we been initialized? 0: no.

    unsigned char *	buffer;		// Start of user-provided buffer (if any)
    size_t		buffer_size;	// Length in bytes of any user-provided buffer
    unsigned char *     append;		// Address in buffer for next addition (append)
    unsigned char *	last_append;	// Start address of last completed 'level' (see stack)

#define USBDESCBLDR_STACK_SIZE 8	// State stack: "open" collections etc.
    unsigned int	stack_top;				// Level depth
    usbdescbldr_level_t	stack[USBDESCBLDR_STACK_SIZE];		// Type of level
    unsigned char *     stack_ptrs[USBDESCBLDR_STACK_SIZE];	// Ptr to beginning of level

    unsigned int	nextStringIndex;	// Next string index to be assigned

    // ...
  } usbdescbldr_ctx_t;


// Use a static for the context, to free callers from having to provide their ownand pass it
// on every call. There is no expectation that this library will be ever need to be re-entrant.
static usbdescbldr_ctx_t gContext = {
  .initialized = 0;
};


// Setup and teardown

// Reset internal state and begin a new descriptor.
// Passing NULL for the buffer indicates a 'dry run' --
// go through all the motions, checks, and size computations
// but do not actually create a descriptor.

usbdescbldr_status_t
usbdescbldr_init(unsigned char *	buffer,
		 size_t		buffer_size)
{
  // Do not concern ourselves with initializing something
  // that wasn't 'end'ed, as we do not have allocations to release.

  memset(& gContext, 0, sizeof(gContext));

  if (buffer != NULL) {
    gContext.buffer = buffer;
    gContext.buffer_size = buffer_size;
    gContext.append = buffer;
    // last_append remains NULL, as there is no 'last' at this point
  }

  gContext.initialized = 1;
  return USBDESCBLDR_OK;
}


// Commit (complete/finish) the descriptor in progress.
// Collections, interfaces, etc left open will be tidied up
// (if possible..?)
usbdescbldr_status_t
usbdescbldr_close(void)
{
  // walk down the level stack and for each level, handle touching up
  // any unwritten lengths, etc.
  return USBDESCBLDR_UNSUPPORTED;
}

// Size of descriptor 'so far'
usbdescbldr_status_t
usbdescbldr_descriptor_size(size_t *used)
{
  if (! gContext.initialized)
    return USBDESCBLDR_UNINITIALIZED;

  if (used == NULL)
    return USBDESCBLDR_INVALID;

  // XXX: I'm thinking this could be used to create multiple
  // "top-level" descriptors all contiguous in the buffer.
  // In which case, do I really want to inform the caller
  // of how much of the buffer has been used /in toto/,
  // or just by the currently-open top-level item alone?
  // (Proper use of gContext.stack_ptrs[] will do the real work, here)

  if (gContext.buffer == NULL)
    *used = (size_t) gContext.append; // 'Dry-run' just counts from (char *)0
  else
    *used = (size_t) (gContext.append - gContext.buffer);

  return USBDESCBLDR_OK;
}

// Address (within buffer) of the last completed level/item.
// Returns NULL when called in 'dry-run' mode.
usbdescbldr_status_t
usbdescbldr_last_address(unsigned char **base_address)
{
  return USBDESCBLDR_UNSUPPORTED;
}

 // Terminate use of the builder. Release any resources.
 // Once this is complete, the API requires an _init() before
 // anything will perform again.
 usbdescbldr_status_t
 usbdescbldr_end(void)
{
  return USBDESCBLDR_UNSUPPORTED;
}


// Constructions

// Generate a USB Device Descriptor. This is one-shot and top-level.
usbdescbldr_status_t
usbdescbldr_usb_device_descriptor(unsigned short bcdUSB,		// USB Spec Version
				  unsigned char	 bDeviceClass,
				  unsigned char  bDeviceSubClass,
				  unsigned char  bDeviceProtocol,
				  unsigned short idVendor,
				  unsigned short idProduct,
				  unsigned short bcdDevice,	// BCD-encoded Release #
				  unsigned char	iManufacturer,	// String index
				  unsigned char	iProduct,	// String index
				  unsigned char	iSerialNumber,	// String Index
				  unsigned char	bNumConfigurations)
{
  return USBDESCBLDR_UNSUPPORTED;
}

// Generate a USB Device Qualifier Descriptor. This is one-shot and top-level.
usbdescbldr_status_t
usbdescbldr_usb_device_qualifier_descriptor(unsigned short  bcdUSB,		// USB Spec Version
					    unsigned char   bDeviceClass,
					    unsigned char   bDeviceSubClass,
					    unsigned char   bDeviceProtocol,
					    unsigned char   bNumConfigurations)
{
  return USBDESCBLDR_UNSUPPORTED;
}

// Begin construction of the Supported Languages String (String Index 0).
// This is a top-level item.
usbdescbldr_status_t
usbdescbldr_open_stringIndex0(void)
{
  return USBDESCBLDR_UNSUPPORTED;
}

// Add a language ID to the Supported Language String
usbdescbldr_status_t
usbdescbldr_add_languageID(unsigned short wLangID)
{
  return USBDESCBLDR_UNSUPPORTED;
}

// Close (and verify) stringIndex0
usbdescbldr_status_t 
usbdescbldr_close_stringIndex0(void)
{
  return USBDESCBLDR_UNSUPPORTED;
}

// Define a new string and obtain its index. This is a one-shot top-level item.
// Pass the string in ASCII and NULL-terminated (a classic C string).
usbdescbldr_status_t 
usbdescbldr_string_descriptor(char *string)
{
  return USBDESCBLDR_UNSUPPORTED;
}

// Begin the Binary Object Store. This is top-level.
usbdescbldr_status_t 
usbdescbldr_open_bos(void)
{
  return USBDESCBLDR_UNSUPPORTED;
}


// Add a Device Capability to the BOS. This is within a BOS, only.
// The typeDependent data can be NULL.
usbdescbldr_status_t 
usbdescbldr_add_device_capability(unsigned char	  bDevCapabilityType,
				  unsigned char * typeDependent,        // Anonymous byte data
				  size_t          typeDependentSize)
{
  return USBDESCBLDR_UNSUPPORTED;
}

// Close off the Binary Object Store. This is top-level.
usbdescbldr_status_t 
usbdescbldr_close_bos(void)
{
  return USBDESCBLDR_UNSUPPORTED;
}




int HelloWorld(void) {
  return 1;
}
