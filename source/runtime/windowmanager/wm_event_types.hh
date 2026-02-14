#pragma once

#include <cstdint>
#include <ostream>

namespace vektor
{

enum wmEventType : int16_t
{
    EVENT_NONE = 0x0000,

#define _EVT_MOUSE_MIN 0x0001

    LEFTMOUSE    = 0x0001, /* 1 */
    MIDDLEMOUSE  = 0x0002, /* 2 */
    RIGHTMOUSE   = 0x0003, /* 3 */
    MOUSEMOVE    = 0x0004, /* 4 */

    BUTTON4MOUSE = 0x0007, /* 7 */
    BUTTON5MOUSE = 0x0008, /* 8 */

    BUTTON6MOUSE = 0x0012, /* 18 */
    BUTTON7MOUSE = 0x0013, /* 19 */

    MOUSEPAN     = 0x000e, /* 14 */
    MOUSEZOOM    = 0x000f, /* 15 */

#define _EVT_MOUSE_MAX 0x0015

#define _EVT_KEYBOARD_MIN 0x0020 /* 32 */

    EVT_ZEROKEY       = 0x0030, /* '0' (48). */
    EVT_ONEKEY        = 0x0031, /* '1' (49). */
    EVT_TWOKEY        = 0x0032, /* '2' (50). */
    EVT_THREEKEY      = 0x0033, /* '3' (51). */
    EVT_FOURKEY       = 0x0034, /* '4' (52). */
    EVT_FIVEKEY       = 0x0035, /* '5' (53). */
    EVT_SIXKEY        = 0x0036, /* '6' (54). */
    EVT_SEVENKEY      = 0x0037, /* '7' (55). */
    EVT_EIGHTKEY      = 0x0038, /* '8' (56). */
    EVT_NINEKEY       = 0x0039, /* '9' (57). */

    EVT_AKEY          = 0x0061, /* 'a' (97). */
    EVT_BKEY          = 0x0062, /* 'b' (98). */
    EVT_CKEY          = 0x0063, /* 'c' (99). */
    EVT_DKEY          = 0x0064, /* 'd' (100). */
    EVT_EKEY          = 0x0065, /* 'e' (101). */
    EVT_FKEY          = 0x0066, /* 'f' (102). */
    EVT_GKEY          = 0x0067, /* 'g' (103). */
    EVT_HKEY          = 0x0068, /* 'h' (104). */
    EVT_IKEY          = 0x0069, /* 'i' (105). */
    EVT_JKEY          = 0x006a, /* 'j' (106). */
    EVT_KKEY          = 0x006b, /* 'k' (107). */
    EVT_LKEY          = 0x006c, /* 'l' (108). */
    EVT_MKEY          = 0x006d, /* 'm' (109). */
    EVT_NKEY          = 0x006e, /* 'n' (110). */
    EVT_OKEY          = 0x006f, /* 'o' (111). */
    EVT_PKEY          = 0x0070, /* 'p' (112). */
    EVT_QKEY          = 0x0071, /* 'q' (113). */
    EVT_RKEY          = 0x0072, /* 'r' (114). */
    EVT_SKEY          = 0x0073, /* 's' (115). */
    EVT_TKEY          = 0x0074, /* 't' (116). */
    EVT_UKEY          = 0x0075, /* 'u' (117). */
    EVT_VKEY          = 0x0076, /* 'v' (118). */
    EVT_WKEY          = 0x0077, /* 'w' (119). */
    EVT_XKEY          = 0x0078, /* 'x' (120). */
    EVT_YKEY          = 0x0079, /* 'y' (121). */
    EVT_ZKEY          = 0x007a, /* 'z' (122). */

    EVT_LEFTARROWKEY  = 0x0089, /* 137 */
    EVT_DOWNARROWKEY  = 0x008a, /* 138 */
    EVT_RIGHTARROWKEY = 0x008b, /* 139 */
    EVT_UPARROWKEY    = 0x008c, /* 140 */

