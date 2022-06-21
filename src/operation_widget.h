#ifndef OPERATION_WIDGET_H
#define OPERATION_WIDGET_H

#include "types.h"

#include "object_widget.h"

#include <QtWidgets/QComboBox>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QToolButton>

namespace operation {
	using Type = Types::OperationType;

	QComboBox* infoComboBox(ObjectType object_type, const QString& info_name, QWidget* parent = nullptr);

	class BaseWidget : public QWidget {
		Q_OBJECT;
	signals:
		void operationComplete();
	public:
		BaseWidget(Type type, ObjectType object_type, QListWidget* list_widget, QWidget* parent = nullptr);
		~BaseWidget();
	public slots:
		void acceptInsert();
		void acceptUpdate();
		void chooseImage();
	protected:
		virtual void addValuesToQuery(QSqlQuery& query) const = 0;

		const QFont TEXT_FONT{ QFont("Constantia", 11) };

		QVBoxLayout* main_layout_;
		QVBoxLayout* image_layout_;
		QHBoxLayout* image_layout_2;
		QVBoxLayout* name_layout_;
		QHBoxLayout* base_layout_;
		QFormLayout* form_layout_;
		QHBoxLayout* button_layout_;

		QPushButton* button_ok_;
		QPushButton* button_cancel_;

		QLabel* image_;
		QToolButton* tool_button_;

		QHash<QString, QWidget*> info_widgets_;

		Type type_;
		ObjectType object_type_;

		QListWidget* list_widget_;
		ObjectWidget* object_widget_{ nullptr };
	private:
		bool checkWidgets() const;
	};

	class BookWidget : public BaseWidget {
		Q_OBJECT;
	public:
		BookWidget(Type type, ObjectType object_type, QListWidget* list_widget, QWidget* parent = nullptr);
		~BookWidget();
	private:
		virtual void addValuesToQuery(QSqlQuery& query) const override;
	};

	class FigureWidget : public BaseWidget {
		Q_OBJECT;
	public:
		FigureWidget(Type type, ObjectType object_type, QListWidget* list_widget, QWidget* parent = nullptr);
		~FigureWidget();
	private:
		virtual void addValuesToQuery(QSqlQuery& query) const override;
	};

	class MovieWidget : public BaseWidget {
		Q_OBJECT;
	public:
		MovieWidget(Type type, ObjectType object_type, QListWidget* list_widget, QWidget* parent = nullptr);
		~MovieWidget();
	private:
		virtual void addValuesToQuery(QSqlQuery& query) const override;
	};

	class MusicAlbumWidget : public BaseWidget {
		Q_OBJECT;
	public:
		MusicAlbumWidget(Type type, ObjectType object_type, QListWidget* list_widget, QWidget* parent = nullptr);
		~MusicAlbumWidget();
	private:
		virtual void addValuesToQuery(QSqlQuery& query) const override;
	};

	class VideoGameWidget : public BaseWidget {
		Q_OBJECT;
	public:
		VideoGameWidget(Type type, ObjectType object_type, QListWidget* list_widget, QWidget* parent = nullptr);
		~VideoGameWidget();
	private:
		virtual void addValuesToQuery(QSqlQuery& query) const override;
	};
}

using OperationType = operation::Type;
using OperationWidget = operation::BaseWidget;

OperationWidget* initOperationWidget(
	OperationType operation_type,
	ObjectType object_type,
	QListWidget* list_widget,
	QWidget* parent = nullptr
);

#endif // !OPERATION_WIDGET_H


