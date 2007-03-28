#ifndef DISPLAY_H
#define DISPLAY_H

#include <ostream>

#include <QWidget>

#include <ctime>
#include <cstdlib>

#include "Classifier.h"
#include "Extractor.h"
#include "../Music/MusicTrack.h"
#include "../Widgets/Tracklist.h"

class Display : public QWidget
{
	Q_OBJECT

public:
	Display(Tracklist *tracklist, QWidget *parent = 0) 
		: QWidget(parent), _tracklist(tracklist) {}

	virtual ~Display() {};
	virtual void clear() = 0;

	/*
	virtual void setExtractor(Extractor *extractor);
	virtual Extractor* getExtractor() const; 

	virtual void setClassifier(Classifier *classifier);
	virtual Classifier* getClassifier() const; 
	*/

public slots: 
	virtual void extract() = 0;
	virtual void predict() = 0;
	virtual void train() = 0;
        virtual void midiXYEvent(unsigned char /* xaxis */, unsigned char /* yaxis */) {}
        virtual void midiPlaylistEvent(bool /* next */) {}
	virtual void reload() {}

signals: 
	virtual void playingTrack(MusicTrack *track) = 0;

protected:
	Tracklist *_tracklist;
};


#endif /* DISPLAY_H */
