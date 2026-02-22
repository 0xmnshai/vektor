#pragma once

#include <pthread.h>
#include <string>

#include "../dna/DNA_windowmanager_types.h"
#include "../rna/RNA_internal_types.h"
#include "../rna/RNA_types.h"
#include "../windowmanager/wm_types.h"
#include "VKE_main.hh"
#include "VKE_string_ref.hh"

#include "../rna/rna_aceess.h"
#include "../windowmanager/wm_api.h"

namespace vektor
{

using vklib::StringRef;
using vklib::StringRefNull;

enum class ContextDataType : uint8_t
{
    Pointer = 0,
    Collection,
    Property,
    String,
    Int64,
};

struct bContextDataResult
{
    PointerRNA              ptr;
    std::vector<PointerRNA> list;
    PropertyRNA*            prop;
    int                     index;
    StringRefNull           str;
    std::optional<int64_t>  int_value;
    const char**            dir;
    ContextDataType         type;
};

using bContextDataCallback = int /*eContextResult*/ (*)(const vkContext*    C,
                                                        const char*         member,
                                                        bContextDataResult* result);

/* Result of context lookups.
 * The specific values are important, and used implicitly in ctx_data_get(). Some functions also
 * still accept/return `int` instead, to ensure that the compiler uses the correct storage size
 * when mixing C/C++ code. */
enum eContextResult
{
    /* The context member was found, and its data is available. */
    CTX_RESULT_OK               = 1,

    /* The context member was not found. */
    CTX_RESULT_MEMBER_NOT_FOUND = 0,

    /* The context member was found, but its data is not available.
     * For example, "active_bone" is a valid context member, but has not data in Object mode. */
    CTX_RESULT_NO_DATA          = -1,
};

enum class CTX_LogFlag : uint8_t
{
    /** Enable logging of context member access. */
    Access      = (1 << 0),
    /** Hide missing/None values from logging. */
    HideMissing = (1 << 1),
};
ENUM_OPERATORS(CTX_LogFlag);

wmWindowManager* CTX_wm_manager(const vkContext* C);

wmWindow*        CTX_wm_window(const vkContext* C);

void             CTX_wm_manager_set(vkContext*       C,
                                    wmWindowManager* wm);

Main*            CTX_data_main(const vkContext* C);

void*            CTX_py_dict_get(const vkContext* C);

void             CTX_data_pointer_set_ptr(bContextDataResult* result,
                                          const PointerRNA*   ptr);

void             CTX_data_type_set(bContextDataResult* result,
                                   ContextDataType     type);

void             CTX_data_list_add_ptr(bContextDataResult* result,
                                       const PointerRNA*   ptr);

bool             CTX_member_logging_get(const vkContext* C);

struct vkContext;

struct bContextPollMsgDyn_Params
{
    /** The result is allocated. */
    char* (*get_fn)(vkContext* C,
                    void*      user_data);
    /** Optionally free the user-data. */
    void (*free_fn)(vkContext* C,
                    void*      user_data);
    void* user_data;
};

struct bContextStoreEntry
{
    std::string                                    name;
    std::variant<PointerRNA, std::string, int64_t> value;
};

struct bContextStore
{
    std::vector<bContextStoreEntry> entries;
    bool                            used = false;
};

const PointerRNA* CTX_store_ptr_lookup(const bContextStore* store,
                                       StringRef            name,
                                       const StructRNA*     type = nullptr);

const PointerRNA* CTX_store_ptr_lookup(const bContextStore* store,
                                       const StringRef      name,
                                       const StructRNA*     type)
{
    for (auto entry = store->entries.rbegin(); entry != store->entries.rend(); ++entry)
    {
        if (entry->name == name && std::holds_alternative<PointerRNA>(entry->value))
        {
            const PointerRNA& ptr = std::get<PointerRNA>(entry->value);
            if (!type || RNA_struct_is_a(ptr.type, type))
            {
                return &ptr;
            }
        }
    }
    return nullptr;
}

template <typename T>
const T* ctx_store_lookup_impl(const bContextStore* store,
                               const StringRef      name)
{
    for (auto entry = store->entries.rbegin(); entry != store->entries.rend(); ++entry)
    {
        if (entry->name == name && std::holds_alternative<T>(entry->value))
        {
            return &std::get<T>(entry->value);
        }
    }
    return nullptr;
}

std::optional<StringRefNull> CTX_store_string_lookup(const bContextStore* store,
                                                     const StringRef      name)
{
    if (const std::string* value = ctx_store_lookup_impl<std::string>(store, name))
    {
        return *value;
    }
    return {};
}

std::optional<int64_t> CTX_store_int_lookup(const bContextStore* store,
                                            const StringRef      name)
{
    if (const int64_t* value = ctx_store_lookup_impl<int64_t>(store, name))
    {
        return *value;
    }
    return {};
}

struct vkContext
{
    int thread;

