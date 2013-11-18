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

#include "BeatTimesSink.h"
#include "../common_source.h"
#include <fstream>
#include <string.h>

using namespace std;
using namespace Marsyas;

const char *szServer = "localhost"; //assume UDP sockets at localhost [CHANGE IF OTHER!!]

BeatTimesSink::BeatTimesSink(mrs_string name):MarSystem("BeatTimesSink", name)
{
  addControls();
  ibiBPM_ = 0.0;
  ibiBPMSum_ = 0.0;
  beatCount_ = 0;
  timeElapsed_ = 0;
  inc_ = 0; //initial beat counting...
  nonCausal_ = true;
  lastIbi_ = 0.0;
  initialOut_ = true;
  initialOut2_ = true;
  initialOut3_ = true;
  tempoVec_.resize(1);
  // mySocket_ = -1;
}

BeatTimesSink::BeatTimesSink(const BeatTimesSink& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem
  // it is necessary to perform this getctrl
  // in the copy constructor in order for cloning to work
  ctrl_hopSize_ = getctrl("mrs_natural/hopSize");
  ctrl_winSize_ = getctrl("mrs_natural/winSize");
  ctrl_srcFs_ = getctrl("mrs_real/srcFs");
  ctrl_destFileName_ = getctrl("mrs_string/destFileName");
  ctrl_mode_ = getctrl("mrs_string/mode");
  ctrl_tickCount_ = getctrl("mrs_natural/tickCount");
  ctrl_curMedianTempo_ = getctrl("mrs_natural/curMedianTempo");
  ctrl_adjustment_ = getctrl("mrs_natural/adjustment");
  ctrl_bestFinalAgentHistory_= getctrl("mrs_realvec/bestFinalAgentHistory");
  ctrl_soundFileSize_= getctrl("mrs_natural/soundFileSize");
  ctrl_nonCausal_ = getctrl("mrs_bool/nonCausal");
  ctrl_socketsPort_ = getctrl("mrs_natural/socketsPort");
  ctrl_tempo_ = getctrl("mrs_real/tempo");
  ibiBPM_ = a.ibiBPM_;
  beatCount_ = a.beatCount_;
  ibiBPMSum_ = a.ibiBPMSum_;
  inc_ = a.inc_;
  ibiBPMVec_ = a.ibiBPMVec_;
  initialOut_ = a.initialOut_;
  initialOut2_ = a.initialOut2_;
  initialOut3_ = a.initialOut3_;
  tempoVec_ = a.tempoVec_;

  // socketsPort_ = a.socketsPort_;
  // mySocket_ = a.mySocket_;
  // myAcceptSocket_ = a.myAcceptSocket_;
}

BeatTimesSink::~BeatTimesSink()
{
}

MarSystem*
BeatTimesSink::clone() const
{
  return new BeatTimesSink(*this);
}

void
BeatTimesSink::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_natural/tickCount", 0, ctrl_tickCount_);
  addctrl("mrs_natural/hopSize", -1, ctrl_hopSize_);
  setctrlState("mrs_natural/hopSize", true);
  addctrl("mrs_natural/winSize", -1, ctrl_winSize_);
  setctrlState("mrs_natural/winSize", true);
  addctrl("mrs_real/srcFs", -1.0, ctrl_srcFs_);
  setctrlState("mrs_real/srcFs", true);
  addctrl("mrs_string/destFileName", "output", ctrl_destFileName_);
  addctrl("mrs_string/mode", "beats+tempo", ctrl_destFileName_);
  setctrlState("mrs_string/mode", true);
  addctrl("mrs_natural/curMedianTempo", 0, ctrl_curMedianTempo_);
  addctrl("mrs_natural/adjustment", 0, ctrl_adjustment_);
  setctrlState("mrs_natural/adjustment", true);
  addctrl("mrs_realvec/bestFinalAgentHistory", realvec(), ctrl_bestFinalAgentHistory_);
  setctrlState("mrs_realvec/bestFinalAgentHistory", true);
  addctrl("mrs_natural/soundFileSize", 0, ctrl_soundFileSize_);
  setctrlState("mrs_natural/soundFileSize", true);
  addctrl("mrs_bool/nonCausal", false, ctrl_nonCausal_);
  setctrlState("mrs_bool/nonCausal", true);
  addctrl("mrs_natural/socketsPort", -1, ctrl_socketsPort_);
  setctrlState("mrs_natural/socketsPort", true);
  addctrl("mrs_real/tempo", 80.0, ctrl_tempo_);


}

