#include "usbdescbuilder.h"

#include "USB.h"
#include "usb_descriptor.h" // (which is copied directly from cypress/core/hardware/inc)

typedef struct _USB_STRING_DESCRIPTOR
{
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  wchar_t  unichar[0];
}__attribute__((packed, aligned(1))) USB_STRING_DESCRIPTOR;

typedef struct _USB_BOS_DESCRIPTOR
{
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint16_t  wTotalLength;
  uint8_t bNumDeviceCaps;
}__attribute__((packed, aligned(1))) USB_STRING_DESCRIPTOR;

#define USB_SS_EP_COMPANION_TYPE UVC_COMPANION // 0x30


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
    
    unsigned int	i_string;	// Next string index to be assigned
    unsigned int  i_devConfig;  // Next device configuration index to be assigned
    
    unsigned short(*fLittleShortToHost)(unsigned short s);
    unsigned short(*fHostToLittleShort)(unsigned short s);
    unsigned int(*fLittleIntToHost)(unsigned int s);
    unsigned int(*fHostToLittleInt)(unsigned int s);
    
    // ...
} usbdescbldr_ctx_t;

static inline size_t
_bufferAvailable(usbdescbldr_ctx_t *ctx)
{
    // Meaningless unless there's a buffer, so presume the caller
    // has already verified that.
    return ctx->bufferSize - (ctx->append - ctx->buffer);
}



// Internal functions may presume that we are initialized, etc.

// In case we have no ntohs() et alia:

static unsigned short
_endianShortNoOp(unsigned short s)
{
    return s;
}


static unsigned int
_endianIntNoOp(unsigned int s)
{
    return s;
}


static unsigned short
_endianShortSwap(unsigned short s)
{
    unsigned short result;

    result = ((((s >> 8) & 0xff) << 0) |
              (((s >> 0) & 0xff) << 8));

    return result;
}


static unsigned int
_endianIntSwap(unsigned int s)
{
    unsigned int result;

    result = ((((s >> 24) & 0xff) << 0) |
              (((s >> 16) & 0xff) << 8) |
              (((s >> 8) & 0xff) << 16) |
              (((s >> 0) & 0xff) << 24));

    return result;
}

// Debugging
// Who knows if we have stdio, etc in our platform context.. make this optional

#define   USE_USBDESCBLDR_SYSLOG
#ifdef    USE_USBDESCBLDR_SYSLOG
#include <stdarg.h>
#include <stdio.h>
#endif // USE_USBDESCBLDR_SYSLOG

static void
_syslog(int severity, const char *format, ...)
{
#ifdef    USE_USBDESCBLDR_SYSLOG
    va_list     va;

    va_start(va, format);
    vfprintf(stderr, format, va);
    va_end(va);
#endif // USE_USBDESCBLDR_SYSLOG
}


static void 
_item_init(usbdescbldr_item_t * item)
{
    memset(item, 0, sizeof(*item));
}



// Make a set of items subordinate to one parent item. This
// is used to both place the items contiguously, in order,
// in memory after the parent item and to allow the parent
// item to account for their accumulated lengths.
// Pass the context, a result item, and the subordinate items.

usbdescbldr_status_t
usbdescbldr_add_children(usbdescbldr_context_t *      ctx,
                          usbdescbldr_item_t *         parent,
                          ...)
{
  // add size to totalsize?
  // walk over arglist
  // add subnode totalsize to totalsize
  // end-loop
  // then lots of smarts COULD be added about progagting values up and down
  // eg adding a string to an interface could set the iInterface value in the interface..
}


// Use a static for the context, to free callers from having to provide their ownand pass it
// on every call. There is no expectation that this library will be ever need to be re-entrant.
static usbdescbldr_ctx_t gContext = {
  .initialized = 0;
};

// API

