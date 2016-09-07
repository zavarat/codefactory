/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef _WIN32
#ifndef WEBRTC_SYSTEM_WRAPPERS_SOURCE_CONDITION_VARIABLE_POSIX_H_
#define WEBRTC_SYSTEM_WRAPPERS_SOURCE_CONDITION_VARIABLE_POSIX_H_

#include <pthread.h>

#include "condition_variable_wrapper.h"
#include "typedefs.h"

namespace cp {

class ConditionVariablePosix : public ConditionVariableWrapper {
 public:
  static ConditionVariableWrapper* Create();
  virtual ~ConditionVariablePosix();

  virtual void SleepCS(CriticalSectionWrapper& crit_sect) OVERRIDE;
  virtual bool SleepCS(CriticalSectionWrapper& crit_sect,
               unsigned long max_time_in_ms) OVERRIDE;
  virtual void Wake() OVERRIDE;
  virtual void WakeAll() OVERRIDE;

 private:
  ConditionVariablePosix();
  int Construct();

 private:
  pthread_cond_t cond_;
};

}  // namespace webrtc

#endif  // WEBRTC_SYSTEM_WRAPPERS_SOURCE_CONDITION_VARIABLE_POSIX_H_
#endif