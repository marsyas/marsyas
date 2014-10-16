/*
** Copyright (C) 2014 George Tzanetakis <gtzan@cs.uvic.ca>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include "vamp_marsyas_plugin.hpp"

#include <marsyas/system/MarSystemManager.h>
#include <marsyas/script/script.h>
#include <marsyas/FileName.h>

#include <vamp-sdk/PluginAdapter.h>

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <cassert>

//FIXME: Only on POSIX:
#include <errno.h>
#include <dirent.h>

using namespace std;

namespace Marsyas {

static MarSystemManager *get_marsystem_manager()
{
  static MarSystemManager *manager = 0;
  if (!manager)
  {
    manager = new MarSystemManager;
  }
  return manager;
}

class VampPluginAdapter: public Vamp::PluginAdapterBase
{
  string m_script_filename;
  MarSystem *m_prototype;

public:
  VampPluginAdapter(const string & script_filename):
    m_script_filename(script_filename)
  {
    ScriptTranslator translator(get_marsystem_manager());
    m_prototype = translator.translateFile(script_filename);
    if (!m_prototype)
    {
      cerr << "ERROR: Failed to create prototype for script: "
           << script_filename << endl;
    }
  }

  ~VampPluginAdapter()
  {
    delete m_prototype;
  }

private:
  virtual Vamp::Plugin * createPlugin (float inputSampleRate)
  {
    return new Marsyas::VampPlugin(m_script_filename, m_prototype, inputSampleRate);
  }
};

VampPlugin::VampPlugin(const string & script, MarSystem * prototype, float inputSampleRate):
    Vamp::Plugin(inputSampleRate),
    m_script_filename(script),
    m_prototype(prototype),
    m_input_sample_rate(inputSampleRate),
    m_channels(0),
    m_block_size(0),
    m_step_size(0),
    m_system(0)
{
  discoverParameters();
}

void VampPlugin::discoverParameters()
{
  if (!m_prototype)
  {
    cerr << "WARNING: Missing prototype!" << endl;
    return;
  }

  const std::map<std::string, MarControlPtr> & controls = m_prototype->controls();
  std::map<std::string, MarControlPtr>::const_iterator it;
  for (it = controls.begin(); it != controls.end(); ++it)
  {
    const MarControlPtr & control = it->second;

    assert(!control.isInvalid());

    if (!control->isPublic())
      continue;

    ParameterDescriptor param;
    if (control->hasType<mrs_real>())
    {
      param.defaultValue = (float) control->to<mrs_real>();
    }
    else if (control->hasType<mrs_natural>())
    {
      param.isQuantized = true;
      param.quantizeStep = 1.0f;
      param.defaultValue = (float) control->to<mrs_natural>();
    }
    else
    {
      continue;
    }
    param.identifier = param.name = control->id();
    param.minValue = -10000.f;
    param.maxValue = 10000.f;

    m_param_descriptors.push_back(param);

    m_params[param.identifier] = param.defaultValue;
  }
}

VampPlugin::~VampPlugin()
{
  delete m_system;
}

string VampPlugin::getIdentifier() const
{
  string id(m_script_filename);
  replace(id.begin(), id.end(), '/', '.');
  replace(id.begin(), id.end(), ' ', '.');
  return string("marsyas.") + id;
}

string VampPlugin::getName() const
{
    return string("Marsyas: ") + m_script_filename;
}

string VampPlugin::getDescription() const
{
    return "Use Marsyas script for audio processing.";
}


string VampPlugin::getMaker() const
{
    return "Jakob Leben";
}

string VampPlugin::getCopyright() const
{
    return "Copyright 2014 Jakob Leben";
}

int VampPlugin::getPluginVersion() const
{
    return 1;
}

Vamp::Plugin::InputDomain VampPlugin::getInputDomain() const
{
    return TimeDomain;
}

size_t VampPlugin::getMinChannelCount() const
{
    return 1;
}

size_t VampPlugin::getMaxChannelCount() const
{
    return 100;
}

size_t VampPlugin::getPreferredBlockSize() const
{
    return 512;
}

size_t VampPlugin::getPreferredStepSize() const
{
    return 512;
}

VampPlugin::ParameterList VampPlugin::getParameterDescriptors() const
{
  return m_param_descriptors;
}

float VampPlugin::getParameter(std::string id) const
{
  map<string, float>::const_iterator it = m_params.find(id);
  if (it == m_params.end())
  {
    cerr << "Invalid parameter id: " << id << endl;
    return 0.f;
  }

  return it->second;
}

void VampPlugin::setParameter(std::string id, float value)
{
  m_params[id] = value;
}

Vamp::Plugin::OutputList VampPlugin::getOutputDescriptors() const
{
    OutputList outputs;

    OutputDescriptor out;
    out.identifier = "output";
    out.name = "Output";
    out.sampleType = OutputDescriptor::FixedSampleRate;
    out.hasFixedBinCount = true;
    if (m_system)
    {
#if 1
      mrs_natural in_rate = m_system->getControl("mrs_natural/inSamples")->to<mrs_natural>();
      mrs_natural out_rate = m_system->getControl("mrs_natural/onSamples")->to<mrs_natural>();
      float input_output_rate_ratio = (float) out_rate / (float) in_rate;
      out.sampleRate = m_input_sample_rate * input_output_rate_ratio;
#endif
      out.sampleRate = m_input_sample_rate / (m_step_size * out_rate);
      out.binCount = m_system->getControl("mrs_natural/onObservations")->to<mrs_natural>();
    }

    outputs.push_back(out);

    return outputs;
}

bool VampPlugin::initialise(size_t channels, size_t stepSize, size_t blockSize)
{
  delete m_system;

  m_channels = channels;
  m_step_size = stepSize;
  m_block_size = blockSize;

  {
    ScriptTranslator translator(get_marsystem_manager());
    m_system = translator.translateFile(m_script_filename);
  }

  if (!m_system)
    return 0;

  cout << "Input format = "
       << channels
       << " / " << blockSize
       << " / " << stepSize
       << " @ " << m_input_sample_rate
       << endl;

  m_system->updControl("mrs_real/israte", (mrs_real) m_input_sample_rate);
  m_system->updControl("mrs_natural/inObservations", (mrs_natural) channels);
  m_system->updControl("mrs_natural/inSamples", (mrs_natural) blockSize);

  applyParameters(m_system);

  mrs_natural m_out_observations = m_system->getControl("mrs_natural/onObservations")->to<mrs_natural>();
  mrs_natural m_out_samples = m_system->getControl("mrs_natural/onSamples")->to<mrs_natural>();

  cout << "Output format = "
       << m_out_observations
       << " / " << m_out_samples
       << " @ " << m_system->getControl("mrs_real/osrate")->to<mrs_real>()
       << endl;

  m_input.create(m_channels, m_block_size);
  m_output.create(m_out_observations, m_out_samples);

  return true;
}

void VampPlugin::applyParameters(MarSystem *system)
{
  map<string, float>::const_iterator it;
  for (it = m_params.begin(); it != m_params.end(); ++it)
  {
    const string & id = it->first;
    float value = it->second;

    MarControlPtr control = system->control(id);

    if (control.isInvalid())
    {
      cerr << "Invalid parameter id: " << id << endl;
      continue;
    }

    if (control->hasType<mrs_real>())
    {
      control->setValue((mrs_real) value);
    }
    else if (control->hasType<mrs_natural>())
    {
      control->setValue((mrs_natural) value);
    }
    else
    {
      cerr << "Invalid type for parameter id: " << id << endl;
    }
  }
}

void VampPlugin::reset()
{
  initialise(m_channels, m_step_size, m_block_size);
}

Vamp::Plugin::FeatureSet VampPlugin::process(const float *const *inputBuffers,
                                             Vamp::RealTime)
{
  Vamp::Plugin::FeatureSet feature_set;
  Vamp::Plugin::FeatureList feature_list;

  for(mrs_natural c = 0; c < (mrs_natural) m_channels; ++c)
  {
    for(mrs_natural s = 0; s < (mrs_natural) m_block_size; ++s)
    {
      m_input(c,s) = inputBuffers[c][s];
    }
  }

  m_system->process(m_input, m_output);

  for(mrs_natural s = 0; s < m_output.getCols(); ++s)
  {
    Feature features;
    for(mrs_natural o = 0; o < m_output.getRows(); ++o)
    {
      features.values.push_back( (float) m_output(o,s) );
    }

    feature_list.push_back(features);
  }

  feature_set[0] = feature_list;

  return feature_set;
}

Vamp::Plugin::FeatureSet VampPlugin::getRemainingFeatures()
{
  Vamp::Plugin::FeatureSet feature_set;
  return feature_set;
}

} // namespace Marsyas


int get_dir_entries(string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL)
    {
        cerr << "Error(" << errno << ") opening directory " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL)
    {
      string dir_name(dirp->d_name);
      if (dir_name != "." && dir_name != "..")
        files.push_back(dir + '/' + dir_name);
    }
    closedir(dp);
    return 0;
}

std::vector<string> find_scripts()
{
  auto script_location = Marsyas::FileName::userAppDataDir();
  script_location.append("marsyas/scripts/vamp");

  std::vector<string> filenames;
  get_dir_entries(script_location.fullname(), filenames);

  return filenames;
}

const VampPluginDescriptor *vampGetPluginDescriptor
(unsigned int version, unsigned int index)
{
  if (version < 1) return 0;

  static bool discovery_done = false;
  static std::vector<Marsyas::VampPluginAdapter*> plugins;

  if (!discovery_done)
  {
    cout << "Discovering Marsyas scripts..." << endl;
    std::vector<string> script_filenames( find_scripts() );
    for(const string & script_filename : script_filenames)
    {
      cout << "Found Marsyas script: " << script_filename << endl;
      plugins.push_back(new Marsyas::VampPluginAdapter(script_filename));
    }
    cout << "Discovering Marsyas scripts done." << endl;

    discovery_done = true;
  }

  if (index < plugins.size())
  {
    return plugins[index]->getDescriptor();
  }

  return 0;
}

