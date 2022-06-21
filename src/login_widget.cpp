#include "login_widget.h"
#include "ui_login_widget.h"

#include <QtWidgets/QMessageBox>

LoginWidget::LoginWidget(QWidget* parent) :
	QWidget(parent),
	ui_(new Ui::LoginWidget) {
	ui_->setupUi(this);

	connect(ui_->pb_login, &QPushButton::clicked, this, &LoginWidget::login);
}

LoginWidget::~LoginWidget() {
	delete ui_;
}

void LoginWidget::connectToDatabase() {
	if (tryToConnect(
			settings_.value("username").toString(),
			settings_.value("password").toString()
		)) {
		emit successLogin();
	}
	else {
		show();
	}
}

const QSqlDatabase& LoginWidget::database() const {
	return database_;
}

bool LoginWidget::isRemember() const {
	return !settings_.value("username").isNull();
}

void LoginWidget::login() {
	if (tryToConnect(ui_->le_user->text(), ui_->le_pwd->text())) {
		if (ui_->cb_remember->isChecked()) {
			settings_.setValue("username", ui_->le_user->text());
			settings_.setValue("password", ui_->le_pwd->text());
		}
		emit successLogin();
		close();
	}
	else {
		QMessageBox::critical(
			this,
			QString::fromUtf8(u8"Ошибка авторизации"),
			QString::fromUtf8(u8"Неверный пользователь или пароль!"),
			QMessageBox::Ok
		);
	}
}

bool LoginWidget::tryToConnect(const QString& username, const QString& password) {
	database_ = QSqlDatabase::addDatabase("QODBC3");
	database_.setDatabaseName(
		QString(
			"DRIVER={SQL Server};"
			"Server=DESKTOP-J0U8QM2;"
			"Database=CollectionCloset;"
			"Persist Security Info=true;"
			"Uid=%1;"
			"Pwd=%2"
		).arg(username, password)
	);
	database_.setUserName(username);
	return database_.open();
}
