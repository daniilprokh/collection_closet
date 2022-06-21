#ifndef COLLECTION_CLOSET_H
#define COLLECTION_CLOSET_H

#include <QtCore/QVector>
#include <QtGui/QShowEvent>
#include <QtSql/QSqlDatabase>

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class CollectionCloset; }
QT_END_NAMESPACE

class CollectionCloset : public QMainWindow {
	Q_OBJECT;
public:
	CollectionCloset(QWidget* parent, const QSqlDatabase& database);
	~CollectionCloset();
public:
	static int const EXIT_CODE_USER_CHANGE{ -123456789 };
public slots:
	void insertObject();
	void updateObject();
	void deleteObject();

	void setSelectedItem(QListWidgetItem* item);
	void sortItems(int field_idx);
	void tabWidgetChanged(int idx);

	void changeUser();
	void exit();
private:
	Ui::CollectionCloset* ui_;

	QListWidgetItem* selected_item_{ nullptr };

	QVector<QStringList> sort_fields_list_;

	QSqlDatabase database_;
	QString username_{ "" };
};

#endif // !COLLECTION_CLOSET_H
