#include <QObject>
using namespace std;

class Analyze: public QObject {
	Q_OBJECT

public:
	Analyze();
	~Analyze();
	void writePitches(string filename);
	void calcDurations();
	void calcNotes();

signals:
	void nextNoteError(float error, int direction);

private:
	int *exercise;
};

