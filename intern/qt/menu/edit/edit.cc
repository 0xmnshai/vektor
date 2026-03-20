#include <QCoreApplication>
#include <QMenu>
#include <QMenuBar>

#include "../../../vpi/intern/VPI_QtWindow.hh"
#include "../../source/runtime/dna/DNA_object_type.h"
#include "../../source/runtime/kernel/ecs/ECS_registry.h"

#include "edit.hh"

namespace qt::menu {

void MENU_edit_register(void *window)
{
  auto *vpi_window = static_cast<vpi::VPI_QtWindow *>(window);
  QMenuBar *menu_bar = vpi_window->menuBar();

  QMenu *menu = menu_bar->addMenu("Edit");

  QMenu *add_3d_menu = menu->addMenu("3D Objects");

  QAction *add_cube = add_3d_menu->addAction("Cube");
  QObject::connect(add_cube, &QAction::triggered, []() {
    vektor::kernel::create_entity(nullptr,
                                  nullptr,
                                  "Cube",
                                  "Contruct a Cube with Default Materials",
                                  (int)vektor::dna::ObjectType::Mesh,
                                  0.0f,
                                  0.0f,
                                  0.0f,
                                  0.26f,
                                  0.27f,
                                  0.29f);
  });

  QAction *add_cylinder = add_3d_menu->addAction("Cylinder");
  QObject::connect(add_cylinder, &QAction::triggered, []() {
    vektor::kernel::create_entity(nullptr,
                                  nullptr,
                                  "Cylinder",
                                  "Contruct a Cyliner with Default Materials",
                                  (int)vektor::dna::ObjectType::Mesh,
                                  0.0f,
                                  0.0f,
                                  0.0f,
                                  0.26f,
                                  0.27f,
                                  0.29f);
  });

  QAction *add_plane = add_3d_menu->addAction("Plane");
  QObject::connect(add_plane, &QAction::triggered, []() {
    vektor::kernel::create_entity(nullptr,
                                  nullptr,
                                  "Plane",
                                  "Contruct a Plane with Default Materials",
                                  (int)vektor::dna::ObjectType::Mesh,
                                  0.0f,
                                  0.0f,
                                  0.0f,
                                  0.26f,
                                  0.27f,
                                  0.29f);
  });

  menu->addSeparator();

  QMenu *add_light_menu = menu->addMenu("Add Light");

  auto create_light = [](const char *name, int type) {
    vektor::kernel::create_entity(nullptr,
                                  nullptr,
                                  name,
                                  "Light Source",
                                  (int)vektor::dna::ObjectType::Light,
                                  0.0f, 5.0f, 0.0f,
                                  1.0f, 1.0f, 1.0f);
  };

  QAction *add_point = add_light_menu->addAction("Point Light");
  QObject::connect(add_point, &QAction::triggered, [create_light]() { create_light("Point Light", 0); });
}

}  // namespace qt::menu