int HelloWorld(void) {
  return 1;
}




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

  const unsigned short endian = 0x1234;

  memset(&gContext, 0, sizeof(gContext));

  // Determine host word order
  if (((unsigned char *)&endian) == 0x34) {
    // Host is little-endian; less to do
    ctx->fLittleShortToHost = _endianShortNoOp;
    ctx->fHostToLittleShort = _endianShortNoOp;
    ctx->fLittleIntToHost = _endianIntNoOp;
    ctx->fHostToLittleInt = _endianIntNoOp;
  } else {
    // Host is big-endian
    ctx->fLittleShortToHost = _endianShortSwap;
    ctx->fHostToLittleShort = _endianShortSwap;
    ctx->fLittleIntToHost = _endianIntSwap;
    ctx->fHostToLittleInt = _endianIntSwap;
  }

  if (buffer != NULL) {
    ctx->buffer = buffer;
    ctx->buffer_size = buffer_size;
    ctx->append = buffer;
    // last_append remains NULL, as there is no 'last' at this point
  }

  ctx->initialized = 1;
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
usbdescbldr_make_device_descriptor(usbdescbldr_context_t *ctx,
                                   usbdescbldr_item_t *item,
                                   usbdescbldr_device_descriptor_short_form_t *form);
{
  USB_DEVICE_DESCRIPTOR *dest;
  unsigned short tShort;

  if(form == NULL)
    return USBDESCBLDR_INVALID;

  // This item has a fixed length; check for 'fit'
  if(ctx->buffer != NULL) {
    if(sizeof(*dest) >= _bufferAvailable(ctx))
      return USBDESCBLDR_NO_SPACE;
  }

  // begin construction
  // Fill in buffer unless in dry-run
  if(ctx->buffer != NULL) {
    dest = (USB_DEVICE_DESCRIPTOR *) ctx->append;

    dest->bLength = sizeof(*dest);

    dest->bDescriptorType = USB_DESCRIPTOR_TYPE_DEVICE;

    tShort = ctx->fHostToLittle(form->bcdUSB);
    memcpy(&dest->bcdUSB, &tShort, sizeof(dest->bcdUSB));

    dest->bDeviceClass = form->bDeviceClass;

    dest->bDeviceSubClass = form->bDeviceSubClass;

    dest->bDeviceProtocol = form->bDeviceProtocol;

    // Default the maxPacketSize: 64 for USB 2.x and 3.x 
    if(form->bcdUSB < 0x0300)
      dest->bMaxPacketSize0 = 64;
    else
      dest->bMaxPacketSize0 = 5; // 2^5 == 64

    tShort = ctx->fHostToLittleShort(form->idVendor);
    memcpy(&dest->idVendor, &tShort, sizeof(dest->idVendor));

    tShort = ctx->fHostToLittleShort(form->idProduct);
    memcpy(&dest->idProduct, &tShort, sizeof(dest->idProduct));

    tShort = ctx->fHostToLittleShort(form->bcdDevice);
    memcpy(&dest->bcdDevice, &tShort, sizeof(dest->bcdDevice));

    dest->iManufacturer = form->iManufacturer;
    dest->iProduct = form->iProduct;
    dest->iSerialNumber = form->iSerialNumber;
    dest->bNumConfigurations = form->bNumConfigurations;
  } // filling in buffer

  // Build the output item
  _item_init(item);
  item->size = sizeof(*dest);
  item->address = ctx->append;

  ctx->append += sizeof(*dest);

  return status;
}


// Generate a USB Device Qualifier Descriptor. This is one-shot and top-level.
usbdescbldr_status_t
usbdescbldr_make_device_qualifier_descriptor(usbdescbldr_context_t * ctx,
                                             usbdescbldr_item_t * item,
                                             usbdescbldr_device_qualifier_short_form_t * form)
{
  USB_DEVICE_QUALIFIER_DESCRIPTOR *dest;
  unsigned short tShort;

  if (form == NULL)
    return USBDESCBLDR_INVALID;

  // This item has a fixed length; check for 'fit'
  if (ctx->buffer != NULL) {
    if (sizeof(*dest) > _bufferAvailable(ctx))
      return USBDESCBLDR_NO_SPACE;
  }

  // begin construction
  // Fill in buffer unless in dry-run
  if (ctx->buffer != NULL) {
    dest = (USB_DEVICE_QUALIFIER_DESCRIPTOR *) ctx->append;

    dest->bLength = sizeof(*dest);

    dest->bDescriptorType = USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER;

    tShort = ctx->fHostToLittleShort(form->bcdUSB);
    memcpy(&dest->bcdUSB, &tShort, sizeof(dest->bcdUSB));

    dest->bDeviceClass = form->bDeviceClass;
    dest->bDeviceSubClass = form->bDeviceSubClass;
    dest->bDeviceProtocol = form->bDeviceProtocol;

    // Default the maxPacketSize: 64 for USB 2.x and 3.x 
    if (form->bcdUSB < 0x0300)
      dest->bMaxPacketSize0 = 64;
    else
      dest->bMaxPacketSize0 = 5; // 2^5 == 64

    dest->bNumConfigurations = form->bNumConfigurations;
    dest->bReserved = 0;
  } // filling in buffer

  // Build result
  _item_init(item);
  item->size = sizeof(*dest);
  item->address = ctx->append;

  // Consume buffer
    ctx->append += sizeof(*dest);
  }

  return status;
}

