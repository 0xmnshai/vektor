
#include <imgui.h>

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>

#include "PY_imgui.hh"

namespace vektor
{

void begin(py::module_& m)
{
    // imgui begin
    m.def(
        "begin", [](const char* name, bool open, int flags) { return ImGui::Begin(name, &open, flags); },
        "Begin a window", py::arg("name"), py::arg("open") = true, py::arg("flags") = 0);
}

void end(py::module_& m)
{
    // imgui end
    m.def("end", []() { return ImGui::End(); }, "End a window");
}

void button(py::module_& m)
{
    // imgui button
    m.def("button", [](const char* label) { return ImGui::Button(label); }, "Button", py::arg("label"));
}

void checkbox(py::module_& m)
{
    // imgui checkbox
    m.def(
        "checkbox", [](const char* label, bool* v) { return ImGui::Checkbox(label, v); }, "Checkbox", py::arg("label"),
        py::arg("v"));
}

void slider(py::module_& m)
{
    // imgui slider
    m.def(
        "slider", [](const char* label, float* v, float v_min, float v_max)
        { return ImGui::SliderFloat(label, v, v_min, v_max); }, "Slider", py::arg("label"), py::arg("v"),
        py::arg("v_min"), py::arg("v_max"));
}

void text(py::module_& m)
{
    // imgui text
    m.def("text", [](const char* text) { return ImGui::Text("%s", text); }, "Text", py::arg("text"));
}

void same_line(py::module_& m)
{
    // imgui same line
    m.def("same_line", []() { return ImGui::SameLine(); }, "Same line");
}

void separator(py::module_& m)
{
    // imgui separator
    m.def("separator", []() { return ImGui::Separator(); }, "Separator");
}

void spacing(py::module_& m)
{
    // imgui spacing
    m.def("spacing", []() { return ImGui::Spacing(); }, "Spacing");
}

PYBIND11_EMBEDDED_MODULE(imgui,
                         m)
{
    m.doc() = "ImGui Python bindings";

    begin(m);
    end(m);

    button(m);
    checkbox(m);
    slider(m);
    text(m);
    same_line(m);
    separator(m);
    spacing(m);
}
} // namespace vektor