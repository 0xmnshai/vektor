
#pragma once

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>

#ifdef __cplusplus
extern "C"
{
#endif

    PyMODINIT_FUNC PyInit_vektor_ui(void);

#ifdef __cplusplus
}
#endif

namespace vektor
{
namespace py = pybind11;

void begin(py::module_& m);
void end(py::module_& m);

void button(py::module_& m);
void checkbox(py::module_& m);
void slider(py::module_& m);

void text(py::module_& m);

void same_line(py::module_& m);
void separator(py::module_& m);

void spacing(py::module_& m);

} // namespace vektor
