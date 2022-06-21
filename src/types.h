#ifndef TYPES_H
#define TYPES_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>

class Types {
	Q_GADGET
public:
	enum ObjectType {
		Book,
		Figure,
		Movie,
		Music_album,
		Video_game
	};
	Q_ENUM(ObjectType)
	static ObjectType fromTablename(QString tablename) {
		tablename.chop(1);
		return QVariant(tablename).value<ObjectType>();
	}
	static QString toInfoTablename(ObjectType object_type, const QString& info_name) {
		return QString("%1_%2s").arg(
			QVariant::fromValue(object_type).toString(),
			info_name.toLower()
		);
	}
	static QString toTablename(ObjectType object_type) {
		return QString("%1s").arg(QVariant::fromValue(object_type).toString());
	}
public:
	enum OperationType {
		Insert,
		Update
	};
	Q_ENUM(OperationType)
private:
	Types() = delete;
};

#endif // !TYPES_H
