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

#ifndef THIRD_PARTY_DC3_PUBLIC_ORDINAL_MAP_H_
#define THIRD_PARTY_DC3_PUBLIC_ORDINAL_MAP_H_

#include <unordered_map>
#include <set>

// A class for mapping a collection of unicodes to the ordinals [1..n]
//
// Usage:
//   OrdinalMap ordmap;
//   for (int i = 0; i < mystr.size(); i++)
//     ordmap.AddChar(mystr[i]);
//   ordmap.Finalize();  // We're done adding characters
//   ordmap.ToChar(ordmap.ToOrdinal(mystr[0]));  // the identity function.
class OrdinalMap {
 public:
  OrdinalMap() : not_a_char_(-1), to_char_(NULL) {}
  ~OrdinalMap() { delete [] to_char_; }

  void AddChar(int ch) { charset_.insert(ch); }

  // Call Finalize() after you've added all the unicodes you care about.
  void Finalize();

  // After Finalize(), the following functions will give you the bijection
  // between unicodes and ordinals [1..n].
  int ToOrdinal(int ch) const;
  int ToChar(int ord) const;

  int MaxOrdinal() const { return charset_.size(); }

  // Sentinels representing "out of range"
  int NotAChar() const { return not_a_char_; }
  int NotAnOrdinal() const { return -1; }

 private:
  std::set<int> charset_;  // contains all of the characters added.
  int not_a_char_;  // a character value to represent

  int ascii_to_ordinal_[128];
  std::unordered_map<int, int> upper_to_ordinal_;

  int *to_char_;
};

#endif  // THIRD_PARTY_DC3_PUBLIC_ORDINAL_MAP_H_
