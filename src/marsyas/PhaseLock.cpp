/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "PhaseLock.h"

#define MINIMUMREAL 0.000001 //(0.000001 minimum float recognized)

using namespace std;
using namespace Marsyas;

PhaseLock::PhaseLock(string name):MarSystem("PhaseLock", name)
{
  addControls();
  t_ = 0;
}

PhaseLock::PhaseLock(const PhaseLock& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem 
  // it is necessary to perform this getctrl 
  // in the copy constructor in order for cloning to work
	ctrl_beatHypotheses_ = getctrl("mrs_realvec/beatHypotheses");
	ctrl_inductionTime_ = getctrl("mrs_natural/inductionTime");
	ctrl_nrPeriodHyps_ = getctrl("mrs_natural/nrPeriodHyps");
	ctrl_nrPhasesPerPeriod_ = getctrl("mrs_natural/nrPhasesPerPeriod");
	ctrl_scoreFunc_ = getctrl("mrs_string/scoreFunc");
	ctrl_sourceFile_ = getctrl("mrs_string/gtBeatsFile");
	ctrl_hopSize_ = getctrl("mrs_natural/hopSize");
	ctrl_srcFs_ = getctrl("mrs_real/srcFs");
	ctrl_mode_ = getctrl("mrs_string/mode");
	ctrl_tickCount_ = getctrl("mrs_natural/tickCount");

	t_ = a.t_;
}

PhaseLock::~PhaseLock()
{
}

MarSystem* 
PhaseLock::clone() const 
{
  return new PhaseLock(*this);
}

void 
PhaseLock::addControls()
{
  //Add specific controls needed by this MarSystem.
	addctrl("mrs_realvec/beatHypotheses", realvec(), ctrl_beatHypotheses_);
	addctrl("mrs_natural/inductionTime", 430, ctrl_inductionTime_);
	addctrl("mrs_natural/nrPeriodHyps", 6, ctrl_nrPeriodHyps_);
	setctrlState("mrs_natural/nrPeriodHyps", true);
	addctrl("mrs_natural/nrPhasesPerPeriod", 20, ctrl_nrPhasesPerPeriod_);
	setctrlState("mrs_natural/nrPhasesPerPeriod", true);
	addctrl("mrs_string/scoreFunc", "regular", ctrl_scoreFunc_);
	setctrlState("mrs_string/scoreFunc", true);
	addctrl("mrs_natural/hopSize", -1, ctrl_hopSize_);
	addctrl("mrs_real/srcFs", -1.0, ctrl_srcFs_);
	addctrl("mrs_string/gtBeatsFile", "input.txt", ctrl_sourceFile_);
	addctrl("mrs_string/mode", "regular", ctrl_mode_);
	setctrlState("mrs_string/mode", true);
	addctrl("mrs_natural/tickCount", 0, ctrl_tickCount_);
}

