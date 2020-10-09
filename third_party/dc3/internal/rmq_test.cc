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

// Sanity check optimized implementations of Range Minimum
// data structures against the trivial implementation.

#include "testing/base/public/gunit.h"

#include "strings/join.h"
#include "third_party/abcl/strings/str_cat.h"
#include "third_party/dc3/internal/trivial_rmq.h"
#include "third_party/dc3/public/rmq.h"
#include "third_party/dc3/public/st_rmq.h"

namespace {

void VerifyVsTrivial(const int *array, int len, RangeMinimumQuery *rmq) {
  TrivialRangeMinimumQuery trmq(array, len);
  for (int i = 0; i < len; i++) {
    for (int j = i + 1; j < len; j++) {
      EXPECT_EQ(array[trmq.Query(i, j)], array[rmq->Query(i, j)])
          << abcl::StrCat("[", i, ", ", j, ")");
    }
  }
}

void VerifySparseTreeVsTrivial(const int *array, int len, int K) {
  SparseTreeRangeMinimumQuery strmq(array, len, K);
  VerifyVsTrivial(array, len, &strmq);
}

const char *text =
    "0:This sample text is 50 characters wide.  The dou"
    "1:ble-quote appears at character 5 and the last te"
    "2:xtual character before the closing double quote "
    "3:appears at character 55. 55-5=50.";

TEST(RmqTest, SparseTreeRangeMinimumTextMatch) {
  int text_len = strlen(text);
  int *text_array = new int[text_len];
  for (int i = 0; i < text_len; i++) {
    text_array[i] = text[i];
  }
  VerifySparseTreeVsTrivial(text_array, text_len, 1);
  VerifySparseTreeVsTrivial(text_array, text_len, 2);
  VerifySparseTreeVsTrivial(text_array, text_len, 3);
  VerifySparseTreeVsTrivial(text_array, text_len, 4);
  VerifySparseTreeVsTrivial(text_array, text_len, 14);
  delete [] text_array;
}

const int sample_array[] = {22, 0, 0, 2, -5, 100, 4, 2, 72, -1 -6, 0};

TEST(RmqTest, SparseTreeRangeMinimumIntMatch) {
  VerifySparseTreeVsTrivial(sample_array, arraysize(sample_array), 0);
  VerifySparseTreeVsTrivial(sample_array, arraysize(sample_array), 2);
  VerifySparseTreeVsTrivial(sample_array, arraysize(sample_array), 3);
  VerifySparseTreeVsTrivial(sample_array, arraysize(sample_array), 4);
  VerifySparseTreeVsTrivial(sample_array, arraysize(sample_array), 15);
}

}  // namespace
