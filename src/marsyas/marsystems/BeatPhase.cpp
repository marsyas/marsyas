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

#include "BeatPhase.h"
#include "../common_source.h"

using std::ostringstream;
using std::cout;
using std::endl;

using namespace Marsyas;

BeatPhase::BeatPhase(mrs_string name):MarSystem("BeatPhase", name)
{
  addControls();
  sampleCount_ = 0;
  current_beat_location_ = 0.0;
  pinSamples_ = 0;


}

BeatPhase::BeatPhase(const BeatPhase& a) : MarSystem(a)
{
  ctrl_tempo_candidates_ = getctrl("mrs_realvec/tempo_candidates");

  ctrl_tempos_ = getctrl("mrs_realvec/tempos");
  ctrl_prev_tempos_ = getctrl("mrs_realvec/prev_tempos");
  ctrl_temposcores_ = getctrl("mrs_realvec/tempo_scores");
  ctrl_prior_tempo_scores_ = getctrl("mrs_realvec/prior_tempo_scores");

  ctrl_phase_tempo_ = getctrl("mrs_real/phase_tempo");
  ctrl_ground_truth_tempo_ = getctrl("mrs_real/ground_truth_tempo");
  ctrl_beats_ = getctrl("mrs_realvec/beats");
  ctrl_bhopSize_ = getctrl("mrs_natural/bhopSize");
  ctrl_bwinSize_ = getctrl("mrs_natural/bwinSize");
  ctrl_hopSize_ = getctrl("mrs_natural/hopSize");
  ctrl_winSize_ = getctrl("mrs_natural/winSize");
  
  ctrl_timeDomain_ = getctrl("mrs_realvec/timeDomain");
  ctrl_nCandidates_ = getctrl("mrs_natural/nCandidates");
  ctrl_beatOutput_ = getctrl("mrs_realvec/beatOutput");
  ctrl_factor_ = getctrl("mrs_real/factor");

  sampleCount_ = 0;
  current_beat_location_ = 0.0;
  pinSamples_ = 0;

}

BeatPhase::~BeatPhase()
{
}

MarSystem*
BeatPhase::clone() const
{
  return new BeatPhase(*this);
}

void
BeatPhase::addControls()
{
  mrs_natural nCandidates = 10;

  //Add specific controls needed by this MarSystem.
  addctrl("mrs_realvec/tempo_candidates", realvec(nCandidates), ctrl_tempo_candidates_);
  addctrl("mrs_realvec/tempos", realvec(nCandidates), ctrl_tempos_);
  addctrl("mrs_realvec/prev_tempos", realvec(nCandidates), ctrl_prev_tempos_);
  addctrl("mrs_realvec/tempo_scores", realvec(nCandidates), ctrl_temposcores_);
  addctrl("mrs_realvec/prior_tempo_scores", realvec(nCandidates), ctrl_prior_tempo_scores_);
  

  addctrl("mrs_real/phase_tempo", 100.0, ctrl_phase_tempo_);
  addctrl("mrs_real/ground_truth_tempo", 100.0, ctrl_ground_truth_tempo_);
  addctrl("mrs_realvec/beats", realvec(), ctrl_beats_);
  addctrl("mrs_natural/hopSize", 64, ctrl_hopSize_);
  addctrl("mrs_natural/winSize", 128, ctrl_winSize_);
  addctrl("mrs_natural/bhopSize", 64, ctrl_bhopSize_);
  addctrl("mrs_natural/bwinSize", 1024, ctrl_bwinSize_);
  addctrl("mrs_realvec/timeDomain", realvec(), ctrl_timeDomain_);
  addctrl("mrs_natural/nCandidates", nCandidates, ctrl_nCandidates_);
  setctrlState("mrs_natural/nCandidates", true);
  addctrl("mrs_realvec/beatOutput", realvec(), ctrl_beatOutput_);
  addctrl("mrs_real/factor", 4.0, ctrl_factor_);

  MarControlAccessor accpt(ctrl_prev_tempos_);
  mrs_realvec& prev_tempos = accpt.to<mrs_realvec>();
  prev_tempos.setval(100.0);

}

