#ifndef OBJECT_H
#define OBJECT_H

#include "types.h"

#include <QtCore/QString>
#include <QtCore/QVariant>

#include <QtGui/QPixmap>

#include <QtSql/QSqlQuery>

namespace object {
	using Type = Types::ObjectType;

	QVariant infoValueFromTablename(const QString& tablename, int id);
	QStringList infoValuesFromTablename(const QString& tablename);

	class Base {
	public:
		Base(Type type, const QSqlQuery& query);
		~Base() = default;
	public:
		QVariant field(const QString& name) const;
		int id() const;
		QPixmap image() const;
		QString name() const;
		Type type() const;
	protected:
		Type type_;
		QVariantHash info_;
	};

	class Book : public Base {
	public:
		Book(Type type, const QSqlQuery& query);
		~Book() = default;
	};

	class Figure : public Base {
	public:
		Figure(Type type, const QSqlQuery& query);
		~Figure() = default;
	};

	class Movie : public Base {
	public:
		Movie(Type type, const QSqlQuery& query);
		~Movie() = default;
	};

	class MusicAlbum : public Base {
	public:
		MusicAlbum(Type type, const QSqlQuery& query);
		~MusicAlbum() = default;
	};

	class VideoGame : public Base {
	public:
		VideoGame(const Type type, const QSqlQuery& query);
		~VideoGame() = default;
	};
}

using Object = object::Base;
using ObjectType = object::Type;
using ObjectPtr = std::unique_ptr<Object>;

ObjectPtr initObject(ObjectType type, const QSqlQuery& query);

#endif // !OBJECT_H
