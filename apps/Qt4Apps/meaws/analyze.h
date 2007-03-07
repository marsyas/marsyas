#include <QObject>
using namespace std;

class Analyze: public QObject {
	Q_OBJECT

public:
	Analyze();
	~Analyze();
	void calculate(string filename);

signals:
	void nextNoteError(float error, int direction);

};

