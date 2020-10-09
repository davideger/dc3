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

#include "third_party/dc3/public/ordinal_map.h"

#include <algorithm>

// Finalize the Unicharset <-> Ordinal Mapping
void OrdinalMap::Finalize() {
  for (int i = 0; i < 128; i++) {
    ascii_to_ordinal_[i] = NotAnOrdinal();
  }
  not_a_char_ = charset_.size() > 0 ? *charset_.begin() - 1 : -1;
  to_char_ = new int[charset_.size() + 1];
  std::set<int>::const_iterator ch = charset_.begin();
  for (int ord = 1; ch != charset_.end(); ++ord, ++ch) {
    to_char_[ord] = *ch;
    if (0 <= *ch && *ch < 128) {
      ascii_to_ordinal_[*ch] = ord;
    } else {
      upper_to_ordinal_[*ch] = ord;
    }
  }
}

int OrdinalMap::ToOrdinal(int ch) const {
  if (0 <= ch && ch < 128)
    return ascii_to_ordinal_[ch];
  else if (upper_to_ordinal_.find(ch) != upper_to_ordinal_.end())
    return upper_to_ordinal_.find(ch)->second;
  return NotAnOrdinal();
}

int OrdinalMap::ToChar(int ord) const {
  if (ord < 1 || ord > charset_.size())
    return NotAChar();
  return to_char_[ord];
}
