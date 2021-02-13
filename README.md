# AVR libUSB++

Coded by Alex Mansfield
Using V-USB as the underlying soft-stack

## About

libUSB++ is an (almost) fully C++ software-stack USB API for AVR. It should run on any hardware with a clock >12MHz. Tested confirmed working on:
ATMega644P

The underlying V-USB implementation is partly unchanged. Most of the Tx/Rx assembly is untouched, but the packet handling code has received some major modifications. The main one being that it is no longer limited to 3 endpoints.
libUSB++ can support **UP TO** a full set of 32 endpoints, 16 in, 16 out, including the default control Endpoint0.
Since the V-USB implementation supports only USB1.1-LowSpeed, **Bulk transfer endpoints are NOT SUPPORTED** by the specification *However*, some operating systems (with/without patches) *may* support them. Because of this, the Communications Device Class (CDC) is not technically supported, Hence I have included a simple interrupt-transfer based virtual Serial port firmware and a driver (Currently only supported for Linux) to go with it.

## Design

The libUSB++ design is very much based around the design of the USB specification. The only structure which actually requires software are **Endpoint**s, which can be either **In** or **Out**. All the other structures are (static/constant) data.

### Device

A USB Device must have a descriptor. A descriptor tells the USB Host about this device. For a device, this includes USB Spec, Vendor ID (VID), Product ID (PID), number of configurations etc.

Example:

```C++
const AVR::USB::Device pgmDevice PROGMEM{
    USB_BCD::USB1_1,            // Specify USB Version 1.1
    0x16c0,                     // VID = 0x16c0 // Van Ooijen Technische Informatica
    0x05dc,                     // PID = 0x05dc // shared ID for use with libusb
    0x0100,                     // Device version = 01.00
    DeviceClass::VendorSpecific,// Device Class = vendor specific
    0x00,                       // Sub-Device Class = unspecified (used for standard classes)
    0x00,                       // Device Protocol = unspecified (used for standard classes)
    pgmConfigurations,          // Array of configurations
    pgmStringTbl,               // String descriptor table
    1,                          // Manufacturer string idx
    2,                          // Product      string idx
    3,                          // Serial No    string idx
};
```

### Configuration

```C++

//Possible configuration for this device
const AVR::USB::Configuration AVR::USBSerial::pgmConfiguration0 PROGMEM{
    ConfigurationAttributes::NONE,  // Properties (remote wakeup/self powered)
    pgmInterfaces,                  // Interfaces for this configuration
    1                               // Configuration value (1-based index into array below)
};

// Array of configurations for this device
const AVR::pgm_array<AVR::USB::Configuration, 1> AVR::USBSerial::pgmConfigurations PROGMEM{{
    pgmConfiguration0               // Possible configuration to be used
}};

```

### Interface

```C++

// Array of endpoints in interface
// These MUST be cast to uni-directional since the array is of Endpoint*
// And EndpointInOut implicitly inherits Endpoint TWICE!
const AVR::pgm_array<Endpoint*, 2> AVR::USBSerial::pgmEndpoints0 PROGMEM
    static_cast<EndpointIn*>(&CustomEndpoint),  // Endpoints in interface
    static_cast<EndpointOut*>(&CustomEndpoint), // Endpoints in interface
}};
// The pointers in the above array do not point to PROGMEM
// These are actual objects statically constructed in SRAM

// Possible alternate interface
const AVR::USB::Interface AVR::USBSerial::pgmInterface0Alt0 PROGMEM
    InterfaceClass::ApplicationSpecific,    // Interface class type
    0,                                      // Interface sub-class (unspecified, used by standard classes)
    0,                                      // Interface protocol  (unspecified, used by standard classes)
    pgmEndpoints0,                          // Array of endpoints for interface
    0,                                      // Interface number
    0,                                      // Alternate interface number
    4                                       // Interface name String idx
};

// Array of possible alternate interfaces
const AVR::pgm_array<Interface, 1> AVR::USBSerial::pgmInterface0 PROGMEM
    pgmInterface0Alt0
}};

```

### Endpoint

Since the Endpoints are the only structure that require software, rather than just configuration, I'll split this part in two. One for the descriptors, and one for the Endpoints themselves.

#### Endpoint Descriptor

```C++
// Descriptor for input endpoint
// yes INPUT. in USB, IN = Dev->Host, OUT = Host->Dev
const EndpointDescriptor AVR::USBSerial::pgmEndpoint0Out PROGMEM{
    EndpointDirection::OUT,             // Endpoint transfer direction
    0,                                  // Endpoint number
    EndpointTransferType::Interrupt,    // Endpoint transfer type
    EndpointSynchronisationType::None,  // Endpoint synchronisation
    EndpointUsageType::Data,            // Endpoint usage type (data/feedback)
    10                                  // Interrupt interval (only relevant for interrupt endpoints)
};
const EndpointDescriptor AVR::USBSerial::pgmEndpoint0In PROGMEM{
    EndpointDirection::In,              // Endpoint transfer direction
    0,                                  // Endpoint number
    EndpointTransferType::Interrupt,    // Endpoint transfer type
    EndpointSynchronisationType::None,  // Endpoint synchronisation
    EndpointUsageType::Data,            // Endpoint usage type (data/feedback)
    10                                  // Interrupt interval (only relevant for interrupt endpoints)
};
```

