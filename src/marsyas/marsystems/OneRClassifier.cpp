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

/**
   \class OneRClassifier
   \brief Implements the OneR classifier.
*/

#include "OneRClassifier.h"
#include "../common_source.h"
#include <cstddef>

using std::ostringstream;
using std::cout;
using std::endl;
using std::vector;
using std::size_t;

using namespace Marsyas;

OneRClassifier::OneRClassifier(const mrs_string name) : MarSystem("OneRClassifier", name)
{
  addControls();
  rule_ = NULL;
  lastModePredict_ = false;
}

//Only thing needing destroying is the current rule.
OneRClassifier::~OneRClassifier()
{
  if(rule_ != NULL)
    delete rule_;
}

MarSystem *OneRClassifier::clone() const
{
  return new OneRClassifier(*this);
}

void
OneRClassifier::addControls()
{
  addctrl("mrs_string/mode", "train");
  addctrl("mrs_natural/nClasses", 1);
  setctrlState("mrs_natural/nClasses", true);
}

void
OneRClassifier::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("OneRClassifier.cpp - OneRClassifier:myUpdate");
  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  setctrl("mrs_natural/onObservations", 2);
  ctrl_osrate_->setValue(ctrl_israte_->to<mrs_real>());
  ctrl_onObsNames_->setValue("OneRClassifier_"
                             + ctrl_inObsNames_->to<mrs_string>() , NOUPDATE);
}

void
OneRClassifier::myProcess(realvec& in, realvec& out)
{
  cout << "OneRClassifier::myProcess" << endl;
  cout << "in.getCols() = " << in.getCols() << endl;
  cout << "in.getRows() = " << in.getRows() << endl;
  //get the current mode, either train of predict mode
  bool trainMode = (getctrl("mrs_string/mode")->to<mrs_string>() == "train");
  row_.stretch(in.getRows());
  if (trainMode)
  {
    if(lastModePredict_ || instances_.getCols()<=0)
    {
      mrs_natural nAttributes = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
      cout << "nAttributes = " << nAttributes << endl;
      instances_.Create(nAttributes);
    }

    lastModePredict_ = false;

    //get the incoming data and append it to the data table
    for (mrs_natural ii=0; ii< inSamples_; ++ii)
    {
      mrs_real label = in(inObservations_-1, ii);
      instances_.Append(in);
      out(0,ii) = label;
      out(1,ii) = label;
    }//for t
  }//if
  else
  { //predict mode

    cout << "OneRClassifier::predict" << endl;
    if(!lastModePredict_)
    {
      //get the number of class labels and build the classifier
      mrs_natural nAttributes = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
      cout << "BUILD nAttributes = " << nAttributes << endl;
      Build(nAttributes);
    }//if
    lastModePredict_ = true;
    cout << "After lastModePredict" << endl;


    //foreach row of predict data, extract the actual class, then call the
    //classifier predict method. Output the actual and predicted classes.
    for (mrs_natural ii=0; ii<inSamples_; ++ii)
    {
      //extract the actual class
      mrs_natural label = (mrs_natural)in(inObservations_-1, ii);

      //invoke the classifier predict method to predict the class
      in.getCol(ii,row_);
      mrs_natural prediction = Predict(row_);
      cout << "PREDICTION = " << prediction << endl;
      cout << "row_ " << row_ << endl;

      //and output actual/predicted classes
      out(0,ii) = (mrs_real)prediction;
      out(1,ii) = (mrs_real)label;
    }//for t
  }//if

}//myProcess

//Create a new rule for this attribute.
//Sorts the data table on this attribute and executes the OneR algorithm.
OneRClassifier::OneRRule *OneRClassifier::newRule(mrs_natural attr, mrs_natural nClasses)
{
  //create the counting variables
  vector<mrs_natural> classifications(instances_.size());
  vector<mrs_real> breakpoints(instances_.size());
  vector<mrs_natural> counts(nClasses);

  //set correct count to 0
  mrs_natural correct = 0;
  mrs_natural lastInstance = (mrs_natural) instances_.size();

  //Sort the data table for this attribute
  instances_.Sort(attr);

  mrs_natural ii = 0;
  mrs_natural cl = 0;			//index of next bucket to create
  mrs_natural it = 0;

  //scan thru all rows in table
  while(ii < lastInstance)
  {
    //zero the current counts
    for(mrs_natural jj=0; jj<(mrs_natural)counts.size(); jj++) counts[jj]=0;
    do
    { //fill it until is has enough of the majority class
      it = instances_.GetClass(++ii);
      counts[it]++;
    } while(counts[it] < minBucketSize_ && ii < lastInstance);

    //while class remains the same, keep on filling
    while(ii < lastInstance && instances_.GetClass(ii) == it)
    {
      counts[it]++;
      ++ii;
    }//while

    //keep on while attr value is the same
    while(ii < lastInstance && instances_.at(ii-1)->at(attr) == instances_.at(ii)->at(attr))
    {
      mrs_natural index = instances_.GetClass(ii++);
      counts[index]++;
    }//while

    for(mrs_natural jj=0; jj<nClasses; jj++)
    {
      if(counts[jj] > counts[it])
      {
        it = jj;
      }//if
    }//for jj

    if(cl > 0)
    { //can we coalesce with previous class?
      if(counts[classifications[cl-1]] == counts[it])
        it = classifications[cl-1];

      if(it == classifications[cl-1])
        cl--;
    }//if

    correct += counts[it];
    classifications[cl] = it;

    if(ii < lastInstance)
      breakpoints[cl] = (((instances_.at(ii-1)->at(attr) + instances_.at(ii)->at(attr)) / 2.0));

    cl++;
  }//while

  //create a new rule with cl branches
  OneRRule *rule = new OneRRule(attr, cl, correct);
  for(mrs_natural vv=0; vv<cl; vv++)
  {
    rule->getClassifications()[vv] = classifications[vv];
    if(vv < (cl-1))
      rule->getBreakpoints()[vv] = breakpoints[vv];

  }//for vv

  return rule;
}//newRule

//Build the classifier from the data table
void
OneRClassifier::Build(mrs_natural nClasses)
{
  //make sure any previous rule is out
  if(rule_!=NULL)
    delete rule_;
  rule_ = NULL;

  //scan through all the attributes(columns) of the table
  for(mrs_natural enu = 0; enu < instances_.getCols()-1; enu++)
  {
    //construct a new rule for this attribute
    OneRClassifier::OneRRule *r = newRule(enu, nClasses);

    //if a current rule does not exist or this new rule is better, replace old rule
    if(!rule_ || r->getCorrect() > rule_->getCorrect())
    {
      if(rule_!=NULL)
        delete rule_;

      rule_ = r;
    }//if
  }//for enu
}//Build

//Predict a class given a row of attribute data
mrs_natural
OneRClassifier::Predict(const realvec& in)
{
  mrs_natural vv = 0;
  mrs_real instValue = in(rule_->getAttr());

  //find the breakpoint whose value exceeds the attribute value.
  while(vv < rule_->getnBreaks()-1 && instValue >= rule_->getBreakpoints()[vv])
  {
    vv++;
  }//while

  //return the class for this prediction.
  return rule_->getClassifications()[vv];
}//Predict
