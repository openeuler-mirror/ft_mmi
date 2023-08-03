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
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InputManagerAnrTest" };
constexpr int32_t TIME_WAIT_FOR_OP = 100;
constexpr int32_t NANOSECOND_TO_MILLISECOND = 1000000;
} // namespace

class InputManagerTest : public testing::Test {
public:
    void SetUp();
    void TearDown();
    static void SetUpTestCase();
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
 * @tc.name: InputManagerTest_SetAnrObserver
 * @tc.desc: Verify the observer for events
 * @tc.type: FUNC
 * @tc.require:
 */
TEST_F(InputManagerTest, InputManagerTest_SetAnrObserver)
{
    class IAnrObserverTest : public IAnrObserver {
    public:
        IAnrObserverTest() : IAnrObserver() {}
        virtual ~IAnrObserverTest() {}
        void OnAnr(int32_t pid) const override {
            MMI_HILOGD("pid=%{public}d is ANR!", pid);
            isAnr = true;
        };
        mutable bool isAnr = false;
    };

    class NoMarkEventProcess : public IInputEventConsumer {
        public:
            virtual void OnInputEvent(std::shared_ptr<KeyEvent> keyEvent) const override { };
            virtual void OnInputEvent(std::shared_ptr<PointerEvent> pointerEvent) const override {
                MMI_HILOGI("recv pointerEvent, but do not process it!");
            };
            virtual void OnInputEvent(std::shared_ptr<AxisEvent> axisEvent) const override { };
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
    auto consumer = GetPtr<NoMarkEventProcess>();
    ASSERT_TRUE(consumer != nullptr);
    MMI::InputManager::GetInstance()->SetWindowInputEventConsumer(consumer, eventHandler);

    std::shared_ptr<IAnrObserverTest> observer = std::make_shared<IAnrObserverTest>();
    MMI::InputManager::GetInstance()->SetAnrObserver(observer);

    for (int i = 0; i < 2; i++) {
        auto pointerEvent = SetupPointerEvent001();
        ASSERT_TRUE(pointerEvent != nullptr);
        InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
        sleep(5); // over 5s, MMIServer believes that anr has occurred
    }
    EXPECT_EQ(observer->isAnr, true);
}
} // namespace MMI
} // namespace OHOS

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
