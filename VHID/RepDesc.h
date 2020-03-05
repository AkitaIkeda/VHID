#include <ntddk.h>

UCHAR ReportDescriptor[] = {
    // Keyboard(STD101)
    0x05,   0x01,       // Usage Page(Generic Desktop),
    0x09,   0x06,       // Usage(Keyboard),
    0xA1,   0x01,       // Collection(Application),
    0x85,   0x01,       //  Report Id(1)
    0x05,   0x07,       //  usage page key codes
    0x19,   0xe0,       //  usage min left control
    0x29,   0xe7,       //  usage max keyboard right gui
    0x15,   0x00,       //  Logical Minimum(0),
    0x25,   0x01,       //  Logical Maximum(1),
    0x75,   0x01,       //  report size 1
    0x95,   0x08,       //  report count 8
    0x81,   0x02,       //  input(Variable)
    0x19,   0x00,       //  usage min 0
    0x29,   0x91,       //  usage max 91
    0x15,   0x00,       //  Logical Minimum(0),
    0x26,   0xff, 0x00, //  logical max 0xff
    0x75,   0x08,       //  report size 8
    0x95,   0x06,       //  report count 4
    0x81,   0x00,       //  Input(Data, Array),
    0xC0,				// End Collection,

    // Mouse
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x02,                    // USAGE (Mouse)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x85, 0x02,                    //  Report Id(2)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x03,                    //     USAGE_MAXIMUM (Button 3)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x95, 0x08,                    //     REPORT_COUNT (8)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x16, 0x00, 0x00,              //     Logical Minimum(0)
    0x26, 0xFF, 0x7F,              //     Logical Maximum(32767)
    0x36, 0x00, 0x00,              //     Physical Minimum(0)
    0x46, 0xFF, 0x7F,              //     Physical Maximum(32767)
    0x66, 0x00, 0x00,              //     Unit(None)
    0x75, 0x10,                    //     Report Size(16)
    0x95, 0x02,                    //     Report Count(2)
    0x81, 0x62,                    //     Input(Data, Variable, Absolute, No Preferred, Null State)
    0x95, 0x05,                    //     REPORT_COUNT (2)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
    0xc0,                          //   END_COLLECTION
    0xc0,                          // END_COLLECTION

	//// Keyboard(STD101)
    //0x05,   0x01,       // Usage Page(Generic Desktop),
    //0x09,   0x06,       // Usage(Keyboard),
    //0xA1,   0x01,       // Collection(Application),
    //0x85,   0x01,       //  Report Id(1)
    //0x05,   0x07,       //  usage page key codes
    //0x19,   0xe0,       //  usage min left control
    //0x29,   0xe7,       //  usage max keyboard right gui
    //0x15,   0x00,       //  Logical Minimum(0),
    //0x25,   0x01,       //  Logical Maximum(1),
    //0x75,   0x01,       //  report size 1
    //0x95,   0x08,       //  report count 8
    //0x81,   0x02,       //  input(Variable)
    //0x19,   0x00,       //  usage min 0
    //0x29,   0x91,       //  usage max 91
    //0x15,   0x00,       //  Logical Minimum(0),
    //0x26,   0xff, 0x00, //  logical max 0xff
    //0x75,   0x08,       //  report size 8
    //0x95,   0x04,       //  report count 4
    //0x81,   0x00,       //  Input(Data, Array),
    //0xC0,				// End Collection,

    //// Mouse
    //0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    //0x09, 0x02,                    // USAGE (Mouse)
    //0xa1, 0x01,                    // COLLECTION (Application)
    //0x85, 0x02,                    //  Report Id(2)
    //0x09, 0x01,                    //   USAGE (Pointer)
    //0xa1, 0x00,                    //   COLLECTION (Physical)
    //0x05, 0x09,                    //     USAGE_PAGE (Button)
    //0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    //0x29, 0x08,                    //     USAGE_MAXIMUM (Button 8)
    //0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    //0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    //0x95, 0x08,                    //     REPORT_COUNT (8)
    //0x75, 0x01,                    //     REPORT_SIZE (1)
    //0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    //0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    //0x09, 0x30,                    //     USAGE (X)
    //0x09, 0x31,                    //     USAGE (Y)
    //0x16, 0x00, 0x00,              //     Logical Minimum(0)
    //0x26, 0xFF, 0x7F,              //     Logical Maximum(32767)
    //0x36, 0x00, 0x00,              //     Physical Minimum(0)
    //0x46, 0xFF, 0x7F,              //     Physical Maximum(32767)
    //0x66, 0x00, 0x00,              //     Unit(None)
    //0x75, 0x10,                    //     Report Size(16)
    //0x95, 0x02,                    //     Report Count(2)
    //0x81, 0x62,                    //     Input(Data, Variable, Absolute, No Preferred, Null State)
    //0xc0,                          //   END_COLLECTION
    //0xc0,                          // END_COLLECTION
};