// Create the language descriptor (actually string, index 0).
// Pass the context, a result item, and the IDs.
usbdescbldr_status_t
usbdescbldr_make_languageIDs(usbdescbldr_context_t *ctx,
                             usbdescbldr_item_t *item,
                             ...)
{
  va_list             va_count, va_do;
  unsigned int        langs;
  unsigned short      lang;
  size_t              needs;
  unsigned char *     drop;
  USB_STRING_DESCRIPTOR *dest = (USB_STRING_DESCRIPTOR *)ctx->append;

  // There may only be one (this is string index zero..)
  if(ctx->i_string != 0)
    return USBDESCBLDR_TOO_MANY;

  va_start(item, va_do);      // One copy for work
  va_copy(va_count, va_do);   // .. one copy just to count

  // Count args until the null-terminator
  do {
    lang = (unsigned short) va_arg(va_count, unsigned int);
    if(lang != 0) langs++;
  } while(lang != 0);
  va_end(va_count);

  // Now we know our length
  needs = sizeof(*dest) + langs * sizeof(lang);

  // Bounds check
  if(needs > 0xff)
    return USBDESCBLDR_OVERSIZED;

  // If not dry-run, be sure we can write
  if(ctx->buffer != NULL && _bufferAvailable() < needs)
    return USBDESCBLDR_NO_SPACE;

  // Continue construction
  if(ctx->buffer != NULL) {
    dest->bLength = needs;
    dest->bDescriptorType = USB_STRING;

    drop = &dest->unichar[0];
    while((lang = (unsigned short) va_arg(va_do, unsigned int)) != 0) {
      lang = ctx->fHostToLittleShort(lang);
      memcpy(drop, &lang, sizeof(lang));
      drop += sizeof(lang);
    }
  }
  va_end(va_do);

  // This counts as string index 0
  ctx->i_string++;

  // Build the item for the caller
  _item_init(item);
  item->address = ctx->append;
  item->size = needs;

  // Advance the buffer
  ctx->append += needs;

  return USBDESCBLDR_OK;
}





// Define a new string and obtain its index. This is a one-shot top-level item.
// Pass the string in ASCII and NULL-terminated (a classic C string). Returns
// the expected string index in *index (optional; NULL if don't care).

// Define a new string and obtain its index. This is a one-shot top-level item.
// Pass the string in ASCII and NULL-terminated (a classic C string).
usbdescbldr_status_t 
usbdescbldr_make_string_descriptor(usbdescbldr_context_t *ctx,
                                   usbdescbldr_item_t *item, // OUT
                                   unsigned char *index, // OUT
                                   char *string) // OUT
{
  USB_STRING_DESCRIPTOR *dest;
  size_t needs;
  unsigned char *drop, *ascii;
  unsigned short wchar;

  if( string == NULL)
    return USBDESCBLDR_INVALID;

  // String indicies are bytes, limiting the number of them:
  if (ctx->i_string > 0xff)
    return USBDESCBLDR_TOO_MANY;
    
  // This has a fixed length, so check up front
  needs = strlen(string) * sizeof(wchar_t) + sizeof(*dest);
  if (needs > 0xff)
    return USBDESCBLDR_OVERSIZED;  // .. as opposed to NO SPACE ..

  // Construct
  // (No need to stack)
  if (ctx->buffer != NULL) {
    if (needs > _bufferAvailable())
      return USBDESCBLDR_NO_SPACE;

    dest = (USB_STRING_DESCRIPTOR *) ctx->append;
    dest->bDescriptorLength = needs;
    dest->bDescriptorType = USB_DESCRIPTOR_TYPE_STRING;

    drop = ctx->append + sizeof(*dest);
    for(ascii = string; *ascii; ++ascii) {		// (Do not copy the NULL)
      wchar = (unsigned short) ascii;			    // (zero-extending, very explicitly)
      wchar = ctx->fHostToLittleShort(wchar);
      memcpy(drop, & wchar, sizeof(wchar));
      drop += sizeof(wchar);
    } 
  }

  // Build the item 
  _item_init(item);
  item->size = needs;
  item->address = ctx->append;

  // Consume buffer space (or just count, in dry run mode)
  ctx->append += needs;

  // Give the caller the assigned string index, if they want it
  if (index != NULL) 
    *index = ctx->i_string;
  ctx->i_string++;

  return USBDESCBLDR_OK;
}


