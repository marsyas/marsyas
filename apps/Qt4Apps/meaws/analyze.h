//#include <QObject>
#include "MarSystemManager.h"
using namespace Marsyas;

using namespace std;

class Analyze {
//class Analyze: public QObject {
//	Q_OBJECT

public:
	Analyze();
	~Analyze();
	void getPitches(string filename);
	void writePitches(string filename);
	void calcDurations();
	void calcNotes();
	void metroDurations();

/*
signals:
	void nextNoteError(float error, int direction);
*/
private:
	int *exercise;
};

