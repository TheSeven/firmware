#include "global.h"
#include "lib/fonts/prop8/prop8.h"


const Fonts::Prop8::Font Fonts::Prop8::small[] =
{
    0x20, 0x6e,  // ' ' - 'n'
    0x00, 0x03, 0x04, 0x04, 0x04, 0x04, 0x09, 0x09,
    0x09, 0x0c, 0x0f, 0x12, 0x15, 0x17, 0x1a, 0x1b,
    0x1e, 0x22, 0x26, 0x2a, 0x2e, 0x32, 0x36, 0x3a,
    0x3e, 0x42, 0x46, 0x47, 0x47, 0x4b, 0x4e, 0x52,
    0x56, 0x56, 0x5a, 0x5e, 0x62, 0x66, 0x6a, 0x6e,
    0x72, 0x76, 0x77, 0x7a, 0x7e, 0x82, 0x87, 0x8c,
    0x90, 0x94, 0x98, 0x9c, 0xa0, 0xa5, 0xa9, 0xae,
    0xb3, 0xb8, 0xbd, 0xc2, 0xc4, 0xc7, 0xc9, 0xc9,
    0xcc, 0xcc, 0xd0, 0xd4, 0xd8, 0xdc, 0xe0, 0xe4,
    0xe8, 0xec, 0xed, 0xf0, 0xf4, 0xf6, 0xfb, 0xff,
    0x00, 0x00, 0x00,  // ' '
    0x5f,  // '!'
     // '"'
     // '#'
     // '$'
    0x23, 0x13, 0x08, 0x64, 0x62,  // '%'
     // '&'
     // '''
    0x1c, 0x22, 0x41,  // '('
    0x41, 0x22, 0x1c,  // ')'
    0x14, 0x08, 0x14,  // '*'
    0x08, 0x1c, 0x08,  // '+'
    0x80, 0x40,  // ','
    0x08, 0x08, 0x08,  // '-'
    0x40,  // '.'
    0x60, 0x1c, 0x03,  // '/'
    0x3e, 0x49, 0x45, 0x3e,  // '0'
    0x44, 0x42, 0x7f, 0x40,  // '1'
    0x62, 0x51, 0x49, 0x46,  // '2'
    0x22, 0x49, 0x49, 0x36,  // '3'
    0x1c, 0x12, 0x7f, 0x10,  // '4'
    0x27, 0x45, 0x45, 0x39,  // '5'
    0x3c, 0x4a, 0x49, 0x31,  // '6'
    0x71, 0x09, 0x05, 0x03,  // '7'
    0x36, 0x49, 0x49, 0x36,  // '8'
    0x26, 0x49, 0x49, 0x3e,  // '9'
    0x24,  // ':'
     // ';'
    0x08, 0x14, 0x22, 0x41,  // '<'
    0x14, 0x14, 0x14,  // '='
    0x41, 0x22, 0x14, 0x08,  // '>'
    0x02, 0x51, 0x09, 0x06,  // '?'
     // '@'
    0x7e, 0x11, 0x11, 0x7e,  // 'A'
    0x7f, 0x49, 0x49, 0x36,  // 'B'
    0x3e, 0x41, 0x41, 0x22,  // 'C'
    0x7f, 0x41, 0x22, 0x1c,  // 'D'
    0x7f, 0x49, 0x49, 0x41,  // 'E'
    0x7f, 0x09, 0x09, 0x01,  // 'F'
    0x3e, 0x41, 0x49, 0x3a,  // 'G'
    0x7f, 0x08, 0x08, 0x7f,  // 'H'
    0x7f,  // 'I'
    0x20, 0x40, 0x3f,  // 'J'
    0x7f, 0x14, 0x22, 0x41,  // 'K'
    0x7f, 0x40, 0x40, 0x40,  // 'L'
    0x7f, 0x02, 0x0c, 0x02, 0x7f,  // 'M'
    0x7f, 0x04, 0x08, 0x10, 0x7f,  // 'N'
    0x3e, 0x41, 0x41, 0x3e,  // 'O'
    0x7f, 0x09, 0x09, 0x06,  // 'P'
    0x3e, 0x41, 0x21, 0x5e,  // 'Q'
    0x7f, 0x19, 0x29, 0x46,  // 'R'
    0x26, 0x49, 0x49, 0x32,  // 'S'
    0x01, 0x01, 0x7f, 0x01, 0x01,  // 'T'
    0x3f, 0x40, 0x40, 0x3f,  // 'U'
    0x1f, 0x20, 0x40, 0x20, 0x1f,  // 'V'
    0x7f, 0x20, 0x18, 0x20, 0x7f,  // 'W'
    0x63, 0x14, 0x08, 0x14, 0x63,  // 'X'
    0x03, 0x04, 0x78, 0x04, 0x03,  // 'Y'
    0x61, 0x51, 0x49, 0x45, 0x43,  // 'Z'
    0x7f, 0x41,  // '['
    0x03, 0x1c, 0x60,  // '\'
    0x41, 0x7f,  // ']'
     // '^'
    0x40, 0x40, 0x40,  // '_'
     // '`'
    0x20, 0x54, 0x54, 0x78,  // 'a'
    0x7f, 0x44, 0x44, 0x38,  // 'b'
    0x38, 0x44, 0x44, 0x28,  // 'c'
    0x38, 0x44, 0x44, 0x7f,  // 'd'
    0x38, 0x54, 0x54, 0x18,  // 'e'
    0x08, 0x7e, 0x09, 0x02,  // 'f'
    0x98, 0xa4, 0xa4, 0x7c,  // 'g'
    0x7f, 0x04, 0x04, 0x78,  // 'h'
    0x7d,  // 'i'
    0x20, 0x40, 0x3d,  // 'j'
    0x7f, 0x10, 0x28, 0x44,  // 'k'
    0x3f, 0x40,  // 'l'
    0x7c, 0x04, 0x18, 0x04, 0x78,  // 'm'
    0x7c, 0x08, 0x04, 0x78,  // 'n'
    0x6f, 0x7d,  // 'o' - '}'
    0x00, 0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c,
    0x21, 0x26, 0x2b, 0x2f, 0x32, 0x35, 0x35, 0x38,
    0x38, 0x44, 0x44, 0x38,  // 'o'
    0xfc, 0x24, 0x24, 0x18,  // 'p'
    0x18, 0x24, 0x24, 0xfc,  // 'q'
    0x7c, 0x08, 0x04, 0x08,  // 'r'
    0x48, 0x54, 0x54, 0x24,  // 's'
    0x04, 0x3f, 0x44, 0x20,  // 't'
    0x3c, 0x40, 0x20, 0x7c,  // 'u'
    0x1c, 0x20, 0x40, 0x20, 0x1c,  // 'v'
    0x3c, 0x40, 0x38, 0x40, 0x3c,  // 'w'
    0x44, 0x28, 0x10, 0x28, 0x44,  // 'x'
    0x0c, 0x50, 0x50, 0x3c,  // 'y'
    0x64, 0x54, 0x4c,  // 'z'
    0x08, 0x36, 0x41,  // '{'
     // '|'
    0x41, 0x36, 0x08,  // '}'
    0, 0,
};