// Generate a Binary Object Store. This is top-level.
// Add the device Capabilities to it to complete the BOS.
usbdescbldr_status_t
usbdescbldr_make_bos_descriptor(usbdescbldr_context_t * ctx,
                                usbdescbldr_item_t * item)
{
  USB_BOS_DESCRIPTOR *dest;

  // There can only be one of these -- add check

  // Check space
  if (ctx->buffer != NULL) {
    if (sizeof(*dest) > _bufferAvailable()) 
      return USBDESCBLDR_NO_SPACE;
  }

  // begin construction
  dest = (USB_BOS_DESCRIPTOR *) ctx->append;
  // Fill in buffer unless in dry-run
  if (ctx->buffer != NULL) {
    dest = (USB_BOS_DESCRIPTOR *) ctx->append;
    dest->bLength = sizeof(*dest);
    dest->bDescriptorType = USB_DESCRIPTOR_TYPE_BOS;
    dest->wTotalLength = 0;
    dest->bNumDeviceCaps = 0;
  }

  // Build the item 
  _item_init(item);
  item->size = sizeof(*dest);
  item->address = ctx->append;

  ctx->append += sizeof(*dest);

  return USBDESCBLDR_OK;
}


// Generate a Device Capability descriptor.
usbdescbldr_status_t
usbdescbldr_make_device_capability_descriptor(usbdescbldr_context_t * ctx,
                                              usbdescbldr_item_t *    item,
                                              unsigned char	          bDevCapabilityType,
                                              unsigned char *	        typeDependent,	// Anonymous byte data
                                              size_t		              typeDependentSize)
{
  return USBDESCBLDR_UNSUPPORTED;
}


// Generate a Standard Interface descriptor.
usbdescbldr_status_t
usbdescbldr_make_standard_interface_descriptor(usbdescbldr_context_t * ctx,
usbdescbldr_item_t * item,
usbdescbldr_standard_interface_short_form_t * form)
{
  USB_INTERFACE_DESCRIPTOR *dest;
  size_t needs;

  if(form == NULL)
    return USBDESCBLDR_INVALID;

  // Config indicies are bytes, limiting the number of them:
  if(ctx->i_devConfig > 0xff)
    return USBDESCBLDR_TOO_MANY;

  // This has a fixed length, so check up front
  needs = sizeof(*dest);
  if(needs > 0xff)
    return USBDESCBLDR_OVERSIZED;  // .. as opposed to NO SPACE ..

  // Construct
  if(ctx->buffer != NULL) {
    if(needs > _bufferAvailable(ctx))
      return USBDESCBLDR_NO_SPACE;

    dest = (USB_INTERFACE_DESCRIPTOR *) ctx->append;
    dest->bLength = needs;
    dest->bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE;

    dest->bInterfaceNumber = form->bInterfaceNumber;
    dest->bAlternateSetting = form->bAlternateSetting;
    dest->bInterfaceClass = form->bInterfaceClass;
    dest->bInterfaceSubclass = form->bInterfaceSubclass;
    dest->bInterfaceProtocol = form->bInterfaceProtocol;
    dest->iInterface = form->iInterface;
  }

  // Build the item 
  _item_init(item);
  item->size = needs;
  item->address = ctx->append;

  // Consume buffer space (or just count, in dry run mode)
  ctx->append += needs;

  return USBDESCBLDR_OK;
}


