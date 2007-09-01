#ifndef ITUNESPAGE_H
#define ITUNESPAGE_H

#include <QWidget>
#include "../Music/MusicCollection.h"

class iTunesPage : public QWidget
{
public:
	iTunesPage(QWidget *parent=0);
	~iTunesPage() {}
	void showEvent(QShowEvent *event);
	
private:
	MusicCollection *_library;
	QComboBox *_trainingCombo;
	QComboBox *_predictCombo;
};

#endif /* ITUNESPAGE_H */
