#include "format_widget.h"

#include <QtCore/QCoreApplication>

FormatWidget::FormatWidget(QWidget* parent) :
	QWidget(parent),
	names_({
		QCoreApplication::translate(nullptr, "Physical"),
		QCoreApplication::translate(nullptr, "Digital")
	}) {
	info_names_[names_[0]] = QCoreApplication::translate(nullptr, "Type");
	info_names_[names_[1]] = QCoreApplication::translate(nullptr, "Storage");

	vertical_layout_ = new QVBoxLayout(this);
	
	name_combo_box_ = new QComboBox(this);
	name_combo_box_->addItems(names_);
	vertical_layout_->addWidget(name_combo_box_);
	{
		horizontal_layout_ = new QHBoxLayout();
		info_label_ = new QLabel(this);
		horizontal_layout_->addWidget(info_label_);
		info_ = new QLineEdit(this);
		horizontal_layout_->addWidget(info_);
	}
	vertical_layout_->addLayout(horizontal_layout_);
	name_combo_box_->setCurrentIndex(0);
	info_label_->setText(info_names_[names_[0]] + ':');
	connect(name_combo_box_, &QComboBox::currentTextChanged, this, &FormatWidget::changeInfoName);
}

FormatWidget::~FormatWidget() {}

QString FormatWidget::value() const {
	QString info(info_->text());
	if (info.isEmpty()) {
		return name_combo_box_->currentText();
	}
	else {
		return name_combo_box_->currentText() + '/' + info;
	}
}
void FormatWidget::setValue(const QString& value) {
	QStringList values(value.split('/'));
	name_combo_box_->setCurrentText(values[0]);
	info_label_->setText(info_names_[values[0]] + ':');
	if (values.size() == 2) {
		info_->setText(values[1]);
	}
}

void FormatWidget::changeInfoName(const QString& name) {
	info_label_->setText(info_names_[name] + ':');
	info_->clear();
}