void
BeatTimesSink::myUpdate(MarControlPtr sender)
{
  MRSDIAG("BeatTimesSink.cpp - BeatTimesSink:myUpdate");
  MarSystem::myUpdate(sender);

  hopSize_ = ctrl_hopSize_->to<mrs_natural>();
  winSize_ = ctrl_winSize_->to<mrs_natural>();
  srcFs_ = ctrl_srcFs_->to<mrs_real>();

  adjustment_ = (mrs_real) ctrl_adjustment_->to<mrs_natural>();

  mode_ = ctrl_mode_->to<mrs_string>();

  bestFinalAgentHistory_ = ctrl_bestFinalAgentHistory_->to<mrs_realvec>();
  soundFileSize_ = ctrl_soundFileSize_->to<mrs_natural>();
  nonCausal_ = ctrl_nonCausal_->to<mrs_bool>();
  socketsPort_ = ctrl_socketsPort_->to<mrs_natural>();
}

mrs_natural
BeatTimesSink::refreshSocket()
{
#ifdef _WIN32
#pragma comment(lib, "Ws2_32.lib") //include Ws2_32.lib
#endif

  /* WSADATA wsaData;
  int error;
  struct sockaddr_in mySckAd;
  DWORD myError;
  myError=GetLastError();
  */

  // error=WSAStartup(MAKEWORD(2,2),&wsaData);
  // mySocket_=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP); //UDP
  // if(mySocket_==INVALID_SOCKET)
  // cout << "Socket Error - Invalid socket!" << endl;
  // myError=GetLastError();

  /* struct hostent *host;
  if((host=gethostbyname(szServer))==NULL)
  {
  	cout << "Socket Error server problem!" << endl;
  }

  memset(&mySckAd,0,sizeof(mySckAd));
  mySckAd.sin_family=AF_INET;
  mySckAd.sin_addr.s_addr=((struct in_addr*) (host->h_addr))->s_addr;
  mySckAd.sin_port=htons((u_short)socketsPort_);
  error=connect(mySocket_,(LPSOCKADDR) &mySckAd ,sizeof(mySckAd));
  myError=GetLastError();
  */
  return 1;
}

mrs_realvec
BeatTimesSink::addMedianVector(mrs_real ibiBPM)
{
  mrs_bool bigger = false;
  mrs_realvec tmp(beatCount_);

  //stretch median bpm vector if it reaches its limit
  if(beatCount_ >= ibiBPMVec_.getSize())
    ibiBPMVec_.stretch(beatCount_);

  for(mrs_natural j = 0; j < beatCount_-1; j++)
  {
    //copy all
    tmp(j) = ibiBPMVec_(j);
  }
  for(mrs_natural i = beatCount_-2; i >=0 ; i--)
  {
    if(ibiBPM > ibiBPMVec_(i))
    {
      ibiBPMVec_(i+1) = ibiBPM;
      for(mrs_natural z = i+1; z < beatCount_-1; z++)
        ibiBPMVec_(z+1) = tmp(z);
      //ibiBPMVec_.stretchWrite((z+1), tmp(z));

      bigger = true;
      break;
    }
  }
  if(!bigger)
  {
    ibiBPMVec_(0) = ibiBPM;
    for(mrs_natural z = 0; z < beatCount_-1; z++)
      ibiBPMVec_(z+1) = tmp(z);
    //ibiBPMVec_.stretchWrite((z+1), tmp(z));
  }

  //for(int i = 0; i<ibiBPMVec_.getSize(); i++)
  //	cout << i << ": " << ibiBPMVec_(i) << "; " << endl;

  return ibiBPMVec_;
}

