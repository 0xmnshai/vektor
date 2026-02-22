#include <Python.h>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>

#include "../dna/DNA_screen_types.h"

#include "./generic/py_capi_utls.h"

#include "bpy_rna.h"

#include "../../intern/clog/COG_log.hh"

#include "../vklib/VKE_assert.h"

#include "../python/bpy_extern.h"

namespace vektor
{

CLG_LOGREF_DECLARE_GLOBAL(VKE_LOG_CONTEXT,
                          "context");

// extern CLG_LogRef* VKE_LOG_CONTEXT;

void CTX_data_pointer_set_ptr(bContextDataResult* result,
                              const PointerRNA*   ptr)
{
    result->ptr = *ptr;
}

void CTX_data_type_set(bContextDataResult* result,
                       ContextDataType     type)
{
    result->type = type;
}

void CTX_data_list_add_ptr(bContextDataResult* result,
                           const PointerRNA*   ptr)
{
    result->list.push_back(*ptr);
}

bool CTX_member_logging_get(const vkContext* C)
{
    return (C->data.log_flag & CTX_LogFlag::Access) != CTX_LogFlag(0);
}

std::optional<std::string> BPY_python_current_file_and_line()
{
    /* Early return if Python is not initialized, usually during startup.
     * This function shouldn't operate if Python isn't initialized yet.
     *
     * In most cases this shouldn't be done, make an exception as it's needed for logging. */
    if (!Py_IsInitialized())
    {
        return std::nullopt;
    }

    PyGILState_STATE           gilstate;
    const bool                 use_gil = !PyC_IsInterpreterActive();
    std::optional<std::string> result  = std::nullopt;
    if (use_gil)
    {
        gilstate = PyGILState_Ensure();
    }

    const char* filename = nullptr;
    int         lineno   = -1;
    PyC_FileAndNum_Safe(&filename, &lineno);

    if (filename)
    {
        result = std::string(filename) + ":" + std::to_string(lineno);
    }

    if (use_gil)
    {
        PyGILState_Release(gilstate);
    }
    return result;
}

static void bpy_context_log_member_error(const vkContext* C,
                                         const char*      message)
{
    const bool use_logging_info   = CLOG_CHECK(VKE_LOG_CONTEXT, CLG_LEVEL_INFO);
    const bool use_logging_member = C && CTX_member_logging_get(C);
    if (!(use_logging_info || use_logging_member))
    {
        return;
    }

    std::optional<std::string> python_location = BPY_python_current_file_and_line();
    const char*                location        = python_location ? python_location->c_str() : "unknown:0";

    if (use_logging_info)
    {
        CLOG_INFO(VKE_LOG_CONTEXT, "%s: %s", location, message);
    }
    else if (use_logging_member)
    {
        CLOG_AT_LEVEL_NOCHECK(VKE_LOG_CONTEXT, CLG_LEVEL_INFO, "%s: %s", location, message);
    }
    else
    {
        BLI_assert_unreachable();
    }
}

bool BPY_context_member_get(vkContext*          C,
                            const char*         member,
                            bContextDataResult* result)
{
    PyGILState_STATE gilstate;
    const bool       use_gil = !PyC_IsInterpreterActive();
    if (use_gil)
    {
        gilstate = PyGILState_Ensure();
    }

    PyObject*   pyctx;
    PyObject*   item;
    PointerRNA* ptr  = nullptr;
    bool        done = false;

    pyctx            = static_cast<PyObject*>(CTX_py_dict_get(C));
    item             = PyDict_GetItemString(pyctx, member);

    if (item == nullptr)
    {
        /* Pass. */
    }
    else if (item == Py_None)
    {
        done = true;
    }
    else if (BPy_StructRNA_Check(item))
    {
        ptr = &reinterpret_cast<BPy_StructRNA*>(item)->ptr.value();

        // result->ptr = ((BPy_StructRNA *)item)->ptr;
        CTX_data_pointer_set_ptr(result, ptr);
        CTX_data_type_set(result, ContextDataType::Pointer);
        done = true;
    }
    else if (PySequence_Check(item))
    {
        PyObject* seq_fast = PySequence_Fast(item, "bpy_context_get sequence conversion");
        if (seq_fast == nullptr)
        {
            PyErr_Print();
        }
        else
        {
            const int  len            = PySequence_Fast_GET_SIZE(seq_fast);
            PyObject** seq_fast_items = PySequence_Fast_ITEMS(seq_fast);
            int        i;

            for (i = 0; i < len; i++)
            {
                PyObject* list_item = seq_fast_items[i];

                if (BPy_StructRNA_Check(list_item))
                {
                    ptr = &reinterpret_cast<BPy_StructRNA*>(list_item)->ptr.value();
                    CTX_data_list_add_ptr(result, ptr);
                }
                else
                {
                    /* Log invalid list item type */
                    std::string message = std::string("'") + member +
                                          "' list item not a valid type in sequence type '" +
                                          Py_TYPE(list_item)->tp_name + "'";
                    bpy_context_log_member_error(C, message.c_str());
                }
            }
            Py_DECREF(seq_fast);
            CTX_data_type_set(result, ContextDataType::Collection);
            done = true;
        }
    }

    if (done == false)
    {
        if (item)
        {
            /* Log invalid member type */
            std::string message = std::string("'") + member + "' not a valid type";
            bpy_context_log_member_error(C, message.c_str());
        }
    }

    if (use_gil)
    {
        PyGILState_Release(gilstate);
    }

    return done;
}
} // namespace vektor