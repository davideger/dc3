// Copyright 2010 Google Inc.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ---
// Author: David Eger

#ifndef THIRD_PARTY_DC3_INTERNAL_TRIVIAL_RMQ_H_
#define THIRD_PARTY_DC3_INTERNAL_TRIVIAL_RMQ_H_

#include "third_party/dc3/public/rmq.h"

// A data structure which provides range minimum queries using
// No precomputation or space and O(n) lookup time.
class TrivialRangeMinimumQuery : public RangeMinimumQuery {
 public:
  TrivialRangeMinimumQuery(const int *arr, int n)
      : RangeMinimumQuery(arr, n) {}
  virtual int Query(int i, int j) const;
};

#endif  // THIRD_PARTY_DC3_INTERNAL_TRIVIAL_RMQ_H_
