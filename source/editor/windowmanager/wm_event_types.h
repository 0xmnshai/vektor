// SPDX-FileCopyrightText: 2024 Vektor Authors
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <Qt>
#include <cstdint>

namespace vektor {

// Custom-data type tags stored in wmEvent::customdata_type
enum wmEventCustomdataType : uint8_t {
  EVT_DATA_TIMER = 2,
  EVT_DATA_DRAGDROP = 3,
  EVT_DATA_NDOF_MOTION = 4,
};

// Tablet active mode
enum wmTabletActiveMode : uint8_t {
  EVT_TABLET_NONE = 0,
  EVT_TABLET_STYLUS = 1,
  EVT_TABLET_ERASER = 2,
};

// Main event-type enum — values matching Qt::Key are intentionally equal for direct cast
enum wmEventType : int {
  EVENT_NONE = 0x0000,

// Mouse / pointer: 0x0001 – 0x001f
#define EVT_MOUSE_MIN 0x0001

  LEFTMOUSE = 0x0001,
  MIDDLEMOUSE = 0x0002,
  RIGHTMOUSE = 0x0003,
  MOUSEMOVE = 0x0004,

  BUTTON4MOUSE = 0x0007,
  BUTTON5MOUSE = 0x0008,
  BUTTON6MOUSE = 0x0012,
  BUTTON7MOUSE = 0x0013,

  MOUSEPAN = 0x000e,
  MOUSEZOOM = 0x000f,
  MOUSEROTATE = 0x0010,
  MOUSESMARTZOOM = 0x0017,

  WHEELUPMOUSE = 0x000a,
  WHEELDOWNMOUSE = 0x000b,
  WHEELINMOUSE = 0x000c,
  WHEELOUTMOUSE = 0x000d,

  WHEELLEFTMOUSE = 0x0014,
  WHEELRIGHTMOUSE = 0x0015,

  INBETWEEN_MOUSEMOVE = 0x0011,

#define EVT_MOUSE_MAX 0x0015

  // IME / tablet: 0x0016 – 0x001b
  WM_IME_COMPOSITE_START = 0x0016,
  WM_IME_COMPOSITE_EVENT = 0x0018,
  WM_IME_COMPOSITE_END = 0x0019,

  TABLET_STYLUS = 0x001a,
  TABLET_ERASER = 0x001b,

// Keyboard: 0x0020 – 0x00ff + Qt extended range
#define EVT_KEYBOARD_MIN 0x0020

  // Digit row
  EVT_ZEROKEY = Qt::Key_0,
  EVT_ONEKEY = Qt::Key_1,
  EVT_TWOKEY = Qt::Key_2,
  EVT_THREEKEY = Qt::Key_3,
  EVT_FOURKEY = Qt::Key_4,
  EVT_FIVEKEY = Qt::Key_5,
  EVT_SIXKEY = Qt::Key_6,
  EVT_SEVENKEY = Qt::Key_7,
  EVT_EIGHTKEY = Qt::Key_8,
  EVT_NINEKEY = Qt::Key_9,

  // Alphabet
  EVT_AKEY = Qt::Key_A,
  EVT_BKEY = Qt::Key_B,
  EVT_CKEY = Qt::Key_C,
  EVT_DKEY = Qt::Key_D,
  EVT_EKEY = Qt::Key_E,
  EVT_FKEY = Qt::Key_F,
  EVT_GKEY = Qt::Key_G,
  EVT_HKEY = Qt::Key_H,
  EVT_IKEY = Qt::Key_I,
  EVT_JKEY = Qt::Key_J,
  EVT_KKEY = Qt::Key_K,
  EVT_LKEY = Qt::Key_L,
  EVT_MKEY = Qt::Key_M,
  EVT_NKEY = Qt::Key_N,
  EVT_OKEY = Qt::Key_O,
  EVT_PKEY = Qt::Key_P,
  EVT_QKEY = Qt::Key_Q,
  EVT_RKEY = Qt::Key_R,
  EVT_SKEY = Qt::Key_S,
  EVT_TKEY = Qt::Key_T,
  EVT_UKEY = Qt::Key_U,
  EVT_VKEY = Qt::Key_V,
  EVT_WKEY = Qt::Key_W,
  EVT_XKEY = Qt::Key_X,
  EVT_YKEY = Qt::Key_Y,
  EVT_ZKEY = Qt::Key_Z,