void
BeatTimesSink::myProcess(realvec& in, realvec& out)
{
  //Frame (tick) counter: (updated from BeatReferee's next time frame -1)
  timeElapsed_ = ctrl_tickCount_->to<mrs_natural>()-1;

  //cout << "BSink: " << timeElapsed_ << endl;

  //FlowThru input
  out = in;

  if(!nonCausal_)
  {
    //If Beat:
    if(in(0,0) == 1.0) //only count after 1st beat
    {
      //For writing only beats after inc_ (to avoid writing first unconsistent beats)
      if(beatCount_ >= inc_)
      {
        //Output BeatTime (in Seconds) = ((timeElapsed_ (inFrames) * hopSize_) - adjustment) / srcFs_
        srcFs_ = ctrl_srcFs_->to<mrs_real>();
        beatTime_ = ((timeElapsed_ * hopSize_) - adjustment_) / srcFs_;

        //cout << "Beat at: " << beatTime_ << " (s)" << endl;

        //after the 1st beat calculate ibi
        if(beatCount_ > inc_)
        {
          ibiBPM_ = (60.0 / (beatTime_ - lastBeatTime_)); //inter-beat-interval (in BPMs)
          tempoVec_.push_back(ibiBPM_);
        }

        //curMedianTempo = (mrs_natural) (ibiBPMVec_((mrs_natural)(beatCount_ / 2.0)) + 0.5);
        mrs_natural curMedianTempo;
        mrs_realvec tempoVecMedian_(1);
        if(tempoVec_.size() > 10)
        {
          tempoVecMedian_.stretch(10); //account for last 10IBIs (11beats)
          mrs_natural ii = 0;
          for(mrs_natural s = (mrs_natural) (tempoVec_.size()-10); s < (mrs_natural) tempoVec_.size(); s++)
          {
            tempoVecMedian_(ii) = tempoVec_.at(s);
            ii++;
          }
        }
        else
        {
          tempoVecMedian_.stretch((mrs_natural)tempoVec_.size());
          for(mrs_natural s = 0; s < (mrs_natural) tempoVec_.size(); s++)
            tempoVecMedian_(s) = tempoVec_.at(s);

        }
        curMedianTempo = (mrs_natural) tempoVecMedian_.median();

        //cout << "Beat at: " << beatTime_ << " (s) - " << curMedianTempo << " (BPMs)" << endl;

        updControl(ctrl_curMedianTempo_, curMedianTempo);

        fstream outStream;
        fstream outStream2;
        fstream outStream3;

        if((strcmp(mode_.c_str(), "beatTimes") == 0) || (strcmp(mode_.c_str(), "beats+meanTempo") == 0)
            || (strcmp(mode_.c_str(), "beats+medianTempo") == 0) || (strcmp(mode_.c_str(), "beats+meanTempo+medianTempo") == 0))
        {
          ostringstream oss;

          //initially a new output file is created
          //(If the file exists, its content is deleted and it is treated as a new file)
          if(initialOut_)
          {
            oss << ctrl_destFileName_->to<mrs_string>() << ".txt";
            outStream.open(oss.str().c_str(), ios::out|ios::trunc);
            outStream << beatTime_ << endl;
            outStream.close();
            initialOut_ = false;
          }

          //output is appended in the end of the file
          else
          {
            oss << ctrl_destFileName_->to<mrs_string>() << ".txt";
            outStream.open(oss.str().c_str(), ios::out|ios::app);

            outStream << beatTime_ << " " << ibiBPM_ << endl;
            //outStream << beatTime_ << endl;
          }
        }

        if((strcmp(mode_.c_str(), "meanTempo") == 0) || (strcmp(mode_.c_str(), "beats+meanTempo") == 0)
            || (strcmp(mode_.c_str(), "beats+meanTempo+medianTempo") == 0))
        {
          ostringstream oss2;
          if(initialOut2_)
          {
            oss2 << ctrl_destFileName_->to<mrs_string>() << "_meanTempo.txt";

            outStream2.open(oss2.str().c_str(), ios::out|ios::trunc);
            outStream2.close();
            initialOut2_ = false;
          }

          if(beatCount_ > inc_)
          {
            ibiBPMSum_ += ibiBPM_;
            mrs_natural output = (mrs_natural) ((ibiBPMSum_ / (beatCount_-inc_)) + 0.5);

            oss2 << ctrl_destFileName_->to<mrs_string>() << "_meanTempo.txt";
            outStream2.open(oss2.str().c_str());
            outStream2 << output << endl;
            outStream2.close();
          }
        }

        if((strcmp(mode_.c_str(), "medianTempo") == 0) || (strcmp(mode_.c_str(), "beats+medianTempo") == 0)
            || (strcmp(mode_.c_str(), "beats+meanTempo+medianTempo") == 0))
        {
          ostringstream oss3;
          if(initialOut3_)
          {
            oss3 << ctrl_destFileName_->to<mrs_string>() << "_medianTempo.txt";
            outStream3.open(oss3.str().c_str(), ios::out|ios::trunc);
            outStream3.close();
            initialOut3_ = false;
          }

          if(beatCount_ > inc_)
          {
            addMedianVector(ibiBPM_);

            mrs_natural output;
            output = (mrs_natural) (ibiBPMVec_((mrs_natural)(beatCount_ / 2.0)) + 0.5);
            ctrl_tempo_->setValue(output * 1.0, NOUPDATE);

            /*
              if(beatCount_ % 2 == 0)
              {
              output = (mrs_natural) (ibiBPMVec_((mrs_natural)(beatCount_ / 2.0)) + 0.5);
              }
              else
              {
              output = (mrs_natural) ((ibiBPMVec_((mrs_natural)floor((beatCount_ / 2.0)))
              + ibiBPMVec_((mrs_natural)ceil((beatCount_ / 2.0))) / 2.0) +0.5);
              }
            */

            oss3 << ctrl_destFileName_->to<mrs_string>() << "_medianTempo.txt";
            outStream3.open(oss3.str().c_str());
            outStream3 << output << endl;
            outStream3.close();

            //MATLAB_PUT(ibiBPMVec_, "IBIVector1");
          }

          else if(beatCount_ == 2) //if only two beats => equal to ibi
          {
            oss3 << ctrl_destFileName_->to<mrs_string>() << "_medianTempo.txt";
            outStream.open(oss3.str().c_str());
            outStream << (mrs_natural) (ibiBPM_ + 0.5); //(+0.5 for round integer)
            outStream.close();
          }
        }


        //MATLAB_PUT(ibiBPMVec_, "IBIVector");
        lastBeatTime_ = beatTime_;
      }
      beatCount_ ++;
    }

    //send beats via UDP sockets
    /* if(socketsPort_ > 0)
    {
      if ( mySocket_ > 1000 )	//check if socket already initialized
    		refreshSocket(); //intialize socket

    	else
    	{
    		//fill the buffer with samples...
    		data_ = in.getData(); //pointer to input data
    		ostringstream msg;
    		msg << *data_ << "(" << tempo_ << ")\n"; //build msg to send via sockets

    		if(*data_ != -1.0) //if valid data
    			send(mySocket_, msg.str().c_str(),strlen(msg.str().c_str()),0);

    	}
    }
    */
  }
  if(nonCausal_)
  {
    if(timeElapsed_ == soundFileSize_-1) //[! -1 for acouting on time of timing reset on backtrace mode]
    {
      //if no beats detected [to avoid writing beatTimes output file]
      if(bestFinalAgentHistory_(0) >= 0.0)
      {
        //reset beatCount, ibiBPMSum, and ibiBPMVec, from causal analysis
        for(mrs_natural i = 0; i < beatCount_; i++)
          ibiBPMVec_(i) = 0.0;
        beatCount_ = 0;
        ibiBPMSum_ = 0.0;

        //first beatTime:
        beatTime_ = ((bestFinalAgentHistory_(0) * hopSize_) - adjustment_) / srcFs_;
        lastBeatTime_ = beatTime_;
        beatTimeTmp_ = beatTime_;
        beatCount_++;

        fstream outStream;
        ostringstream oss;
        if((strcmp(mode_.c_str(), "beatTimes") == 0) || (strcmp(mode_.c_str(), "beats+medianTempo") == 0)
            || (strcmp(mode_.c_str(), "beats+meanTempo") == 0) || (strcmp(mode_.c_str(), "beats+meanTempo+medianTempo") == 0))
        {
          oss << ctrl_destFileName_->to<mrs_string>() << ".txt";
          outStream.open(oss.str().c_str(), ios::out|ios::trunc);
          outStream << beatTime_ << endl;
        }

        //remaining beatTimes
        for(int i = 1; i < bestFinalAgentHistory_.getCols(); i++)
        {
          beatTime_ = ((bestFinalAgentHistory_(i) * hopSize_) - adjustment_) / srcFs_;

          mrs_real ibi = (beatTime_ - lastBeatTime_);

          //cout << "BEAT " << i << ": " << beatTime_ << "; IBI: " << ibi << endl;

          //mrs_real nextIbi = 0;
          mrs_real nextBeatTime = 0;
          //if(i < bestFinalAgentHistory_.getCols()-1) //everything commented is for handle transition (see thres below)
          //{
          nextBeatTime = ((bestFinalAgentHistory_(i+1) * hopSize_) - adjustment_) / srcFs_;
          //nextIbi = (nextBeatTime - beatTimeTmp_);
          //}
          //
          //else //if last beat always write
          //{
          //	ibiBPM_ = (60.0 / ibi); //inter-beat-interval (in BPMs)
          //	outStream << beatTime_ << " " << ibiBPM_ << endl;

          addMedianVector(ibiBPM_); //for calculating medianTempo
          //}

          //cout << "i: " << i << "; beatTime: " << beatTime_ << "; ibi: " << ibi << "; lastIbi: " << lastIbi_ << endl;
          //to avoid supurious beats at the transitions (on best agent changes)
          //[prioritize replacing (next) agent]
          //if(nextIbi > (0.3*ibi)) //REMOVE!?!?!?
          //{
          ibiBPM_ = (60.0 / ibi); //inter-beat-interval (in BPMs)

          if((strcmp(mode_.c_str(), "beatTimes") == 0) || (strcmp(mode_.c_str(), "beats+meanTempo") == 0)
              || (strcmp(mode_.c_str(), "beats+medianTempo") == 0) || (strcmp(mode_.c_str(), "beats+meanTempo+medianTempo") == 0))
          {
            outStream << beatTime_ << " " << ibiBPM_ << endl;
          }

          addMedianVector(ibiBPM_); //for calculating medianTempo
          ibiBPMSum_ += ibiBPM_; //for calculating meanTempo
          lastBeatTime_ = beatTime_;
          beatTimeTmp_ = nextBeatTime;
          beatCount_ ++;
          //}
          lastIbi_ = ibi;
        }

        if((strcmp(mode_.c_str(), "meanTempo") == 0) || (strcmp(mode_.c_str(), "beats+meanTempo") == 0)
            || (strcmp(mode_.c_str(), "beats+meanTempo+medianTempo") == 0))
        {
          ostringstream oss2;
          fstream outStream2;
          oss2 << ctrl_destFileName_->to<mrs_string>() << "_meanTempo.txt";
          outStream2.open(oss2.str().c_str(), ios::out|ios::trunc);
          outStream2 << (mrs_natural) ((ibiBPMSum_/beatCount_) + 0.5) << endl;
          outStream2.close();
        }

        if((strcmp(mode_.c_str(), "medianTempo") == 0) || (strcmp(mode_.c_str(), "beats+medianTempo") == 0)
            || (strcmp(mode_.c_str(), "beats+meanTempo+medianTempo") == 0))
        {
          mrs_natural output;
          output = (mrs_natural) (ibiBPMVec_((mrs_natural)(beatCount_ / 2.0)) + 0.5);

          ostringstream oss3;
          fstream outStream3;
          oss3 << ctrl_destFileName_->to<mrs_string>() << "_medianTempo.txt";
          outStream3.open(oss3.str().c_str(), ios::out|ios::trunc);
          outStream3 << output << endl;
          outStream3.close();

          //MATLAB_PUT(ibiBPMVec_, "IBIVector");
          //MATLAB_PUT(bestFinalAgentHistory_, "bestAgentHistory");
        }
      }
    }
  }
  //MATLAB_PUT(out, "BeatTimesSink");
}