void
BeatPhase::myUpdate(MarControlPtr sender)
{
  // no need to do anything BeatPhase-specific in myUpdate
  MarSystem::myUpdate(sender);


  inSamples_ = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
  mrs_natural nCandidates = getctrl("mrs_natural/nCandidates")->to<mrs_natural>();
  factor_ = getctrl("mrs_real/factor")->to<mrs_real>();


  MarControlAccessor acc_t(ctrl_tempos_);
  mrs_realvec& tempos = acc_t.to<mrs_realvec>();
  tempos.stretch(nCandidates);

  MarControlAccessor acc_ts(ctrl_temposcores_);
  mrs_realvec& temposcores = acc_ts.to<mrs_realvec>();
  temposcores.stretch(nCandidates);

  MarControlAccessor acc_pts(ctrl_prior_tempo_scores_);
  mrs_realvec& prior_tempo_scores = acc_pts.to<mrs_realvec>();
  prior_tempo_scores.stretch(nCandidates);
  prior_tempo_scores.setval(1.0/nCandidates);


  MarControlAccessor acc_tc(ctrl_tempo_candidates_);
  mrs_realvec& tempocandidates = acc_tc.to<mrs_realvec>();
  tempocandidates.stretch(nCandidates * 2);


  if (pinSamples_ != inSamples_)
  {
    {
      MarControlAccessor acc(ctrl_beats_);
      mrs_realvec& beats = acc.to<mrs_realvec>();
      beats.create(inSamples_);

      // Output all the beats that are detected via a MarControl
      MarControlAccessor beatOutputAcc(ctrl_beatOutput_);
      mrs_realvec& beatOutput = beatOutputAcc.to<mrs_realvec>();
      beatOutput.create(inSamples_);
    }
  }

  prev_tempo_ = 100; 
  pinSamples_ = inSamples_;

  average_beat_height_ = 0.1; 

}


