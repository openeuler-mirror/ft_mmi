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
#include "bytrace_adapter.h"

#include <string>

#include "define_multimodal.h"
// #include "hitrace_meter.h"
#include "mmi_log.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "BytraceAdapter" };
const std::string onKeyEvent { "OnKeyEvent" };
const std::string onTouchEvent { "OnTouchEvent" };
const std::string onPointerEvent { "OnPointerEvent" };
const std::string keyEventDispatch { "KeyEventDispatch" };
const std::string touchEventDispatch { "TouchEventDispatch" };
const std::string pointerEventDispatch { "PointerEventDispatch" };
const std::string keyEventSubscribe { "KeyEventSubscribe" };
const std::string pointerEventIntercept { "PointerEventIntercept" };
const std::string touchEventIntercept { "TouchEventIntercept" };
const std::string keyEventIntercept { "KeyEventIntercept" };
const std::string startEvent { "StartEvent" };
const std::string launchEvent { "LaunchEvent" };
const std::string stopEvent { "StopEvent" };
constexpr int32_t START_ID = 1;
constexpr int32_t LAUNCH_ID = 2;
constexpr int32_t STOP_ID = 3;
} // namespace

void BytraceAdapter::StartBytrace(std::shared_ptr<KeyEvent> keyEvent)
{
    CHKPV(keyEvent);
    // int32_t keyId = keyEvent->GetId();
    // StartAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, onKeyEvent, keyId);
    // HITRACE_METER_NAME(HITRACE_TAG_MULTIMODALINPUT, "service report keyId=" + std::to_string(keyId));
}

void BytraceAdapter::StartBytrace(std::shared_ptr<PointerEvent> pointerEvent, TraceBtn traceBtn)
{
    CHKPV(pointerEvent);
    // int32_t eventId = pointerEvent->GetId();
    // if (traceBtn == TRACE_START) {
    //     if (pointerEvent->GetSourceType() == PointerEvent::SOURCE_TYPE_MOUSE) {
    //         StartAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, onPointerEvent, eventId);
    //         HITRACE_METER_NAME(HITRACE_TAG_MULTIMODALINPUT, "service report pointerId:" + std::to_string(eventId));
    //     } else {
    //         StartAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, onTouchEvent, eventId);
    //         HITRACE_METER_NAME(HITRACE_TAG_MULTIMODALINPUT, "service report touchId:" + std::to_string(eventId));
    //     }
    // } else {
    //     if (pointerEvent->GetSourceType() == PointerEvent::SOURCE_TYPE_MOUSE) {
    //         FinishAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, onPointerEvent, eventId);
    //     } else {
    //         FinishAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, onTouchEvent, eventId);
    //     }
    // }
}

void BytraceAdapter::StartBytrace(std::shared_ptr<KeyEvent> key, HandlerType handlerType)
{
    CHKPV(key);
    int32_t keyCode = key->GetKeyCode();
    std::string checkKeyCode;
    switch (handlerType) {
        case KEY_INTERCEPT_EVENT: {
            checkKeyCode = "Intercept keycode:" + std::to_string(keyCode);
            break;
        }
        case KEY_LAUNCH_EVENT: {
            checkKeyCode = "Launch keycode:" + std::to_string(keyCode);
            break;
        }
        case KEY_SUBSCRIBE_EVENT: {
            checkKeyCode = "Subscribe keycode:" + std::to_string(keyCode);
            break;
        }
        case KEY_DISPATCH_EVENT: {
            checkKeyCode = "Dispatch keycode:" + std::to_string(keyCode);
            break;
        }
        default: {
            checkKeyCode = "Unknow keycode:" + std::to_string(keyCode);
            break;
        }
    }
    // HITRACE_METER_NAME(HITRACE_TAG_MULTIMODALINPUT, checkKeyCode);
    // int32_t keyId = key->GetId();
    // FinishAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, onKeyEvent, keyId);
}

void BytraceAdapter::StartBytrace(std::shared_ptr<KeyEvent> keyEvent, TraceBtn traceBtn, HandlerType handlerType)
{
    CHKPV(keyEvent);
    // int32_t keyId = keyEvent->GetId();
    // int32_t keyCode = keyEvent->GetKeyCode();
    // if (traceBtn == TRACE_START) {
    //     switch (handlerType) {
    //         case KEY_INTERCEPT_EVENT: {
    //             StartAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, keyEventIntercept, keyId);
    //             HITRACE_METER_NAME(HITRACE_TAG_MULTIMODALINPUT, "client Intercept keyCode:" + std::to_string(keyCode));
    //             break;
    //         }
    //         case KEY_SUBSCRIBE_EVENT: {
    //             StartAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, keyEventSubscribe, keyId);
    //             HITRACE_METER_NAME(HITRACE_TAG_MULTIMODALINPUT, "client subscribe keyCode:" + std::to_string(keyCode));
    //             break;
    //         }
    //         case KEY_DISPATCH_EVENT: {
    //             StartAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, keyEventDispatch, keyId);
    //             HITRACE_METER_NAME(HITRACE_TAG_MULTIMODALINPUT, "client dispatch keyCode:" + std::to_string(keyCode));
    //             break;
    //         }
    //         default: {
    //             HITRACE_METER_NAME(HITRACE_TAG_MULTIMODALINPUT, "Unknow keycode:" + std::to_string(keyCode));
    //             break;
    //         }
    //     }
    // } else {
    //     switch (handlerType) {
    //         case KEY_INTERCEPT_EVENT: {
    //             FinishAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, keyEventIntercept, keyId);
    //             break;
    //         }
    //         case KEY_SUBSCRIBE_EVENT: {
    //             FinishAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, keyEventSubscribe, keyId);
    //             break;
    //         }
    //         case KEY_DISPATCH_EVENT: {
    //             FinishAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, keyEventDispatch, keyId);
    //             break;
    //         }
    //         default: {
    //             HITRACE_METER_NAME(HITRACE_TAG_MULTIMODALINPUT, "Unknow keycode:" + std::to_string(keyCode));
    //             break;
    //         }
    //     }
    // }
}

