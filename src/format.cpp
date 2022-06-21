#include "format.h"

#include <QtCore/QCoreApplication>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QVariant>

Format::Format(const QByteArray& json) {
	QJsonDocument doc(QJsonDocument::fromJson(json));
	auto& map(doc.object().toVariantMap());
	name_ = map.value("name").toString();
	if (map.find("info") != map.end()) {
		if (name_ == QCoreApplication::translate(nullptr, "Digital")) {
			info_ = map.value("info").toMap().value("storage").toString();
		}
		else {
			info_ = map.value("info").toMap().value("type").toString();
		}
	}
}

QString Format::fromJson(QByteArray& json) {
	QString result;
	QJsonDocument doc(QJsonDocument::fromJson(json));
	auto& map(doc.object().toVariantMap());
	result = map.value("name").toString();
	if (map.find("info") != map.end()) {
		if (result == QCoreApplication::translate(nullptr, "Digital")) {
			result.push_back('/');
			result.push_back(map.value("info").toMap().value("storage").toString());
		}
		else {
			result.push_back('/');
			result.push_back(map.value("info").toMap().value("type").toString());
		}
	}
	return result;
}

QString Format::toJson(const QString& value) {
	QStringList values(value.split('/'));
	QVariantMap map;
	map["name"] = values[0];
	if (values.size() == 2) {
		if (values[0] == QCoreApplication::translate(nullptr, "Digital")) {
			map["info"] = QVariantMap{ {"storage", values[1]} };
		}
		else {
			map["info"] = QVariantMap{ {"type", values[1]} };
		}
	}
	return QString::fromUtf8(QJsonDocument(QJsonObject::fromVariantMap(map)).toJson());
}

QString Format::toString() const {
	if (info_.has_value()) {
		return QString("%1/%2").arg(name_, info_.value());
	}
	else {
		return name_;
	}
}

QByteArray Format::toJson() const {
	QVariantMap map;
	map["name"] = name_;
	if (info_.has_value()) {
		if (name_ == QCoreApplication::translate(nullptr, "Digital")) {
			map["info"] = QVariantMap{ {"storage", info_.value()} };
		}
		else {
			map["info"] = QVariantMap{ {"type", info_.value()} };
		}
	}
	return QJsonDocument(QJsonObject::fromVariantMap(map)).toJson();
}