void
BeatPhase::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;



  mrs_real ground_truth_tempo = ctrl_ground_truth_tempo_->to<mrs_real>();
  // used for evaluation experiments


  // The tempo candidates and their scores
  MarControlAccessor acctc(ctrl_tempo_candidates_);
  mrs_realvec& tempo_candidates = acctc.to<mrs_realvec>();


  MarControlAccessor acct(ctrl_tempos_);
  mrs_realvec& tempos = acct.to<mrs_realvec>();

  MarControlAccessor accpt(ctrl_prev_tempos_);
  mrs_realvec& prev_tempos = accpt.to<mrs_realvec>();
  
  
  MarControlAccessor accts(ctrl_temposcores_);
  mrs_realvec& tempo_scores = accts.to<mrs_realvec>();


   MarControlAccessor accpts(ctrl_prior_tempo_scores_);  
   mrs_realvec& prior_tempo_scores = accpts.to<mrs_realvec>();


  // Demultiplex candidates and scores
  for (int i=0; i < tempo_candidates.getSize()/2; i++)
  {
    tempos(i) = tempo_candidates(2*i+1) / factor_;
    tempo_scores(i) = tempo_candidates(2*i);
  }

  // normalize to pdf
  tempo_scores /= tempo_scores.sum();



  // holds the tempo scores based on cross-correlation with pulse train
  mrs_realvec onset_scores;
  onset_scores.create(tempo_scores.getSize());
  // holds the best matching phase for each tempo candidate
  mrs_realvec tempo_phases;
  tempo_phases.create(tempo_scores.getSize());

  /*
  // make sure the tempo candidates are reasonable
  for (int k=0; k < tempos.getSize(); k++)
  {
    if (tempos(k) < 50.0)
  	tempos(k) = 0;
    if (tempos(k) > 200)
  	tempos(k) = 0;

  }
  */

  // The winSize and hopSize of the onset strength function
  // needed to output correct beat location times
  // mrs_natural bwinSize = ctrl_bwinSize_->to<mrs_natural>();
  mrs_natural bhopSize = ctrl_bhopSize_->to<mrs_natural>();
  mrs_natural bwinSize = ctrl_bwinSize_->to<mrs_natural>();
  mrs_natural hopSize = ctrl_hopSize_->to<mrs_natural>();
  mrs_natural winSize = ctrl_winSize_->to<mrs_natural>();

  
  mrs_real oss_hop_win_ratio = winSize / hopSize;

  // std::cout << "OSS HOP WIN RATIO " << oss_hop_win_ratio << std::endl;
  // std::cout << "OSS rate" << osrate_ << std::endl;
  MarControlAccessor acc(ctrl_beats_);
  mrs_realvec& beats = acc.to<mrs_realvec>();


  for (o=0; o < inObservations_; o++)
  {
    for (t = 0; t < inSamples_; t++)
    {
      out (o,t) = in(o,t);
      beats (o,t) = 0.0;
    }
  }

  in.normMaxMin();

  for (o=0; o < inObservations_; o++)
  {
    for (t = 0; t < inSamples_; t++)
    {
      if (in(0,t) < 0.5 * average_beat_height_)
	in(0,t) =0.0; 
    }
  }

  mrs_real tempo;
  mrs_real period;
  mrs_natural phase;
  mrs_real cross_correlation = 0.0;
  mrs_real max_crco=0.0;
  mrs_natural max_phase = 0;
  mrs_realvec phase_correlations;


  // loop for cross-correlating
  // pulse trains with onset strength function
  // for each tempo shift the pulse train until the best match is found
  // tempo_scores holds the values of the cross-correlation
  // and onset_scores holds the variance of cross-correlation among different phases
  // for a particular tempo candidate
  for (o=0; o < inObservations_; o++)
  {
    for (int k=0; k < tempos.getSize(); k++)
    {
      max_crco = 0.0;

      tempo = tempos(k);
      if (tempo < 1)
	period = 0;
      else
	period = (oss_hop_win_ratio * osrate_) * 60.0 / tempo; // flux hopSize is half the winSize
      
      mrs_natural period_int = (mrs_natural)(period+0.5);

      if (period_int > 1)
      {
        //cout<<tempo<<"\t"<<period_int;
        phase_correlations.create( period_int );

        for (phase=inSamples_-1; phase > inSamples_-1-period_int; phase--)
        {
          cross_correlation = 0.0;
          // correlate with pulse train with half-beats and double beats
          for (int b=0; b < 4; b++)
          {
            mrs_natural temp_t;
            temp_t = phase - b * period_int;

            // 4 beats
            if (temp_t >= 0) {
              cross_correlation += in(o, temp_t);
              //cout<<"\t"<<temp_t;
            }

            // slow down by 2.0
            temp_t = phase - b * period_int * 2;
            if (temp_t >= 0) {
              cross_correlation += 0.5 * in(o, temp_t);
              //cout<<"\t"<<temp_t;
            }

            // slow down by 3
            temp_t = phase - b * period_int * 3 / 2;
            if (temp_t >= 0) {
              cross_correlation += 0.5 * in(o, temp_t);
              //cout<<"\t"<<temp_t;
            }
            //cout<<endl;


          }

          // quarter beats
          /* for (int b = 0; b < 8; b++)
          {
            mrs_natural temp_t;
            temp_t = phase - b * 0.25 * period;
            if (temp_t >= 0)
            {
          	  cross_correlation += (0.2 * in(o, temp_t));
            }
          }
          */

          phase_correlations(inSamples_-1-phase) = cross_correlation;
          if (cross_correlation > max_crco)
          {
            max_crco = cross_correlation;
            max_phase = phase;
          }

        }
        onset_scores(k) = phase_correlations.var();
        tempo_scores(k) = max_crco;
        tempo_phases(k) = max_phase;
        beats.setval(0.0);

        //printf("\t%f\t%f", max_crco, onset_scores(k));
        //cout<<endl;
      }
    }
  }



  // renormalize scores
  onset_scores /= onset_scores.sum();
  tempo_scores /= tempo_scores.sum();

  // combine the cross-correlation score with the variance
  for (int i=0; i < tempo_scores.getSize(); i++)
    tempo_scores(i) = onset_scores(i) + tempo_scores(i);

  // renormalize
  tempo_scores /= tempo_scores.sum();



  

  mrs_natural nTempos = tempos.getSize();
  mrs_realvec transition_probs(nTempos);

  
  for (int i=0; i < nTempos; i++)
    {
      mrs_real transition_prob_sum = 0.0;       
      for (int j=0 ; j < nTempos; j++)
	{
	  transition_probs(j) = exp(-6.0 * fabs(tempos(i)/prev_tempos(j) - 1));
	  // transition_probs(j) = 1.0 / nTempos; 
	  // transition_prob_sum += (transition_probs(j) * prior_tempo_scores(j));
	  transition_prob_sum = transition_probs.maxval() * prior_tempo_scores(j);
	}
      transition_prob_sum *= tempo_scores(i);
      prior_tempo_scores(i) = transition_prob_sum;
      prior_tempo_scores /= prior_tempo_scores.sum();      
    }

  mrs_real prior_max_score = 0.0;
  mrs_natural prior_argmax_score = 0;
  for (int i=0; i < nTempos; i++)
   {
     if (prior_tempo_scores(i) > prior_max_score)
       {
	 prior_max_score = prior_tempo_scores(i);
	 prior_argmax_score = i;
       }
   }

  
  // std::cout << "PRIOR_ARGMAX_SCORE" << prior_argmax_score << std::endl;
  mrs_real prob_max_tempo = tempos(prior_argmax_score);
  // prob_max_tempo = ground_truth_tempo;
  

 
  // pick the maximum scoring tempo candidate
  mrs_real max_score = 0.0;
  int max_i=0;
  for (int i= 0; i < tempos.getSize(); i++)
  {
    if (tempo_scores(i) > max_score)
    {
      max_score = tempo_scores(i);
      max_i = i;
    }
  }


  // instead of the max local tempo select the probabilistic max tempo 
  mrs_real beat_period;

  mrs_natural tempo_i;


  // return the best tempo candidate and the
  // corresponding score in both the tempo vector
  // as well as the control phase_tempo
  
  mrs_real swap_tempo = tempos(0);
  mrs_real swap_score = tempo_scores(0);
  mrs_real swap_phase = tempo_phases(0); 
  tempos(0) = tempos(max_i);
  tempo_scores(0) = tempo_scores(max_i);
  tempo_phases(0) = tempo_phases(max_i); 
  
  if (max_i != 0) {
    tempos(max_i) = swap_tempo;
    tempo_scores(max_i) = swap_score;
    tempo_phases(max_i) = swap_phase;
  }

  max_score = 0.0;
  max_i=0;
  for (int i= 1; i < tempos.getSize(); i++)
  {
    if (tempo_scores(i) > max_score)
    {
      max_score = tempo_scores(i);
      max_i = i;
    }
  }

  swap_tempo = tempos(1);
  swap_score = tempo_scores(1);
  swap_phase = tempo_phases(1);
  
  tempos(1) = tempos(max_i);
  tempo_scores(1) = tempo_scores(max_i);
  tempo_phases(1) = tempo_phases(max_i); 
  
  if (max_i != 1) {
    tempos(max_i) = swap_tempo;
    tempo_scores(max_i) = swap_score;
    tempo_phases(max_i) = swap_phase; 
  }

  
  ctrl_phase_tempo_->setValue(tempos(0));


  for (int i= 0; i < tempos.getSize(); i++)
    {
      /* std::cout << "TEMPO(" << i << ") = " << tempos(i) << ":-----:" << tempo_phases(i) << std::endl;
      std::cout << "start block : " << block_start << std::endl;
      */ 
      beat_period = 60.0 / tempos(i);      
      if (fabs(tempos(i) - prob_max_tempo) < 2.0)
	{
	  tempo_i = i;
	  tempo = tempos(i);
	  beat_period = 60.0 / tempo;
	  if (tempo >= 50)
	    period = oss_hop_win_ratio * osrate_ * 60.0 / tempo; // flux hopSize is half the winSize
	  else
	    period = 0;		  
	  period = (mrs_natural)(period+0.5);
	}
    }

  


    
  // std::cout << "osrate_ =  " << osrate_ << std::endl;
  
  tempo = tempos(tempo_i); 
  mrs_natural temp_t = (mrs_natural) tempo_phases(tempo_i);
  beat_period = 60.0 / tempos(tempo_i);
  mrs_natural mt_t;
  mrs_real init_beat_location;

  mrs_real start_block = sampleCount_  / (oss_hop_win_ratio * osrate_); 
  mrs_real end_block = (sampleCount_  + bhopSize) / (oss_hop_win_ratio * osrate_);

  
  mrs_real beat_percentage = 0.75;

  if ((temp_t > 0) && (temp_t < inSamples_))
    {
      mt_t = temp_t;
      init_beat_location = (sampleCount_ + mt_t -(inSamples_-1 -bhopSize)) / (oss_hop_win_ratio * osrate_);
      if ((init_beat_location - current_beat_location_) / beat_period > beat_percentage)
	{
	  average_beat_height_ = (average_beat_height_ + in(0,mt_t)) * 0.5;
	  current_beat_location_ = init_beat_location;
	  std::cout << current_beat_location_ << std::endl;
	}
    }

  for (int i= 0; i < tempos.getSize(); i++)
    prev_tempos(i) = tempos(i);

  prev_tempo_ = tempo;  
  sampleCount_ += bhopSize;
}
