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

/**
    \class OneRClassifier
    \brief Implements the OneR classifier.

*/

#ifndef MARSYAS_OneRClassifier_H
#define MARSYAS_OneRClassifier_H

#include <marsyas/system/MarSystem.h>
#include <marsyas/WekaData.h>

namespace Marsyas
{

class OneRClassifier: public MarSystem
{
private:

  //This class represents one rule of the OneR algorithm.
  class OneRRule
  {
  public:
    //Construct a rule with inst count, attribute this rule represents, number of breaks and correct value
    OneRRule(mrs_natural attr, mrs_natural nBreaks, mrs_natural correct)
    {
      attr_ = attr;
      nBreaks_ = nBreaks;
      correct_ = correct;

      //size the classifications and breakpoints accrding to nbreaks.
      //breakpoints has one less because the last entry represents infinity
      classifications_.resize(nBreaks_);
      breakpoints_.resize(nBreaks_-1);
    }

    //nothing to destroy
    ~OneRRule() {}

    //get the correct count from this rule
    mrs_natural getCorrect()const {return correct_;};

    //get the breakpoints and classifications vectors
    inline std::vector<mrs_natural>& getClassifications() {return classifications_;}
    inline std::vector<mrs_real>& getBreakpoints() {return breakpoints_;}

    //get the number of breakpoints(set in ctor)
    inline mrs_natural getnBreaks()const {return nBreaks_;}

    //get the attribute this rule is for.
    inline mrs_natural getAttr()const {return attr_;}

  private:
    //mrs_natural numInst_;
    mrs_natural attr_;
    mrs_natural nBreaks_;
    mrs_natural correct_;

    std::vector<mrs_natural>classifications_;
    std::vector<mrs_real>breakpoints_;
  };

private:
  void addControls();
  void myUpdate(MarControlPtr sender);

  //this is a table of all the attribute data. It is built
  //when mode = "train"
  WekaData instances_;

  //construct a new rule
  OneRRule *newRule(mrs_natural attr, mrs_natural nLabels);

  //Predict a class. This is done when mode is "predict"
  mrs_natural Predict(const realvec& in);

  //build the classifier. This is performed when the done flag goes to true.
  void Build(mrs_natural nLabels);

  //The current rule during the build classifier stage
  OneRRule *rule_;

  //the minimum number of buckes
  static const mrs_natural minBucketSize_ = 6;

  //keeps track of the last mode found when myProcess is called.
  //It is used to determine when the incoming data changes from
  //one state to the other.
  //When train -> predict : Build the classifier and get ready for prediction
  //When predict -> train : Clear the classifier and start retraining
  bool lastModePredict_;
  mrs_realvec row_;
public:
  OneRClassifier(const std::string name);
  ~OneRClassifier();

  MarSystem *clone() const;
  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas
#endif

