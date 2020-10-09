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
//
// An implementation of the DC3 algorithm for Suffix Array Construction.
// This implementation is currently O(n log n) but can be made O(n) by improving
// the implementation of Range Minimum Query.
//
// Based on: "Simple Linear Work Suffix Array Construction" (2003)
//             by Juha Karkkainen, Peter Sanders, and Stefan Burkhardt
//

#include <algorithm>

// #include "base/logging.h"
#include "third_party/dc3/public/dc3.h"
#include "third_party/dc3/public/st_rmq.h"

// lexicographic ordering for pairs and triples.
static bool leq(int a1, int a2, int b1, int b2) {
  return (a1 < b1 || (a1 == b1 && a2 <= b2));
}

static bool leq(int a1, int a2, int a3, int b1, int b2, int b3) {
  return (a1 < b1 || (a1 == b1 && leq(a2, a3, b2, b3)));
}

static void inc(int *p) { ++(*p); }

// Stably sort a[0..n-1] to b[0..n-1] with keys in 0..K from r
static void RadixPass(const int *a, int *b, const int *r, int n, int K) {
  int *c = new int[K + 1];  // counters
  for (int i = 0;  i <= K;  i++) c[i] = 0;
  for (int i = 0;  i < n;  i++) {
    //CHECK(r[a[i]] <= K);
    c[r[a[i]]]++;
  }
  for (int i = 0, sum = 0; i <= K; i++) {  // exclusive prefix sums
     int t = c[i];
     c[i] = sum;
     sum += t;
  }
  for (int i = 0;  i < n;  i++) b[c[r[a[i]]]++] = a[i];     // sort
  delete [] c;
}

// Given the suffix array for 0 mod 3 suffixes of a string S, and an index in
// the suffix array i, return the starting index of the corresponding suffix
// of S.
int StrIndex(const int *SA0, int i) {
  return SA0[i];
}

// Given the suffix array for 1 and 2 mod 3 suffixes of a string S (renamed
// using the ranks of consecutive triples as described in SuffixArray())
// and an index in the suffix array i, return the starting index of the
// corresponding suffix of S.
int StrIndex(const int *SA12, int n0, int i) {
  if (SA12[i] < n0)
    return SA12[i] * 3 + 1;  // the 1 mod 3 side of SA12
  else
    return (SA12[i] - n0) * 3 + 2;  // the 2 mod 3 side of SA12
}

// Check that arr[0, n) are all values in [lo, hi)
void DCheckArrayRange(const int *arr, int n, int lo, int hi) {
#if 0
  if (VLOG_IS_ON(2)) {
    // This checking is quite expensive, and we should not
    // do it unless the caller is really quite sure they're trying
    // to debug this module.
    if (n == 0)
      return;
    int minval, maxval;
    minval = maxval = arr[0];
    for (int i = 1; i < n; i++) {
      if (arr[i] < minval) minval = arr[i];
      if (arr[i] > maxval) maxval = arr[i];
    }
    CHECK(minval >= lo);
    CHECK(maxval < hi);
  }
#endif
}

// Check that s[i,i+overlap) = s[j,j+overlap) and s[i+overlap] < s[j+overlap]
void DCheckOrderAndOverlap(const int *s, int n, int i, int j, int overlap) {
#if 0
  if (VLOG_IS_ON(2)) {
    // This checking is quite expensive, and we should not
    // do it unless the caller is really quite sure they're trying
    // to debug this module.
    CHECK(0 <= i && i < n);
    CHECK(0 <= j && j < n);
    CHECK(i + overlap <= n);
    CHECK(j + overlap <= n);
    for (int x = 0; x < overlap; x++) {
      CHECK(s[i + x] == s[j + x]);
    }
    CHECK(s[i + overlap] < s[j + overlap]);
  }
#endif
}


