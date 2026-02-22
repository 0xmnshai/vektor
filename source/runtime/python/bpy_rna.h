#pragma once

#include <Python.h>
#include <pybind11/pybind11.h>

#include "../rna/RNA_types.h"

namespace vektor
{

extern PyTypeObject pyrna_struct_Type;
extern PyTypeObject pyrna_prop_Type;

#define BPy_StructRNA_Check(v) (PyObject_TypeCheck(v, &pyrna_struct_Type))
#define BPy_StructRNA_CheckExact(v) (Py_TYPE(v) == &pyrna_struct_Type)
#define BPy_PropertyRNA_Check(v) (PyObject_TypeCheck(v, &pyrna_prop_Type))
#define BPy_PropertyRNA_CheckExact(v) (Py_TYPE(v) == &pyrna_prop_Type)

struct BPy_StructRNA
{
    PyObject_HEAD /* Required Python macro. */

            std::optional<PointerRNA>
            ptr;
    bool    freeptr;
};

} // namespace vektor