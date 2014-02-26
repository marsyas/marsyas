/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "Cascade.h"


using std::ostringstream;
using std::vector;

using namespace Marsyas;

Cascade::Cascade(mrs_string name):
  MarSystem("Cascade", name),
  m_valid_output(false)
{
  isComposite_ = true;
}

Cascade::Cascade(const Cascade & other):
  MarSystem(other),
  m_valid_output(false)
{
}

Cascade::~Cascade()
{
}

MarSystem*
Cascade::clone() const
{
  return new Cascade(*this);
}

void
Cascade::myUpdate(MarControlPtr sender)
{
  child_count_t child_count = marsystems_.size();

  m_valid_output = true;

  if (!child_count)
  {
    MarSystem::myUpdate(sender);
    return;
  }

  m_child_info.resize(child_count);

  ostringstream out_obs_names;
  mrs_natural out_observations = 0;
  mrs_natural out_samples = 0;
  mrs_real out_sample_rate = 0.0;

  MarControlPtr observations = getControl("mrs_natural/inObservations");
  MarControlPtr samples = getControl("mrs_natural/inSamples");
  MarControlPtr rate = getControl("mrs_real/israte");
  MarControlPtr obs_names = getControl("mrs_string/inObsNames");

  for (child_count_t i=0; i < child_count; ++i)
  {
    MarSystem * child = marsystems_[i];
    system_info & child_info = m_child_info[i];

    child->setctrl("mrs_natural/inObservations", observations);
    child->setctrl("mrs_natural/inSamples", samples);
    child->setctrl("mrs_real/israte", rate);
    child->setctrl("mrs_string/inObsNames", obs_names);
    observations = child->getControl("mrs_natural/onObservations");
    samples = child->getControl("mrs_natural/onSamples");
    rate = child->getControl("mrs_real/osrate");
    obs_names = child->getControl("mrs_string/onObsNames");

    mrs_natural child_out_observations = observations->to<mrs_natural>();
    mrs_natural child_out_samples = samples->to<mrs_natural>();
    mrs_natural child_out_sr = rate->to<mrs_real>();

    child_info.buffer.create(child_out_observations, child_out_samples);

    if (i == 0)
    {
      out_samples = child_out_samples;
      out_sample_rate = child_out_sr;
    }
    else if ( (out_samples != child_out_samples) ||
              (out_sample_rate != child_out_sr) )
    {
      m_valid_output = false;
    }

    out_observations += child_out_observations;
    out_obs_names << obs_names;
  }

  if (!m_valid_output)
    MRSWARN("Cascade: children have incompatible output formats. Output disabled.");

  //forward flow propagation
  setctrl(ctrl_onSamples_, out_samples);
  setctrl(ctrl_onObservations_, out_observations);
  setctrl(ctrl_osrate_, out_sample_rate);
  setctrl(ctrl_onObsNames_, out_obs_names.str());
}

void
Cascade::myProcess(realvec& in, realvec& out)
{
  child_count_t child_count = marsystems_.size();

  if (!m_valid_output)
    return;

  if (!child_count)
  {
    out = in;
    return;
  }

  if (child_count == 1)
  {
    marsystems_[0]->process(in, out);
    return;
  }

  mrs_natural out_obs_offset = 0;

  {
    realvec & child_out = m_child_info[0].buffer;

    marsystems_[0]->process(in, child_out);

    for (mrs_natural o = 0; o < child_out.getRows(); o++)
      for (mrs_natural t = 0; t < onSamples_; t++)
        out(o, t) = child_out(o, t);

    out_obs_offset = child_out.getRows();
  }

  for (child_count_t i = 1; i < child_count; ++i)
  {
    realvec & child_out = m_child_info[i].buffer;
    realvec & prev_child_out = m_child_info[i-1].buffer;

    marsystems_[i]->process(prev_child_out, child_out);

    for (mrs_natural o = 0; o < child_out.getRows(); o++)
      for (mrs_natural t = 0; t < onSamples_; t++)
        out(out_obs_offset + o, t) = child_out(o, t);

    out_obs_offset += child_out.getRows();
  }
}