// Find the suffix array SA of s[0..n-1] in {1..K}^n
// require s[n]=s[n+1]=s[n+2]=0, n >= 2
void DC3::SuffixArray(const int *s, int *SA, int *LCP, int n, int K,
                      int memlimit) {
  //VLOG(2) << "DC3::SuffixArray(s, SA, LCP, " << n << ", " << K << ")";
  DCheckArrayRange(s, n, 0, K + 1);
  int n0 = (n + 2) / 3;
  int n2 = n / 3;
  int n02 = n0 + n2;
  int *s12  = new int[n02 + 3];
  int *SA12 = new int[n02 + 3];
  int *LCP12 = new int[n02 + 3];
  s12[n02] = s12[n02 + 1] = s12[n02 + 2] = 0;
  SA12[n02] = SA12[n02 + 1] = SA12[n02 + 2] = 0;
  LCP12[n02 - 1] = LCP12[n02] = LCP12[n02 + 1] = LCP12[n02 + 2] = 0;

  int *s0   = new int[n0];
  int *SA0  = new int[n0];
  int *LCP0 = new int[n0];

  // For many loops below we insert an extra "dummy" mod 1 suffix so
  // that there are as many mod 1 entries as there are mod 0 entries.
  int n1pad = (n % 3) == 1 ? 1 : 0;

  // generate positions of 1 mod 3 and 2 mod 3 suffixes
  // Example: n = 12 => s12 = [1,2,4,5,7,8,10,11]
  for (int i = 0, j = 0; i < n + n1pad; i++) {
    if (i % 3 != 0) {
      s12[j++] = i;
    }
  }

  // Radix sort the mod 1 and mod 2 triples
  RadixPass(s12, SA12, s + 2, n02, K);
  RadixPass(SA12, s12, s + 1, n02, K);
  RadixPass(s12, SA12, s  , n02, K);

  // At this point, SA12 contains the mod 1 and mod 2 indices, sorted
  // by the three-letter sequences they index:
  //   s[SA12[i]], s[SA12[i] + 1], s[SA12[i] + 2]

  // Find lexicographic names of triples
  int name = 0, c0 = -1, c1 = -1, c2 = -1, lcp;
  int *plcp = &lcp;
  for (int i = 0;  i < n02;  i++) {
    *plcp = 0;
    int ti = SA12[i];
    if (s[ti] == c0) {
      inc(plcp);
      if (s[ti + 1] == c1) {
        inc(plcp);
        if (s[ti + 2] == c2) {
          inc(plcp);
        }
      }
    }
    if (*plcp != 3) {
      name++;
      c0 = s[ti];
      c1 = s[ti + 1];
      c2 = s[ti + 2];
    }
    if (ti % 3 == 1) {
      s12[ti / 3] = name;  // left half
    } else {
      s12[ti / 3 + n0] = name;  // right half
    }
    plcp = &LCP12[i];
  }
  DCheckArrayRange(LCP12, n02, 0, n02);

  // At this point, we have an implicit name map: s[i] -> [1..n02] + [bot]
  // which (may) order the 1&2 mod 3 suffixes of s lexicographically.
  // 0 mod 3 values map to bottom, and we store in s12 the mappings of
  // s12 = name|s[1 mod 3] :: name|s[2 mod 3]
  //
  // It may happen that there are duplicate triples, though, in which
  // case we must recurse for unique names.

  if (name < n02) {
    // Some duplicate triple names. Recursing.
    DC3::SuffixArray(s12, SA12, LCP12, n02, name,
                     memlimit - 3 * n * sizeof(int));
    // store unique names in s12 using the suffix array
    for (int i = 0; i < n02; i++) s12[SA12[i]] = i + 1;
    DCheckArrayRange(LCP12, n02, 0, n02);
    // And adjust the Longest Common Prefixes.
    for (int i = 0; i < n02 - 1; i++) {
      int ti = StrIndex(SA12, n0, i);
      int tn = StrIndex(SA12, n0, i + 1);
      int overlap = 3 * LCP12[i];
      while (s[ti + overlap] == s[tn + overlap])
        overlap++;
      LCP12[i] = overlap;
    }
  } else {
    // Triples unique.
    for (int i = 0; i < n02; i++)
      SA12[s12[i] - 1] = i;
  }

  int memleft = memlimit - 3 * (n + 3) * sizeof(int);
  SparseTreeRangeMinimumQuery RMQ12(
      LCP12, n02, SparseTreeRangeMinimumQuery::MemLimit(n, memleft));

  // At this point, SA12 contains the suffix array for the s12 subset of s
  // where SA12: [0 .. n02-1] -> [0 .. n02-1] is interpreted as follows:
  // if 0 <= SA12[i] < n0,
  //    then idx in s is SA12[i] * 3 + 1
  //         idx in s12 is SA12[i]
  //         idx in LCP12 is i
  //    [nota bene: be careful, there is an "extra" mod 1 suffix if n%3 = 1]
  //
  // if n0 <= SA12[i] < n02
  //    then idx in s is (SA12[i] - n0) * 3 + 2
  //         idx in s12 is SA12[i]
  //         idx in LCP12 is i

  // Stably sort the mod 0 suffixes from SA12 by their first character
  for (int i = 0, j = 0;  i < n02;  i++) {
    if (SA12[i] < n0) {
      s0[j++] = SA12[i] * 3;
    }
  }
  RadixPass(s0, SA0, s, n0, K);
  // At this point, SA0 contains the 0 mod 3 indices [0, 3, 6, 9...] sorted
  //  by suffix s[SA[i]..)

  for (int i = 0; i < n0 - 1; i++) {
    if (s[SA0[i]] != s[SA0[i + 1]]) {
      LCP0[i] = 0;
    } else {
      int s12i = s12[SA0[i] / 3] - 1;
      int s12j = s12[SA0[i + 1] / 3] - 1;
      LCP0[i] = 1 + LCP12[RMQ12.Query(std::min(s12i, s12j), std::max(s12i, s12j))];
    }
    DCheckOrderAndOverlap(s, n, SA0[i], SA0[i + 1], LCP0[i]);
  }
  LCP0[n0 - 1] = 0;

  // At this point, LC0[i] contains the length of overlap between
  //   s[SA[i]..) and s[SA[i+1]..)

  DCheckArrayRange(SA0, n0, 0, n);
  DCheckArrayRange(SA12, n02, 0, n02);
  DCheckArrayRange(LCP0, n0, 0, n);
  DCheckArrayRange(LCP12, n02, 0, n);

  // merge sorted SA0 suffixes and sorted SA12 suffixes
  for (int y0 = 0, y1 = n1pad, ys = 0; ys < n; ys++) {
    DC3::MergeNext(ys, s, SA, LCP, n0, n02,
                   y1, s12, SA12, LCP12, &RMQ12,
                   y0, SA0, LCP0);
    if (ys > 0)
      DCheckOrderAndOverlap(s, n, SA[ys - 1], SA[ys], LCP[ys - 1]);
  }
  LCP[n - 1] = 0;
  delete [] s12;
  delete [] SA12;
  delete [] LCP12;
  delete [] s0;
  delete [] SA0;
  delete [] LCP0;
}

