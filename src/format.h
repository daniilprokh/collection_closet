#ifndef FORMAT_H
#define FORMAT_H

#include <QtCore/QByteArray>
#include <QtCore/QString>

#include <optional>

class Format {
public:
	Format(const QByteArray& json);
	~Format() = default;

	static QString fromJson(QByteArray& json);
	static QString toJson(const QString& value);

	QString toString() const;
	QByteArray toJson() const;

private:
	QString name_;
	std::optional<QString> info_;
};

#endif // !FORMAT_H
