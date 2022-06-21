#include "collection_closet.h"
#include "login_widget.h"

#include <QtWidgets/QApplication>
#include <QtCore/QTextCodec>
#include <QtCore/QTranslator>

int main(int argc, char* argv[]) {
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

	QCoreApplication::setOrganizationName("Prokhorov Daniil");
	QCoreApplication::setOrganizationDomain("https://github.com/daniilprokh");
	QCoreApplication::setApplicationName("CollectionCloset");

	QApplication app(argc, argv);

	QTranslator translator;
	if (translator.load(":tr/resources/collection_closet.qm")) {
		QCoreApplication::installTranslator(&translator);
	}

	int exitCode(0);
	do {
		LoginWidget* login_widget(new LoginWidget());
		CollectionCloset* closet;
		QObject::connect(login_widget, &LoginWidget::successLogin, [&closet, &login_widget] {
				closet = new CollectionCloset(nullptr, login_widget->database());
				closet->show();
				login_widget->deleteLater();
			}
		);
		if (login_widget->isRemember()) {
			login_widget->connectToDatabase();
		}
		else {
			login_widget->show();
		}
		exitCode = app.exec();
	} while (exitCode == CollectionCloset::EXIT_CODE_USER_CHANGE);
	return exitCode;
}