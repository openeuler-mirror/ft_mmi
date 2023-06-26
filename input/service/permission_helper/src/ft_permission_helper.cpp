/*
 * Copyright (c) 2022 Huawei Technologies Co., Ltd.
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

#include "permission_helper.h"
#include "proto.h"
#include "ipc_skeleton.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, MMI_LOG_DOMAIN, "PermissionHelper"};
} // namespace

PermissionHelper::PermissionHelper() {}
PermissionHelper::~PermissionHelper() {}

bool PermissionHelper::CheckPermission(uint32_t required)
{
    return true;
}

bool PermissionHelper::CheckMonitor()
{
    return true;
}

bool PermissionHelper::CheckHapPermission(uint32_t tokenId, uint32_t required)
{
    return true;
}

bool PermissionHelper::CheckMonitorPermission(uint32_t tokenId)
{
    return true;
}

int32_t PermissionHelper::GetTokenType()
{
    return TOKEN_HAP;
}
} // namespace MMI
} // namespace OHOS