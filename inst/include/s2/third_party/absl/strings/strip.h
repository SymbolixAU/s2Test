//
// Copyright 2017 The Abseil Authors.
//
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
// -----------------------------------------------------------------------------
// File: strip.h
// -----------------------------------------------------------------------------
//
// This file contains various functions for stripping substrings from a string.
#ifndef S2_THIRD_PARTY_ABSL_STRINGS_STRIP_H_
#define S2_THIRD_PARTY_ABSL_STRINGS_STRIP_H_

#include <cstddef>
#include <string>

#include "s2/third_party/absl/base/macros.h"
#include "s2/third_party/absl/strings/ascii.h"
#include "s2/third_party/absl/strings/ascii_ctype.h"
#include "s2/third_party/absl/strings/match.h"
#include "s2/third_party/absl/strings/string_view.h"

namespace absl {

// ConsumePrefix()
//
// Strips the `expected` prefix from the start of the given string, returning
// `true` if the strip operation succeeded or false otherwise.
//
// Example:
//
//   absl::string_view input("abc");
//   EXPECT_TRUE(absl::ConsumePrefix(&input, "a"));
//   EXPECT_EQ(input, "bc");
inline bool ConsumePrefix(absl::string_view* str, absl::string_view expected) {
  if (!absl::StartsWith(*str, expected)) return false;
  str->remove_prefix(expected.size());
  return true;
}
// ConsumeSuffix()
//
// Strips the `expected` suffix from the end of the given string, returning
// `true` if the strip operation succeeded or false otherwise.
//
// Example:
//
//   absl::string_view input("abcdef");
//   EXPECT_TRUE(absl::ConsumeSuffix(&input, "def"));
//   EXPECT_EQ(input, "abc");
inline bool ConsumeSuffix(absl::string_view* str, absl::string_view expected) {
  if (!absl::EndsWith(*str, expected)) return false;
  str->remove_suffix(expected.size());
  return true;
}

// StripPrefix()
//
// Returns a view into the input string 'str' with the given 'prefix' removed,
// but leaving the original string intact. If the prefix does not match at the
// start of the string, returns the original string instead.
inline absl::string_view StripPrefix(absl::string_view str,
                                     absl::string_view prefix) {
  if (absl::StartsWith(str, prefix)) str.remove_prefix(prefix.size());
  return str;
}

// StripSuffix()
//
// Returns a view into the input string 'str' with the given 'suffix' removed,
// but leaving the original string intact. If the suffix does not match at the
// end of the string, returns the original string instead.
inline absl::string_view StripSuffix(absl::string_view str,
                                     absl::string_view suffix) {
  if (absl::EndsWith(str, suffix)) str.remove_suffix(suffix.size());
  return str;
}

}  // namespace absl


// Returns a copy of the input string 'str' with the given 'prefix' removed. If
// the prefix doesn't match, returns a copy of the original string.
//
// The "Try" version stores the stripped string in the 'result' out-param and
// returns true iff the prefix was found and removed. It is safe for 'result' to
// point back to the input string.
//
// See also absl::ConsumePrefix().
inline string StripPrefixString(absl::string_view str,
                                absl::string_view prefix) {
  return string(absl::StripPrefix(str, prefix));
}
inline bool TryStripPrefixString(absl::string_view str,
                                 absl::string_view prefix, string* result) {
  bool res = absl::ConsumePrefix(&str, prefix);
  result->assign(str.begin(), str.end());
  return res;
}

// Returns a copy of the input string 'str' with the given 'suffix' removed. If
// the suffix doesn't match, returns a copy of the original string.
//
// The "Try" version stores the stripped string in the 'result' out-param and
// returns true iff the suffix was found and removed. It is safe for 'result' to
// point back to the input string.
//
// See also absl::ConsumeSuffix().
inline string StripSuffixString(absl::string_view str,
                                absl::string_view suffix) {
  return string(absl::StripSuffix(str, suffix));
}
inline bool TryStripSuffixString(absl::string_view str,
                                 absl::string_view suffix, string* result) {
  bool res = absl::ConsumeSuffix(&str, suffix);
  result->assign(str.begin(), str.end());
  return res;
}

// Replaces any of the characters in 'remove' with the character 'replace_with'.
//
void ReplaceCharacters(char* str, size_t len, absl::string_view remove,
                       char replace_with);
void ReplaceCharacters(string* s, absl::string_view remove, char replace_with);

// Replaces the character 'remove' with the character 'replace_with'.
//
inline void ReplaceCharacter(char* str, size_t len, char remove,
                             char replace_with) {
  for (char* end = str + len; str != end; ++str) {
    if (*str == remove) *str = replace_with;
  }
}

// Removes whitespace from both ends of the given string. This function has
// various overloads for passing the input string as a C-string + length,
// string, or absl::string_view. If the caller is using NUL-terminated strings,
// it is the caller's responsibility to insert the NUL character at the end of
// the substring.
ABSL_DEPRECATED("Use absl::StripAsciiWhitespace() instead")
inline void StripWhitespace(const char** str, ptrdiff_t* len) {
  auto stripped = absl::StripAsciiWhitespace(absl::string_view(*str, *len));
  *len = stripped.size();
  if (*len) *str = &*stripped.begin();
}
ABSL_DEPRECATED("Use absl::StripAsciiWhitespace() instead")
inline void StripWhitespace(char** str, ptrdiff_t* len) {
  auto stripped = absl::StripAsciiWhitespace(absl::string_view(*str, *len));
  *len = stripped.size();
  if (*len) *str = const_cast<char*>(&*stripped.begin());
}
ABSL_DEPRECATED("Use absl::StripAsciiWhitespace() instead")
inline void StripWhitespace(string* str) { absl::StripAsciiWhitespace(str); }

ABSL_DEPRECATED("Use absl::StripAsciiWhitespace() instead")
inline void StripWhitespace(absl::string_view* str) {
  *str = absl::StripAsciiWhitespace(*str);
}

// Removes whitespace from the end of the given string.
inline void StripTrailingWhitespace(string* s) {
  absl::StripTrailingAsciiWhitespace(s);
}

// Returns a pointer to the first non-whitespace character in 'str'. Never
// returns nullptr. 'str' must be NUL-terminated.
inline const char* SkipLeadingWhitespace(const char* str) {
  while (absl::ascii_isspace(*str)) ++str;
  return str;
}
inline char* SkipLeadingWhitespace(char* str) {
  while (absl::ascii_isspace(*str)) ++str;
  return str;
}

template <typename T>
ABSL_DEPRECATED("stop passing int*")
inline typename std::enable_if<std::is_same<T, int>::value, void>::type
StripWhitespace(const char** str, T* len) {
  ptrdiff_t pdt = *len;
  StripWhitespace(str, &pdt);
  *len = static_cast<int>(pdt);
}
template <typename T>
ABSL_DEPRECATED("stop passing int*")
inline typename std::enable_if<std::is_same<T, int>::value, void>::type
StripWhitespace(char** str, T* len) {
  ptrdiff_t pdt = *len;
  StripWhitespace(str, &pdt);
  *len = static_cast<int>(pdt);
}


#endif  // S2_THIRD_PARTY_ABSL_STRINGS_STRIP_H_
