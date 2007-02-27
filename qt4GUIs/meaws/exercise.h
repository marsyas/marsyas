#include <QDialog>
#include <QString>
class QAction;
class QMenu;
class QTextEdit;
class QLabel;
class QErrorMessage;

using namespace std;

class Exercise : public QDialog {
	Q_OBJECT

public:
	Exercise();
	~Exercise();
	QString getName();
	//QImage getLily();
	void getLily();

public slots:
	void nextNoteError(float error, int direction);

private:
	QString name;

	QStringList lily_input;
	int note;
};