#### Endpoint Class

Since Endpoints handle data between the host/device, they require some software in order to function correctly. Endpoints are split into 4 categories. 

- **EndpointIn** : Handles data Dev->Host
- **EndpointOut** : Handles data Host->Dev
- **EndpointInOut** : Handles data Host->Dev AND Dev->Host
- **Endpoint0** : Special endpoint that is present on all devices even if not present in descriptors. Handles setup/configuration of the device.

```C++

// Class may have member data associated with it.
class CustomEndpointIn : public AVR::USB::EndpointIn
{
public:
    void in() override
    // Overrides the virtual base class in()
    {
        // if there's already data in the Tx Buffer
        // we may want to clear it (optional)
        if(txLen()) clear();
        uint8_t len = 0;
        // if we have any data available to send
        // populate txBuf
        // Example:
        // txBuf[len++] = 'a';

        // Once we are done, generate the packet
        // ALWAYS use `getDataPID()` unless you know what you're doing
        // since the standard requires Data PID toggling
        // genPacket handles CRC etc.
        genPacket(getDataPID(), len);
    }
}

// Class may have member data associated with it.
class CustomEndpointOut : public AVR::USB::EndpointOut
{
public:
    bool out(uint8_t *rxBuf, uint8_t &rxLen, bool setup) override
    // Overrides the virtual base class out()
    {
        /** rxBuf contains data
         * [0] : PID
         * [1:rxLen-2] : Data
         * [rxLen-2:rxLen-1] : CRC
         */

        /** Once the endpoint is done with all
         * the data in rxBuf it MUST set rxLen = 0;
         * This marks the input buffer as clear
         * for use.
         */
        rxLen = 0;
        // If handled by function, return true.
        return true;
    }
}

// Same as classes above, except that it is both input AND output
// Class may have member data associated with it.
class CustomEndpointInOut : public AVR::USB::EndpointInOut
{
public:
    bool out(uint8_t *rxBuf, uint8_t &rxLen, bool setup) override
    // Overrides the virtual base class out()
    {
        // ... See CustomEndpointOut::out()
    }

    void in() override
    // Overrides the virtual base class in()
    {
        // ... See CustomEndpointIn::in()
    }
}


// This is a special case that must be handled with caution!
// Class may have member data associated with it.
class CustomEndpoint0 : public AVR::USB::Endpoint0
{
public:
    // Similarly to EndpointInOut, Endpoint0 has in() and out();
    bool out(uint8_t *rxBuf, uint8_t &rxLen, bool setup) override
    // Overrides the virtual base class out()
    {
        //Endpoint0 MUST check if transaction is a setup!
        if(setup){
            //If so, allow Endpoint0 to handle it
            //If setup() returns true, then the transaction
            //has been handled
            if(Endpoint0::setup(rxBuf, rxLen)) return true;
            //Otherwise, we should handle it ourselves
            //...
        }
        /** Once the endpoint is done with all
         * the data in rxBuf it MUST set rxLen = 0;
         * This marks the input buffer as clear
         * for use.
         */
        rxLen = 0;
        // If handled by function, return true.
        return true;
    }

    void in() override
    // Overrides the virtual base class in()
    {
        // if there's already data in the Tx Buffer
        // we may want to clear it (optional)
        if(txLen()) clear();
        // or maybe we just want to ignore it
        // if(txLen()) return;

        uint8_t len = 0;
        // if we have any data available to send
        // populate txBuf
        // Example:
        // txBuf[len++] = 'a';

        // Once we are done, generate the packet
        // ALWAYS use `getDataPID()` unless you know what you're doing
        // since the standard requires Data PID toggling
        // genPacket handles CRC etc.
        genPacket(getDataPID(), len);
    }
    
}

```

### String Descriptors

String descriptors hold data that could differ between devices, such as the **Serial number**, or names for each of the Interfaces/Configurations.
A device may have multiple sets of String Descriptors which will each have their own language.
Currently only 1 language is supported at a time.

```C++

// USB Strings use UTF-16 Wide chars.
const wchar_t MfrStr[] PROGMEM = L"bit.ly/amusb";
const wchar_t ProdStr[] PROGMEM = L"libusb++";
const wchar_t SerStr[] PROGMEM = L"CustomSerialNo";
const wchar_t ItfStr[] PROGMEM = L"USBserial";

const AVR::pgm_array<AVR::USB::StringDescriptorTable::Str, 4> pgmStrings PROGMEM{{
    {sizeof(MfrStr)-2, MfrStr},
    {sizeof(ProdStr)-2, ProdStr},
    {sizeof(SerStr)-2, SerStr},
    {sizeof(ItfStr)-2, ItfStr},
}};
const StringDescriptorTable AVR::USBSerial::pgmStringTbl PROGMEM{
    LanguageID::English_United_States,      // Language
    pgmStrings                              // Strings
};
```
