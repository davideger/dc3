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

#include "third_party/dc3/public/st_rmq.h"

#include <algorithm>

//#include "base/logging.h"

inline int log2(int x) {
  int rv = 0;
  while (x >>= 1) { rv++; }
  return rv;
}

SparseTreeRangeMinimumQuery::SparseTreeRangeMinimumQuery(
    const int *arr,
    int n,
    int K)
    : RangeMinimumQuery(arr, n),
      K_(std::max(1, std::min(30, K))),
      num_blocks_(1 + log2(n) / K_) {
  // VLOG(2) << "SparseTreeRangeMinimumQuery(n = " << n
  //        << ", K = " << K << ")";
  block_ = new int[num_blocks_ * n_];
  global_min_ = arr_[0];
  for (int i = 0; i < n_; i++) {
    block_[i] = i;
    if (arr_[i] < global_min_) global_min_ = arr_[i];
  }

  for (int p = 1; p <= K_ * (num_blocks_ - 1); p++) {
    // Compute index of the minima over block size 2^p in thisblock
    // from the 2^(p-1) block size data in prevblock.
    int prevlevel = (p + K_ - 2) / K_;
    int thislevel = (p + K_ - 1) / K_;
    int *prevblock = block_ + n_ * prevlevel;
    int *thisblock = block_ + n_ * thislevel;
    for (int i = 0; i < n_; i++) {
      int j = std::min(n - 1, i + (1 << (p - 1)));
      int i1 = prevblock[i];
      int i2 = prevblock[j];
      thisblock[i] = (arr_[i1] <= arr_[i2]) ? i1 : i2;
    }
  }
}

int SparseTreeRangeMinimumQuery::MemLimit(int n, int memlimit) {
  int block_size = sizeof(int) * n;
  int available_blocks = memlimit / block_size - 1;
  if (available_blocks < 1)
    return 40;
  return 1 + log2(n) / available_blocks;
}

int SparseTreeRangeMinimumQuery::Query(int i, int j) const {
  if (j <= i || j <= 0 || i >= n_) {
    // LOG(ERROR) << "Bad Range Minimum Query: [" << i << ", " << j << ")";
    return -1;
  }
  i = std::max(i, 0);
  j = std::min(j, n_);

  int level = log2(j - i) / K_;
  int bs = 1 << (level * K_);

  int min_possible = global_min_;
  if (level + 1 < num_blocks_) {
    min_possible = arr_[block_[(level + 1) * n_ + i]];
  }

  int minidx = block_[level * n_ + i];
  for (i += bs; i + bs <= j; i += bs) {
    if (arr_[minidx] == min_possible)
      return minidx;  // short circuit if we've already reached bottom.
    int idx = block_[level * n_ + i];
    if (arr_[idx] < arr_[minidx]) minidx = idx;
  }
  int idx = block_[level * n_ + j - bs];
  if (arr_[idx] < arr_[minidx]) minidx = idx;

  return minidx;
}
