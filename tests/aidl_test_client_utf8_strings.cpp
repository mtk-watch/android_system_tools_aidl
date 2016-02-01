/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "aidl_test_client_utf8_strings.h"

#include <android-base/logging.h>
#include <binder/Status.h>
#include <utils/StrongPointer.h>

#include <memory>
#include <string>
#include <vector>

#include "android/aidl/tests/ITestService.h"
#include "test_helpers.h"

// libutils:
using android::sp;

// libbinder:
using android::binder::Status;

// generated
using android::aidl::tests::ITestService;

using std::unique_ptr;
using std::string;
using std::vector;

namespace android {
namespace aidl {
namespace tests {
namespace client {

bool ConfirmUtf8InCppStringRepeat(const sp<ITestService>& s) {
  const vector<string> utf8_inputs = {
    string("Deliver us from evil."),
    string(),
    string("\0\0", 2),
    // Similarly, the utf8 encodings of the small letter yee and euro sign.
    string("\xF0\x90\x90\xB7\xE2\x82\xAC"),
  };
  LOG(INFO) << "Confirming repeating utf8 strings works.";

  for (const auto& input : utf8_inputs) {
    string reply;
    Status status = s->RepeatUtf8CppString(input, &reply);
    if (!status.isOk() || input != reply) {
      LOG(ERROR) << "Failed while requesting service to repeat utf8 string=\""
                 << input
                 << "\". Got status=" << status.toString8()
                 << " and output=" << reply;
      return false;
    }
  }

  unique_ptr<string> ret;
  Status repeat_null_status = s->RepeatNullableUtf8CppString(nullptr, &ret);
  if (!repeat_null_status.isOk() || ret) {
    LOG(ERROR) << "RepeatNullableUtf8CppString(null) did not return null";
    return false;
  }

  for (const auto& input : utf8_inputs) {
    unique_ptr<string> reply;
    Status status = s->RepeatNullableUtf8CppString(
        unique_ptr<string>(new string(input)), &reply);
    if (!status.isOk()) {
      LOG(ERROR) << "Got status=" << status.toString8() << " while repeating "
                    "nullable utf8 string " << input;
      return false;
    }
    if (!reply) {
      LOG(ERROR) << "Got null reply while repeating nullable utf8 string "
                 << input;
      return false;
    }
    if (input != *reply) {
      LOG(ERROR) << "Failed while requesting service to repeat utf8 string=\""
                 << input
                 << "\". Got status=" << status.toString8()
                 << " and output=" << *reply;
      return false;
    }
  }

  return true;
}

bool ConfirmUtf8InCppStringArrayReverse(const sp<ITestService>& s) {
  LOG(INFO) << "Confirming passing and returning utf8 string arrays works.";
  if (!ReverseArray(s, &ITestService::ReverseUtf8CppString,
                    {string{"a"}, string{}, string{"\xc3\xb8"}})) {
    return false;
  }

  return true;
}

}  // namespace client
}  // namespace tests
}  // namespace aidl
}  // namespace android
