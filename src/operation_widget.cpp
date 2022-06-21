#include "operation_widget.h"

#include "format_widget.h"

#include <QtCore/QCoreApplication>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>

//#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>

namespace operation {

	QComboBox* infoComboBox(ObjectType object_type, const QString& info_name, QWidget* parent) {
		QComboBox* info_combo_box(new QComboBox(parent));
		info_combo_box->addItems(
			object::infoValuesFromTablename(Types::toInfoTablename(object_type, info_name))
		);
		info_combo_box->setFont(QFont("Constantia", 11));
		return info_combo_box;
	}

	BaseWidget::BaseWidget(Type type, ObjectType object_type, QListWidget* list_widget, QWidget* parent) :
		QWidget(parent),
		type_(type),
		object_type_(object_type),
		list_widget_(list_widget) {
		main_layout_ = new QVBoxLayout(this);
		{
			base_layout_ = new QHBoxLayout();
			base_layout_->addStretch();
			{
				image_layout_ = new QVBoxLayout();
				image_layout_->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
				QLabel* image_label(object::fieldNameLabel("Image", TEXT_FONT, this));
				image_label->setAlignment(Qt::AlignHCenter);
				image_layout_->addWidget(image_label);
				image_ = new QLabel(this);
				image_layout_->addWidget(image_);
				{
					image_layout_2 = new QHBoxLayout();
					info_widgets_["Image"] = new QLineEdit(this);
					qobject_cast<QLineEdit*>(info_widgets_["Image"])->setFont(TEXT_FONT);
					image_layout_2->addWidget(info_widgets_["Image"]);
					tool_button_ = new QToolButton(this);
					connect(tool_button_, &QToolButton::clicked, this, &BaseWidget::chooseImage);
					tool_button_->setFont(TEXT_FONT);
					image_layout_2->addWidget(tool_button_);
				}
				image_layout_->addLayout(image_layout_2);
				image_layout_->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
			}
			base_layout_->addLayout(image_layout_);
			base_layout_->addStretch();
			{
				name_layout_ = new QVBoxLayout();
				name_layout_->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum,QSizePolicy::Expanding));
				QLabel* name_label(object::fieldNameLabel("Name", TEXT_FONT, this));
				name_label->setAlignment(Qt::AlignHCenter);
				name_layout_->addWidget(name_label);
				info_widgets_["Name"] = new QLineEdit(this);
				qobject_cast<QLineEdit*>(info_widgets_["Name"])->setFont(TEXT_FONT);
				name_layout_->addWidget(info_widgets_["Name"]);
				name_layout_->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
			}
			base_layout_->addLayout(name_layout_);
			base_layout_->addStretch();
		}
		main_layout_->addLayout(base_layout_);
		{
			form_layout_ = new QFormLayout();
			info_widgets_["Format"] = new FormatWidget(this);
			qobject_cast<FormatWidget*>(info_widgets_["Format"])->setFont(TEXT_FONT);
			form_layout_->addRow(
				object::fieldNameLabel("Format", TEXT_FONT, this),
				info_widgets_["Format"]
			);
		}
		main_layout_->addLayout(form_layout_);
		{
			button_layout_ = new QHBoxLayout();
			button_layout_->addStretch();
			button_ok_ = new QPushButton(QString::fromUtf8(u8"Ок"), this);
			button_ok_->setFont(TEXT_FONT);
			button_layout_->addWidget(button_ok_);
			button_layout_->addStretch();
			button_cancel_ = new QPushButton(QString::fromUtf8(u8"Отмена"), this);
			button_cancel_->setFont(TEXT_FONT);
			connect(button_cancel_, &QPushButton::clicked, this, &BaseWidget::close);
			button_layout_->addWidget(button_cancel_);
			button_layout_->addStretch();
		}
		main_layout_->addLayout(button_layout_);

		connect(this, &BaseWidget::operationComplete, this, &BaseWidget::close);

		switch (type) {
		case Type::Insert:
			connect(button_ok_, &QPushButton::clicked, this, &BaseWidget::acceptInsert);
			break;
		case Type::Update:
			object_widget_ = qobject_cast<ObjectWidget*>(list_widget_->itemWidget(list_widget_->currentItem()));
			connect(button_ok_, &QPushButton::clicked, this, &BaseWidget::acceptUpdate);
			qobject_cast<QLineEdit*>(info_widgets_["Name"])->setText(
				object_widget_->object_->name()
			);
			qobject_cast<QLineEdit*>(info_widgets_["Image"])->setText(
				object_widget_->object_->field("Image").toString()
			);
			image_->setPixmap(object_widget_->object_->image().scaled(
				QSize(200, 200),
				Qt::IgnoreAspectRatio,
				Qt::SmoothTransformation
			));
			image_->setAlignment(Qt::AlignHCenter);
			qobject_cast<FormatWidget*>(info_widgets_["Format"])->setValue(
				object_widget_->object_->field("Format").toString()
			);
			break;
		default:
			break;
		}
	}

	BaseWidget::~BaseWidget() {}

	void BaseWidget::acceptInsert() {
		if (checkWidgets()) {
			QString tablename(list_widget_->whatsThis());
			QStringList field_names(object::fieldNamesFromTablename(tablename));
			QString insert_fields;
			for (int idx(1); idx < field_names.size(); idx += 1) {
				insert_fields.push_back(field_names[idx] + ", ");
			}
			insert_fields.chop(2);
			QString values("?, ");
			values = values.repeated(field_names.size() - 1);
			values.chop(2);
			QSqlQuery query_insert(QSqlDatabase::database());
			query_insert.prepare(
				QString(
					"INSERT INTO %1 (%2) "
					"VALUES (%3)"
				).arg(tablename, insert_fields, values)
			);
			query_insert.addBindValue(qobject_cast<QLineEdit*>(info_widgets_["Name"])->text());
			query_insert.addBindValue(qobject_cast<QLineEdit*>(info_widgets_["Image"])->text());
			query_insert.addBindValue(Format::toJson(qobject_cast<FormatWidget*>(info_widgets_["Format"])->value()));
			addValuesToQuery(query_insert);
			query_insert.exec();
			QListWidgetItem* item(new QListWidgetItem());
			QSqlQuery query_select(QSqlDatabase::database());
			query_select.exec(QString("SELECT * FROM %1 WHERE Name = '%2'").arg(tablename,
				qobject_cast<QLineEdit*>(info_widgets_["Name"])->text()));
			query_select.first();
			ObjectPtr object(initObject(object_type_, query_select));
			object_widget_ = new ObjectWidget(std::move(object), field_names, list_widget_);
			item->setSizeHint(object_widget_->sizeHint());
			list_widget_->addItem(item);
			list_widget_->setItemWidget(item, object_widget_);
			emit operationComplete();
		}
		else {

		}
	}

	void BaseWidget::acceptUpdate() {
		if (checkWidgets()) {
			QString tablename(list_widget_->whatsThis());
			QStringList field_names(object::fieldNamesFromTablename(tablename));
			QString update_fields;
			for (int idx(1); idx < field_names.size(); idx += 1) {
				update_fields.push_back(field_names[idx] + "=?, ");
			}
			update_fields.chop(2);
			QSqlQuery query_update(QSqlDatabase::database());
			query_update.prepare(QString("UPDATE %1 SET %2 WHERE ID = %3").arg(
				tablename, update_fields, QString::number(object_widget_->object_->id())
			));
			query_update.addBindValue(qobject_cast<QLineEdit*>(info_widgets_["Name"])->text());
			query_update.addBindValue(qobject_cast<QLineEdit*>(info_widgets_["Image"])->text());
			query_update.addBindValue(Format::toJson(qobject_cast<FormatWidget*>(info_widgets_["Format"])->value()));
			addValuesToQuery(query_update);
			query_update.exec();
			QSqlQuery query_select(QSqlDatabase::database());
			query_select.exec(QString("SELECT * FROM %1 WHERE ID = %2").arg(
				tablename, QString::number(object_widget_->object_->id())
			));
			query_select.first();
			ObjectPtr object(initObject(object_type_, query_select));
			object_widget_->updateWidget(std::move(object), field_names);
			emit operationComplete();
		}
		else {

		}
	}

	void BaseWidget::chooseImage() {
		QString path(
			QFileDialog::getOpenFileName(
				this,
				QString::fromUtf8(u8"Выберете изображение.."),
				QString(),
				QString::fromUtf8(u8"Изображение (*.png)")
			)
		);
		qobject_cast<QLineEdit*>(info_widgets_["Image"])->setText(
			path
		);
		QPixmap pixmap(path, "PNG");
		image_->setPixmap(pixmap.scaled(
			QSize(200, 200),
			Qt::IgnoreAspectRatio,
			Qt::SmoothTransformation)
		);
	}

	bool BaseWidget::checkWidgets() const {
		for (const auto& key : info_widgets_.keys()) {
			if (info_widgets_[key] == nullptr && key != "Image" && key != "Format") {
				return false;
			}
		}
		return true;
	}

	BookWidget::BookWidget(Type type, ObjectType object_type, QListWidget* list_widget, QWidget* parent) :
		BaseWidget(type, object_type, list_widget, parent) {
		info_widgets_["Author"] = new QLineEdit(this);
		qobject_cast<QLineEdit*>(info_widgets_["Author"])->setFont(TEXT_FONT);
		form_layout_->addRow(
			object::fieldNameLabel("Author", TEXT_FONT, this),
			info_widgets_["Author"]
		);
		info_widgets_["Type"] = new QLineEdit(this);
		qobject_cast<QLineEdit*>(info_widgets_["Type"])->setFont(TEXT_FONT);
		form_layout_->addRow(
			object::fieldNameLabel("Type", TEXT_FONT, this),
			info_widgets_["Type"]
		);
		info_widgets_["Genre"] = infoComboBox(object_type_, "Genre", this);

		form_layout_->addRow(
			object::fieldNameLabel("Genre", TEXT_FONT, this),
			info_widgets_["Genre"]
		);
		if (type == Type::Update) {
			qobject_cast<QLineEdit*>(info_widgets_["Author"])->setText(
				object_widget_->object_->field("Author").toString()
			);
			qobject_cast<QLineEdit*>(info_widgets_["Type"])->setText(
				object_widget_->object_->field("Type").toString()
			);
			qobject_cast<QComboBox*>(info_widgets_["Genre"])->setCurrentText(
				object_widget_->object_->field("Genre").toString()
			);
		}
	}

	BookWidget::~BookWidget() {}

	void BookWidget::addValuesToQuery(QSqlQuery& query) const {
		query.addBindValue(qobject_cast<QLineEdit*>(info_widgets_["Author"])->text());
		query.addBindValue(qobject_cast<QLineEdit*>(info_widgets_["Type"])->text());
		query.addBindValue(qobject_cast<QComboBox*>(info_widgets_["Genre"])->currentIndex() + 1);
	}

	FigureWidget::FigureWidget(Type type, ObjectType object_type, QListWidget* list_widget, QWidget* parent) :
		BaseWidget(type, object_type, list_widget, parent) {
		info_widgets_["Size"] = new QLineEdit(this);
		qobject_cast<QLineEdit*>(info_widgets_["Size"])->setFont(TEXT_FONT);
		form_layout_->addRow(
			object::fieldNameLabel("Size", TEXT_FONT, this),
			info_widgets_["Size"]
		);
		info_widgets_["Height"] = new QLineEdit(this);
		qobject_cast<QLineEdit*>(info_widgets_["Height"])->setFont(TEXT_FONT);
		form_layout_->addRow(
			object::fieldNameLabel("Height", TEXT_FONT, this),
			info_widgets_["Height"]
		);
		info_widgets_["Publisher"] = new QLineEdit(this);
		qobject_cast<QLineEdit*>(info_widgets_["Publisher"])->setFont(TEXT_FONT);
		form_layout_->addRow(
			object::fieldNameLabel("Publisher", TEXT_FONT, this),
			info_widgets_["Publisher"]
		);
		if (type == Type::Update) {
			qobject_cast<QLineEdit*>(info_widgets_["Size"])->setText(
				object_widget_->object_->field("Size").toString()
			);
			qobject_cast<QLineEdit*>(info_widgets_["Height"])->setText(
				object_widget_->object_->field("Height").toString()
			);
			qobject_cast<QLineEdit*>(info_widgets_["Publisher"])->setText(
				object_widget_->object_->field("Publisher").toString()
			);
		}
	}

	FigureWidget::~FigureWidget() {}

	void FigureWidget::addValuesToQuery(QSqlQuery& query) const {
		query.addBindValue(qobject_cast<QLineEdit*>(info_widgets_["Size"])->text());
		query.addBindValue(qobject_cast<QLineEdit*>(info_widgets_["Height"])->text());
		query.addBindValue(qobject_cast<QLineEdit*>(info_widgets_["Publisher"])->text());
	}

	MovieWidget::MovieWidget(Type type, ObjectType object_type, QListWidget* list_widget, QWidget* parent) :
		BaseWidget(type, object_type, list_widget, parent) {
		info_widgets_["Directed_by"] = new QLineEdit(this);
		qobject_cast<QLineEdit*>(info_widgets_["Directed_by"])->setFont(TEXT_FONT);
		form_layout_->addRow(
			object::fieldNameLabel("Directed_by", TEXT_FONT, this),
			info_widgets_["Directed_by"]
		);
		info_widgets_["Genre"] = infoComboBox(object_type_, "Genre", this);
		form_layout_->addRow(
			object::fieldNameLabel("Genre", TEXT_FONT, this),
			info_widgets_["Genre"]
		);
		info_widgets_["Release_year"] = new QSpinBox(this);
		qobject_cast<QSpinBox*>(info_widgets_["Release_year"])->setRange(1800, 2022);
		qobject_cast<QSpinBox*>(info_widgets_["Release_year"])->setFont(TEXT_FONT);
		form_layout_->addRow(
			object::fieldNameLabel("Release_year", TEXT_FONT, this),
			info_widgets_["Release_year"]
		);
		if (type == Type::Update) {
			qobject_cast<QLineEdit*>(info_widgets_["Directed_by"])->setText(
				object_widget_->object_->field("Directed_by").toString()
			);
			qobject_cast<QComboBox*>(info_widgets_["Genre"])->setCurrentText(
				object_widget_->object_->field("Genre").toString()
			);
			qobject_cast<QSpinBox*>(info_widgets_["Release_year"])->setValue(
				object_widget_->object_->field("Release_year").toInt()
			);
		}
	}

	MovieWidget::~MovieWidget() {}

	void MovieWidget::addValuesToQuery(QSqlQuery& query) const {
		query.addBindValue(qobject_cast<QLineEdit*>(info_widgets_["Directed_by"])->text());
		query.addBindValue(qobject_cast<QComboBox*>(info_widgets_["Genre"])->currentIndex() + 1);
		query.addBindValue(qobject_cast<QSpinBox*>(info_widgets_["Release_year"])->value());
	}

	MusicAlbumWidget::MusicAlbumWidget(Type type, ObjectType object_type, QListWidget* list_widget, QWidget* parent) :
		BaseWidget(type, object_type, list_widget, parent) {
		info_widgets_["Performer"] = new QLineEdit(this);
		qobject_cast<QLineEdit*>(info_widgets_["Performer"])->setFont(TEXT_FONT);
		form_layout_->addRow(
			object::fieldNameLabel("Performer", TEXT_FONT, this),
			info_widgets_["Performer"]
		);
		info_widgets_["Genre"] = infoComboBox(object_type_, "Genre", this);
		form_layout_->addRow(
			object::fieldNameLabel("Genre", TEXT_FONT, this),
			info_widgets_["Genre"]
		);
		info_widgets_["Release_year"] = new QSpinBox(this);
		qobject_cast<QSpinBox*>(info_widgets_["Release_year"])->setRange(1800, 2022);
		qobject_cast<QSpinBox*>(info_widgets_["Release_year"])->setFont(TEXT_FONT);
		form_layout_->addRow(
			object::fieldNameLabel("Release_year", TEXT_FONT, this),
			info_widgets_["Release_year"]
		);
		if (type == Type::Update) {
			qobject_cast<QLineEdit*>(info_widgets_["Performer"])->setText(
				object_widget_->object_->field("Performer").toString()
			);
			qobject_cast<QComboBox*>(info_widgets_["Genre"])->setCurrentText(
				object_widget_->object_->field("Genre").toString()
			);
			qobject_cast<QSpinBox*>(info_widgets_["Release_year"])->setValue(
				object_widget_->object_->field("Release_year").toInt()
			);
		}
	}

	MusicAlbumWidget::~MusicAlbumWidget() {}

	void MusicAlbumWidget::addValuesToQuery(QSqlQuery& query) const {
		query.addBindValue(qobject_cast<QLineEdit*>(info_widgets_["Performer"])->text());
		query.addBindValue(qobject_cast<QComboBox*>(info_widgets_["Genre"])->currentIndex() + 1);
		query.addBindValue(qobject_cast<QSpinBox*>(info_widgets_["Release_year"])->value());
	}

	VideoGameWidget::VideoGameWidget(Type type, ObjectType object_type, QListWidget* list_widget, QWidget* parent) :
		BaseWidget(type, object_type, list_widget, parent) {
		info_widgets_["Platform"] = infoComboBox(object_type_, "Platform", this);
		form_layout_->addRow(
			object::fieldNameLabel("Platform", TEXT_FONT, this),
			info_widgets_["Platform"]
		);
		info_widgets_["Developer"] = new QLineEdit(this);
		qobject_cast<QLineEdit*>(info_widgets_["Developer"])->setFont(TEXT_FONT);
		form_layout_->addRow(
			object::fieldNameLabel("Developer", TEXT_FONT, this),
			info_widgets_["Developer"]
		);
		info_widgets_["Publisher"] = new QLineEdit(this);
		qobject_cast<QLineEdit*>(info_widgets_["Publisher"])->setFont(TEXT_FONT);
		form_layout_->addRow(
			object::fieldNameLabel("Publisher", TEXT_FONT, this),
			info_widgets_["Publisher"]
		);
		info_widgets_["Genre"] = infoComboBox(object_type_, "Genre", this);
		form_layout_->addRow(
			object::fieldNameLabel("Genre", TEXT_FONT, this),
			info_widgets_["Genre"]
		);
		info_widgets_["Release_year"] = new QSpinBox(this);
		qobject_cast<QSpinBox*>(info_widgets_["Release_year"])->setRange(1800, 2022);
		qobject_cast<QSpinBox*>(info_widgets_["Release_year"])->setFont(TEXT_FONT);
		form_layout_->addRow(
			object::fieldNameLabel("Release_year", TEXT_FONT, this),
			info_widgets_["Release_year"]
		);
		if (type == Type::Update) {
			qobject_cast<QComboBox*>(info_widgets_["Platform"])->setCurrentText(
				object_widget_->object_->field("Genre").toString()
			);
			qobject_cast<QLineEdit*>(info_widgets_["Developer"])->setText(
				object_widget_->object_->field("Developer").toString()
			);
			qobject_cast<QLineEdit*>(info_widgets_["Publisher"])->setText(
				object_widget_->object_->field("Publisher").toString()
			);
			qobject_cast<QComboBox*>(info_widgets_["Genre"])->setCurrentText(
				object_widget_->object_->field("Genre").toString()
			);
			qobject_cast<QSpinBox*>(info_widgets_["Release_year"])->setValue(
				object_widget_->object_->field("Release_year").toInt()
			);
		}
	}

	VideoGameWidget::~VideoGameWidget() {}

	void VideoGameWidget::addValuesToQuery(QSqlQuery& query) const {
		query.addBindValue(qobject_cast<QComboBox*>(info_widgets_["Platform"])->currentIndex() + 1);
		query.addBindValue(qobject_cast<QLineEdit*>(info_widgets_["Developer"])->text());
		query.addBindValue(qobject_cast<QLineEdit*>(info_widgets_["Publisher"])->text());
		query.addBindValue(qobject_cast<QComboBox*>(info_widgets_["Genre"])->currentIndex() + 1);
		query.addBindValue(qobject_cast<QSpinBox*>(info_widgets_["Release_year"])->value());
	}
}

OperationWidget* initOperationWidget(
	OperationType operation_type,
	ObjectType object_type,
	QListWidget* list_widget,
	QWidget* parent
) {
	switch (object_type) {
	case ObjectType::Book:
		return new operation::BookWidget(operation_type, object_type, list_widget, parent);
	case ObjectType::Figure:
		return new operation::FigureWidget(operation_type, object_type, list_widget, parent);
	case ObjectType::Movie:
		return new operation::MovieWidget(operation_type, object_type, list_widget, parent);
	case ObjectType::Music_album:
		return new operation::MusicAlbumWidget(operation_type, object_type, list_widget, parent);
	case ObjectType::Video_game:
		return new operation::VideoGameWidget(operation_type, object_type, list_widget, parent);
	default:
		return nullptr;
	}
}
