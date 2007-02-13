#include <QDialog>
#include <QString>
class QAction;
class QMenu;
class QTextEdit;
class QLabel;
class QErrorMessage;

using namespace std;

class User : public QDialog {
	Q_OBJECT

public:
	User();
	~User();
	void dataDialogue();

private:
	QString name;

};

