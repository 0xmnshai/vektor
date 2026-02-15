
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
        "begin",
        [](const char* name, py::object open, int flags)
        {
            if (open.is_none())
            {
                return ImGui::Begin(name, NULL, flags);
            }
            bool is_open  = open.cast<bool>();
            bool expanded = ImGui::Begin(name, &is_open, flags);
            return expanded;
        },
        "Begin a window", py::arg("name"), py::arg("open") = py::none(), py::arg("flags") = 0);
}

void end(py::module_& m)
{
    // imgui end
    m.def("end", []() { ImGui::End(); }, "End a window");
}

void button(py::module_& m)
{
    // imgui button
    m.def(
        "button", [](const char* label, float w, float h) { return ImGui::Button(label, ImVec2(w, h)); }, "Button",
        py::arg("label"), py::arg("width") = 0.0f, py::arg("height") = 0.0f);
}

void checkbox(py::module_& m)
{
    // imgui checkbox
    m.def(
        "checkbox",
        [](const char* label, bool v)
        {
            ImGui::Checkbox(label, &v);
            return v;
        },
        "Checkbox", py::arg("label"), py::arg("v"));
}

void slider(py::module_& m)
{
    // imgui slider
    m.def(
        "slider_float",
        [](const char* label, float v, float v_min, float v_max)
        {
            ImGui::SliderFloat(label, &v, v_min, v_max);
            return v;
        },
        "Slider Float", py::arg("label"), py::arg("v"), py::arg("v_min"), py::arg("v_max"));
}

void text(py::module_& m)
{
    // imgui text
    m.def("text", [](const char* text) { ImGui::Text("%s", text); }, "Text", py::arg("text"));
}

void same_line(py::module_& m)
{
    // imgui same line
    m.def("same_line", []() { ImGui::SameLine(); }, "Same line");
}

void separator(py::module_& m)
{
    // imgui separator
    m.def("separator", []() { ImGui::Separator(); }, "Separator");
}

void spacing(py::module_& m)
{
    // imgui spacing
    m.def("spacing", []() { ImGui::Spacing(); }, "Spacing");
}

PYBIND11_EMBEDDED_MODULE(vektor_ui,
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