#include "Arduino.h"

// Bitmap image for the no-connection status
static const unsigned char PROGMEM noConn[] PROGMEM =
{
        // 'none', 11x8px
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdb, 0x60
};

// Bitmap image for the bad-connection status
static const unsigned char PROGMEM badConn[] PROGMEM =
{
        // 'bad', 11x8px
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0xdb, 0x60
};

// Bitmap image for the stable-connection status
static const unsigned char PROGMEM stabletConn[] PROGMEM =
{
        // 'stable', 11x8px
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x18, 0x00, 0xd8, 0x00, 0xdb, 0x60
};

// Bitmap image for the good-connection status
static const unsigned char PROGMEM goodConn[] PROGMEM =
{
        // 'good', 11x8px
        0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x1b, 0x00, 0x1b, 0x00, 0xdb, 0x00, 0xdb, 0x60
};

// Bitmap image for the excellent-connection status
static const unsigned char PROGMEM excellentConn[] PROGMEM =
{
        // 'excellent', 11x8px
        0x00, 0x60, 0x00, 0x60, 0x03, 0x60, 0x03, 0x60, 0x1b, 0x60, 0x1b, 0x60, 0xdb, 0x60, 0xdb, 0x60
};