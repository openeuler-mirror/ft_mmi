/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef UTIL_NAPI_ERROR_H
#define UTIL_NAPI_ERROR_H

#include <map>
#include <string>

namespace OHOS {
namespace MMI {
const std::string ERR_CODE = "code";
struct NapiError {
    int32_t errorCode;
    std::string msg;
};

enum NapiErrorCode : int32_t {
    OTHER_ERROR = -1,
    COMMON_PERMISSION_CHECK_ERROR = 201,
    COMMON_PARAMETER_ERROR = 401,
    COOPERATOR_TARGET_DEV_DESCRIPTOR_ERROR = 4400001,
    COOPERATOR_FAIL = 4400002,
    COOPERATOR_DEVICE_ID_ERROE = 4400003,
};
} // namespace MMI
} // namespace OHOS
#endif // UTIL_NAPI_ERROR_H
