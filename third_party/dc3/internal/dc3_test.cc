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

#include "file/base/file.h"
#include "file/base/helpers.h"
#include "file/base/path.h"
#include "testing/base/public/gunit.h"
#include "third_party/dc3/public/dc3.h"
#include "util/task/status.h"

namespace {

string DataFile(const string &filename) {
  return file::JoinPath(
      FLAGS_test_srcdir, "google3/third_party/dc3/internal/testdata", filename);
}

class Dc3Test : public ::testing::Test {
 protected:
  void SetUp() {
    CHECK_OK(file::GetContents(DataFile("blobbox.cpp.2007.txt"), &text_file_,
                               file::Defaults()));
  }
  string text_file_;
};

// Assert that array[0..n-1] holds a permutation of [0..n-1]
void AssertIsPermutation(int *array, int n) {
  bool *accounted_for_indices = new bool[n];
  for (int i = 0; i < n; i++) {
    accounted_for_indices[i] = false;
  }

  for (int i = 0; i < n; i++) {
    int index = array[i];
    ASSERT_GE(index, 0);
    ASSERT_LT(index, n);
    accounted_for_indices[index] = true;
  }

  for (int i = 0; i < n; i++) {
    EXPECT_EQ(accounted_for_indices[i], true);
  }

  delete []accounted_for_indices;
}

void AssertOrderAndOverlapsAreCorrect(const string &s, int *SA, int *LCP) {
  if (s.size() == 0)
    return;
  for (int i = 0; i < s.size() - 1; i++) {
    int expected_overlap = LCP[i], overlap = 0;
    int x = SA[i], y = SA[i+1];
    ASSERT_GE(expected_overlap, 0);
    ASSERT_LE(x + expected_overlap, s.size());
    ASSERT_LE(y + expected_overlap, s.size());
    while (x + overlap < s.size() && y + overlap < s.size() &&
           s[x + overlap] == s[y + overlap]) {
      overlap++;
    }
    EXPECT_LT(s[x + overlap], s[y + overlap]);
    EXPECT_EQ(overlap, expected_overlap);
  }
}

void TestDC3(const string &str) {
  int *SA, *LCP;
  DC3::SuffixArray(str, &SA, &LCP, 300000);
  int n = str.size();

  ASSERT_TRUE(SA != NULL);
  ASSERT_TRUE(LCP != NULL);

  AssertIsPermutation(SA, n);
  AssertOrderAndOverlapsAreCorrect(str, SA, LCP);

  delete []SA;
  delete []LCP;
}

TEST_F(Dc3Test, Digits) {
  string digits = "123456789";
  TestDC3(digits);
}

TEST_F(Dc3Test, Yabbadabbado) {
  string digits = "yabbadabbado";
  TestDC3(digits);
}

TEST_F(Dc3Test, Empty) {
  string empty;
  TestDC3(empty);
}

TEST_F(Dc3Test, FiftyAs) {
  string as;
  for (int i = 0; i < 50; i++) {
    as.push_back('a');
  }
  TestDC3(as);
}

TEST_F(Dc3Test, HundredFiftyAs) {
  string as;
  for (int i = 0; i < 150; i++) {
    as.push_back('a');
  }
  TestDC3(as);
}

TEST_F(Dc3Test, TwoHundredFiftyAs) {
  string as;
  for (int i = 0; i < 250; i++) {
    as.push_back('a');
  }
  TestDC3(as);
}


TEST_F(Dc3Test, FiftyThousandAs) {
  string as;
  for (int i = 0; i < 50000; i++) {
    as.push_back('a');
  }
  TestDC3(as);
}


TEST_F(Dc3Test, ProducesCorrectSuffixForBookLcsCC) {
  TestDC3(text_file_);
}

}  // namespace
