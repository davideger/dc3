# An implementation of the DC3 algorithm for Suffix Array Construction.

This is an implementation of
[Simple Linear Work Suffix Array Construction](http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.137.7871) (2003)
by Juha Karkkainen, Peter Sanders, and Stefan Burkhardt.

Implementation Copyright 2010 Google Inc.
Distributed under the Apache 2.0 License.

For more details see LICENSE and public/dc3.h

## Suffix Arrays and Longest Common Prefix Arrays:

Say you have a long string of text (think book-length) and you
would like to find the longest string in the text that is repeated.
One way to do this is to create a data structure called a suffix array.
As an example, let's take the string "bananarama."

Suffixes of "bananarama":

```
     bananarama
     ananarama
     nanarama
     anarama
     narama
     arama
     rama
     ama
     ma
     a
```

A Suffix Array is simply the lexicographic ordering of these suffixes:

```
    LCP SA  Suffixes (ordered lexicographically)
     1   9  a
     1   7  ama
     3   1  ananarama
     1   3  anarama
     0   5  arama
     0   0  bananarama
     0   8  ma
     2   2  nanarama
     0   6  narama
     0   4  rama
```

There's no need to actually allocate new space for the suffixes --
you can just present the order of their indices in the original string.

The longest repeated substring of "bananarama" is "ana" and can be seen by looking down the suffix array and noting which pair of (subsequent) suffixes overlap the most.  The Longest Common Prefix array (LCP) is what makes that part easy.  LCP[i] holds the length of the overlap of the suffixes starting from SA[i] and SA[i+1].

## Performance Notes:

Since we use a Sparse Tree Range Minimum Query, our costs are
`O(n log n)` instead of `O(n)`.  This could be improved.

+ Memory (in integers): `~ N * (13 + 2/9 log (2N/3))`
+ Worst Case Time: `O(N log N)`

## Example Usage:

```cpp
   #include "third_party/dc3/public/dc3.h"

   int *LCP, *SA;
   string interesting_text;
   // Memory limit should reflect the max allowed memory usage.
   DC3::SuffixArray(interesting_text, &SA, &LCP,
       (interesting_text.size() + 4) * (20 * sizeof(int)));

   int max_lcp_idx = 0;
   for (int i = 1; i < interesting_text.size(); i++) {
     if (LCP[i] > LCP[max_lcp_idx])
       max_lcp_idx = i;
   }

   cout << "The longest repeated string is: \""
        << interesting_text.substr(max_lcp_idx, LCP[max_lcp_idx])
        << "\" which occurs at positions " << SA[max_lcp_idx]
        << " and " << SA[max_lcp_idx + 1];
```
