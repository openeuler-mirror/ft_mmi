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
#ifndef EVENT_LOG_HELPER_H
#define EVENT_LOG_HELPER_H

#include <memory>

#include "define_multimodal.h"
#include "input_event.h"
#include "key_event.h"
#include "mmi_log.h"
#include "pointer_event.h"

namespace OHOS {
namespace MMI {
class EventLogHelper {
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "EventLogHelper" };

public:
    template <class T>
    static void PrintEventData(std::shared_ptr<T> event, int32_t actionType, int32_t itemNum);
    template <class T>
    static void PrintEventData(std::shared_ptr<T> event);

private:
    static void Print(const std::shared_ptr<KeyEvent> event)
    {
        // if (!HiLogIsLoggable(OHOS::MMI::MMI_LOG_DOMAIN, LABEL.tag, LOG_DEBUG)
        //     && event->GetKeyCode() != KeyEvent::KEYCODE_POWER) {
        //     return;
        // }
        std::vector<KeyEvent::KeyItem> eventItems { event->GetKeyItems() };
        MMI_HILOGI("KeyCode:%{public}d,ActionTime:%{public}" PRId64 ",ActionStartTime:%{public}" PRId64
            ",EventType:%{public}s,Flag:%{public}d,KeyAction:%{public}s,NumLock:%{public}d,"
            "CapsLock:%{public}d,ScrollLock:%{public}d,EventNumber:%{public}d,keyItemsCount:%{public}zu",
            event->GetKeyCode(), event->GetActionTime(), event->GetActionStartTime(),
            InputEvent::EventTypeToString(event->GetEventType()), event->GetFlag(),
            KeyEvent::ActionToString(event->GetKeyAction()), event->GetFunctionKey(KeyEvent::NUM_LOCK_FUNCTION_KEY),
            event->GetFunctionKey(KeyEvent::CAPS_LOCK_FUNCTION_KEY),
            event->GetFunctionKey(KeyEvent::SCROLL_LOCK_FUNCTION_KEY),
            event->GetId(), eventItems.size());
        for (const auto &item : eventItems) {
            MMI_HILOGI("DeviceNumber:%{public}d,KeyCode:%{public}d,DownTime:%{public}" PRId64 ",IsPressed:%{public}d,"
                "GetUnicode:%{public}d", item.GetDeviceId(), item.GetKeyCode(), item.GetDownTime(), item.IsPressed(),
                item.GetUnicode());
        }
        std::vector<int32_t> pressedKeys = event->GetPressedKeys();
        std::vector<int32_t>::const_iterator cItr = pressedKeys.cbegin();
        if (cItr != pressedKeys.cend()) {
            std::string tmpStr = "Pressed keyCode: [" + std::to_string(*(cItr++));
            for (; cItr != pressedKeys.cend(); ++cItr) {
                tmpStr += ("," + std::to_string(*cItr));
            }
            MMI_HILOGI("%{public}s]", tmpStr.c_str());
        }
    }

    static void Print(const std::shared_ptr<PointerEvent> event)
    {
        std::vector<int32_t> pointerIds { event->GetPointerIds() };
        MMI_HILOGD("EventType:%{public}s,ActionTime:%{public}" PRId64 ",Action:%{public}d,"
            "ActionStartTime:%{public}" PRId64 ",Flag:%{public}d,PointerAction:%{public}s,"
            "SourceType:%{public}s,ButtonId:%{public}d,VerticalAxisValue:%{public}.2f,"
            "HorizontalAxisValue:%{public}.2f,PinchAxisValue:%{public}.2f,PointerId:%{public}d,"
            "PointerCount:%{public}zu,EventNumber:%{public}d",
            InputEvent::EventTypeToString(event->GetEventType()), event->GetActionTime(),
            event->GetAction(), event->GetActionStartTime(), event->GetFlag(),
            event->DumpPointerAction(), event->DumpSourceType(), event->GetButtonId(),
            event->GetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_VERTICAL),
            event->GetAxisValue(PointerEvent::AXIS_TYPE_SCROLL_HORIZONTAL),
            event->GetAxisValue(PointerEvent::AXIS_TYPE_PINCH),
            event->GetPointerId(), pointerIds.size(), event->GetId());