    /* windowmanager context */
    struct
    {
        wmWindowManager*          manager;
        wmWindow*                 window;
        // WorkSpace*                workspace;
        bScreen*                  screen;
        ScrArea*                  area;
        ARegion*                  region;
        ARegion*                  region_popup;
        // wmGizmoGroup*             gizmo_group;
        const bContextStore*      store;

        /* Operator poll. */
        /**
         * Store the reason the poll function fails (static string, not allocated).
         * For more advanced formatting use `operator_poll_msg_dyn_params`.
         */
        const char*               operator_poll_msg;
        /**
         * Store values to dynamically to create the string (called when a tool-tip is shown).
         */
        bContextPollMsgDyn_Params operator_poll_msg_dyn_params;
    } wm;

    /* data context */
    struct
    {
        Main*       main;
        Scene*      scene;

        int         recursion;
        /** True if python is initialized. */
        bool        py_init;
        void*       py_context;
        /**
         * If we need to remove members, do so in a copy
         * (keep this to check if the copy needs freeing).
         */
        void*       py_context_orig;
        /** Logging control flags (access, hide_missing). */
        CTX_LogFlag log_flag;
        /** Optional flag to disallow writing via RNA. */
        const bool* rna_disallow_writes;
    } data;
};

wmWindowManager* CTX_wm_manager(const vkContext* C);
wmWindow*        CTX_wm_window(const vkContext* C);
ARegion*         CTX_wm_region(const vkContext* C);
bScreen*         CTX_wm_screen(const vkContext* C);
ScrArea*         CTX_wm_area(const vkContext* C);

inline void      CTX_wm_area_set(vkContext* C,
                                 ScrArea*   area)
{
    C->wm.area = area;
}
inline void CTX_wm_window_set(vkContext* C,
                              wmWindow*  window)
{
    C->wm.window = window;
}
inline void CTX_wm_region_set(vkContext* C,
                              ARegion*   region)
{
    C->wm.region = region;
}
inline void CTX_wm_region_popup_set(vkContext* C,
                                    ARegion*   region)
{
    C->wm.region_popup = region;
}
inline ARegion* CTX_wm_region_popup(const vkContext* C)
{
    return C->wm.region_popup;
}

ARegion* CTX_wm_region(const vkContext* C)
{
    StructRNA* RNA_Region = nullptr;
    return static_cast<ARegion*>(ctx_wm_python_context_get(C, "region", RNA_Region, C->wm.region));
}

vkContext* CTX_create()
{
    vkContext* C = MEM_new_zeroed<vkContext>(__func__);
    return C;
}

vkContext* CTX_copy(const vkContext* C)
{
    vkContext* newC = MEM_new_zeroed<vkContext>(__func__);
    *newC           = *C;

    memset(&newC->wm.operator_poll_msg_dyn_params, 0, sizeof(newC->wm.operator_poll_msg_dyn_params));

    return newC;
}

// decleare
void CTX_wm_operator_poll_msg_clear(vkContext* C);

// define
void CTX_wm_operator_poll_msg_clear(vkContext* C)
{
    bContextPollMsgDyn_Params* params = &C->wm.operator_poll_msg_dyn_params;
    if (params->free_fn != nullptr)
    {
        params->free_fn(C, params->user_data);
    }
    params->get_fn          = nullptr;
    params->free_fn         = nullptr;
    params->user_data       = nullptr;

    C->wm.operator_poll_msg = nullptr;
}

void CTX_free(vkContext* C)
{
    /* This may contain a dynamically allocated message, free. */
    CTX_wm_operator_poll_msg_clear(C);

    MEM_delete(C);
}

// define & declaring here for now
bScreen* CTX_wm_screen(const vkContext* C);

bScreen* CTX_wm_screen(const vkContext* C)
{
    StructRNA* screen_rna = {}; // RNA_Screen; -> i need to implmenent this.

    return static_cast<bScreen*>(ctx_wm_python_context_get(C, "screen", screen_rna, C->wm.screen));
}

static pthread_t mainid;

int              BLI_thread_is_main()
{
    return pthread_equal(pthread_self(), mainid);
}

/** Simple logging for context data results. */
static void ctx_member_log_access(const vkContext*          C,
                                  const char*               member,
                                  const bContextDataResult& result,
                                  const eContextResult      lookup_result)
{
    const bool use_logging = true; // CLOG_CHECK(VKE_LOG_CONTEXT, CLG_LEVEL_TRACE) || CTX_member_logging_get(C);

    if (!use_logging)
    {
        return;
    }

    /* If hiding missing is enabled and the member was not found, skip logging. */
    if (C && bool(C->data.log_flag & CTX_LogFlag::HideMissing))
    {
        if (lookup_result == CTX_RESULT_MEMBER_NOT_FOUND)
        {
            return;
        }
    }

    std::string member_name = member;
    std::string ctx_result_brief_repr(const bContextDataResult& result); // will implemnet later from context.cc

    std::string value_repr = ctx_result_brief_repr(result);
    const char* value_desc = value_repr.c_str();

#ifdef WITH_PYTHON
    /* Get current Python location if available and Python is properly initialized. */
    std::optional<std::string> python_location;
    if (CTX_py_init_get(C))
    {
        python_location = BPY_python_current_file_and_line();
    }
    const char* location = python_location ? python_location->c_str() : "unknown:0";
#else
    const char* location = "unknown:0";
#endif

    const char* VKE_LOG_CONTEXT = "VKE_LOG_CONTEXT";
    const char* CLG_LEVEL_TRACE = "CLG_LEVEL_TRACE";

    /* Use TRACE level when available, otherwise force output when Python logging is enabled. */
    const char* format          = "%s: %s=%s";
    if (true) // CLOG_CHECK(VKE_LOG_CONTEXT, CLG_LEVEL_TRACE))
    {
        // CLOG_TRACE(VKE_LOG_CONTEXT, format, location, member, value_desc);
    }
    else if (true) // CTX_member_logging_get(C))
    {
        /* Force output at TRACE level even if not enabled via command line. */
        // CLOG_AT_LEVEL_NOCHECK(VKE_LOG_CONTEXT, CLG_LEVEL_TRACE, format, location, member, value_desc);
    }
}

static eContextResult ctx_data_get(vkContext*          C,
                                   const char*         member,
                                   bContextDataResult* result)
{
    bScreen* screen;
    ScrArea* area;
    ARegion* region;
    int      done = 0, recursion = C->data.recursion;
    int      ret = 0;

    result       = {};

    /* Don't allow UI context access from non-main threads. */
    if (!BLI_thread_is_main())
    {
        return CTX_RESULT_MEMBER_NOT_FOUND;
    }

    /* we check recursion to ensure that we do not get infinite
     * loops requesting data from ourselves in a context callback */

    /* Ok, this looks evil...
     * if (ret) done = -(-ret | -done);
     *
     * Values in order of importance
     * (0, -1, 1) - Where 1 is highest priority
     */
    if (done != 1 && recursion < 1 && C->wm.store)
    {
        C->data.recursion = 1;

        if (const PointerRNA* ptr = CTX_store_ptr_lookup(C->wm.store, member, nullptr))
        {
            result->ptr = *ptr;
            done        = 1;
        }
        else if (std::optional<StringRefNull> str = CTX_store_string_lookup(C->wm.store, member))
        {
            result->str  = *str;
            result->type = ContextDataType::String;
            done         = 1;
        }
        else if (std::optional<int64_t> int_value = CTX_store_int_lookup(C->wm.store, member))
        {
            result->int_value = int_value;
            result->type      = ContextDataType::Int64;
            done              = 1;
        }
    }
    if (done != 1 && recursion < 2 && (region = CTX_wm_region(C)))
    {
        C->data.recursion = 2;
        if (region->runtime->type && region->runtime->type->context)
        {
            ret = region->runtime->type->context(C, member, result);
            if (ret)
            {
                done = -(-ret | -done);
            }
        }
    }
    if (done != 1 && recursion < 3 && (area = CTX_wm_area(C)))
    {
        C->data.recursion = 3;
        if (area->type && area->type->context)
        {
            ret = area->type->context(C, member, result);
            if (ret)
            {
                done = -(-ret | -done);
            }
        }
    }

    if (done != 1 && recursion < 4 && (screen = CTX_wm_screen(C)))
    {
        bContextDataCallback cb = reinterpret_cast<bContextDataCallback>(screen->context);
        C->data.recursion       = 4;
        if (cb)
        {
            ret = cb(C, member, result);
            if (ret)
            {
                done = -(-ret | -done);
            }
        }
    }

    C->data.recursion           = recursion;

    eContextResult final_result = eContextResult(done);

    /* Log context result if we're in a temp_override and we got a successful or no-data result. */
    if (ELEM(final_result, CTX_RESULT_OK, CTX_RESULT_NO_DATA))
    {
        ctx_member_log_access(C, member, *result, final_result);
    }

    return final_result;
}

bool ctx_data_pointer_verify(const vkContext* C,
                             const char*      member,
                             void**           pointer)
{
    bContextDataResult result;
    if (ctx_data_get(const_cast<vkContext*>(C), member, &result) == CTX_RESULT_OK)
    {
        // BLI_assert(result.type == ContextDataType::Pointer);
        *pointer = result.ptr.data;
        return true;
    }

    *pointer = nullptr;
    return false;
}

// define & declaring here for now
Scene* CTX_data_scene(const vkContext* C);

Scene* CTX_data_scene(const vkContext* C)
{
    Scene* scene;
    if (ctx_data_pointer_verify(C, "scene", reinterpret_cast<void**>(&scene)))
    {
        return scene;
    }

    return C->data.scene;
}

} // namespace vektor