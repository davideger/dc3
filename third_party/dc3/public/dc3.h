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

#ifndef THIRD_PARTY_DC3_PUBLIC_DC3_H_
#define THIRD_PARTY_DC3_PUBLIC_DC3_H_

#include <set>
#include <string>
#include <vector>

#include "third_party/dc3/public/ordinal_map.h"
#include "third_party/dc3/public/rmq.h"

// An implementation of the DC3 algorithm for Suffix Array Construction.
//
// Based on: "Simple Linear Work Suffix Array Construction" (2003)
//             by Juha Karkkainen, Peter Sanders, and Stefan Burkhardt
// http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.137.7871
//
// Explanation of Suffix Arrays and Longest Common Prefix Arrays:
//
//   Say you have a long string of text (think book-length) and you
//   would like to find the longest string in the text that is repeated.
//   One way to do this is to create a data structure called a suffix array.
//   As an example, let's take the string "bananarama."
//
//   Suffixes of "bananarama":
//
//     bananarama
//     ananarama
//     nanarama
//     anarama
//     narama
//     arama
//     rama
//     ama
//     ma
//     a
//
//   A Suffix Array is simply the lexicographic ordering of these suffixes:
//
//    LCP SA  Suffixes (ordered lexicographically)
//     1   9  a
//     1   7  ama
//     3   1  ananarama
//     1   3  anarama
//     0   5  arama
//     0   0  bananarama
//     0   8  ma
//     2   2  nanarama
//     0   6  narama
//     0   4  rama
//
//   There's no need to actually allocate new space for the suffixes --
//   you can just present the order of their indices in the original string.
//
//   The longest repeated substring of "bananarama" is "ana" and can be seen
//   by looking down the suffix array and noting which pair of (subsequent)
//   suffixes overlap the most.  The Longest Common Prefix array (LCP) is
//   what makes that part easy.  LCP[i] holds the length of the overlap of
//   the suffixes starting from SA[i] and SA[i+1].
//
// Performance Notes:
//
//   Since we use a Sparse Tree Range Minimum Query, our costs are
//   O(n log n) instead of O(n).  This could be improved.
//
//   Memory (in integers): ~ N * (13 + 2/9 log (2N/3))
//   Worst Case Time: O(N log N)
//
// Example Usage:
//
//   #include "third_party/dc3/public/dc3.h"
//
//   int *LCP, *SA;
//   string interesting_text;
//   // Memory limit should reflect the max allowed memory usage.
//   DC3::SuffixArray(interesting_text, &SA, &LCP,
//       (interesting_text.size() + 4) * (20 * sizeof(int)));
//
//   int max_lcp_idx = 0;
//   for (int i = 1; i < interesting_text.size(); i++) {
//     if (LCP[i] > LCP[max_lcp_idx])
//       max_lcp_idx = i;
//   }
//
//   cout << "The longest repeated string is: \""
//        << interesting_text.substr(max_lcp_idx, LCP[max_lcp_idx])
//        << "\" which occurs at positions " << SA[max_lcp_idx]
//        << " and " << SA[max_lcp_idx + 1];
class DC3 {
 public:
  // Calculate two arrays of length >= s.size() that contain the Suffix
  // Array and Longest Common Prefix Array for the given string s.
  // The Suffix Array should be a permutation of [0..n-1].
  // We allocate the two integer arrays with new[] and put in *SA and *LCP.
  template<typename StrType>
  static void SuffixArray(const StrType &str, int **SA, int **LCP,
                          int memlimit) {
    int n = str.size();
    *SA = new int[n + 3];
    (*SA)[0] = (*SA)[n] = (*SA)[n + 1] = (*SA)[n + 2] = 0;
    *LCP = new int[n + 3];
    (*LCP)[0] = (*LCP)[n] = (*LCP)[n + 1] = (*LCP)[n + 2] = 0;

    if (n < 2)
      return;

    int *STR = new int[n + 3];
    STR[n] = STR[n + 1] = STR[n + 2] = 0;

    OrdinalMap ordmap;
    for (int i = 0; i < str.size(); i++) {
      ordmap.AddChar(str[i]);
    }
    ordmap.Finalize();
    for (int i = 0; i < str.size(); i++) {
      STR[i] = ordmap.ToOrdinal(str[i]);
    }
    SuffixArray(STR, *SA, *LCP, str.size(), ordmap.MaxOrdinal(),
                memlimit - 3 * sizeof(int) * (n + 3));
    delete [] STR;
    //VLOG(1) << "SuffixArray(str[" << str.size() << "], SA, LCP); Done!";
  }

 private:
  // Given a string s taken from [1..K]^n, compute the Suffix Array SA.
  // We requite s[n] = s[n+1] = s[n+2] = 0, and that n >= 2.
  static void SuffixArray(const int *s, int *SA, int *LCP, int n, int K,
                          int memlimit);

  // The part of SuffixArray() that chooses the lesser of suffixes described
  // by of SA0[y0] or SA12[y1] to be the next SA[ys] and increments the one of
  // y0 or y1 it chose.  MergeNext also sets LCP[ys - 1].
  //
  // See implementation comments in dc3.cc for the gory details.
  static void MergeNext(
      int ys,  const int *s, int *SA, int *LCP, int n0, int n02,
      int &y1, const int *s12, const int *SA12, const int *LCP12,
          const RangeMinimumQuery *RMQ12,
      int &y0,                 const int *SA0, const int *LCP0);
};

#endif  // THIRD_PARTY_DC3_PUBLIC_DC3_H_
