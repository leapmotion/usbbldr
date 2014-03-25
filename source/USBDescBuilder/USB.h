#pragma once
#include <winusb.h>

//! Class-specific USB descriptor types.
enum USB_DESC_TYPE : char
{
  USB_DEVICE        = 0x01,
  USB_CONFIGURATION = 0x02,
  USB_STRING        = 0x03,
	UVC_INTERFACE			= 0x04,
	UVC_ENDPOINT			= 0x05,
	USB_POWER   			= 0x08,
  USB_INTERFACE_ASSOCIATION = 11,
	UVC_CS_UNDEFINED		= 0x20,
	UVC_CS_DEVICE			= 0x21,
	UVC_CS_CONFIGURATION	= 0x22,
	UVC_CS_STRING			= 0x23,
	UVC_CS_INTERFACE		= 0x24,
	UVC_CS_ENDPOINT			= 0x25,
 UVC_COMPANION  = 0x30,
  USB_DESC_TYPE_LAST
};

//! bmRequest.Dir
typedef enum _BMREQUEST_DIR
{
    BMREQUEST_DIR_HOST_TO_DEVICE = 0,
    BMREQUEST_DIR_DEVICE_TO_HOST = 1,
} BMREQUEST_DIR;

//! bmRequest.Type
typedef enum _BMREQUEST_TYPE
{
    //! Standard request. See \ref USB_REQUEST_ENUM
    BMREQUEST_TYPE_STANDARD = 0,

    //! Class-specific request.
    BMREQUEST_TYPE_CLASS = 1,

    //! Vendor-specific request
    BMREQUEST_TYPE_VENDOR = 2,
} BMREQUEST_TYPE;

//! bmRequest.Recipient
typedef enum _BMREQUEST_RECIPIENT
{
    //! Request is for a device.
    BMREQUEST_RECIPIENT_DEVICE = 0,

    //! Request is for an interface of a device.
    BMREQUEST_RECIPIENT_INTERFACE = 1,

    //! Request is for an endpoint of a device.
    BMREQUEST_RECIPIENT_ENDPOINT = 2,

    //! Request is for a vendor-specific purpose.
    BMREQUEST_RECIPIENT_OTHER = 3,
} BMREQUEST_RECIPIENT;

//! Standard USB descriptor types. For more information, see section 9-5 of the USB 3.0 specifications.
typedef enum _USB_DESCRIPTOR_TYPE
{
    //! Device descriptor type.
    USB_DESCRIPTOR_TYPE_DEVICE = 0x01,

    //! Configuration descriptor type.
    USB_DESCRIPTOR_TYPE_CONFIGURATION = 0x02,

    //! String descriptor type.
    USB_DESCRIPTOR_TYPE_STRING = 0x03,

    //! Interface descriptor type.
    USB_DESCRIPTOR_TYPE_INTERFACE = 0x04,

    //! Endpoint descriptor type.
    USB_DESCRIPTOR_TYPE_ENDPOINT = 0x05,

    //! Device qualifier descriptor type.
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER = 0x06,

    //! Config power descriptor type.
    USB_DESCRIPTOR_TYPE_CONFIG_POWER = 0x07,

    //! Interface power descriptor type.
    USB_DESCRIPTOR_TYPE_INTERFACE_POWER = 0x08,

    //! Interface association descriptor type.
    USB_DESCRIPTOR_TYPE_INTERFACE_ASSOCIATION = 0x0B,
} USB_DESCRIPTOR_TYPE;


#pragma pack(push, 1)
struct USB_SETUP_PACKET
{
  unsigned Recipient: 2;
  unsigned Reserved: 3;
  unsigned Type: 2;
  unsigned Dir: 1;

  unsigned Request:8;
  unsigned Value:16;
  unsigned Index:16;
  unsigned Length:16;
};

static_assert(sizeof(USB_SETUP_PACKET) == 8, "Setup packet is required to be exactly eight bytes, by spec");

struct usb_descriptor_header {
  BYTE bLength;
  USB_DESC_TYPE bDescriptorType;
};
static_assert(sizeof(usb_descriptor_header) == 2, "Invalid USB descriptor header size");

