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
                                  (int)vektor::dna::ObjectType::Mesh,
                                  (int)vektor::dna::ObjectType::Mesh,
                                  0.0f,
                                  0.0f,
                                  0.0f,
                                  0.2f,
                                  0.2f,
                                  0.22f);
  });

  QAction *add_cylinder = add_3d_menu->addAction("Cylinder");
  QObject::connect(add_cylinder, &QAction::triggered, []() {
    vektor::kernel::create_entity(nullptr,
                                  nullptr,
                                  "Cylinder",
                                  (int)vektor::dna::ObjectType::Mesh,
                                  (int)vektor::dna::ObjectType::Mesh,
                                  0.0f,
                                  0.0f,
                                  0.0f,
                                  0.2f,
                                  0.2f,
                                  0.22f);
  });

  QAction *add_plane = add_3d_menu->addAction("Plane");
  QObject::connect(add_plane, &QAction::triggered, []() {
    vektor::kernel::create_entity(nullptr,
                                  nullptr,
                                  "Plane",
                                  (int)vektor::dna::ObjectType::Mesh,
                                  (int)vektor::dna::ObjectType::Mesh,
                                  0.0f,
                                  0.0f,
                                  0.0f,
                                  0.2f,
                                  0.2f,
                                  0.22f);
  });
}

}  // namespace qt::menu
