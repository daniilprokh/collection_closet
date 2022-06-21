#include "object_widget.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

namespace object {
	QLabel* fieldNameLabel(const QString& field_name, const QFont& font, QWidget* parent) {
		QLabel* label(new QLabel(parent));
		label->setText(QCoreApplication::translate(
			nullptr,
			QString(field_name).replace('_', ' ').toUtf8().constData()) + ':'
		);
		label->setFont(font);
		return label;
	}
	QStringList fieldNamesFromTablename(const QString& tablename) {
		QStringList field_names;
		QSqlQuery query(QSqlDatabase::database());
		query.exec(
			QString(
				"SELECT COLUMN_NAME FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_NAME = N'%1'"
			).arg(tablename)
		);
		while (query.next()) {
			field_names.push_back(query.value(0).toString());
		}
		return field_names;
	}
}

ObjectWidget::ObjectWidget(ObjectPtr&& object, const QStringList& field_names, QWidget* parent) :
	object_(std::move(object)),
	QWidget(parent) {
	main_layout_ = new QHBoxLayout(this);
	info_labels_["Image"] = new QLabel(this);
	info_labels_["Image"]->setPixmap(
		object_->image().scaled(
			QSize(250, 250),
			Qt::IgnoreAspectRatio,
			Qt::SmoothTransformation
		)
	);
	main_layout_->addWidget(info_labels_["Image"]);
	sub_layout_ = new QVBoxLayout();
	info_labels_["Name"] = new QLabel(object_->name(), this);
	info_labels_["Name"]->setFont(QFont("Constantia", 14, QFont::Bold));
	sub_layout_->addWidget(info_labels_["Name"]);
	form_layout_ = new QFormLayout();

	for (int idx(3); idx < field_names.size(); idx += 1) {
		info_labels_[field_names[idx]] = new QLabel(
			object_->field(field_names[idx]).toString(), this
		);
		info_labels_[field_names[idx]]->setFont(QFont("Constantia", 12));
		form_layout_->addRow(
			object::fieldNameLabel(field_names[idx], QFont("Constantia", 12, QFont::Bold), this),
			info_labels_[field_names[idx]]
		);
	}

	sub_layout_->addLayout(form_layout_);
	main_layout_->addLayout(sub_layout_);
	main_layout_->addStretch();
	main_layout_->setSizeConstraint(QLayout::SetFixedSize);
}

ObjectWidget::~ObjectWidget() {}

void ObjectWidget::updateWidget(ObjectPtr&& object, const QStringList& field_names) {
	object_ = std::move(object);
	info_labels_["Image"]->setPixmap(
		object_->image().scaled(
			QSize(250, 250),
			Qt::IgnoreAspectRatio,
			Qt::SmoothTransformation
		)
	);
	info_labels_["Name"]->setText(object_->name());
	for (int idx(3); idx < field_names.size(); idx += 1) {
		info_labels_[field_names[idx]]->setText(object_->field(field_names[idx]).toString());
	}
}