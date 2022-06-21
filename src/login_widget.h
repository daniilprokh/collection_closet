#ifndef LOGIN_WIDGET_H
#define LOGIN_WIDGET_H

#include <QtCore/QSettings>
#include <QtWidgets/QWidget>
#include <QtSql/QSqlDatabase>

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWidget; }
QT_END_NAMESPACE

class LoginWidget : public QWidget {
	Q_OBJECT
signals:
	void successLogin();
public:
	explicit LoginWidget(QWidget* parent = nullptr);
	~LoginWidget();
public:
	const QSqlDatabase& database() const;
	void connectToDatabase();
	bool isRemember() const;
public slots:
	void login();
private:
	bool tryToConnect(const QString& username, const QString& password);
private:
	Ui::LoginWidget* ui_;
	QSettings settings_;
	QSqlDatabase database_;
};

#endif // !LOGIN_WIDGET_H
