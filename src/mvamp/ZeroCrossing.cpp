#include "ZeroCrossing.h"

using std::string;
using std::vector;
using std::cerr;
using std::endl;


ZeroCrossing::ZeroCrossing(float inputSampleRate) :
  Plugin(inputSampleRate),
  m_stepSize(0),
  m_previousSample(0.0f),
  m_network(0)
{
  MarSystemManager mng;
  m_network = mng.create("Series", "series");
  m_network->addMarSystem(mng.create("RealvecSource", "src"));
  m_network->addMarSystem(mng.create("ZeroCrossings", "zcrs"));
}

ZeroCrossing::~ZeroCrossing()
{
  delete m_network;
  m_network = 0;
}

string
ZeroCrossing::getIdentifier() const
{
  return "zerocrossing";
}

string
ZeroCrossing::getName() const
{
  return "Zero Crossings";
}

string
ZeroCrossing::getDescription() const
{
  return "Detect and count zero crossing points";
}

string
ZeroCrossing::getMaker() const
{
  return "Marsyas Plugins";
}

int
ZeroCrossing::getPluginVersion() const
{
  return 2;
}

string
ZeroCrossing::getCopyright() const
{
  return "GPL v3 license";
}

bool
ZeroCrossing::initialise(size_t channels, size_t stepSize, size_t blockSize)
{
  if (channels < getMinChannelCount() || channels > getMaxChannelCount())
    return false;

  m_stepSize = std::min(stepSize, blockSize);
  m_network->updctrl("mrs_natural/inSamples", (int)m_stepSize);
  return true;
}

void
ZeroCrossing::reset()
{
  m_previousSample = 0.0f;
}

ZeroCrossing::OutputList
ZeroCrossing::getOutputDescriptors() const
{
  OutputList list;

  OutputDescriptor zc;
  zc.identifier = "counts";
  zc.name = "Zero Crossing Counts";
  zc.description = "The number of zero crossing points per processing block";
  zc.unit = "crossings";
  zc.hasFixedBinCount = true;
  zc.binCount = 1;
  zc.hasKnownExtents = false;
  zc.isQuantized = true;
  zc.quantizeStep = 1.0;
  zc.sampleType = OutputDescriptor::OneSamplePerStep;
  list.push_back(zc);

  return list;
}

ZeroCrossing::FeatureSet
ZeroCrossing::process(const float *const *inputBuffers,
                      Vamp::RealTime timestamp)
{
  if (m_stepSize == 0) {
    cerr << "ERROR: ZeroCrossing::process: "
         << "ZeroCrossing has not been initialised"
         << endl;
    return FeatureSet();
  }

  FeatureSet returnFeatures;

  size_t count = 0;

  // Stuff inputBuffers into a realvec
  realvec r(m_stepSize);
  for (size_t i = 0; i < m_stepSize; ++i) {
    r(i) = inputBuffers[0][i];
  }

  m_network->updctrl("RealvecSource/src/mrs_realvec/data", r);

  mrs_natural insamples;
  double val = 0.0;

  insamples = m_network->getctrl("mrs_natural/inSamples")->to<mrs_natural>();

  while (!m_network->getctrl("RealvecSource/src/mrs_bool/done")->to<mrs_bool>()) {
    m_network->tick();
    val = m_network->getctrl("mrs_realvec/processedData")->to<mrs_realvec>()(0) * insamples;
  }

  count = (int)val;


  Feature feature;
  feature.hasTimestamp = false;
  feature.values.push_back(count);

  returnFeatures[0].push_back(feature);

  return returnFeatures;
}

ZeroCrossing::FeatureSet
ZeroCrossing::getRemainingFeatures()
{
  return FeatureSet();
}

