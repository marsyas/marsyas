#include "MarsyasBExtractLPCC.h"
using namespace Marsyas;
using std::string;
using std::vector;
using std::cerr;
using std::endl;


MarsyasBExtractLPCC::MarsyasBExtractLPCC(float inputSampleRate) :
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
  featExtractor->updctrl("mrs_string/enableLPCChild", "Series/lpccbranch");
  featureNetwork->addMarSystem(featExtractor);

  // Add the featureNetwork to the main network
  m_network->addMarSystem(featureNetwork);

}

MarsyasBExtractLPCC::~MarsyasBExtractLPCC()
{
  delete m_network;
  m_network = 0;
}

string
MarsyasBExtractLPCC::getIdentifier() const
{
  return "marsyas_bextract_lpcc";
}

string
MarsyasBExtractLPCC::getName() const
{
  return "Marsyas - Batch Feature Extract - Linear Prediction Cepstral Coefficients";
}

string
MarsyasBExtractLPCC::getDescription() const
{
  return "Marsyas - Batch Feature Extract - Linear Prediction Cepstral Coefficients";
}

string
MarsyasBExtractLPCC::getMaker() const
{
  return "Marsyas Plugins";
}

int
MarsyasBExtractLPCC::getPluginVersion() const
{
  return 2;
}

string
MarsyasBExtractLPCC::getCopyright() const
{
  return "GPL v3 license";
}

bool
MarsyasBExtractLPCC::initialise(size_t channels, size_t stepSize, size_t blockSize)
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
  m_network->updctrl("Series/featureNetwork/TimbreFeatures/featExtractor/mrs_natural/winSize", (int)blockSize);

  return true;
}

void
MarsyasBExtractLPCC::reset()
{
  m_previousSample = 0.0f;
}

MarsyasBExtractLPCC::OutputList
MarsyasBExtractLPCC::getOutputDescriptors() const
{
  OutputList list;

  OutputDescriptor output;
  output.identifier = "linear_prediction_cepstral_coefficients";
  output.name = "Linear Prediction Cepstral Coefficients";
  output.description = "The value of the Linear Prediction Cepstral Coefficients";
  output.unit = "value";
  output.hasFixedBinCount = true;
  output.binCount = 12;
  output.hasKnownExtents = false;
  output.isQuantized = false;
  output.sampleType = OutputDescriptor::OneSamplePerStep;
  list.push_back(output);

  return list;
}

MarsyasBExtractLPCC::FeatureSet
MarsyasBExtractLPCC::process(const float *const *inputBuffers,
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

MarsyasBExtractLPCC::FeatureSet
MarsyasBExtractLPCC::getRemainingFeatures()
{
  return FeatureSet();
}

