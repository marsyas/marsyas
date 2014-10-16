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

#ifndef VAMP_MARSYAS_PLUGIN_INCLUDED
#define VAMP_MARSYAS_PLUGIN_INCLUDED

#include <vamp-sdk/Plugin.h>
#include <marsyas/system/MarSystem.h>
#include <map>

namespace Marsyas {

class VampPlugin : public Vamp::Plugin
{

public:
    VampPlugin(const std::string & script, MarSystem *prototype, float inputSampleRate);
    virtual ~VampPlugin();

    bool initialise(size_t channels, size_t stepSize, size_t blockSize);
    void reset();

    std::string getIdentifier() const;
    std::string getName() const;
    std::string getDescription() const;
    std::string getMaker() const;
    std::string getCopyright() const;
    int getPluginVersion() const;

    InputDomain getInputDomain() const;
    size_t getMinChannelCount() const;
    size_t getMaxChannelCount() const;
    size_t getPreferredBlockSize() const;
    size_t getPreferredStepSize() const;

    OutputList getOutputDescriptors() const;

    ParameterList getParameterDescriptors() const;
    float getParameter(std::string) const;
    void setParameter(std::string, float);

    FeatureSet process(const float *const *inputBuffers,
                       Vamp::RealTime timestamp);

    FeatureSet getRemainingFeatures();

private:
    void discoverParameters();
    void applyParameters(MarSystem *system);

    std::string m_script_filename;
    MarSystem * m_prototype;

    ParameterList m_param_descriptors;
    std::map<std::string, float> m_params;
    float m_input_sample_rate;
    size_t m_channels;
    size_t m_block_size;
    size_t m_step_size;
    MarSystem *m_system;
    realvec m_input;
    realvec m_output;
};

} // namespace Marsyas

#endif // VAMP_MARSYAS_PLUGIN_INCLUDED
