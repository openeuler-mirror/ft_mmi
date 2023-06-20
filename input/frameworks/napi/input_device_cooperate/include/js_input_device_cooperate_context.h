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

#ifndef JS_INPUT_DEVICE_COOPERATE_CONTEXT_H
#define JS_INPUT_DEVICE_COOPERATE_CONTEXT_H

#include <memory>
#include <mutex>

#include "cooperation_message.h"
#include "js_input_device_cooperate_manager.h"
#include "nocopyable.h"

namespace OHOS {
namespace MMI {
class JsInputDeviceCooperateContext {
public:
    JsInputDeviceCooperateContext();
    ~JsInputDeviceCooperateContext();
    DISALLOW_COPY_AND_MOVE(JsInputDeviceCooperateContext);

    static napi_value Export(napi_env env, napi_value exports);
    static napi_value Enable(napi_env env, napi_callback_info info);
    static napi_value Start(napi_env env, napi_callback_info info);
    static napi_value Stop(napi_env env, napi_callback_info info);
    static napi_value GetState(napi_env env, napi_callback_info info);
    static napi_value On(napi_env env, napi_callback_info info);
    static napi_value Off(napi_env env, napi_callback_info info);

    std::shared_ptr<JsInputDeviceCooperateManager> GetJsInputDeviceCooperateMgr();

private:
    std::shared_ptr<JsInputDeviceCooperateManager> mgr_ { nullptr };
    std::mutex mutex_;
    napi_ref contextRef_ { nullptr };

    static napi_value CreateInstance(napi_env env);
    static napi_value JsConstructor(napi_env env, napi_callback_info info);
    static JsInputDeviceCooperateContext *GetInstance(napi_env env);
    static void DeclareDeviceCooperateInterface(napi_env env, napi_value exports);
    static void DeclareDeviceCooperateData(napi_env env, napi_value exports);
    static napi_value EnumClassConstructor(napi_env env, napi_callback_info info);
};
} // namespace MMI
} // namespace OHOS
#endif // JS_INPUT_DEVICE_COOPERATE_CONTEXT_H