void BytraceAdapter::StartBytrace(
    std::shared_ptr<PointerEvent> pointerEvent, TraceBtn traceBtn, HandlerType handlerType)
{
    CHKPV(pointerEvent);
    int32_t eventId = pointerEvent->GetId();
    // if (traceBtn == TRACE_START) {
    //     if (handlerType == POINT_DISPATCH_EVENT) {
    //         if (pointerEvent->GetSourceType() == PointerEvent::SOURCE_TYPE_MOUSE) {
    //             StartAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, pointerEventDispatch, eventId);
    //             HITRACE_METER_NAME(HITRACE_TAG_MULTIMODALINPUT, "client dispatch pointerId:" + std::to_string(eventId));
    //         } else {
    //             StartAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, touchEventDispatch, eventId);
    //             HITRACE_METER_NAME(HITRACE_TAG_MULTIMODALINPUT, "client dispatch touchId:" + std::to_string(eventId));
    //         }
    //     } else {
    //         if (pointerEvent->GetSourceType() == PointerEvent::SOURCE_TYPE_MOUSE) {
    //             StartAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, pointerEventIntercept, eventId);
    //             HITRACE_METER_NAME(HITRACE_TAG_MULTIMODALINPUT,
    //                 "client Intercept pointerId:" + std::to_string(eventId));
    //         } else {
    //             StartAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, touchEventIntercept, eventId);
    //             HITRACE_METER_NAME(HITRACE_TAG_MULTIMODALINPUT, "client Intercept touchId:" + std::to_string(eventId));
    //         }
    //     }
    // } else {
    //     if (handlerType == POINT_DISPATCH_EVENT) {
    //         if (pointerEvent->GetSourceType() == PointerEvent::SOURCE_TYPE_MOUSE) {
    //             FinishAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, pointerEventDispatch, eventId);
    //         } else {
    //             FinishAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, touchEventDispatch, eventId);
    //         }
    //     }
    //     if (handlerType == POINT_INTERCEPT_EVENT) {
    //         if (pointerEvent->GetSourceType() == PointerEvent::SOURCE_TYPE_MOUSE) {
    //             FinishAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, pointerEventIntercept, eventId);
    //         } else {
    //             FinishAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, touchEventIntercept, eventId);
    //         }
    //     }
    // }
}

void BytraceAdapter::StartBytrace(TraceBtn traceBtn, EventType eventType)
{
    std::string checkKeyCode;
    // if (traceBtn == TRACE_START) {
    //     switch (eventType) {
    //         case START_EVENT: {
    //             StartAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, startEvent, START_ID);
    //             checkKeyCode = "crossing startId:" + std::to_string(START_ID);
    //             HITRACE_METER_NAME(HITRACE_TAG_MULTIMODALINPUT, checkKeyCode);
    //             break;
    //         }
    //         case LAUNCH_EVENT: {
    //             StartAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, launchEvent, LAUNCH_ID);
    //             checkKeyCode = "crossing launchId:" + std::to_string(LAUNCH_ID);
    //             HITRACE_METER_NAME(HITRACE_TAG_MULTIMODALINPUT, checkKeyCode);
    //             break;
    //         }
    //         case STOP_EVENT: {
    //             StartAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, stopEvent, STOP_ID);
    //             checkKeyCode = "crossing stopId:" + std::to_string(STOP_ID);
    //             HITRACE_METER_NAME(HITRACE_TAG_MULTIMODALINPUT, checkKeyCode);
    //             break;
    //         }
    //     }
    // } else {
    //     switch (eventType) {
    //         case START_EVENT: {
    //             FinishAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, keyEventIntercept, START_ID);
    //             break;
    //         }
    //         case LAUNCH_EVENT: {
    //             FinishAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, keyEventIntercept, LAUNCH_ID);
    //             break;
    //         }
    //         case STOP_EVENT: {
    //             FinishAsyncTrace(HITRACE_TAG_MULTIMODALINPUT, keyEventIntercept, STOP_ID);
    //             break;
    //         }
    //     }
    // }
}
} // namespace MMI
} // namespace OHOS