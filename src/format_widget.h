#ifndef FORMAT_WIDGET_H
#define FORMAT_WIDGET_H

#include "format.h"

#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>

class FormatWidget : public QWidget {
	Q_OBJECT;
public:
	FormatWidget(QWidget* parent = nullptr);
	~FormatWidget();

	QString value() const;
	void setValue(const QString& value);
public slots:
	void changeInfoName(const QString& name);
private:
	QVBoxLayout* vertical_layout_;
	QHBoxLayout* horizontal_layout_;

	QStringList names_;
	QHash<QString,QString> info_names_;

	QComboBox* name_combo_box_;
	QLabel* info_label_;
	QLineEdit* info_;
};

#endif // !FORMAT_WIDGET_H