        for (const auto& pointerId : pointerIds) {
            PointerEvent::PointerItem item;
            if (!event->GetPointerItem(pointerId, item)) {
                MMI_HILOGE("Invalid pointer: %{public}d.", pointerId);
                return;
            }
            MMI_HILOGD("pointerId:%{public}d,DownTime:%{public}" PRId64 ",IsPressed:%{public}d,DisplayX:%{public}d,"
                "DisplayY:%{public}d,WindowX:%{public}d,WindowY:%{public}d,Width:%{public}d,Height:%{public}d,"
                "TiltX:%{public}.2f,TiltY:%{public}.2f,ToolDisplayX:%{public}d,ToolDisplayY:%{public}d,"
                "ToolWindowX:%{public}d,ToolWindowY:%{public}d,ToolWidth:%{public}d,ToolHeight:%{public}d,"
                "Pressure:%{public}.2f,ToolType:%{public}d,LongAxis:%{public}d,ShortAxis:%{public}d",
                pointerId, item.GetDownTime(), item.IsPressed(), item.GetDisplayX(),
                item.GetDisplayY(), item.GetWindowX(), item.GetWindowY(), item.GetWidth(), item.GetHeight(),
                item.GetTiltX(), item.GetTiltY(), item.GetToolDisplayX(), item.GetToolDisplayY(),
                item.GetToolWindowX(), item.GetToolWindowY(), item.GetToolWidth(), item.GetToolHeight(),
                item.GetPressure(), item.GetToolType(), item.GetLongAxis(), item.GetShortAxis());
        }
        std::vector<int32_t> pressedKeys = event->GetPressedKeys();
        std::vector<int32_t>::const_iterator cItr = pressedKeys.cbegin();
        if (cItr != pressedKeys.cend()) {
            std::string tmpStr = "Pressed keyCode: [" + std::to_string(*(cItr++));
            for (; cItr != pressedKeys.cend(); ++cItr) {
                tmpStr += ("," + std::to_string(*cItr));
            }
            MMI_HILOGD("%{public}s]", tmpStr.c_str());
        }
    }
};

template <class T>
void EventLogHelper::PrintEventData(std::shared_ptr<T> event, int32_t actionType, int32_t itemNum)
{
    CHKPV(event);
    // if (HiLogIsLoggable(OHOS::MMI::MMI_LOG_DOMAIN, LABEL.tag, LOG_DEBUG)) {
        static int64_t nowTimeUSec = 0;
        static int32_t dropped = 0;
        if (event->GetAction() == EVENT_TYPE_POINTER) {
            if ((actionType == POINTER_ACTION_MOVE) && (event->GetActionTime() - nowTimeUSec <= TIMEOUT)) {
                ++dropped;
                return;
            }
            if (actionType == POINTER_ACTION_UP && itemNum == FINAL_FINGER) {
                MMI_HILOGD("This touch process discards %{public}d high frequent events", dropped);
                dropped = 0;
            }
            nowTimeUSec = event->GetActionTime();
        }
        EventLogHelper::Print(event);
    // }
}

template <class T>
void EventLogHelper::PrintEventData(std::shared_ptr<T> event)
{
    CHKPV(event);
    // if (HiLogIsLoggable(OHOS::MMI::MMI_LOG_DOMAIN, LABEL.tag, LOG_DEBUG)
    //     || (event->GetAction() == InputEvent::EVENT_TYPE_KEY)) {
        EventLogHelper::Print(event);
    // }
}
} // namespace MMI
} // namespace OHOS
#endif // EVENT_LOG_HELPER_H