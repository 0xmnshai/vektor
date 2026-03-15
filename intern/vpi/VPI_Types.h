#pragma once

#include <cstdint>
enum VPI_TSuccess { VPI_kFailure = 0, VPI_kSuccess };

enum VPI_EventType {
  VPI_kEventUnknown = 0,
  VPI_kCursorMove,
  VPI_kMouseButton,
  VPI_kMouseWheel,
  VPI_kKey,
  VPI_kChar,
  VPI_kEventCount,
  VPI_kTrackpad,

  VPI_kWindowClose,
  VPI_kWindowResize,
  VPI_kWindowMove,
};

enum VPI_TWindowState {
  VPI_kWindowStateNormal = 0,
  VPI_kWindowStateMaximized = 1,
  VPI_kWindowStateMinimized = 2,
  VPI_kWindowStateFullScreen = 3,
};

enum VPI_TVisibility { VPI_kNotVisible = 0, VPI_kPartiallyVisible, VPI_kFullyVisible };

typedef enum { VPI_kAxisNone = 0, VPI_kAxisX = (1 << 0), VPI_kAxisY = (1 << 1) } VPI_TAxisFlag;

enum VPI_TVSyncModes {
  /** Up to the GPU driver to choose. */
  VPI_kVSyncModeUnset = -2,
  /** Adaptive sync (OpenGL only). */
  VPI_kVSyncModeAuto = -1,
  /** Disable, useful for unclasped redraws for testing performance. */
  VPI_kVSyncModeOff = 0,
  /** Force enable. */
  VPI_kVSyncModeOn = 1,
};

struct VPI_ContextParams {
  bool is_stereo_visual;
  bool is_debug;
  VPI_TVSyncModes vsync;
};

struct VPI_TEventCursorData {
  int32_t x;
  int32_t y;
  VPI_EventType type;
};

struct VPI_TEventMouseButtonData {
  int32_t x;
  int32_t y;
  uint32_t button;
  uint32_t modifiers;
  VPI_EventType type;
};

struct VPI_TEventMouseWheelData {
  int32_t x;
  int32_t y;
  int32_t delta_x;
  int32_t delta_y;
  uint32_t modifiers;
};

struct VPI_TEventKeyData {
  uint32_t key;
  uint32_t modifiers;
  bool is_repeat;
};

struct VPI_TEventCharData {
  uint32_t character;
  uint32_t modifiers;
};