void
PhaseLock::myUpdate(MarControlPtr sender)
{
	MRSDIAG("PhaseLock.cpp - PhaseLock:myUpdate");

	inductionTime_ = ctrl_inductionTime_->to<mrs_natural>();
	nrPeriodHyps_ = ctrl_nrPeriodHyps_->to<mrs_natural>();
	nrPhasesPerPeriod_ = ctrl_nrPhasesPerPeriod_->to<mrs_natural>();
	scoreFunc_ = ctrl_scoreFunc_->to<mrs_string>();
	mode_ = ctrl_mode_->to<mrs_string>();

	ctrl_onSamples_->setValue(3, NOUPDATE);
	ctrl_onObservations_->setValue(nrPeriodHyps_, NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

	nInitHyp_ = nrPeriodHyps_ * nrPhasesPerPeriod_; //Nr. of initial hypothesis

	hypSignals_.create(nInitHyp_, inductionTime_+1); //N hypothesis signals of induction timming size.
	sum_.create(nInitHyp_);
	maxSum_.create(nrPeriodHyps_);
	maxSumInd_.create(nrPeriodHyps_);
	period_.create(nrPeriodHyps_);
	bestPhasePerPeriod_.create(nrPeriodHyps_);
	metricalSalience_.create(nrPeriodHyps_);
	rawScore_.create(nrPeriodHyps_);
	rawScoreNorm_.create(nrPeriodHyps_);
	score_.create(nrPeriodHyps_);
	scoreNorm_.create(nrPeriodHyps_);
	
	//Zero hypSignals:
	for(int i = 0; i < nInitHyp_; i++)
	{
		for(int j = 0; j < inductionTime_+1; j++)
		{
			hypSignals_(i, j) = 0.0;
		}
		//Zero sum:
		sum_(i) = 0.0;
	}
}

mrs_natural
PhaseLock::metricalRelation(mrs_bool duple, mrs_bool triple, mrs_real period1, mrs_real period2)
{
	mrs_real periodA = (period1 > period2 ? period1 : period2);
	mrs_real periodB = (period1 > period2 ? period2 : period1);
	mrs_real multiple = periodA / periodB;
	mrs_real tolerance = 0.15; //4/periodB;

	mrs_natural floorMultiple = (mrs_natural) floor(multiple);
	mrs_natural ceilMultiple = (mrs_natural) ceil(multiple);
	mrs_natural rel = 0;

	//if(print)	
		//cout << "Per1:" << period1 << "; Per2:" << period2 << "; Multiple:" << multiple
		//<< "(" << (multiple - tolerance) << ":" << (multiple + tolerance) << ")" << endl;

	//if periods are integerly related (with a given tolerance)
	if(multiple - tolerance <= floorMultiple)
	{
		if(floorMultiple == 2 || floorMultiple == 4)
			rel = 6 - floorMultiple; //then retrieve integer relation
		else if(floorMultiple == 3)
			rel = 3; //then retrieve integer relation
		else if(floorMultiple >= 5 && floorMultiple <= 8)
			rel = 1;
		else 
			rel = 0;
	}

	else if(multiple + tolerance >= ceilMultiple)
	{
		if(ceilMultiple == 2 || ceilMultiple == 4)
			rel = 6 - ceilMultiple; //then retrieve integer relation
		else if(ceilMultiple == 3)
			rel = 3; //then retrieve integer relation
		else if(ceilMultiple >= 5 && ceilMultiple <= 8)
			rel = 1;
		else rel = 0;
	}

	else rel = 0; //else retrieve 0 (periods non metricaly related)

	if(duple && rel == 3)
		rel = 0;
	if(triple && (rel == 4 || rel == 2))
		rel = 0;
	else rel = rel;

	return rel;
}

mrs_realvec
PhaseLock::calcRelationalScore(mrs_bool duple, mrs_realvec rawScoreVec)
{
	mrs_realvec score;
	score.create(nrPeriodHyps_);
	for(int i = 0; i < nrPeriodHyps_; i++)
	{
		if(period_(i) > 0)
		{
			score(i) = 2*5*rawScoreVec(i);
			for(int j = 0; j < nrPeriodHyps_; j++)
			{
				if(j != i && period_(i) > 0 && period_(j) > 0)
				{
					mrs_natural metricalRel = 0;
					if(duple)
						metricalRel = metricalRelation(true, false, (mrs_natural)period_(i), (mrs_natural)period_(j));
					else
						metricalRel = metricalRelation(false, true, (mrs_natural)period_(i), (mrs_natural)period_(j));
					
					score(i) += rawScoreVec(j) * metricalRel;

					//cout << "; MRel" << i << ":" << metricalRel << "; rawScore" << j <<":" << rawScoreVec(j) 
					//	<< "; Score:" << score(i) <<endl;
				}
			}
		}
	}
	return score;
}

void
PhaseLock::inputGT(realvec& in, realvec& out, mrs_string gtFilePath)
{
	hopSize_ = ctrl_hopSize_->to<mrs_natural>();
	srcFs_ = ctrl_srcFs_->to<mrs_real>();

	inStream_.open(gtFilePath.c_str());

	getline (inStream_, line_);

	//for beat groundtruth files (in line, separated by spaces):
	beatTime1_ = strtod(line_.substr(0, line_.find(" ")).c_str(), NULL);
	beatTime2_ = strtod(line_.substr(line_.find(" ")+1, line_.find(" ", 1)).c_str(), NULL);
	//==========================================================
	//cout << "beat1: " << beatTime1_ << "; beat2: " << beatTime2_ << endl;
	
	//if to assure that it could read from file => beatFile in column (ifnot => beatFile in row)
	if(beatTime1_ == beatTime2_ || beatTime2_ == 0 || beatTime2_ > 40)
	{
		beatTime1_ = atof(line_.c_str());
		getline (inStream_, line_);
		beatTime2_ = atof(line_.c_str());
	}

	ibi_ = (mrs_natural) (((beatTime2_ - beatTime1_) * srcFs_ / hopSize_));
	phase_ = (mrs_natural) (beatTime1_ * srcFs_ / hopSize_);

	//Retrieve best score within induction window -> for starting score normalized with remaining analysis
	mrs_realvec signal;
	signal.create(inductionTime_+1);
	mrs_natural frameCount = 0;
	mrs_natural k = - (mrs_natural) ((phase_/ibi_)-MINIMUMREAL);
	do
	{
		frameCount = (mrs_natural) (phase_ + k * ibi_);
		if(frameCount > inductionTime_ || ibi_ == 0 || phase_ == 0) break;
		
		signal(frameCount) = 1.0;
		k++;

	}while (frameCount < inductionTime_);
	
	for(int i = 0; i < inductionTime_; i++)
	{
		if(signal(i) == 1.0)
		{	
			mrs_real maxLocal = 0.0;
			for (int j = i-5; j <= i+5; j++) //MAX 5!
			{
				if(j >= 0 && j < inductionTime_)
				{
					if(in(j) > maxLocal)
						maxLocal = in(j);
				}
			}
			maxScore_ += maxLocal;
			//cout << "period_: " << ibi_ << "; Phase: " << phase_ << "; Sum: " << maxScore_ << endl;
		}	
	}

	cout << "\nInduction replaced by ground-truth file at: " << gtFilePath << endl;
	//cout << "Beat1: " << beatTime1_ << "(" << phase_ << "); Beat2: " << beatTime2_ << "; Period: " 
	//	<< ibi_ << "; Score:" << maxScore_ << endl;

	//aditional initial score normalization dependent of score function (due to their relative weights)
	if(strcmp(scoreFunc_.c_str(), "correlation") == 0 || strcmp(scoreFunc_.c_str(), "squareCorr") == 0 )
		maxScore_ *= 5; //("correlation" and "squareCorr" are, in average, 5times more reactive than "regular")

	//Period:
	out(0, 0) = ibi_; //period in frames
	//Phase:
	out(0, 1) = phase_; //phase in frames
	//score 
	out(0, 2) = maxScore_; //initialScore

	inStream_.close();
}

void
PhaseLock::regularFunc(realvec& in, realvec& out)
{
	//matrix with all the N generated beat hypotheses in the induction stage
	beatHypotheses_ = ctrl_beatHypotheses_->to<mrs_realvec>();

	//Build N hypotheses signals (phase + k*periods):
	mrs_realvec beatCount;
	beatCount.create(nInitHyp_);
	for(int h = 0; h < nInitHyp_; h++)
	{	
		mrs_natural frameCount = 0;
		//mrs_natural k = 0;
		mrs_natural phase = (mrs_natural) beatHypotheses_(h, 1);
		mrs_natural k = - (mrs_natural) ((phase/beatHypotheses_(h, 0))-MINIMUMREAL);

		//cout << "h:" << h << " - k:" << k << "; period: " << beatHypotheses_(h, 0) << "; phase: " << phase 
		//	<< "(" << ((phase/beatHypotheses_(h, 0))-MINIMUMREAL) << ")" << endl;

		beatCount(h) = 0;

		do
		{
			//frameCount = phase + k*period_
			//cout << "Hypothesis (" << h << "): Phase: " << phase << " period_: " << beatHypotheses_(h, 1) << endl;
			frameCount = (mrs_natural) (phase + k * beatHypotheses_(h, 0));
			if(frameCount > inductionTime_ || beatHypotheses_(h, 0) == 0 || beatHypotheses_(h, 1) == 0) break;
			
			hypSignals_(h, frameCount) = 1.0;
			beatCount(h)++;
			k++;

		}while (frameCount < inductionTime_);

		//Calculate Sum of every (period_, phase) pair multiplied by the
		//accumulated flux signal (with a tolerance = 4)-> The highest sums will correspond
		//to the best hypotheses
		for(int i = 0; i < inductionTime_; i++)
		{
			if(hypSignals_(h, i) == 1.0)
			{	
				mrs_real maxLocal = 0.0;
				for (int j = i-3; j <= i+3; j++) //MAX 5!
				{
					if(j >= 0 && j < inductionTime_)
					{
						if(in(j) > maxLocal)
							maxLocal = in(j);
					}
				}
				sum_(h) += maxLocal;
			}
			//if(hypSignals_(h, i) == 1.0)
			//	cout << "period_: " << beatHypotheses_(h, 0) << "; Phase: " << i << "; FLUX: " << in(i) << endl;
		}

		//sum_(h) = sum_(h) / beatCount(h);
		//cout << "SUM" << h << ": " << sum_(h) << "; Phase: " << phase << "; period_: " << beatHypotheses_(h, 0) << endl;
	}

	//Retrieve best M (nrPeriodHyps_) {period_, phase} pairs, by period:		
	for(int i = 0; i < nrPeriodHyps_; i++)
	{
		if(beatHypotheses_(i*nrPhasesPerPeriod_, 0) > 0) //if the given period_ > 0 (= valid period_)
		{
			for(int j = i*nrPhasesPerPeriod_; j < (i*nrPhasesPerPeriod_)+nrPhasesPerPeriod_; j++)
			//for(int j = i; j < sum_.getSize(); j+=nrPhasesPerPeriod_)
			{
				if(sum_(j) > maxSum_(i))
				{
					maxSum_(i) = sum_(j);
					maxSumInd_(i) = j;
				}
			}

			period_(i) = beatHypotheses_((mrs_natural)maxSumInd_(i), 0); //each period_ hypothesis
			//best phase for each hypothesis (backtraced) 
			//= chosen_phase - (((chosen_phase / period_)-MINIMUMREAL) * period_)
			bestPhasePerPeriod_(i) = (mrs_natural) (beatHypotheses_((mrs_natural)maxSumInd_(i), 1) 
				- ((mrs_natural) ((beatHypotheses_((mrs_natural)maxSumInd_(i), 1)/period_(i))-MINIMUMREAL)) * period_(i));
			//metricalSalience_ = amplitude of each ACF peak (representing each period_ hypothesis)
			metricalSalience_(i) = beatHypotheses_((mrs_natural)maxSumInd_(i), 2) * 10000; //(x10000 for real value)
			rawScore_(i) = pow(metricalSalience_(i),2) * maxSum_(i) / sqrt(beatCount((mrs_natural)maxSumInd_(i)));

		}

		//cout << i << "-MetSal:" << metricalSalience_(i) << "; Sum:" << maxSum_(i) << "(" 
		//	<< beatCount((mrs_natural)maxSumInd_(i)) << "); rawScore:" << rawScore_(i) 
		//	<< "; period:" << period_(i) << "; Phase:" << bestPhasePerPeriod_(i) << endl; 
	}

	mrs_real scoreDuple = 0.0;
	mrs_real scoreTriple = 0.0;
	mrs_natural metricalRel = 0;
	for(int i = 0; i < nrPeriodHyps_; i++)
	{
		if(period_(i) > 0)
		{
			for(int j = i+1; j < nrPeriodHyps_; j++)
			{
				mrs_natural metricalRel;
				if(period_(j) > 0)
				{
					metricalRel = metricalRelation(false, false, (mrs_natural)period_(i), (mrs_natural)period_(j));
					//cout << "Period1(" << i << "):" << period_(i) << ";Period2(" << j << "):" << period_(j) 
					//	<< ";REL:" << metricalRel << endl;
					if(metricalRel == 2 || metricalRel == 4)
					{
						if(i == 0)
							scoreDuple += rawScore_(i);

						scoreDuple += rawScore_(j);
					}
					if(metricalRel == 3)
					{
						if(i == 0)
							scoreTriple += rawScore_(i);

						scoreTriple += rawScore_(j);
					}
				}
			}
		}
	}

	//cout << "ScoreDuple:" << scoreDuple << "; ScoreTriple:" << scoreTriple << endl;

	if(scoreDuple > scoreTriple)
	{
		score_ = calcRelationalScore(true, rawScore_);
	}
	else if(scoreDuple < scoreTriple)
	{
		score_ = calcRelationalScore(false, rawScore_);
	}
	else if(scoreDuple == scoreTriple)
	{
		score_ = rawScore_;
	}

	mrs_real maxScore = 0.0;
	mrs_natural avgPeriod = 0;
	mrs_real avgMaxSum = 0.0;
	mrs_real maxMaxSum = 0.0;
	mrs_natural maxScoreInd = -1;
	for(int i = 0; i < nrPeriodHyps_; i++)
	{
		if(score_(i) > maxScore)
		{
			maxScore = score_(i);
			maxScoreInd = i;
		}

		if(maxSum_(i) > maxMaxSum)
			maxMaxSum = maxSum_(i);

		avgMaxSum += maxSum_(i);
		avgPeriod += (mrs_natural) period_(i);

		//cout << "Score" << i << ":" << score_(i) << "; Period:" << period_(i) << "; Phase:" 
		//	<< bestPhasePerPeriod_(i) << "(" << beatHypotheses_((mrs_natural)maxSumInd_(i), 1) << ")"
		//	<< "; Score:" << maxScore << "(" << maxScoreInd << ")" << endl;
	}

	avgMaxSum /= nrPeriodHyps_;
	avgPeriod /= nrPeriodHyps_;

	for(int i = 0; i < nrPeriodHyps_; i++)
	{
		//period_:
		out(i, 0) = period_(i);
		//Phase:
		out(i, 1) = bestPhasePerPeriod_(i);
		
		//Score:
		//(score normalized by a score respecting the maximum (or average!?) number of beats in the induction window)
		//mrs_real finalScore = (score_(i) / maxScore) * avgMaxSum;
		mrs_real finalScore = (score_(i) / maxScore) * maxMaxSum;

		//aditional initial score normalization dependent of score function (due to their relative weights)
		if(strcmp(scoreFunc_.c_str(), "correlation") == 0 || strcmp(scoreFunc_.c_str(), "squareCorr") == 0 )
			finalScore *= 5; //("correlation" and "squareCorr" are, in average, 5times more reactive than "regular")
		
		//cout << "scoreFunc:" << scoreFunc_ << "; avgMaxSum:" << avgMaxSum << "; avgPeriod:" << avgPeriod << endl;

		out(i, 2) = finalScore;
	}

	//MATLAB_PUT(in, "Flux_FlowThrued");
	//MATLAB_PUT(hypSignals_, "HypSignals");
	//MATLAB_PUT(beatHypotheses_, "BeatHypotheses");
	//MATLAB_PUT(out, "FinalHypotheses");
}

void 
PhaseLock::myProcess(realvec& in, realvec& out)
{
	//t_ is constantly updated with the referee's next time frame
	t_ = ctrl_tickCount_->to<mrs_natural>();

	//cout << "PLock: " << t_ << endl;

	//Output only defined just after induction time
	//until then output is undefined...
	for (o=0; o < onObservations_; o++)
    {
		for (t = 0; t < onSamples_; t++)
		{
			out(o,t) = -1.0;
		}
    }

	//After induction, given the initial beat hypotheses
	//Calculate the N (nrPeriodHyps_) best (period_, phase) pairs + respective scores:
	if (t_ == inductionTime_)
	{

		if(strcmp(mode_.c_str(), "ground-truth") == 0)
		{
			ostringstream oss;
			oss << ctrl_sourceFile_->to<mrs_string>() << ".txt";

			//check if ground-truth file, with .txt extension, exists (ifnot try with .beats extension)
			if(FILE * file = fopen(oss.str().c_str(), "r"))
				inputGT(in, out, oss.str());

			//else, check if ground-truth file, with .beats extension, exists (ifnot return to normal induction)
			else
			{
				oss.str("");
				oss << ctrl_sourceFile_->to<mrs_string>() << ".beats";
				if(FILE * file = fopen(oss.str().c_str(), "r"))
					inputGT(in, out, oss.str());
				else
				{
					mode_ = "regular";
					cout << "\nInduction ground-truth do not exists! -> " << ctrl_sourceFile_->to<mrs_string>() << "(.txt/.beats)" 
						<< "\nRunning normal induction...";
				}
			}	
		}

		if(strcmp(mode_.c_str(), "regular") == 0)
			regularFunc(in , out);
	}

	//MATLAB_PUT(out, "PhaseLockOut");
	//MATLAB_EVAL("hold on;");
	//MATLAB_EVAL("plot(Flux_FlowThrued), g");
	//MATLAB_EVAL("FluxFlowTS = [FluxFlowTS, Flux_FlowThrued];");
	//cout << "T-" << t_ << ": " << in(inSamples_-1) << endl;
}

