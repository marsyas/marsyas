#include "MarsyasBExtractCentroid.h"
using namespace Marsyas;
using std::string;
using std::vector;
using std::cerr;
using std::endl;


MarsyasBExtractCentroid::MarsyasBExtractCentroid(float inputSampleRate) :
  Plugin(inputSampleRate),
  m_stepSize(0),
  m_previousSample(0.0f),
  m_network(0)
{
  MarSystemManager mng;
  // Overall extraction and classification network
  m_network = mng.create("Series", "mainNetwork");

  // Build the overall feature calculation network
  MarSystem* featureNetwork = mng.create("Series", "featureNetwork");

  // Add a realvec as the source
  featureNetwork->addMarSystem(mng.create("RealvecSource", "src"));

  // Convert the data to mono
  featureNetwork->addMarSystem(mng.create("Stereo2Mono", "m2s"));

  // Setup the feature extractor
  MarSystem* featExtractor = mng.create("TimbreFeatures", "featExtractor");

  featExtractor->updctrl("mrs_string/enableSPChild", "Centroid/cntrd");
  featureNetwork->addMarSystem(featExtractor);

  // Add the featureNetwork to the main network
  m_network->addMarSystem(featureNetwork);

}

MarsyasBExtractCentroid::~MarsyasBExtractCentroid()
{
  delete m_network;
  m_network = 0;
}

string
MarsyasBExtractCentroid::getIdentifier() const
{
  return "marsyas_bextract_centroid";
}

string
MarsyasBExtractCentroid::getName() const
{
  return "Marsyas - Batch Feature Extract - Centroid";
}

string
MarsyasBExtractCentroid::getDescription() const
{
  return "Marsyas - Batch Feature Extract - Centroid";
}

string
MarsyasBExtractCentroid::getMaker() const
{
  return "Marsyas Plugins";
}

int
MarsyasBExtractCentroid::getPluginVersion() const
{
  return 2;
}

string
MarsyasBExtractCentroid::getCopyright() const
{
  return "GPL v3 license";
}

bool
MarsyasBExtractCentroid::initialise(size_t channels, size_t stepSize, size_t blockSize)
{
  if (channels < getMinChannelCount() ||
      channels > getMaxChannelCount()) return false;

  m_stepSize = std::min(stepSize, blockSize);

  // src has to be configured with hopSize frame length in case a
  // ShiftInput is used in the feature extraction network
  m_network->updctrl("mrs_natural/inSamples", (int)stepSize);

  // Link the "done" control of the input RealvecSource to the "done"
  // control of the network
  m_network->linkctrl("mrs_bool/done", "Series/featureNetwork/RealvecSource/src/mrs_bool/done");

  // Update the window size
  //featureNetwork->updctrl("TimbreFeatures/featExtractor/mrs_natural/winSize", (int)blockSize);
  m_network->updctrl("Series/featureNetwork/TimbreFeatures/featExtractor/mrs_natural/winSize", (int)blockSize);

  return true;
}

void
MarsyasBExtractCentroid::reset()
{
  m_previousSample = 0.0f;
}

MarsyasBExtractCentroid::OutputList
MarsyasBExtractCentroid::getOutputDescriptors() const
{
  OutputList list;

  OutputDescriptor output;
  output.identifier = "centroid_value";
  output.name = "Centroid value";
  output.description = "The value of the centroid";
  output.unit = "value";
  output.hasFixedBinCount = true;
  output.binCount = 1;
  output.hasKnownExtents = false;
  output.isQuantized = false;
  output.sampleType = OutputDescriptor::OneSamplePerStep;
  list.push_back(output);

  return list;
}

MarsyasBExtractCentroid::FeatureSet
MarsyasBExtractCentroid::process(const float *const *inputBuffers,
                                 Vamp::RealTime timestamp)
{
  if (m_stepSize == 0) {
    cerr << "ERROR: MarsyasBExtractSFM::process: "
         << "MarsyasBExtractSFM has not been initialised"
         << endl;
    return FeatureSet();
  }

  // The feature we are going to return to the host
  FeatureSet returnFeatures;
  Feature feature;
  feature.hasTimestamp = false;

  // Stuff inputBuffers into a realvec
  realvec r(m_stepSize);
  for (size_t i = 0; i < m_stepSize; ++i) {
    r(i) = inputBuffers[0][i];
  }

  // Load the network with the data
  m_network->updctrl("Series/featureNetwork/RealvecSource/src/mrs_realvec/data", r);

  // Tick the network once, which will process one window of data
  m_network->tick();

  // Get the data out of the network
  realvec output_realvec = m_network->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
  for (int i = 0; i < output_realvec.getRows(); ++i) {
    feature.values.push_back(output_realvec(i));
  }

  returnFeatures[0].push_back(feature);

  return returnFeatures;
}

MarsyasBExtractCentroid::FeatureSet
MarsyasBExtractCentroid::getRemainingFeatures()
{
  return FeatureSet();
}

