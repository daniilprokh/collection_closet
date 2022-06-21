#ifndef OBJECT_WIDGET_H
#define OBJECT_WIDGET_H

#include "object.h"

#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace object {
	QLabel* fieldNameLabel(const QString& field_name, const QFont& font, QWidget* parent = nullptr);
	QStringList fieldNamesFromTablename(const QString& tablename);
}

class ObjectWidget : public QWidget {
	Q_OBJECT;
public:
	ObjectWidget(ObjectPtr&& object, const QStringList& field_names, QWidget* parent = nullptr);
	~ObjectWidget();

	void updateWidget(ObjectPtr&& object, const QStringList& field_names);

	ObjectPtr object_;
protected:
	QHBoxLayout* main_layout_;
	QVBoxLayout* sub_layout_;
	QFormLayout* form_layout_;

	QHash<QString, QLabel*> info_labels_;
};

#endif // !OBJECT_WIDGET_H
