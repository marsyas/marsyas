// Copyright 2007-2010, Thomas Walters
//
// AIM-C: A C++ implementation of the Auditory Image Model
// http://www.acousticscale.org/AIMC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*!
 * \file
 * \brief Modifiable List of Strobe Points - helper for SAI generation
 *
 * \author Tom Walters <tom@acousticscale.org>
 * \date created 2007/08/22
 * \version \$Id: StrobeList.h 51 2010-03-30 22:06:24Z tomwalters $
 */

#ifndef AIMC_SUPPORT_STROBELIST_H_
#define AIMC_SUPPORT_STROBELIST_H_

#include <iostream>
//using namespace std;

#include <cmath>
#include <deque>
#include <cstddef>

namespace Marsyas {
using std::deque;
struct StrobePoint {
  int time;
  double weight;
  double working_weight;
  StrobePoint() {
    time = 0;
    weight = 0.0;
    working_weight = 0.0;
  }
};

class StrobeList {
public:
  /*! \brief Create a new strobe list
   */
  inline StrobeList() {
    strobes_.resize(0);
  };

  inline ~StrobeList() {
  };

  /*! \brief Return the strobe time (in samples, can be negative)
   */
  inline StrobePoint Strobe(std::size_t strobe_number) {
    return strobes_.at(strobe_number);
  };

  /*! \brief Set the strobe weight
   */
  inline void SetWeight(std::size_t strobe_number, double weight) {
    strobes_.at(strobe_number).weight = weight;
  };

  /*! \brief Set the strobe's working weight
   */
  inline void SetWorkingWeight(std::size_t strobe_number, double working_weight) {
    strobes_.at(strobe_number).working_weight = working_weight;
  };

  /*! \brief Add a strobe to the list (must be in order)
   */
  inline void AddStrobe(int time, double weight) {
    StrobePoint s;
    s.time = time;
    s.weight = weight;
    strobes_.push_back(s);
  };

  /*! \brief Delete a strobe from the list
   */
  inline void DeleteFirstStrobe() {
    strobes_.pop_front();
  };

  /*! \brief Get the number of strobes
   */
  inline std::size_t strobe_count() const {
    return strobes_.size();
  };

  /*! \brief Shift the position of all strobes by subtracting offset from
   *  the time value of each
   */
  inline void ShiftStrobes(int offset) {
    for (unsigned int i = 0; i < strobes_.size(); ++i)
      strobes_[i].time -= offset;
  };

  void printStrobes() {
    for (unsigned int i = 0; i < strobes_.size(); ++i) {
      std::cout << strobes_[i].time << " " << strobes_[i].weight << strobes_[i].working_weight << std::endl;
    }

  }

private:
  deque<StrobePoint> strobes_;
};
}  // namespace aimc

#endif /* AIMC_SUPPORT_STROBELIST_H_ */
