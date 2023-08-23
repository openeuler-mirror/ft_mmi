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
    std::shared_ptr<PointerEvent> SetupPointerEvent001();
};

void InputManagerTest::SetUpTestCase()
{
    ASSERT_TRUE(TestUtil->Init());
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

std::shared_ptr<PointerEvent> InputManagerTest::SetupPointerEvent001()
{
    auto pointerEvent = PointerEvent::Create();
    CHKPP(pointerEvent);
    int64_t downTime = GetNanoTime() / NANOSECOND_TO_MILLISECOND;
    pointerEvent->SetSourceType(PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent->SetPointerAction(PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    pointerEvent->SetButtonId(PointerEvent::MOUSE_BUTTON_LEFT);
    pointerEvent->SetPointerId(0);
    pointerEvent->SetButtonPressed(PointerEvent::MOUSE_BUTTON_LEFT);
    PointerEvent::PointerItem item;
    item.SetPointerId(0);
    item.SetDownTime(downTime);
    item.SetPressed(true);

    item.SetDisplayX(50);
    item.SetDisplayY(50);
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
 * @tc.name: InputManagerTest_SubscribeKeyEvent_01
 * @tc.desc: Verify subscribe KEYCODE_1 key event.
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(InputManagerTest, InputManagerTest_SubscribeKeyEvent_01)
{
    CALL_DEBUG_ENTER;
    ASSERT_TRUE(MMIEventHdl.InitClient());
    std::set<int32_t> preKeys;
    std::shared_ptr<KeyOption> keyOption1 = std::make_shared<KeyOption>();
    keyOption1->SetPreKeys(preKeys);
    keyOption1->SetFinalKey(KeyEvent::KEYCODE_1);
    keyOption1->SetFinalKeyDown(true);
    keyOption1->SetFinalKeyDownDuration(0);
    int32_t subscribeId1 = -1;
    subscribeId1 = InputManager::GetInstance()->SubscribeKeyEvent(keyOption1,
        [](std::shared_ptr<KeyEvent> keyEvent) {
        EventLogHelper::PrintEventData(keyEvent);
        MMI_HILOGD("Subscribe key event KEYCODE_1 down trigger callback");
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    InputManager::GetInstance()->UnsubscribeKeyEvent(subscribeId1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

/**
 * @tc.name: InputManagerTest_SimulateKeyEvent_001
 * @tc.desc: Verify simulate the back home is pressed
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(InputManagerTest, InputManagerTest_SimulateKeyEvent_001)
{
    CALL_DEBUG_ENTER;
    std::shared_ptr<KeyEvent> injectDownEvent = KeyEvent::Create();
    ASSERT_TRUE(injectDownEvent != nullptr);
    int64_t downTime = -1;
    KeyEvent::KeyItem kitDown;
    kitDown.SetKeyCode(KeyEvent::KEYCODE_HOME);
    kitDown.SetPressed(true);
    kitDown.SetDownTime(downTime);
    injectDownEvent->SetKeyCode(KeyEvent::KEYCODE_HOME);
    injectDownEvent->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    injectDownEvent->AddPressedKeyItems(kitDown);
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    TestSimulateInputEvent(injectDownEvent);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
}

/**
 * @tc.name: InputManagerTest_SimulateKeyEvent_002
 * @tc.desc: Verify simulate the back key is pressed and lifted
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(InputManagerTest, InputManagerTest_SimulateKeyEvent_002)
{
    CALL_DEBUG_ENTER;
    std::shared_ptr<KeyEvent> injectDownEvent = KeyEvent::Create();
    ASSERT_TRUE(injectDownEvent != nullptr);
    int64_t downTime = 0;
    KeyEvent::KeyItem kitDown;
    kitDown.SetKeyCode(KeyEvent::KEYCODE_BACK);
    kitDown.SetPressed(true);
    kitDown.SetDownTime(downTime);
    injectDownEvent->SetKeyCode(KeyEvent::KEYCODE_BACK);
    injectDownEvent->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    injectDownEvent->AddPressedKeyItems(kitDown);
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    TestSimulateInputEvent(injectDownEvent);
#endif // OHOS_BUILD_ENABLE_KEYBOARD

    std::shared_ptr<KeyEvent> injectUpEvent = KeyEvent::Create();
    ASSERT_TRUE(injectUpEvent != nullptr);
    KeyEvent::KeyItem kitUp;
    kitUp.SetKeyCode(KeyEvent::KEYCODE_BACK);
    kitUp.SetPressed(false);
    kitUp.SetDownTime(downTime);
    injectUpEvent->SetKeyCode(KeyEvent::KEYCODE_BACK);
    injectUpEvent->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    injectUpEvent->RemoveReleasedKeyItems(kitUp);
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    TestSimulateInputEvent(injectUpEvent);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
}

/**
 * @tc.name: InputManagerTest_SimulateKeyEvent_003
 * @tc.desc: Verify simulate key exception event
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(InputManagerTest, InputManagerTest_SimulateKeyEvent_003)
{
    CALL_DEBUG_ENTER;
    std::shared_ptr<KeyEvent> injectDownEvent = KeyEvent::Create();
    ASSERT_TRUE(injectDownEvent != nullptr);
    int64_t downTime = GetNanoTime() / NANOSECOND_TO_MILLISECOND;
    KeyEvent::KeyItem kitDown;
    kitDown.SetKeyCode(KeyEvent::KEYCODE_UNKNOWN);
    kitDown.SetPressed(true);
    kitDown.SetDownTime(downTime);
    injectDownEvent->SetKeyCode(KeyEvent::KEYCODE_UNKNOWN);
    injectDownEvent->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    injectDownEvent->AddPressedKeyItems(kitDown);
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    TestSimulateInputEvent(injectDownEvent, TestScene::EXCEPTION_TEST);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
}

/**
 * @tc.name: InputManagerTest_SimulateKeyEvent_004
 * @tc.desc: Verify simulate the fn key is long pressed and lifted
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(InputManagerTest, InputManagerTest_SimulateKeyEvent_004)
{
    CALL_DEBUG_ENTER;
    std::shared_ptr<KeyEvent> injectDownEvent = KeyEvent::Create();
    ASSERT_TRUE(injectDownEvent != nullptr);
    int64_t downTime = GetNanoTime() / NANOSECOND_TO_MILLISECOND;
    KeyEvent::KeyItem kitDown;
    kitDown.SetKeyCode(KeyEvent::KEYCODE_FN);
    kitDown.SetPressed(true);
    kitDown.SetDownTime(downTime);
    injectDownEvent->SetKeyCode(KeyEvent::KEYCODE_FN);
    injectDownEvent->SetKeyAction(KeyEvent::KEY_ACTION_DOWN);
    injectDownEvent->AddPressedKeyItems(kitDown);
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    TestSimulateInputEvent(injectDownEvent);
#endif // OHOS_BUILD_ENABLE_KEYBOARD

    std::shared_ptr<KeyEvent> injectUpEvent = KeyEvent::Create();
    ASSERT_TRUE(injectUpEvent != nullptr);
    downTime = GetNanoTime() / NANOSECOND_TO_MILLISECOND;
    KeyEvent::KeyItem kitUp;
    kitUp.SetKeyCode(KeyEvent::KEYCODE_FN);
    kitUp.SetPressed(false);
    kitUp.SetDownTime(downTime);
    injectUpEvent->SetKeyCode(KeyEvent::KEYCODE_FN);
    injectUpEvent->SetKeyAction(KeyEvent::KEY_ACTION_UP);
    injectUpEvent->RemoveReleasedKeyItems(kitUp);
#ifdef OHOS_BUILD_ENABLE_KEYBOARD
    TestSimulateInputEvent(injectUpEvent);
#endif // OHOS_BUILD_ENABLE_KEYBOARD
}

/**
 * @tc.name: InputManagerTest_ListenSimulateInputEvent
 * @tc.desc: Verify the Listener for simulate input event
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(InputManagerTest, ListenSimulateInputEvent)
{
    class WindowEventConsumerTmp : public IInputEventConsumer {
        public:
            virtual void OnInputEvent(std::shared_ptr<KeyEvent> keyEvent) const override { };
            virtual void OnInputEvent(std::shared_ptr<PointerEvent> pointerEvent) const override
            {
                PointerEvent::PointerItem pointerItem;
                int32_t pointId = pointerEvent->GetPointerId();
                if (!pointerEvent->GetPointerItem(pointId, pointerItem)) {
                    return;
                }
                pointerId = pointId;
                buttonId = pointerEvent->GetButtonId();
                x = pointerItem.GetDisplayX();
                y = pointerItem.GetDisplayY();
                sourceType = pointerEvent->GetSourceType();
                pointerAction = pointerEvent->GetPointerAction();
                pointerEvent->MarkProcessed();
            };
            virtual void OnInputEvent(std::shared_ptr<AxisEvent> axisEvent) const override { };
            mutable int32_t pointerId { -1 };
            mutable int32_t buttonId { -1 };
            mutable int32_t x { -1 };
            mutable int32_t y { -1 };
            mutable int32_t sourceType { -1 };
            mutable int32_t pointerAction { -1 };
    };

    auto runner = AppExecFwk::EventRunner::Create(true);
    ASSERT_TRUE(runner != nullptr);
    auto eventHandler = std::make_shared<AppExecFwk::EventHandler>(runner);
    ASSERT_TRUE(eventHandler != nullptr);
    uint64_t runnerThreadId = 0;

    auto fun = [&runnerThreadId]() {
        runnerThreadId = GetThisThreadId();
        MMI_HILOGD("Create eventHandler is threadId:%{public}" PRIu64, runnerThreadId);
        ASSERT_TRUE(runnerThreadId != 0);
    };
    eventHandler->PostSyncTask(fun, AppExecFwk::EventHandler::Priority::IMMEDIATE);
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    auto consumer = GetPtr<WindowEventConsumerTmp>();
    ASSERT_TRUE(consumer != nullptr);
    MMI::InputManager::GetInstance()->SetWindowInputEventConsumer(consumer, eventHandler);

    auto pointerEvent = SetupPointerEvent001();
    InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    EXPECT_EQ(consumer->pointerId, 0);
    EXPECT_EQ(consumer->x, 50);
    EXPECT_EQ(consumer->y, 50);
    EXPECT_EQ(consumer->sourceType, PointerEvent::SOURCE_TYPE_MOUSE);
    EXPECT_EQ(consumer->pointerAction, PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    EXPECT_EQ(consumer->buttonId, PointerEvent::MOUSE_BUTTON_LEFT);
}

/**
 * @tc.name: InputManagerTest_PrintKeyInfo
 * @tc.desc: Verify the Listener for key input event
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(InputManagerTest, PrintKeyInfo)
{
    class PrintKeyInfoConsumer : public IInputEventConsumer {
        public:
            virtual void OnInputEvent(std::shared_ptr<KeyEvent> keyEvent) const override {
                std::vector<KeyEvent::KeyItem> eventItems { keyEvent->GetKeyItems() };
                printf("KeyCode:%s,KeyAction:%s\n",
                    keyEvent->KeyCodeToString(keyEvent->GetKeyCode()),
                    keyEvent->ActionToString(keyEvent->GetKeyAction()));
                keyEvent->MarkProcessed();
             };
            virtual void OnInputEvent(std::shared_ptr<PointerEvent> pointerEvent) const override {
                pointerEvent->MarkProcessed();
            };
            virtual void OnInputEvent(std::shared_ptr<AxisEvent> axisEvent) const override {
                axisEvent->MarkProcessed();
            };
    };

    auto runner = AppExecFwk::EventRunner::Create(true);
    ASSERT_TRUE(runner != nullptr);
    auto eventHandler = std::make_shared<AppExecFwk::EventHandler>(runner);
    ASSERT_TRUE(eventHandler != nullptr);
    uint64_t runnerThreadId = 0;

    auto fun = [&runnerThreadId]() {
        runnerThreadId = GetThisThreadId();
        MMI_HILOGD("Create eventHandler is threadId:%{public}" PRIu64, runnerThreadId);
        ASSERT_TRUE(runnerThreadId != 0);
    };
    eventHandler->PostSyncTask(fun, AppExecFwk::EventHandler::Priority::IMMEDIATE);
    std::this_thread::sleep_for(std::chrono::milliseconds(TIME_WAIT_FOR_OP));
    auto consumer = GetPtr<PrintKeyInfoConsumer>();
    ASSERT_TRUE(consumer != nullptr);
    MMI::InputManager::GetInstance()->SetWindowInputEventConsumer(consumer, eventHandler);

    printf("请在30秒内按键盘任意键进行测试:\n");
    sleep(30);
    printf("测试结束\n");
}

/**
 * @tc.name: InputManagerTest_SetGetPointerStyle_001
 * @tc.desc: Sets the pointer style of the window
 * @tc.type: FUNC
 * @tc.require: I530XS
 */
TEST_F(InputManagerTest, InputManagerTest_SetGetPointerStyle_001)
{
    CALL_DEBUG_ENTER;
    auto window = WindowUtilsTest::GetInstance()->GetWindow();
    uint32_t windowId = window->GetWindowId();
    int32_t setPointerStyle;
    int32_t getPointerStyle;

    for (int32_t i = 0; i < (int32_t)(MOUSE_ICON::MIDDLE_BTN_NORTH_SOUTH_WEST_EAST); i++) {
        setPointerStyle = (MOUSE_ICON)i;
        EXPECT_EQ(RET_OK, InputManager::GetInstance()->SetPointerStyle(windowId, setPointerStyle));
        EXPECT_EQ(RET_OK, InputManager::GetInstance()->GetPointerStyle(windowId, getPointerStyle));
        EXPECT_EQ(setPointerStyle, getPointerStyle);
    }
    EXPECT_EQ(RET_OK, InputManager::GetInstance()->SetPointerStyle(windowId, MOUSE_ICON::DEFAULT));
}
} // namespace MMI
} // namespace OHOS

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
