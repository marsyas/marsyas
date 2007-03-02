#ifndef PLOT_H
#define PLOT_H

#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include "SpectrogramData.h"

class Plot: public QwtPlot
{
    Q_OBJECT

private:
	QwtPlotSpectrogram *d_spectrogram;

public:
    Plot(QWidget * = NULL);

		void setData(SpectrogramData& data)
		{
			d_spectrogram->setData(data);
			replot();
		}

public slots:
    void showContour(bool on);
    void showSpectrogram(bool on);
};

#endif