usbdescbldr_status_t
usbdescbldr_make_endpoint_descriptor(usbdescbldr_context_t * ctx,
usbdescbldr_item_t * item,
usbdescbldr_endpoint_short_form_t * form)
{
  USB_ENDPOINT_DESCRIPTOR *dest;
  unsigned short tShort;
  size_t needs;

  if(form == NULL)
    return USBDESCBLDR_INVALID;

  // Config indicies are bytes, limiting the number of them:
  if(ctx->i_devConfig > 0xff)
    return USBDESCBLDR_TOO_MANY;

  // This has a fixed length, so check up front
  needs = sizeof(*dest);
  if(needs > 0xff)
    return USBDESCBLDR_OVERSIZED;  // .. as opposed to NO SPACE ..

  // Construct
  if(ctx->buffer != NULL) {
    if(needs > _bufferAvailable(ctx))
      return USBDESCBLDR_NO_SPACE;

    dest = (USB_ENDPOINT_DESCRIPTOR *) ctx->append;
    dest->bLength = needs;
    dest->bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE;

    dest->bEndpointAddress = form->bEndpointAddress;
    dest->bmAttributes = form->bmAttributes;
    tShort = ctx->fHostToLittleShort(form->wMaxPacketSize);
    memcpy(&dest->wMaxPacketSize, &tShort, sizeof(dest->wMaxPacketSize));
    dest->bInterval = form->bInterval;
  }

  // Build the item 
  _item_init(item);
  item->size = needs;
  item->address = ctx->append;

  // Consume buffer space (or just count, in dry run mode)
  ctx->append += needs;

  return USBDESCBLDR_OK;
}


usbdescbldr_status_t
usbdescbldr_make_ss_ep_companion_descriptor(usbdescbldr_context_t * ctx,
usbdescbldr_item_t * item,
usbdescbldr_ss_ep_companion_short_form_t * form)
{
  USB_SS_EP_COMPANION_DESCRIPTOR *dest;
  unsigned short tShort;
  size_t needs;

  if(form == NULL)
    return USBDESCBLDR_INVALID;

  // This has a fixed length, so check up front
  needs = sizeof(*dest);
  if(needs > 0xff)
    return USBDESCBLDR_OVERSIZED;  // .. as opposed to NO SPACE ..

  // Construct
  if(ctx->buffer != NULL) {
    if(needs > _bufferAvailable(ctx))
      return USBDESCBLDR_NO_SPACE;

    dest = (USB_SS_EP_COMPANION_DESCRIPTOR *) ctx->append;
    dest->bLength = needs;
    dest->bDescriptorType = USB_SS_EP_COMPANION_TYPE;

    dest->bMaxBurst = form->bMaxBurst;
    dest->bmAttributes = form->bmAttributes;
    tShort = ctx->fHostToLittleShort(form->wBytesPerInterval);
    memcpy(&dest->wBytesPerInterval, &tShort, sizeof(dest->wBytesPerInterval));
  }

  // Build the item 
  _item_init(item);
  item->size = needs;
  item->address = ctx->append;

  // Consume buffer space (or just count, in dry run mode)
  ctx->append += needs;

  return USBDESCBLDR_OK;
}

usbdescbldr_status_t
usbdescbldr_make_interface_association_descriptor(usbdescbldr_context_t * ctx,
usbdescbldr_item_t * item,
usbdescbldr_iad_short_form_t * form)
{
  USB_INTERFACE_ASSOCIATION_DESCRIPTOR *dest;
  unsigned short tShort;
  size_t needs;

  if(form == NULL)
    return USBDESCBLDR_INVALID;

  // This has a fixed length, so check up front
  needs = sizeof(*dest);
  if(needs > 0xff)
    return USBDESCBLDR_OVERSIZED;  // .. as opposed to NO SPACE ..

  // Construct
  if(ctx->buffer != NULL) {
    if(needs > _bufferAvailable(ctx))
      return USBDESCBLDR_NO_SPACE;

    dest = (USB_INTERFACE_ASSOCIATION_DESCRIPTOR *) ctx->append;
    dest->bLength = needs;
    dest->bDescriptorType = USB_INTERFACE_ASSOCIATION;

    dest->bFirstInterface = form->bFirstInterface;
    dest->bInterfaceCount = form->bInterfaceCount;
    dest->bFunctionClass = form->bFunctionClass;
    dest->bFunctionSubClass = form->bFunctionSubClass;
    dest->bFunctionProtocol = form->bFunctionProtocol;
    dest->iFunction = form->iFunction;
  }

  // Build the item 
  _item_init(item);
  item->size = needs;
  item->address = ctx->append;

  // Consume buffer space (or just count, in dry run mode)
  ctx->append += needs;

  return USBDESCBLDR_OK;
}
