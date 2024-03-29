// Copyright 2000 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS-IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

//
//

#include "s2/util/coding/coder.h"

#include <algorithm>
#include <cassert>

//#include <glog/logging.h>
#include "s2/extra/logging.h"

#include "s2/third_party/absl/base/integral_types.h"

// An initialization value used when we are allowed to
unsigned char Encoder::kEmptyBuffer = 0;

Encoder::Encoder()
  : underlying_buffer_(&kEmptyBuffer) {
}

Encoder::~Encoder() {
  if (underlying_buffer_ != &kEmptyBuffer) {
    delete[] underlying_buffer_;
  }
}

int Encoder::varint32_length(uint32 v) {
  return Varint::Length32(v);
}

int Encoder::varint64_length(uint64 v) {
  return Varint::Length64(v);
}

void Encoder::EnsureSlowPath(size_t N) {
  CHECK(ensure_allowed());
  assert(avail() < N);
  assert(length() == 0 || orig_ == underlying_buffer_);

  // Double buffer size, but make sure we always have at least N extra bytes
  const size_t current_len = length();
  const size_t new_capacity = std::max(current_len + N, 2 * current_len);

  unsigned char* new_buffer = new unsigned char[new_capacity];
  memcpy(new_buffer, underlying_buffer_, current_len);
  if (underlying_buffer_ != &kEmptyBuffer) {
    delete[] underlying_buffer_;
  }
  underlying_buffer_ = new_buffer;

  orig_ = new_buffer;
  limit_ = new_buffer + new_capacity;
  buf_ = orig_ + current_len;
  CHECK(avail() >= N);
}

void Encoder::RemoveLast(size_t N) {
  CHECK(length() >= N);
  buf_ -= N;
}

void Encoder::Resize(size_t N) {
  CHECK(length() >= N);
  buf_ = orig_ + N;
  assert(length() == N);
}
