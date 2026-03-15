#include <QAbstractItemView>
#include <QHeaderView>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTimer>
#include <QTreeView>
#include <QVBoxLayout>
#include <iostream>

#include "../WIDGET_outliner.h"

#include "../../../../../source/runtime/dna/DNA_object_type.h"
#include "../../../../../source/runtime/rna/RNA_ecs_registry.h"
#include "../../../../source/runtime/kernel/ecs/ECS_registry.h"

#include "RNA_define.h"

namespace qt::dock {
using namespace vektor::dna;

OutlinerWidget::OutlinerWidget(QWidget *parent) : QWidget(parent)
{
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  search_bar_ = new QLineEdit();
  // search_bar_->setPlaceholderText("Search ...");
  // search_bar_->setStyleSheet(
  //     "QLineEdit { background-color: #2a2a2a; border: 1px solid #3a3a3a; "
  //     "border-radius: 0px; padding: 4px; color: #ccc; margin: 4px; }");

  // layout->addWidget(search_bar_);

  tree_view_ = new QTreeView(this);
  model_ = new QStandardItemModel(this);
  model_->setHorizontalHeaderLabels({"Label", "Type", ""});

  proxy_model_ = new QSortFilterProxyModel(this);
  proxy_model_->setSourceModel(model_);
  proxy_model_->setFilterCaseSensitivity(Qt::CaseInsensitive);
  proxy_model_->setFilterKeyColumn(0);  // Filter by Label

  tree_view_->setModel(proxy_model_);
  tree_view_->setAlternatingRowColors(true);
  tree_view_->setColumnWidth(0, 200);
  tree_view_->setColumnWidth(1, 60);
  tree_view_->setColumnWidth(2, 30);
  tree_view_->setUniformRowHeights(true);
  tree_view_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tree_view_->setSelectionMode(QAbstractItemView::ExtendedSelection);

  tree_view_->setStyleSheet(
      "QTreeView { background-color: #212121; alternate-background-color: "
      "#2b2b2b; "
      "border: none; color: #bbb; outline: none; }"
      "QTreeView::item:selected { background-color: #3e5f8a; color: white; }"
      "QHeaderView::section { background-color: #333; color: #888; border: "
      "1px solid #222; padding: 4px; }");

  layout->addWidget(tree_view_);

  timer_ = new QTimer(this);
  connect(timer_, &QTimer::timeout, this, &OutlinerWidget::refresh_entities);
  timer_->start(1000);

  filter_timer_ = new QTimer(this);
  filter_timer_->setSingleShot(true);

  connect(filter_timer_, &QTimer::timeout, this, &OutlinerWidget::apply_filter);
  connect(search_bar_, &QLineEdit::textChanged, this, &OutlinerWidget::on_search_text_changed);

  connect(tree_view_, &QTreeView::clicked, this, &OutlinerWidget::on_item_clicked);
}

void OutlinerWidget::on_item_clicked(const QModelIndex &index)
{
  QModelIndex source_index = proxy_model_->mapToSource(index);
  QStandardItem *item = model_->itemFromIndex(source_index);

  if (item && item->data(Qt::UserRole).isValid()) {
    auto entity = item->data(Qt::UserRole).toUInt();

    vektor::rna::RNA_ecs_set_selected(
        &vektor::kernel::ECSRegistry::instance(), (entt::entity)entity, true);
    vektor::rna::RNA_ecs_set_active(
        &vektor::kernel::ECSRegistry::instance(), (entt::entity)entity, true);
  }
}

void OutlinerWidget::on_search_text_changed(const QString &)
{
  filter_timer_->start(200);  // 200ms debounce
}

void OutlinerWidget::apply_filter()
{
  proxy_model_->setFilterFixedString(search_bar_->text());
}

void OutlinerWidget::refresh_entities()
{
  model_->removeRows(0, model_->rowCount());

  auto *root_item = new QStandardItem("Scene Collection");
  model_->appendRow({root_item, new QStandardItem(""), new QStandardItem("")});

  auto *collection_item = new QStandardItem("Collection");
  root_item->appendRow({collection_item, new QStandardItem(""), new QStandardItem("")});

  auto &registry = vektor::kernel::ECSRegistry::instance().registry();
  auto objects_view = registry.view<vektor::dna::Object>();

  for (auto entity : objects_view) {
    auto &obj = objects_view.get<vektor::dna::Object>(entity);
    QString name = QString::fromUtf8(obj.id_name);

    auto *name_item = new QStandardItem(name);
    auto *type_item = new QStandardItem(
        QString::fromUtf8(vektor::rna::rna_enum_object_type_to_string(obj.object_type)));
    auto *extra_item = new QStandardItem("");

    name_item->setData((unsigned int)entity, Qt::UserRole);
    type_item->setData((unsigned int)entity, Qt::UserRole);
    extra_item->setData((unsigned int)entity, Qt::UserRole);

    collection_item->appendRow({name_item, type_item, extra_item});
  }

  tree_view_->expandAll();
}

}  // namespace qt::dock
