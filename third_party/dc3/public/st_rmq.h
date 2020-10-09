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

#ifndef THIRD_PARTY_DC3_PUBLIC_ST_RMQ_H_
#define THIRD_PARTY_DC3_PUBLIC_ST_RMQ_H_

#include "third_party/dc3/public/rmq.h"

// A data structure which provides range minimum queries using
// O(n log n) precomputation time and space and O(1) lookup time.
class SparseTreeRangeMinimumQuery : public RangeMinimumQuery {
 public:
  SparseTreeRangeMinimumQuery(const int *arr, int n, int K = 3);

  // Given a memory limit in bytes and an input in number of ints,
  // produce an appropriate K to try to stay within the memory limit.
  static int MemLimit(int n, int memlimit);
  virtual int Query(int i, int j) const;
  virtual ~SparseTreeRangeMinimumQuery() { delete[] block_; }

 private:
  const int K_;  // Use 1/K_* as much memory for 2^K_* query time.
  const int num_blocks_;  // = 1 + log2(n_) / K_

  // block_ is arranged into num_blocks_ blocks of length n, where
  // block_[x*n + i] is the index j in [i, i + 2^(K_ * x)) minimizing arr[j].
  int *block_;

  // global minimum value of the array, used to decrease
  // the average number of memory accesses for K > 1.
  int global_min_;
};

#endif  // THIRD_PARTY_DC3_PUBLIC_ST_RMQ_H_