enum USB_CC: char {
  USB_CC_VIDEO = 0x0E
};

/** Video interface subclass code (A.2) */
enum uvc_int_subclass_code:char {
	UVC_SC_UNDEFINED = 0x00,
	UVC_SC_VIDEOCONTROL = 0x01,
	UVC_SC_VIDEOSTREAMING = 0x02,
	UVC_SC_VIDEO_INTERFACE_COLLECTION = 0x03
};

struct usb_if_desc:
  usb_descriptor_header
{
  BYTE bInterfaceNumber;
  BYTE bAlternateSetting;
  BYTE bNumEndpoints;
  USB_CC bInterfaceClass;
  union {
    uvc_int_subclass_code bUvcInterfaceSubClass;
  };
  BYTE bInterfaceProtocol;
  BYTE iInterface;
};
static_assert(sizeof(usb_if_desc) == 9, "VC Interface Descriptor size mismatch");

enum EndpointTransferType : char {
  TransferTypeUnknown = 0,
  TransferTypeIso = 1,
  TransferTypeBulk = 2,
  TransferTypeInterrupt = 3
};

enum EndpointSyncType : char {
  SyncTypeNone = 0,
  SyncTypeAsynch = 1,
  SyncTypeSync = 2
};

enum EndpointDirection : char {
  EndpointDirectionOut = 0,
  EndpointDirectionIn = 1
};

struct usb_endpoint:
  usb_descriptor_header
{
  unsigned char EndpointNumber:4;
  unsigned char:3;
  EndpointDirection EndpointDirection:1;
  EndpointTransferType TransferType:2;
  EndpointSyncType SyncType:2;
  unsigned char:0;
  WORD wMaxPacketSize;
  BYTE bInterval;
};
static_assert(sizeof(usb_endpoint) == 7, "Endpoint descriptor size mismatch");

/// <summary>
/// Common enumerator specification:
/// </summary>
/// <remarks>
/// It's generally safe to reinterpret-cast pointers of this type to pointers of related types.  The
/// sizes and layouts of all descriptor_iterator instances is guaranteed to be consistent.
/// </remarks>
template<class T>
class descriptor_iterator {
public:
  static_assert(std::is_same<void*, T>::value || std::is_base_of<usb_descriptor_header, T>::value, "T is not a usb descriptor type");

  /// <summary>
  /// Trivial constructor which creates a new iterator starting at some basis up to some extent
  /// </summary>
  descriptor_iterator(const T* ptr, size_t size):
    m_ptr(ptr),
    m_end((char*)m_ptr + size)
  {
    static_assert(sizeof(*this) == sizeof(descriptor_iterator<void*>), "Descriptor iterator has a non-compliant size");
  }

protected:
  const T* m_ptr;
  const char* m_end;

  template<class T>
  friend class descriptor_iterator;

public:
  /// <summary>
  /// Reinterprets the bound pointer as the specified type
  /// </summary>
  template<class W>
  const W*const& as(void) const {return (W*&)m_ptr;}

  template<class F>
  operator descriptor_iterator<F>&(void) {
    return *(descriptor_iterator<F>*)this;
  }

  descriptor_iterator& operator++(void) {return (char*&)m_ptr += m_ptr->bLength, *this;}

  const T* operator->(void) const {
    // Trivial bounds check:
    assert((char*)&m_ptr[1] <= m_end);
    return m_ptr;
  }

  const T& operator*(void) const {return **this;}
  operator bool(void) const {return (char*)m_ptr < m_end;}
  operator const T*const&(void) const {return m_ptr;}
};

template<>
class descriptor_iterator<USB_CONFIGURATION_DESCRIPTOR>:
  public descriptor_iterator<usb_descriptor_header>
{
public:
  descriptor_iterator(const USB_CONFIGURATION_DESCRIPTOR& desc):
    descriptor_iterator<usb_descriptor_header>(
      (const usb_descriptor_header*)&desc,
      desc.wTotalLength
    )
  {}
};

#pragma pack(pop)