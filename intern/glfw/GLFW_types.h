#pragma once

#include <cstdint>

namespace vektor
{

class GLFW_ITimerTask;

enum GLFW_TWindowState
{
    GLFW_kWindowStateNormal     = 0,
    GLFW_kWindowStateMaximized  = 1,
    GLFW_kWindowStateMinimized  = 2,
    GLFW_kWindowStateFullScreen = 3,
};

enum GLFW_TVSyncModes
{
    /** Up to the GPU driver to choose. */
    GLFW_kVSyncModeUnset = -2,
    /** Adaptive sync (OpenGL only). */
    GLFW_kVSyncModeAuto  = -1,
    /** Disable, useful for unclasped redraws for testing performance. */
    GLFW_kVSyncModeOff   = 0,
    /** Force enable. */
    GLFW_kVSyncModeOn    = 1,
};

struct GLFW_ContextParams
{
    bool             is_stereo_visual;
    bool             is_debug;
    GLFW_TVSyncModes vsync;
};

using GLFW_TUserDataPtr  = void*;
using GLFW_TEventDataPtr = const void*;

class GLFW_TimerTask;
using GLFW_TimerProcPtr = void (*)(GLFW_ITimerTask* task,
                                   uint64_t         time);

enum GLFW_TFireTimeConstant
{
    GLFW_kFireTimeNever = 0xFFFFFFFF
};

enum GLFW_TDrawingContextType
{
    GLFW_kDrawingContextTypeNone = 0,
    GLFW_kDrawingContextTypeOpenGL,
};

struct GLFW_GPUDevice
{
    /** Index of the GPU device in the list provided by the platform. */
    int      index;
    /** (PCI) Vendor ID of the GPU. */
    uint32_t vendor_id;
    /** Device ID of the GPU provided by the vendor. */
    uint32_t device_id;
};

struct GLFW_GPUSettings
{
    int                      flags;

    GLFW_TVSyncModes         vsync;
    GLFW_TDrawingContextType context_type;
    GLFW_GPUDevice           preferred_device;
};

enum GLFW_TEventType
{
    GLFW_kEventUnknown = 0,

    GLFW_kEventCursorMove,
    GLFW_kEventButtonDown,
    GLFW_kEventButtonUp,

    GLFW_kEventWheel,

    GLFW_kEventTrackpad,

#ifdef WITH_INPUT_NDOF

    GLFW_kEventNDOFMotion,
    GLFW_kEventNDOFButton,
#endif

    GLFW_kEventKeyDown,
    GLFW_kEventKeyUp,

    GLFW_kEventQuitRequest,

    GLFW_kEventWindowClose,
    GLFW_kEventWindowActivate,
    GLFW_kEventWindowDeactivate,
    GLFW_kEventWindowUpdate,
    GLFW_kEventWindowUpdateDecor,
    GLFW_kEventWindowSize,
    GLFW_kEventWindowMove,
    GLFW_kEventWindowDPIHintChanged,

    GLFW_kEventDraggingEntered,
    GLFW_kEventDraggingUpdated,
    GLFW_kEventDraggingExited,
    GLFW_kEventDraggingDropDone,

    GLFW_kEventOpenMainFile,
    GLFW_kEventNativeResolutionChange,

    GLFW_kEventImeCompositionStart,
    GLFW_kEventImeComposition,
    GLFW_kEventImeCompositionEnd,

#define GLFW_kNumEventTypes (GLFW_kEventImeCompositionEnd + 1)
};

} // namespace vektor
