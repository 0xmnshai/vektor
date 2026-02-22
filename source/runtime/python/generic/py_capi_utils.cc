#include <Python.h>
#include "py_capi_utls.h"

namespace vektor
{
bool PyC_IsInterpreterActive()
{
    /* instead of PyThreadState_Get, which calls Py_FatalError */
    return (PyThreadState_GetDict() != nullptr);
}

void PyC_FileAndNum(const char **r_filename, int *r_lineno)
{
  PyFrameObject *frame;
  PyCodeObject *code;

  if (r_filename) {
    *r_filename = nullptr;
  }
  if (r_lineno) {
    *r_lineno = -1;
  }

  if (!(frame = PyEval_GetFrame())) {
    return;
  }
  if (!(code = PyFrame_GetCode(frame))) {
    return;
  }

  /* when executing a script */
  if (r_filename) {
    *r_filename = PyUnicode_AsUTF8(code->co_filename);
  }

  /* when executing a module */
  if (r_filename && *r_filename == nullptr) {
    /* try an alternative method to get the r_filename - module based
     * references below are all borrowed (double checked) */
    PyObject *mod_name = PyDict_GetItemString(PyEval_GetGlobals(), "__name__");
    if (mod_name) {
      PyObject *mod = PyDict_GetItem(PyImport_GetModuleDict(), mod_name);
      if (mod) {
        PyObject *mod_file = PyModule_GetFilenameObject(mod);
        if (mod_file) {
          *r_filename = PyUnicode_AsUTF8(mod_name);
          Py_DECREF(mod_file);
        }
        else {
          PyErr_Clear();
        }
      }

      /* unlikely, fallback */
      if (*r_filename == nullptr) {
        *r_filename = PyUnicode_AsUTF8(mod_name);
      }
    }
  }

  if (r_lineno) {
    *r_lineno = PyFrame_GetLineNumber(frame);
  }

  Py_DECREF(code);
}


void PyC_FileAndNum_Safe(const char** r_filename,
                         int*         r_lineno)
{
    if (!PyC_IsInterpreterActive())
    {
        return;
    }

    PyC_FileAndNum(r_filename, r_lineno);
}
} // namespace vektor