  // Arrow keys
  EVT_LEFTARROWKEY = Qt::Key_Left,
  EVT_DOWNARROWKEY = Qt::Key_Down,
  EVT_RIGHTARROWKEY = Qt::Key_Right,
  EVT_UPARROWKEY = Qt::Key_Up,

  // Numeric keypad — private range 0x0096..0x009f
  EVT_PAD0 = 0x0096,
  EVT_PAD1 = 0x0097,
  EVT_PAD2 = 0x0098,
  EVT_PAD3 = 0x0099,
  EVT_PAD4 = 0x009a,
  EVT_PAD5 = 0x009b,
  EVT_PAD6 = 0x009c,
  EVT_PAD7 = 0x009d,
  EVT_PAD8 = 0x009e,
  EVT_PAD9 = 0x009f,

  EVT_PADASTERKEY = 0x00a0,
  EVT_PADSLASHKEY = 0x00a1,
  EVT_PADMINUS = 0x00a2,
  EVT_PADENTER = 0x00a3,
  EVT_PADPLUSKEY = 0x00a4,
  EVT_PADPERIOD = 0x00c7,

  // Navigation cluster
  EVT_PAUSEKEY = Qt::Key_Pause,
  EVT_INSERTKEY = Qt::Key_Insert,
  EVT_HOMEKEY = Qt::Key_Home,
  EVT_PAGEUPKEY = Qt::Key_PageUp,
  EVT_PAGEDOWNKEY = Qt::Key_PageDown,
  EVT_ENDKEY = Qt::Key_End,

  EVT_UNKNOWNKEY = 0x00ab,
  EVT_OSKEY = Qt::Key_Meta,
  EVT_GRLESSKEY = 0x00ad,

  // Media keys
  EVT_MEDIAPLAY = Qt::Key_MediaPlay,
  EVT_MEDIASTOP = Qt::Key_MediaStop,
  EVT_MEDIAFIRST = Qt::Key_MediaPrevious,
  EVT_MEDIALAST = Qt::Key_MediaNext,
  EVT_APPKEY = Qt::Key_Menu,
  EVT_HYPER = Qt::Key_Hyper_L,

  EVT_CAPSLOCKKEY = Qt::Key_CapsLock,

  // Modifier keys — L/R distinction handled via QKeyEvent::nativeVirtualKey
  EVT_LEFTCTRLKEY = Qt::Key_Control,
  EVT_LEFTALTKEY = Qt::Key_Alt,
  EVT_RIGHTALTKEY = Qt::Key_AltGr,
  EVT_RIGHTCTRLKEY = Qt::Key_Control,
  EVT_RIGHTSHIFTKEY = Qt::Key_Shift,
  EVT_LEFTSHIFTKEY = Qt::Key_Shift,

  // Special character keys
  EVT_ESCKEY = Qt::Key_Escape,
  EVT_TABKEY = Qt::Key_Tab,
  EVT_RETKEY = Qt::Key_Return,
  EVT_SPACEKEY = Qt::Key_Space,
  EVT_LINEFEEDKEY = 0x00de,
  EVT_BACKSPACEKEY = Qt::Key_Backspace,
  EVT_DELKEY = Qt::Key_Delete,
  EVT_SEMICOLONKEY = Qt::Key_Semicolon,
  EVT_PERIODKEY = Qt::Key_Period,
  EVT_COMMAKEY = Qt::Key_Comma,
  EVT_QUOTEKEY = Qt::Key_Apostrophe,
  EVT_ACCENTGRAVEKEY = Qt::Key_QuoteLeft,
  EVT_MINUSKEY = Qt::Key_Minus,
  EVT_PLUSKEY = Qt::Key_Plus,
  EVT_SLASHKEY = Qt::Key_Slash,
  EVT_BACKSLASHKEY = Qt::Key_Backslash,
  EVT_EQUALKEY = Qt::Key_Equal,
  EVT_LEFTBRACKETKEY = Qt::Key_BracketLeft,
  EVT_RIGHTBRACKETKEY = Qt::Key_BracketRight,

#define EVT_KEYBOARD_MAX 0x00ff
#define EVT_QT_SPECIAL_MIN ((int)Qt::Key_Escape)
#define EVT_QT_SPECIAL_MAX ((int)Qt::Key_Launch9)

