#include <QDialog>
#include <QString>
class QAction;
class QMenu;
class QTextEdit;
class QLabel;
class QErrorMessage;

using namespace std;

const int NewUser=0;
const int OpenUser=1;

class User : public QDialog {
	Q_OBJECT

public:
	User(const int type);
	User();
	~User();
	QString getName();

public slots:
	void setUserInfo();

private:
	void newUser();
	void openUser();

	QString name;

};

