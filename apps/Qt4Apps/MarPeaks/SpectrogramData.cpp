#include "SpectrogramData.h"
#include "qwt_double_rect.h"

using namespace std;
using namespace Marsyas;

SpectrogramData::SpectrogramData() : 
					QwtRasterData(QwtDoubleRect(0.0, 0.0, 1000.0, 512.0))
{
}

SpectrogramData::~SpectrogramData()
{
}

QwtRasterData*
SpectrogramData::copy() const
{
	SpectrogramData* spectrogramData = new SpectrogramData();
	spectrogramData->spectrogram_ = spectrogram_;

	return spectrogramData;
}

QwtDoubleInterval
SpectrogramData::range() const
{
	return QwtDoubleInterval(-100.0, 0.0);
}

double
SpectrogramData::value(double x, double y) const
{
	int time= (int)x;
	int freq = (int)y;
	
	if(time < spectrogram_.getCols())
		if(freq < spectrogram_.getRows())
		{
			return (double)spectrogram_(freq, time);
		}
		
	return -100.0;
}

void 
SpectrogramData::addSpectrum(const realvec* spectrum)
{
	//stretch current spectrogram with new data
	mrs_natural freqBins = spectrum->getRows();
	mrs_natural timeLength = spectrogram_.getCols()+1;
	spectrogram_.stretch(freqBins, timeLength);
	
	//fill it with the received spectrum data
	for(mrs_natural k=0; k < freqBins; ++k)
	{
		spectrogram_(k, timeLength-1) = (*spectrum)(k);
	}

	//spectrogram_ = *spectrum;
}

void
SpectrogramData::clear()
{
	spectrogram_.create(0);//clear
}