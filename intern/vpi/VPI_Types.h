#pragma once

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
