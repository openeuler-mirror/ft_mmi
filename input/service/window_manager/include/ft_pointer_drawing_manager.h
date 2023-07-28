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

#ifndef POINTER_DRAWING_MANAGER_H
#define POINTER_DRAWING_MANAGER_H

#include <iostream>
#include <list>
#include "nocopyable.h"
#include "device_observer.h"
#include "i_pointer_drawing_manager.h"
#include "mouse_event_normalize.h"
#include "struct_multimodal.h"

namespace OHOS {
namespace MMI {
class PointerDrawingManager : public IPointerDrawingManager,
                              public IDeviceObserver,
                              public std::enable_shared_from_this<PointerDrawingManager> {
public:
    static const int32_t IMAGE_WIDTH = 64;
    static const int32_t IMAGE_HEIGHT = 64;

public:
    PointerDrawingManager();
    DISALLOW_COPY_AND_MOVE(PointerDrawingManager);
    ~PointerDrawingManager();
    void DrawPointer(int32_t displayId, int32_t physicalX, int32_t physicalY,
        const MOUSE_ICON mouseStyle = MOUSE_ICON::DEFAULT) override;
    void UpdateDisplayInfo(const DisplayInfo& displayInfo) override;
    void OnDisplayInfo(const DisplayGroupInfo& displayGroupInfo) override;
    void OnWindowInfo(const WinInfo &info) override;
    void UpdatePointerDevice(bool hasPointerDevicee, bool isPointerVisible) override;
    bool Init() override;
    void DeletePointerVisible(int32_t pid) override;
    int32_t SetPointerVisible(int32_t pid, bool visible) override;
    int32_t SetPointerStyle(int32_t pid, int32_t windowId, int32_t pointerStyle) override;
    int32_t GetPointerStyle(int32_t pid, int32_t windowId, int32_t &pointerStyle) override;
    void DrawPointerStyle() override;
    bool IsPointerVisible() override;
    void SetPointerLocation(int32_t pid, int32_t x, int32_t y) override;
    void SetMouseDisplayState(bool state) override;
    bool GetMouseDisplayState() const override;
    void AdjustMouseFocus(ICON_TYPE iconType, int32_t &physicalX, int32_t &physicalY);
    bool GetScreenSize(int32_t &width, int32_t &height) override;

private:
    void FixCursorPosition(int32_t &physicalX, int32_t &physicalY);
    void UpdatePointerVisible();
    std::shared_ptr<void> ptrDrawMgrHdl_ { nullptr };
    bool firstPointerDraw_ = { true };
    void OpenPointerDrawManagerHdl();

private:
    struct PidInfo {
        int32_t pid { 0 };
        bool visible { false };
    };
    bool hasDisplay_ { false };
    DisplayInfo displayInfo_ {};
    bool hasPointerDevice_ { false };
    int32_t lastPhysicalX_ { -1 };
    int32_t lastPhysicalY_ { -1 };
    int32_t lastMouseStyle_ { -1 };
    int32_t pid_ { 0 };
    int32_t windowId_ { 0 };
    int32_t imageWidth_ { 0 };
    int32_t imageHeight_ { 0 };
    std::list<PidInfo> pidInfos_;
    bool mouseDisplayState_ { false };
};
} // namespace MMI
} // namespace OHOS
#endif // POINTER_DRAWING_MANAGER_H