    EVT_F1KEY         = 0x012c, /* 300 */
    EVT_F2KEY         = 0x012d, /* 301 */
    EVT_F3KEY         = 0x012e, /* 302 */
    EVT_F4KEY         = 0x012f, /* 303 */
    EVT_F5KEY         = 0x0130, /* 304 */
    EVT_F6KEY         = 0x0131, /* 305 */
    EVT_F7KEY         = 0x0132, /* 306 */
    EVT_F8KEY         = 0x0133, /* 307 */
    EVT_F9KEY         = 0x0134, /* 308 */
    EVT_F10KEY        = 0x0135, /* 309 */
    EVT_F11KEY        = 0x0136, /* 310 */
    EVT_F12KEY        = 0x0137, /* 311 */
    EVT_F13KEY        = 0x0138, /* 312 */
    EVT_F14KEY        = 0x0139, /* 313 */
    EVT_F15KEY        = 0x013a, /* 314 */
    EVT_F16KEY        = 0x013b, /* 315 */
    EVT_F17KEY        = 0x013c, /* 316 */
    EVT_F18KEY        = 0x013d, /* 317 */
    EVT_F19KEY        = 0x013e, /* 318 */
    EVT_F20KEY        = 0x013f, /* 319 */
    EVT_F21KEY        = 0x0140, /* 320 */
    EVT_F22KEY        = 0x0141, /* 321 */
    EVT_F23KEY        = 0x0142, /* 322 */
    EVT_F24KEY        = 0x0143, /* 323 */

    // Special Modifiers
    EVT_ESCKEY        = 0x00da,
    EVT_TABKEY        = 0x00db,
    EVT_RETKEY        = 0x00dc,
    EVT_SPACEKEY      = 0x00dd,
    EVT_LEFTSHIFTKEY  = 0x00d9,
    EVT_RIGHTSHIFTKEY = 0x00d8,
    EVT_LEFTCTRLKEY   = 0x00d4,
    EVT_RIGHTCTRLKEY  = 0x00d7,
    EVT_LEFTALTKEY    = 0x00d5,

    EVT_PRESS         = 0x0100, /* 256 */
    EVT_RELEASE       = 0x0101, /* 257 */
    EVT_CLICK         = 0x0102, /* 258 */
    EVT_DBL_CLICK     = 0x0103, /* 259 */
    EVT_CLICK_DRAG    = 0x0104, /* 260 */

    EVT_SHIFT         = (1 << 0), /* 1 */
    EVT_CTRL          = (1 << 1), /* 2 */
    EVT_ALT           = (1 << 2), /* 4 */
    EVT_OSKEY         = (1 << 3), /* 8 */

#define _EVT_KEYBOARD_MAX 0x0104
};

inline std::ostream& operator<<(std::ostream& os,
                                wmEventType   key)
{
    return os << static_cast<uint16_t>(key);
}

template <typename T,
          typename... Args>
constexpr bool ELEM(const T& v,
                    const Args&... args)
{
    return ((v == args) || ...);
}

#define ISTIMER(event_type) ((event_type) >= TIMER && (event_type) <= _TIMER_MAX)

#define ISKEYBOARD(event_type)                                                                                         \
    (((event_type) >= _EVT_KEYBOARD_MIN && (event_type) <= _EVT_KEYBOARD_MAX) ||                                       \
     ((event_type) >= EVT_F1KEY && (event_type) <= EVT_F24KEY))

#define ISMOUSE(event_type) ((event_type) >= _EVT_MOUSE_MIN && (event_type) <= _EVT_MOUSE_MAX)

#define ISMOUSE_MOTION(event_type) ELEM(event_type, MOUSEMOVE, INBETWEEN_MOUSEMOVE)

#define ISMOUSE_BUTTON(event_type)                                                                                     \
    (ELEM(event_type, LEFTMOUSE, MIDDLEMOUSE, RIGHTMOUSE, BUTTON4MOUSE, BUTTON5MOUSE, BUTTON6MOUSE, BUTTON7MOUSE))

#define ISKEYBOARD_OR_BUTTON(event_type) (ISMOUSE_BUTTON(event_type) || ISKEYBOARD(event_type))
} // namespace vektor