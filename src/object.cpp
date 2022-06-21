#include "object.h"

#include "format.h"

#include <QtSql/QSqlDatabase>
#include <QtCore/QDebug>

namespace object {

	QVariant infoValueFromTablename(const QString& tablename, int id) {
		QSqlQuery query(QSqlDatabase::database());
		query.exec(
			QString(
				"SELECT Name FROM %1 WHERE ID = %2"
			).arg(tablename, QString::number(id)));
		query.first();
		return query.value(0);
	}

	QStringList infoValuesFromTablename(const QString& tablename) {
		QStringList info_values;
		QSqlQuery query(QSqlDatabase::database());
		query.exec(
			QString("SELECT Name FROM %1").arg(tablename)
		);
		while (query.next()) {
			info_values.push_back(query.value(0).toString());
		}
		return info_values;
	}

	Base::Base(Type type, const QSqlQuery& query) :
		type_(type) {
		info_["ID"] = query.value("ID");
		info_["Name"] = query.value("Name");
		info_["Format"] = Format::fromJson(query.value("Format").toByteArray());
		info_["Image"] = query.value("Image").toString();
	}

	QVariant Base::field(const QString& name) const {
		return info_.value(name);
	}

	int Base::id() const {
		return info_["ID"].toInt();
	}

	QPixmap Base::image() const {
		return QPixmap(info_["Image"].toString(), "PNG");
	}
	QString Base::name() const {
		return info_["Name"].toString();
	}

	Type Base::type() const {
		return type_;
	}

	Book::Book(Type type, const QSqlQuery& query) :
		Object(type, query) {
		info_["Author"] = query.value("Author");
		info_["Type"] = query.value("Type");
		info_["Genre"] = infoValueFromTablename(
			Types::toInfoTablename(type, "Genre"),
			query.value("Genre").toInt()
		); 
	}

	Figure::Figure(Type type, const QSqlQuery& query) :
		Object(type, query) {
		info_["Size"] = query.value("Size");
		info_["Height"] = query.value("Height");
		info_["Publisher"] = query.value("Publisher");
	}

	Movie::Movie(Type type, const QSqlQuery& query) :
		Object(type, query) {
		info_["Directed_by"] = query.value("Directed_by");
		info_["Genre"] = infoValueFromTablename(
			Types::toInfoTablename(type, "Genre"),
			query.value("Genre").toInt()
		);
		info_["Release_year"] = query.value("Release_year");
	}

	MusicAlbum::MusicAlbum(Type type, const QSqlQuery& query) :
		Object(type, query) {
		info_["Performer"] = query.value("Performer");
		info_["Genre"] = infoValueFromTablename(
			Types::toInfoTablename(type, "Genre"),
			query.value("Genre").toInt()
		);
		info_["Release_year"] = query.value("Release_year");
	}

	VideoGame::VideoGame(Type type, const QSqlQuery& query) :
		Object(type, query) {
		info_["Platform"] = infoValueFromTablename(
			Types::toInfoTablename(type, "Platform"),
			query.value("Platform").toInt()
		);
		info_["Developer"] = query.value("Developer");
		info_["Publisher"] = query.value("Publisher");
		info_["Genre"] = infoValueFromTablename(
			Types::toInfoTablename(type, "Genre"),
			query.value("Genre").toInt()
		);
		info_["Release_year"] = query.value("Release_year");
	}
}

ObjectPtr initObject(ObjectType type, const QSqlQuery& query) {
	switch (type) {
	case ObjectType::Book:
		return std::make_unique<object::Book>(type, query);
	case ObjectType::Figure:
		return std::make_unique<object::Figure>(type, query);
	case ObjectType::Movie:
		return std::make_unique<object::Movie>(type, query);
	case ObjectType::Music_album:
		return std::make_unique<object::MusicAlbum>(type, query);
	case ObjectType::Video_game:
		return std::make_unique<object::VideoGame>(type, query);
	default:
		return nullptr;
	}
}