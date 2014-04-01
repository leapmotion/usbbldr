/* Copyright (c) 2014 LEAP Motion. All rights reserved.
*
* The intellectual and technical concepts contained herein are proprietary and
* confidential to Leap Motion, and are protected by trade secret or copyright
* law. Dissemination of this information or reproduction of this material is
* strictly forbidden unless prior written permission is obtained from LEAP
* Motion.
*/

#include <string.h>
#include "USB.h"
#include "USBExtras.h"
#include "usbdescbuilder.h"

// Next: do some of the basics and make sure that the ideas, when detailed into code,
// actually are decent ideas. If so, complete the API.

// Ongoing: add the remaining descriptor types (... work ...)
// Ongoing: this is a library. The API must be documented (fairly well)
// Ongoing: the whole smart-grouping part

// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// Internals


static size_t
_bufferAvailable(usbdescbldr_ctx_t *ctx)
{
    // Meaningless unless there's a buffer, so presume the caller
    // has already verified that.
    return ctx->bufferSize - (ctx->append - ctx->buffer);
}


// In case we have no ntohs() et alia:

static uint16_t
_endianShortNoOp(uint16_t s)
{
    return s;
}


static unsigned int
_endianIntNoOp(unsigned int s)
{
    return s;
}


static uint16_t
_endianShortSwap(uint16_t s)
{
    uint16_t result;

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

// //////////////////////////////////////////////////////////////////
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

// //////////////////////////////////////////////////////////////////
// Item actions

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
usbdescbldr_add_children(usbdescbldr_ctx_t *      ctx,
                          usbdescbldr_item_t *         parent,
                          ...)
{
  // Everything should include itself.
  if(parent->totalSize == 0)
    parent->totalSize = parent->size;

  // walk over arglist
  // add subnode totalsize to totalsize
  // end-loop
  // then lots of smarts COULD be added about propagating values up and down
  // eg adding a string to an interface could set the iInterface value in the interface..

  return USBDESCBLDR_UNINITIALIZED;
}

// //////////////////////////////////////////////////////////////////
// Buffer search helpers
// When generating the final results, these are used to walk through the
// buffer. The buffer is not necessarily in the "right order", depending on
// the sequence of calls issued by the caller.

typedef struct {
  usbdescbldr_ctx_t * ctx;
  unsigned char * at;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubtype;
} usbdescbldr_iterator_t;

static void
usbdescbldr_iterator(usbdescbldr_ctx_t *ctx, uint8_t bDescriptorType, uint8_t bDescriptorSubtype, usbdescbldr_iterator_t *iter)
{
  iter->ctx = ctx;
  iter->at = ctx->buffer;
  iter->bDescriptorType = bDescriptorType;
  iter->bDescriptorSubtype = iter->bDescriptorSubtype;
}


static unsigned char *
usbdescbldr_iterator_next(usbdescbldr_iterator_t *iter)
{
  USB_DESCRIPTOR_HEADER * hdr = (USB_DESCRIPTOR_HEADER *) iter->at;
  unsigned char * result = NULL;

  while(result == NULL && iter->at < iter->ctx->append) {
    switch(iter->bDescriptorType) {
    default:
      // For unambiguous DescriptorTypes, match easy:
      if(hdr->bDescriptorType == iter->bDescriptorType) {
        result = iter->at;
      }

      // Add subtype'd types here.. will need their descriptor types
      // unless we presume the bDescriptorSubType always follows the type
      // case .. :

    } // switch (type)

    iter->at += hdr->bLength;     // Always advance the cursor, for next time
  }  // while (more buffer remains)

  return result;
}

// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// API

int HelloWorld(void) {
  return 1;
}



// //////////////////////////////////////////////////////////////////
// Setup and teardown

// Reset internal state and begin a new descriptor.
// Passing NULL for the buffer indicates a 'dry run' --
// go through all the motions, checks, and size computations
// but do not actually create a descriptor.

usbdescbldr_status_t
usbdescbldr_init(usbdescbldr_ctx_t *  ctx,
                 unsigned char *      buffer,
                 size_t               bufferSize)
{
  // Do not concern ourselves with initializing something
  // that wasn't 'end'ed, as we do not have allocations to release.

  const uint16_t endian = 0x1234;

  memset(ctx, 0, sizeof(*ctx));

  // Determine host uint16_t order
  if ((*(uint8_t *)&endian) == 0x34) {
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
    ctx->bufferSize = bufferSize;
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
  return USBDESCBLDR_OK;
}


// Terminate use of the builder. Release any resources.
// Once this is complete, the API requires an _init() before
// anything will perform again.
usbdescbldr_status_t
usbdescbldr_end(void)
{
  return USBDESCBLDR_OK;
}

// //////////////////////////////////////////////////////////////////
// Constructions


/*! \fn usbdescbldr_status_t
usbdescbldr_make_device_descriptor(usbdescbldr_ctx_t *ctx,
                                   usbdescbldr_item_t *item,
                                   usbdescbldr_device_descriptor_short_form_t *form)
\brief Create the Device Descriptor.

Build a device descriptor based upon those values given in the short form by the
caller.
\param [in] ctx The Builder context.
\param [in,out] A Builder item to describe the results.
\param [in] form Those Device Descriptor values which must be specified by the caller.
*/


// Generate a USB Device Descriptor. This is one-shot and top-level.
usbdescbldr_status_t
usbdescbldr_make_device_descriptor(usbdescbldr_ctx_t *ctx,
                                   usbdescbldr_item_t *item,
                                   usbdescbldr_device_descriptor_short_form_t *form)
{
  USB_DEVICE_DESCRIPTOR *dest;
  uint16_t tShort;

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
    memset(dest, 0, sizeof(*dest));

    dest->header.bLength = sizeof(*dest);
    dest->header.bDescriptorType = USB_DESCRIPTOR_TYPE_DEVICE;

    tShort = ctx->fHostToLittleShort(form->bcdUSB);
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

  return USBDESCBLDR_OK;
}


// Generate a USB Device Qualifier Descriptor. This is one-shot and top-level.
usbdescbldr_status_t
usbdescbldr_make_device_qualifier_descriptor(usbdescbldr_ctx_t * ctx,
                                             usbdescbldr_item_t * item,
                                             usbdescbldr_device_qualifier_short_form_t * form)
{
  USB_DEVICE_QUALIFIER_DESCRIPTOR *dest;
  uint16_t tShort;

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

    dest->header.bLength = sizeof(*dest);

    dest->header.bDescriptorType = USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER;

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

  return USBDESCBLDR_OK;
}


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

usbdescbldr_status_t
usbdescbldr_make_device_configuration_descriptor(usbdescbldr_ctx_t * ctx,
usbdescbldr_item_t * item,
uint8_t * index,
usbdescbldr_device_configuration_short_form_t * form)
{
  USB_CONFIGURATION_DESCRIPTOR *dest;

  if(form == NULL)
    return USBDESCBLDR_INVALID;

  // This item has a fixed length; check for 'fit'
  if(ctx->buffer != NULL) {
    if(sizeof(*dest) > _bufferAvailable(ctx))
      return USBDESCBLDR_NO_SPACE;
  }

  // begin construction
  // Fill in buffer unless in dry-run
  if(ctx->buffer != NULL) {
    dest = (USB_CONFIGURATION_DESCRIPTOR *) ctx->append;
    memset(dest, 0, sizeof(*dest));

    dest->header.bLength = sizeof(*dest);
    dest->header.bDescriptorType = USB_DESCRIPTOR_TYPE_CONFIGURATION;

    dest->bNumInterfaces = form->bNumInterfaces;        // XXX: this could be derived at build-time from the descendants
    dest->iConfiguration = form->iConfiguration; // XXX: make up my mind. accept from caller or generate dynamically
    dest->bmAttributes = form->bmAttributes;
    dest->MaxPower = form->bMaxPower;
  } // filling in buffer

  // Build result
  _item_init(item);
  item->size = sizeof(*dest);
  item->address = ctx->append;

  // Consume buffer
  ctx->append += sizeof(*dest);

  return USBDESCBLDR_OK;
}


// Create the language descriptor (actually string, index 0).
// Pass the context, a result item, and the IDs.

/*! \fn usbdescbldr_status_t
usbdescbldr_make_languageIDs(usbdescbldr_ctx_t *ctx,
                             usbdescbldr_item_t *item,
                             ...)
\brief Define the supported languages descriptor.

The supported languages are actually stored in string descriptor #0.
Hence, this must be performed before any other string descriptors are
made.
\param [in] ctx The Builder context.
\param [in,out] The item resulting from this request.
\param [in] ... The Language IDs (passed as ints). This list MUST be terminated with 0.
*/
usbdescbldr_status_t
usbdescbldr_make_languageIDs(usbdescbldr_ctx_t *ctx,
                             usbdescbldr_item_t *item,
                             ...)
{
  va_list             va_count, va_do;
  unsigned int        langs;
  uint16_t            lang;
  size_t              needs;
  unsigned char *     drop;
  USB_STRING_DESCRIPTOR *dest = (USB_STRING_DESCRIPTOR *)ctx->append;

  // There may only be one (this is string index zero..)
  if(ctx->i_string != 0)
    return USBDESCBLDR_TOO_MANY;

  va_start(va_do, item);      // One copy for work
  va_copy(va_count, va_do);   // .. one copy just to count

  // Count args until the null-terminator
  langs = 0;
  do {
    lang = (uint16_t) va_arg(va_count, unsigned int);
    if(lang != 0) langs++;
  } while(lang != 0);
  va_end(va_count);

  // Now we know our length
  needs = sizeof(*dest) + langs * sizeof(lang);

  // Bounds check
  if(needs > 0xff)
    return USBDESCBLDR_OVERSIZED;

  // If not dry-run, be sure we can write
  if(ctx->buffer != NULL && _bufferAvailable(ctx) < needs)
    return USBDESCBLDR_NO_SPACE;

  // Continue construction
  if(ctx->buffer != NULL) {
    dest->header.bLength = needs;
    dest->header.bDescriptorType = USB_DESCRIPTOR_TYPE_STRING;

    // In Strings, the string unichars immediately follow the header
    drop = ((unsigned char *) dest) + sizeof(USB_DESCRIPTOR_HEADER);
    while((lang = (uint16_t) va_arg(va_do, unsigned int)) != 0) {
      lang = ctx->fHostToLittleShort(lang);
      memcpy(drop, &lang, sizeof(lang));
      drop += sizeof(lang);
    }
  }
  va_end(va_do);

  // Build the item for the caller
  _item_init(item);
  item->address = ctx->append;
  item->size = needs;
  item->index = ctx->i_string;

  // Advance the buffer
  ctx->append += needs;

  // This counts as string index 0
  ctx->i_string++;

  return USBDESCBLDR_OK;
}





// Define a new string and obtain its index. This is a one-shot top-level item.
// Pass the string in ASCII and NULL-terminated (a classic C string). Returns
// the expected string index in *index (optional; NULL if don't care).

// Define a new string and obtain its index. This is a one-shot top-level item.
// Pass the string in ASCII and NULL-terminated (a classic C string).
usbdescbldr_status_t 
usbdescbldr_make_string_descriptor(usbdescbldr_ctx_t *ctx,
                                   usbdescbldr_item_t *item, // OUT
                                   uint8_t *index, // OUT
                                   char *string) // OUT
{
  USB_STRING_DESCRIPTOR *dest;
  size_t needs;
  unsigned char *drop, *ascii;
  uint16_t wchar;

  if( string == NULL)
    return USBDESCBLDR_INVALID;

  // String indices are bytes, limiting the number of them:
  if (ctx->i_string > 0xff)
    return USBDESCBLDR_TOO_MANY;
    
  // This has a fixed length, so check up front
  needs = strlen(string) * sizeof(wchar_t) + sizeof(*dest);
  if (needs > 0xff)
    return USBDESCBLDR_OVERSIZED;  // .. as opposed to NO SPACE ..

  // Construct
  // (No need to stack)
  if (ctx->buffer != NULL) {
    if (needs > _bufferAvailable(ctx))
      return USBDESCBLDR_NO_SPACE;

    dest = (USB_STRING_DESCRIPTOR *) ctx->append;
    dest->header.bLength = needs;
    dest->header.bDescriptorType = USB_DESCRIPTOR_TYPE_STRING;

    // In Strings, the string unichars immediately follow the header
    drop = ((unsigned char *) dest) + sizeof(USB_DESCRIPTOR_HEADER);
    for(ascii = string; *ascii; ++ascii) {		// (Do not copy the NULL)
      wchar = (uint16_t) ascii;			    // (zero-extending, very explicitly)
      wchar = ctx->fHostToLittleShort(wchar);
      memcpy(drop, & wchar, sizeof(wchar));
      drop += sizeof(wchar);
    } 
  }

  // Build the item 
  _item_init(item);
  item->size = needs;
  item->address = ctx->append;
  item->index = ctx->i_string;

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
usbdescbldr_make_bos_descriptor(usbdescbldr_ctx_t * ctx,
                                usbdescbldr_item_t * item)
{
  USB_BOS_DESCRIPTOR *dest;

  // There can only be one of these -- add check

  // Check space
  if (ctx->buffer != NULL) {
    if (sizeof(*dest) > _bufferAvailable(ctx)) 
      return USBDESCBLDR_NO_SPACE;
  }

  // begin construction
  dest = (USB_BOS_DESCRIPTOR *) ctx->append;
  // Fill in buffer unless in dry-run
  if (ctx->buffer != NULL) {
    dest = (USB_BOS_DESCRIPTOR *) ctx->append;
    memset(dest, 0, sizeof(*dest));
    dest->header.bLength = sizeof(*dest);
    dest->header.bDescriptorType = USB_DESCRIPTOR_TYPE_BOS;
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
usbdescbldr_make_device_capability_descriptor(usbdescbldr_ctx_t * ctx,
                                              usbdescbldr_item_t *    item,
                                              uint8_t	          bDevCapabilityType,
                                              uint8_t *	        typeDependent,	// Anonymous uint8_ta data
                                              size_t		              typeDependentSize)
{
  return USBDESCBLDR_UNSUPPORTED;
}


// Generate a Standard Interface descriptor.
usbdescbldr_status_t
usbdescbldr_make_standard_interface_descriptor(usbdescbldr_ctx_t * ctx,
                                                usbdescbldr_item_t * item,
                                                usbdescbldr_standard_interface_short_form_t * form)
{
  USB_INTERFACE_DESCRIPTOR *dest;
  size_t needs;

  if(form == NULL)
    return USBDESCBLDR_INVALID;

  // Config indicies are uint8_tas, limiting the number of them:
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
    memset(dest, 0, sizeof(*dest));

    dest->header.bLength = needs;
    dest->header.bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE;

    dest->bInterfaceNumber = form->bInterfaceNumber;
    dest->bAlternateSetting = form->bAlternateSetting;
    dest->bInterfaceClass = form->bInterfaceClass;
    dest->bInterfaceSubClass   = form->bInterfaceSubClass;
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
usbdescbldr_make_endpoint_descriptor(usbdescbldr_ctx_t * ctx,
                                     usbdescbldr_item_t * item,
                                     usbdescbldr_endpoint_short_form_t * form)
{
  USB_ENDPOINT_DESCRIPTOR *dest;
  uint16_t tShort;
  size_t needs;

  if(form == NULL)
    return USBDESCBLDR_INVALID;

  // Config indicies are uint8_tas, limiting the number of them:
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
    memset(dest, 0, sizeof(*dest));

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
  item->index = form->bEndpointAddress;

  // Consume buffer space (or just count, in dry run mode)
  ctx->append += needs;

  return USBDESCBLDR_OK;
}


usbdescbldr_status_t
usbdescbldr_make_ss_ep_companion_descriptor(usbdescbldr_ctx_t * ctx,
usbdescbldr_item_t * item,
usbdescbldr_ss_ep_companion_short_form_t * form)
{
  USB_SS_EP_COMPANION_DESCRIPTOR *dest;
  uint16_t tShort;
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
    memset(dest, 0, sizeof(*dest));

    dest->header.bLength = needs;
    dest->header.bDescriptorType = USB_DESCRIPTOR_TYPE_SS_EP_COMPANION;

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
usbdescbldr_make_interface_association_descriptor(usbdescbldr_ctx_t * ctx,
                                                  usbdescbldr_item_t * item,
                                                  usbdescbldr_iad_short_form_t * form)
{
  USB_INTERFACE_ASSOCIATION_DESCRIPTOR *dest;
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
    memset(dest, 0, sizeof(*dest));

    dest->header.bLength = needs;
    dest->header.bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION;

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



usbdescbldr_status_t
usbdescbldr_make_video_control_interface_descriptor(usbdescbldr_ctx_t * ctx,
usbdescbldr_item_t * item,
usbdescbldr_standard_interface_short_form_t * form)
{
  // really, this is only stubbed out in case we'd like to free the 
  // caller from specifying the class/subclass/protocol.
  form->bInterfaceClass = USB_INTERFACE_CC_VIDEO;
  form->bInterfaceSubClass = USB_INTERFACE_VC_SC_VIDEOCONTROL;
  form->bInterfaceProtocol = USB_INTERFACE_VC_PC_PROTOCOL_15;

  return usbdescbldr_make_standard_interface_descriptor(ctx, item, form);
}



// The VC CS Interface Header Descriptor. It is treated as a header for 
// numerous items that will follow it once built. The header itself has
// a variable number of interfaces at the end, which won't be known
// until the interfaces are added.

usbdescbldr_status_t
usbdescbldr_make_vc_cs_interface_descriptor(usbdescbldr_ctx_t * ctx,
usbdescbldr_item_t * item,
unsigned int dwClockFrequency
)
{
  USB_VC_CS_INTERFACE_DESCRIPTOR * dest;
  size_t needs;
  uint16_t tShort;
  uint32_t tInt;

  if(item == NULL)
    return USBDESCBLDR_INVALID;

  // This has a variable length, so can only enforce the header part
  needs = sizeof(*dest);
  if(needs > 0xff)
    return USBDESCBLDR_OVERSIZED;  // .. as opposed to NO SPACE ..

  // Construct
  if(ctx->buffer != NULL) {
    if(needs > _bufferAvailable(ctx))
      return USBDESCBLDR_NO_SPACE;

    dest = (USB_VC_CS_INTERFACE_DESCRIPTOR *) ctx->append;
    memset(dest, 0, sizeof(*dest));

    dest->header.bLength = needs;
    dest->header.bDescriptorType = USB_DESCRIPTOR_TYPE_VC_CS_INTERFACE;
    dest->header.bDescriptorSubtype = USB_INTERFACE_SUBTYPE_VC_HEADER;

    tShort = ctx->fHostToLittleShort(0x0150);
    memcpy(&dest->bcdUVC, &tShort, sizeof(dest->bcdUVC));

    tInt = ctx->fHostToLittleInt(dwClockFrequency);
    memcpy(&dest->dwClockFrequency, &tInt, sizeof(dest->dwClockFrequency));
  }

  // Build the item 
  _item_init(item);
  item->size = needs;
  item->address = ctx->append;

  // Consume buffer space (or just count, in dry run mode)
  ctx->append += needs;

  return USBDESCBLDR_OK;
}

