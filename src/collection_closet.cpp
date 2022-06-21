#include "collection_closet.h"
#include "ui_collection_closet.h"

#include "object_widget.h"
#include "operation_widget.h"

#include <QtCore/QSettings>

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlTableModel>

#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QListWidgetItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QSizePolicy>

#include <memory>

CollectionCloset::CollectionCloset(QWidget* parent, const QSqlDatabase& database) :
	QMainWindow(parent),
	ui_(new Ui::CollectionCloset),
	database_(database) {
	ui_->setupUi(this);

	connect(ui_->a_change_user, &QAction::triggered, this, &CollectionCloset::changeUser);
	connect(ui_->a_exit, &QAction::triggered, this, &CollectionCloset::exit);
	connect(ui_->pb_insert, &QPushButton::clicked, this, &CollectionCloset::insertObject);
	connect(ui_->pb_update, &QPushButton::clicked, this, &CollectionCloset::updateObject);
	connect(ui_->pb_delete, &QPushButton::clicked, this, &CollectionCloset::deleteObject);
	connect(ui_->tab_widget, &QTabWidget::currentChanged, this, &CollectionCloset::tabWidgetChanged);
	connect(ui_->cb_sort, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CollectionCloset::sortItems);
	for (QString& tablename : database_.tables()) {
		if (tablename[0] < 'A' || tablename[0] > 'Z' || tablename.endsWith("genres") || tablename.endsWith("platforms")) {
			continue;
		}
		QString label(QCoreApplication::translate(nullptr, QString(tablename).replace('_', ' ').toUtf8().constData()));

		ObjectType type(Types::fromTablename(tablename));
		QStringList field_names(object::fieldNamesFromTablename(tablename));
		QListWidget* list(new QListWidget(ui_->tab_widget));
		list->setWhatsThis(tablename);
		QSqlQuery query(QSqlDatabase::database());
		query.exec(QString("SELECT * FROM %1").arg(tablename));
		connect(list, &QListWidget::itemClicked, this, &CollectionCloset::setSelectedItem);
		while (query.next()) {
			QListWidgetItem* item(new QListWidgetItem(list));
			ObjectPtr object_ptr(initObject(type, query));
			ObjectWidget* object_widget(new ObjectWidget(std::move(object_ptr), field_names, list));
			item->setSizeHint(object_widget->sizeHint());
			list->addItem(item);
			list->setItemWidget(item, object_widget);
		}
		field_names.removeOne("Image");
		sort_fields_list_.push_back(field_names);

		ui_->tab_widget->addTab(list, label);
	}

	if (database_.userName() == "collector") {
		ui_->pb_update->setEnabled(false);
		ui_->pb_delete->setEnabled(false);
	}
	else {
		ui_->pb_insert->hide();
		ui_->pb_update->hide();
		ui_->pb_delete->hide();
	}
	
}

CollectionCloset::~CollectionCloset() {
	delete ui_;
}

void CollectionCloset::insertObject() {
	auto list(qobject_cast<QListWidget*>(ui_->tab_widget->currentWidget()));
	OperationWidget* operation_widget(initOperationWidget(
		OperationType::Insert,
		Types::fromTablename(list->whatsThis()),
		list)
	);
	operation_widget->show();
}

void CollectionCloset::updateObject() {
	selected_item_->setSelected(false);
	selected_item_ = nullptr;
	ui_->pb_update->setEnabled(false);
	ui_->pb_delete->setEnabled(false);
	auto list(qobject_cast<QListWidget*>(ui_->tab_widget->currentWidget()));
	OperationWidget* operation_widget(initOperationWidget(
		OperationType::Update,
		Types::fromTablename(list->whatsThis()),
		list)
	);
	operation_widget->show();
}

void CollectionCloset::deleteObject() {
	auto list(qobject_cast<QListWidget*>(ui_->tab_widget->currentWidget()));
	int id(qobject_cast<ObjectWidget*>(list->itemWidget(selected_item_))->object_->id());
	selected_item_->setSelected(false);
	QSqlQuery query_delete(QSqlDatabase::database());
	query_delete.exec(QString("DELETE FROM %1 WHERE ID = %2").arg(
		list->whatsThis(), QString::number(id)
	));
	list->removeItemWidget(list->currentItem());
	delete list->takeItem(list->currentRow());
	selected_item_ = nullptr;
	ui_->pb_update->setEnabled(false);
	ui_->pb_delete->setEnabled(false);
}

void CollectionCloset::setSelectedItem(QListWidgetItem* item) {
	if (selected_item_ == item) {
		item->setSelected(false);
		selected_item_ = nullptr;
		ui_->pb_update->setEnabled(false);
		ui_->pb_delete->setEnabled(false);
	}
	else {
		item->setSelected(true);
		selected_item_ = item;
		ui_->pb_update->setEnabled(true);
		ui_->pb_delete->setEnabled(true);
	}
}

void CollectionCloset::tabWidgetChanged(int idx) {
	if (selected_item_) {
		selected_item_->setSelected(false);
		selected_item_ = nullptr;
		ui_->pb_update->setEnabled(false);
		ui_->pb_delete->setEnabled(false);
	}
	ui_->cb_sort->clear();
	for (const QString& field : sort_fields_list_[idx]) {
		ui_->cb_sort->addItem(QCoreApplication::translate(nullptr, QString(field).replace('_', ' ').toUtf8().constData()));
	}
}

void CollectionCloset::sortItems(int field_idx) {
	if (field_idx == -1) {
		return;
	}
	auto list(qobject_cast<QListWidget*>(ui_->tab_widget->currentWidget()));
	QMultiMap<QString, ObjectWidget*> object_list;
	while (list->count() != 0) {
		ObjectWidget* ob(qobject_cast<ObjectWidget*>(list->itemWidget(list->item(0))));
		ObjectWidget* object_widget(
			new ObjectWidget(
				std::move(ob->object_),
				object::fieldNamesFromTablename(list->whatsThis()),
				list)
		);
		list->removeItemWidget(list->item(0));
		delete ob;
		delete list->takeItem(0);
		QString sort_value(
			object_widget->object_->field(
					sort_fields_list_[ui_->tab_widget->currentIndex()][field_idx]).toString()
		);
		object_list.insert(sort_value, object_widget);
	}
	for (auto key : object_list.uniqueKeys()) {
		for (auto value : object_list.values(key)) {
			QListWidgetItem* item(new QListWidgetItem(list));
			item->setSizeHint(value->sizeHint());
			list->addItem(item);
			list->setItemWidget(item, value);
		}
		
	}
}

void CollectionCloset::changeUser() {
	QSettings settings;
	settings.remove("username");
	settings.remove("password");
	QCoreApplication::exit(EXIT_CODE_USER_CHANGE);
	deleteLater();
}

void CollectionCloset::exit() {
	QCoreApplication::exit();
}