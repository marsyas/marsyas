#ifndef SPECTROGRAMDATA_H
#define SPECTROGRAMDATA_H

#include <vector>
#include "realvec.h"
#include "qwt_raster_data.h"
#include "qwt_double_interval.h"

class SpectrogramData : public QwtRasterData
{
private:
	Marsyas::realvec spectrogram_;

public:
	SpectrogramData();
	~SpectrogramData();

	virtual QwtRasterData* copy() const;
	virtual QwtDoubleInterval range() const;
	virtual double value(double x, double y) const;

	void addSpectrum(const Marsyas::realvec* spectrum);
	void clear();
};

#endif