// Decide which of the suffixes indicated by SA0[y0] or SA12[y1] are
// lexicographically lower and should be put next in SA[ys].
//   SA0[y0]   == StrIndex ==>  s[i...)
//   SA12[y1]  == StrIndex ==>  s[j...)
//
// If ys > 0, then set LCP[ys - 1].
//
// SA12 [y1]  <- SA12 contains s12 indices.
//  SA0 [y0]  <- SA0 contains s indices.
//   SA [ys]  <- SA  contains s indices.
//       |
//  the point we're inserting
//
// As in the code of SuffixArray(), n0 is the number of mod 0 suffixes, and
// is the break point between the mod 1 and mod 2 suffixes in SA12.
//
// LCP0 and LCP12 are longest common prefix lengths in terms of s.
void DC3::MergeNext(int ys,  const int *s, int *SA, int *LCP, int n0, int n02,
                    int &y1, const int *s12, const int *SA12, const int *LCP12,
                    const RangeMinimumQuery *RMQ12,
                    int &y0,                 const int *SA0, const int *LCP0) {
  // Set SA[ys]
  if (y0 == n0) {
    // Only 1 & 2 mod 3 suffixes left.
    SA[ys] = StrIndex(SA12, n0, y1);
  } else if (y1 == n02) {
    // Only 0 mod 3 suffixes left.
    SA[ys] = StrIndex(SA0, y0);
  } else {
    // We have to choose between a 1 or 2 mod 3 suffix and a 0 mod 3 suffix.
    int i = StrIndex(SA0, y0);
    int j = StrIndex(SA12, n0, y1);
    bool mod0_smaller;

    if (SA12[y1] < n0) {
      // SA12[y1] corresponds to a 1 mod 3 suffix of s
      mod0_smaller = leq(s[i], s12[i / 3],
                         s[j], s12[SA12[y1] + n0]);
    } else {
      // SA12[y1] corresponds to a 2 mod 3 suffix of s
      mod0_smaller = leq(s[i], s[i + 1], s12[i / 3 + n0],
                         s[j], s[j + 1], s12[SA12[y1] - n0 + 1]);
    }

    SA[ys] = mod0_smaller ? i : j;
  }

  if (ys > 0) {  // Set LCP[ys - 1].
    int lcp = 0;

    // i, j, y1t have dummy values - or gcc will whine.
    int i = -1;  // the 0 mod 3 index (if applicable)
    int j = -1;  // the 1 or 2 mod 3 index (if applicable)
    int y1t = -1;  // y1t = y1 or y1 -1, according to which led to j

    // set i, j, or y1t if it is actually used below.
    if (SA[ys - 1] % 3 == 0) {
      i = SA[ys - 1];
      y1t = y1;
    }
    if (SA[ys] % 3 == 0) {
      i = SA[ys];
      y1t = y1 - 1;
    }
    if (SA[ys - 1] % 3 != 0) { j = SA[ys - 1]; }
    if (SA[ys] % 3 != 0) { j = SA[ys]; }

    // Calculate lcp.
    if (SA[ys - 1] % 3 == 0 && SA[ys] % 3 == 0) {
      lcp = LCP0[y0 - 1];
    } else if (SA[ys - 1] % 3 != 0 && SA[ys] % 3 != 0) {
      lcp = LCP12[y1 - 1];
    } else if (SA[ys - 1] % 3 == 1 || SA[ys] % 3 == 1) {
      // 0 mod 3 versus 1 mod 3
      if (s[i] == s[j]) {
        if (SA12[y1t] + n0 == n02) {
          lcp = 1;
        } else {
          int s12i = s12[i / 3] - 1;
          int s12j = s12[SA12[y1t] + n0] - 1;
          lcp = 1 + LCP12[RMQ12->Query(std::min(s12i, s12j), std::max(s12i, s12j))];
        }
      }
    } else {
      // 0 mod 3 versus 2 mod 3
      if ((s[i] == s[j]) && (inc(&lcp), s[i + 1] == s[j + 1])) {
        if (i / 3 + n0 == n02) {
          lcp = 2;
        } else {
          int s12i = s12[i / 3 + n0] - 1;
          int s12j = s12[SA12[y1t] - n0 + 1] - 1;
          lcp = 2 + LCP12[RMQ12->Query(std::min(s12i, s12j), std::max(s12i, s12j))];
        }
      }
    }
    LCP[ys - 1] = lcp;
  }

  // Finally, advance the index of the set of suffixes we consumed.
  if (SA[ys] % 3 == 0) {
    y0++;
  } else {
    y1++;
  }
}
