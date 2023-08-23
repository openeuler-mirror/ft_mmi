/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd.
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

#include "event_log_helper.h"
#include "event_util_test.h"
#include "input_handler_type.h"
#include "mmi_log.h"
#include "multimodal_event_handler.h"
#include "system_info.h"
#include "util.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputManagerTest" };
constexpr int32_t TIME_WAIT_FOR_OP = 100;
constexpr int32_t NANOSECOND_TO_MILLISECOND = 1000000;
} // namespace

class InputManagerTest : public testing::Test {
public:
    void SetUp();
    void TearDown();
    static void SetUpTestCase();
    std::string GetEventDump();
    std::shared_ptr<PointerEvent> SetupPointerEvent001(int x, int y, int action);
};

void InputManagerTest::SetUpTestCase()
{
}

void InputManagerTest::SetUp()
{
    TestUtil->SetRecvFlag(RECV_FLAG::RECV_FOCUS);
}

void InputManagerTest::TearDown()
{
    TestUtil->AddEventDump("");
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
}

std::string InputManagerTest::GetEventDump()
{
    return TestUtil->GetEventDump();
}

std::shared_ptr<PointerEvent> InputManagerTest::SetupPointerEvent001(int x, int y, int action)
{
    auto pointerEvent = PointerEvent::Create();
    int64_t downTime = 0;
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent->SetPointerAction(action);
    pointerEvent->SetButtonId(PointerEvent::MOUSE_BUTTON_LEFT);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetButtonPressed(PointerEvent::MOUSE_BUTTON_LEFT);
    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    item.SetDownTime(downTime);
    item.SetPressed(true);
    item.SetDisplayX(x);
    item.SetDisplayY(y);
    item.SetWindowX(70);
    item.SetWindowY(70);
    item.SetWidth(0);
    item.SetHeight(0);
    item.SetPressure(0);
    item.SetDeviceId(0);
    pointerEvent->AddPointerItem(item);
    return pointerEvent;
}

/**
 * @用例名称: InputManagerTest_WindowStartMove
 * @用例描述: 使用模拟鼠标事件拖动窗口
 * @用例类型: 接口测试
 * @用例要求: 测试过程中不要移动物理鼠标，否则会干扰模拟鼠标事件
 */
TEST_F(InputManagerTest, WindowStartMove)
{
    CALL_DEBUG_ENTER;
    int winX = 0;
    int winY = 0;
    OHOS::sptr<OHOS::Rosen::WindowOption> option(new OHOS::Rosen::WindowOption());
    option->SetDisplayId(0); // default displayId is 0
    option->SetWindowRect({winX, winY, 600, 600}); // test window size is 600*600
    option->SetWindowType(OHOS::Rosen::WindowType::APP_MAIN_WINDOW_BASE);
    option->SetWindowMode(OHOS::Rosen::WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowName("mmi-test-window");
    option->SetMainHandlerAvailable(false);
    auto window = OHOS::Rosen::Window::Create(option->GetWindowName(), option);
    window->Show();

    class ClockDemoEventConsumerWindow : public OHOS::Rosen::IInputEventConsumer
    {
        public:
            ClockDemoEventConsumerWindow(OHOS::sptr<OHOS::Rosen::Window> window)
            {
                window_ = window;
            }
            bool OnInputEvent(const std::shared_ptr<OHOS::MMI::KeyEvent>& keyEvent) const override { return true; };
            bool OnInputEvent(const std::shared_ptr<OHOS::MMI::AxisEvent>& axisEvent) const override { return true; };
            bool OnInputEvent(const std::shared_ptr<OHOS::MMI::PointerEvent>& pointerEvent) const override
            {
                window_->StartMove();
                pointerEvent->MarkProcessed();
                OHOS::Rosen::Rect r = window_->GetRect();
                OHOS::MMI::PointerEvent::PointerItem pointerItem;
                int32_t pointId = pointerEvent->GetPointerId();
                if (!pointerEvent->GetPointerItem(pointId, pointerItem)) {
                    return false;
                }
                return true;
            }
        private:
            OHOS::sptr<OHOS::Rosen::Window> window_ = nullptr;
    };

    auto listener = std::make_shared<ClockDemoEventConsumerWindow>(window);
    window->SetInputEventConsumer(listener);

    int x = 20; // X轴初始坐标
    int y = 20; // Y轴初始坐标
    int dist = 0;
    int step = 5;
    InputManager::GetInstance()->SimulateInputEvent(SetupPointerEvent001(x, y, PointerEvent::POINTER_ACTION_BUTTON_DOWN));
    sleep(1);
    for (int i = 0; i < 3; i++) {
        x += step;
        y += step;
        dist += step;
        InputManager::GetInstance()->SimulateInputEvent(SetupPointerEvent001(x, y, PointerEvent::POINTER_ACTION_MOVE));
        sleep(1);
    }

    EXPECT_EQ(window->GetRect().posX_ - winX, dist);
    EXPECT_EQ(window->GetRect().posY_ - winY, dist);
}
} // namespace MMI
} // namespace OHOS

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