  // Function keys
  EVT_F1KEY = Qt::Key_F1,
  EVT_F2KEY = Qt::Key_F2,
  EVT_F3KEY = Qt::Key_F3,
  EVT_F4KEY = Qt::Key_F4,
  EVT_F5KEY = Qt::Key_F5,
  EVT_F6KEY = Qt::Key_F6,
  EVT_F7KEY = Qt::Key_F7,
  EVT_F8KEY = Qt::Key_F8,
  EVT_F9KEY = Qt::Key_F9,
  EVT_F10KEY = Qt::Key_F10,
  EVT_F11KEY = Qt::Key_F11,
  EVT_F12KEY = Qt::Key_F12,
  EVT_F13KEY = Qt::Key_F13,
  EVT_F14KEY = Qt::Key_F14,
  EVT_F15KEY = Qt::Key_F15,
  EVT_F16KEY = Qt::Key_F16,
  EVT_F17KEY = Qt::Key_F17,
  EVT_F18KEY = Qt::Key_F18,
  EVT_F19KEY = Qt::Key_F19,
  EVT_F20KEY = Qt::Key_F20,
  EVT_F21KEY = Qt::Key_F21,
  EVT_F22KEY = Qt::Key_F22,
  EVT_F23KEY = Qt::Key_F23,
  EVT_F24KEY = Qt::Key_F24,

  // NDOF / 3D Connexion: 0x0190 – 0x01c0
  NDOF_MOTION = 0x0190,

#define NDOF_MIN NDOF_MOTION
#define NDOF_BUTTON_MIN NDOF_BUTTON_MENU

  NDOF_BUTTON_MENU = 0x0191,
  NDOF_BUTTON_FIT = 0x0192,
  NDOF_BUTTON_TOP = 0x0193,
  NDOF_BUTTON_BOTTOM = 0x0194,
  NDOF_BUTTON_LEFT = 0x0195,
  NDOF_BUTTON_RIGHT = 0x0196,
  NDOF_BUTTON_FRONT = 0x0197,
  NDOF_BUTTON_BACK = 0x0198,
  NDOF_BUTTON_ISO1 = 0x0199,
  NDOF_BUTTON_ISO2 = 0x019a,
  NDOF_BUTTON_ROLL_CW = 0x019b,
  NDOF_BUTTON_ROLL_CCW = 0x019c,
  NDOF_BUTTON_SPIN_CW = 0x019d,
  NDOF_BUTTON_SPIN_CCW = 0x019e,
  NDOF_BUTTON_TILT_CW = 0x019f,
  NDOF_BUTTON_TILT_CCW = 0x01a0,
  NDOF_BUTTON_ROTATE = 0x01a1,
  NDOF_BUTTON_PANZOOM = 0x01a2,
  NDOF_BUTTON_DOMINANT = 0x01a3,
  NDOF_BUTTON_PLUS = 0x01a4,
  NDOF_BUTTON_MINUS = 0x01a5,
  NDOF_BUTTON_V1 = 0x01a6,
  NDOF_BUTTON_V2 = 0x01a7,
  NDOF_BUTTON_V3 = 0x01a8,
  NDOF_BUTTON_1 = 0x01aa,
  NDOF_BUTTON_2 = 0x01ab,
  NDOF_BUTTON_3 = 0x01ac,
  NDOF_BUTTON_4 = 0x01ad,
  NDOF_BUTTON_5 = 0x01ae,
  NDOF_BUTTON_6 = 0x01af,
  NDOF_BUTTON_7 = 0x01b0,
  NDOF_BUTTON_8 = 0x01b1,
  NDOF_BUTTON_9 = 0x01b2,
  NDOF_BUTTON_10 = 0x01b3,
  NDOF_BUTTON_11 = 0x01b4,
  NDOF_BUTTON_12 = 0x01b5,
  NDOF_BUTTON_SAVE_V1 = 0x01be,
  NDOF_BUTTON_SAVE_V2 = 0x01bf,
  NDOF_BUTTON_SAVE_V3 = 0x01c0,

#define NDOF_MAX NDOF_BUTTON_SAVE_V3
#define NDOF_BUTTON_MAX NDOF_BUTTON_SAVE_V3

  // Internal engine events: 0x0100 – 0x011f (system/timer), 0x5000 – 0x507f (editor/UI)
  WINDEACTIVATE = 0x0104,

  // Timer slots
  TIMER = 0x0110,
  TIMER0 = 0x0111,
  TIMER1 = 0x0112,
  TIMER2 = 0x0113,
  TIMERJOBS = 0x0114,
  TIMERAUTOSAVE = 0x0115,
  TIMERREPORT = 0x0116,
  TIMERREGION = 0x0117,
  TIMERNOTIFIER = 0x0118,

#define TIMER_MAX 0x011f

  // Action-zones
  EVT_ACTIONZONE_AREA = 0x5000,
  EVT_ACTIONZONE_REGION = 0x5001,
  EVT_ACTIONZONE_REGION_QUAD = 0x5002,
  EVT_ACTIONZONE_FULLSCREEN = 0x5011,

  // Editor UI
  EVT_FILESELECT = 0x5020,
  EVT_BUT_OPEN = 0x5021,
  EVT_MODAL_MAP = 0x5022,
  EVT_DROP = 0x5023,
  EVT_BUT_CANCEL = 0x5024,
  EVT_GIZMO_UPDATE = 0x5025,
};

// Classification macros

#define ISTIMER(t) ((t) >= TIMER && (t) <= TIMER_MAX)

#define ISKEYBOARD(t) \
  (((t) >= EVT_KEYBOARD_MIN && (t) <= EVT_KEYBOARD_MAX) || \
   ((t) >= EVT_QT_SPECIAL_MIN && (t) <= EVT_QT_SPECIAL_MAX))

#define ISKEYMODIFIER(t) \
  ((t) == EVT_LEFTCTRLKEY || (t) == EVT_RIGHTCTRLKEY || (t) == EVT_LEFTALTKEY || \
   (t) == EVT_RIGHTALTKEY || (t) == EVT_LEFTSHIFTKEY || (t) == EVT_RIGHTSHIFTKEY || \
   (t) == EVT_OSKEY || (t) == EVT_HYPER)

#define ISKEYBOARD_OR_BUTTON(t) (ISMOUSE_BUTTON(t) || ISKEYBOARD(t) || ISNDOF_BUTTON(t))

#define ISMOUSE(t) ((t) >= EVT_MOUSE_MIN && (t) <= EVT_MOUSE_MAX)

#define ISMOUSE_MOTION(t) ((t) == MOUSEMOVE || (t) == INBETWEEN_MOUSEMOVE)

#define ISMOUSE_BUTTON(t) \
  ((t) == LEFTMOUSE || (t) == MIDDLEMOUSE || (t) == RIGHTMOUSE || (t) == BUTTON4MOUSE || \
   (t) == BUTTON5MOUSE || (t) == BUTTON6MOUSE || (t) == BUTTON7MOUSE)

#define ISMOUSE_WHEEL(t) \
  (((t) >= WHEELUPMOUSE && (t) <= WHEELOUTMOUSE) || (t) == WHEELLEFTMOUSE || \
   (t) == WHEELRIGHTMOUSE)

#define ISMOUSE_GESTURE(t) ((t) >= MOUSEPAN && (t) <= MOUSESMARTZOOM)

#define ISNDOF(t) ((t) >= NDOF_MIN && (t) <= NDOF_MAX)

#define ISNDOF_BUTTON(t) ((t) >= NDOF_BUTTON_MIN && (t) <= NDOF_BUTTON_MAX)

#define IS_EVENT_ACTIONZONE(t) \
  ((t) == EVT_ACTIONZONE_AREA || (t) == EVT_ACTIONZONE_REGION || \
   (t) == EVT_ACTIONZONE_REGION_QUAD || (t) == EVT_ACTIONZONE_FULLSCREEN)

#define ISHOTKEY(t) \
  ((ISKEYBOARD(t) || ISMOUSE_BUTTON(t) || ISMOUSE_WHEEL(t) || ISNDOF_BUTTON(t)) && \
   !ISKEYMODIFIER(t))

// Event-type bitmask for filter sets in key-maps
enum eEventType_Mask : uint32_t {
  EVT_TYPE_MASK_KEYBOARD_MODIFIER = (1u << 0),
  EVT_TYPE_MASK_KEYBOARD = (1u << 1),
  EVT_TYPE_MASK_MOUSE_WHEEL = (1u << 2),
  EVT_TYPE_MASK_MOUSE_GESTURE = (1u << 3),
  EVT_TYPE_MASK_MOUSE_BUTTON = (1u << 4),
  EVT_TYPE_MASK_MOUSE = (1u << 5),
  EVT_TYPE_MASK_NDOF = (1u << 6),
  EVT_TYPE_MASK_ACTIONZONE = (1u << 7),
};

#define EVT_TYPE_MASK_ALL \
  (EVT_TYPE_MASK_KEYBOARD | EVT_TYPE_MASK_MOUSE | EVT_TYPE_MASK_NDOF | EVT_TYPE_MASK_ACTIONZONE)
#define EVT_TYPE_MASK_HOTKEY_INCLUDE \
  (EVT_TYPE_MASK_KEYBOARD | EVT_TYPE_MASK_MOUSE | EVT_TYPE_MASK_NDOF)
#define EVT_TYPE_MASK_HOTKEY_EXCLUDE EVT_TYPE_MASK_KEYBOARD_MODIFIER

bool WM_event_type_mask_test(wmEventType event_type, eEventType_Mask mask);

// Qt 6 <-> wmEventType translation helpers
wmEventType WM_event_type_from_qt_key(Qt::Key qt_key, quint32 native_vk = 0);
wmEventType WM_event_type_from_qt_mouse_button(Qt::MouseButton button);
wmEventType WM_event_type_from_qt_wheel(int delta, bool horizontal);

// wmEvent::val values for EVT_FILESELECT
enum wmEventFileSelectValue : int {
  EVT_FILESELECT_FULL_OPEN = 1,
  EVT_FILESELECT_EXEC = 2,
  EVT_FILESELECT_CANCEL = 3,
  EVT_FILESELECT_EXTERNAL_CANCEL = 4,
};

// Gesture modal values stored in wmEvent::val — do not renumber
enum wmGestureModalValue : int {
  GESTURE_MODAL_CANCEL = 1,
  GESTURE_MODAL_CONFIRM = 2,
  GESTURE_MODAL_SELECT = 3,
  GESTURE_MODAL_DESELECT = 4,
  GESTURE_MODAL_NOP = 5,
  GESTURE_MODAL_CIRCLE_ADD = 6,
  GESTURE_MODAL_CIRCLE_SUB = 7,
  GESTURE_MODAL_BEGIN = 8,
  GESTURE_MODAL_IN = 9,
  GESTURE_MODAL_OUT = 10,
  GESTURE_MODAL_CIRCLE_SIZE = 11,
  GESTURE_MODAL_MOVE = 12,
  GESTURE_MODAL_SNAP = 13,
  GESTURE_MODAL_FLIP = 14,
};

}  // namespace vektor
