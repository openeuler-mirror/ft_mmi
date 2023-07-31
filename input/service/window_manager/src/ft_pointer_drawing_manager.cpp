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

#include <dlfcn.h>
#include "ft_pointer_drawing_manager.h"
#include "define_multimodal.h"
#include "input_device_manager.h"
#include "mmi_log.h"
#include "util.h"

namespace OHOS {
namespace MMI {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "ftPointerDrawingManager" };
constexpr int32_t PAD_SCREEN_WIDTH = 2560;
constexpr int32_t PHONE_SCREEN_WIDTH = 2160;
constexpr int32_t SMALL_ICON_WIDTH = 40;
constexpr int32_t SMALL_ICON_HEIGHT = 40;
constexpr int32_t MIDDLE_ICON_WIDTH = 60;
constexpr int32_t MIDDLE_ICON_HEIGHT = 60;
constexpr int32_t LARGE_ICON_WIDTH = 80;
constexpr int32_t LARGE_ICON_HEIGHT = 80;
} // namespace

using FUN_PTR_DRAW_GET_INSTANCE = uintptr_t (*)();
using FUN_PTR_DRAW_INIT = bool (*)(uintptr_t);
using FUN_PTR_DRAW_UPDATE_DISPLAY_INFO = bool (*)(const uintptr_t, int32_t, int32_t, int32_t);
using FUN_PTR_DRAW_DRAWING = bool (*)(const uintptr_t, int32_t, int32_t, int32_t, int32_t);
using FUN_PTR_DRAW_FREE_DRAWING = void (*)(const uintptr_t);
using FUN_PTR_DRAW_GET_SCREEN_SIZE = bool (*)(const uintptr_t, int32_t *, int32_t *);
using FUN_PTR_DRAW_SET_PTR_VISIBLE = bool (*)(const uintptr_t, int32_t , bool);

constexpr const char *LIB_POINTER_DRAW = "/usr/lib64/libpointerdraw.so";
constexpr int32_t DEFAULT_DISPLAY_WIDTH = 1000;
constexpr int32_t DEFAULT_DISPLAY_HEIGHT = 1000;
constexpr int32_t DEFAULT_DISPLAY_ID = 0;

typedef struct
{
    void *dlHandle;
    uintptr_t ptrDrawMgrInstance;
    FUN_PTR_DRAW_GET_INSTANCE ftPtrDrawMgrGetInstance;
    FUN_PTR_DRAW_INIT ftPtrDrawMgrInit;
    FUN_PTR_DRAW_UPDATE_DISPLAY_INFO ftPtrDrawMgrUpdataDispInfo;
    FUN_PTR_DRAW_DRAWING ftPtrDrawMgrDrawPointer;
    FUN_PTR_DRAW_FREE_DRAWING ftPtrDrawMgrFreeInstance;
    FUN_PTR_DRAW_GET_SCREEN_SIZE ftPtrDrawMgrGetScreenSize;
    FUN_PTR_DRAW_SET_PTR_VISIBLE FTPtrDrawMgrSetPointerVisible;
} PtrDrawMgrHdl;
} // namespace MMI
} // namespace OHOS

namespace OHOS {
namespace MMI {
PointerDrawingManager::PointerDrawingManager()
{
    InitStyle();
    OpenPointerDrawManagerHdl();
}

PointerDrawingManager::~PointerDrawingManager()
{
    if (ptrDrawMgrHdl_ != nullptr) {
        std::shared_ptr<PtrDrawMgrHdl> hdl = std::static_pointer_cast<PtrDrawMgrHdl>(ptrDrawMgrHdl_);
        if (hdl->ftPtrDrawMgrFreeInstance != nullptr && hdl->ptrDrawMgrInstance != 0) {
            hdl->ftPtrDrawMgrFreeInstance(hdl->ptrDrawMgrInstance);
            hdl->ptrDrawMgrInstance = 0;
        }
        if (hdl->dlHandle != nullptr) {
            dlclose(hdl->dlHandle);
            hdl->dlHandle = nullptr;
        }
    }
}

void PointerDrawingManager::DrawPointer(int32_t displayId, int32_t physicalX, int32_t physicalY,
    const MOUSE_ICON mouseStyle)
{
    CALL_DEBUG_ENTER;
    MMI_HILOGD("Display:%{public}d,physicalX:%{public}d,physicalY:%{public}d,mouseStyle:%{public}d",
        displayId, physicalX, physicalY, mouseStyle);
    FixCursorPosition(physicalX, physicalY);
    lastPhysicalX_ = physicalX;
    lastPhysicalY_ = physicalY;

    auto it = mouseIcons_.find(MOUSE_ICON(mouseStyle));
    if (it == mouseIcons_.end()) {
        MMI_HILOGE("unsupport mouse style=%{public}d", mouseStyle);
        return;
    }

    AdjustMouseFocus(ICON_TYPE(mouseIcons_[mouseStyle]), physicalX, physicalY);
    if (ptrDrawMgrHdl_ == nullptr) {
        MMI_HILOGE("ptrDrawMgrHdl_ null! can not drawPointer");
        return;
    }

    std::shared_ptr<PtrDrawMgrHdl> hdl = std::static_pointer_cast<PtrDrawMgrHdl>(ptrDrawMgrHdl_);
    if (hdl->ftPtrDrawMgrUpdataDispInfo == nullptr || hdl->ftPtrDrawMgrInit == nullptr ||
        hdl->ftPtrDrawMgrDrawPointer == nullptr || hdl->ptrDrawMgrInstance == 0) {
        MMI_HILOGE("error, can not draw Pointer");
        return;
    }

    if (firstPointerDraw_) {
        MMI_HILOGD("first Pointer Draw");
        hdl->ftPtrDrawMgrUpdataDispInfo(hdl->ptrDrawMgrInstance, DEFAULT_DISPLAY_ID, DEFAULT_DISPLAY_WIDTH, DEFAULT_DISPLAY_HEIGHT);
        hdl->ftPtrDrawMgrInit(hdl->ptrDrawMgrInstance);
        firstPointerDraw_ = false;
    }

    lastMouseStyle_ = mouseStyle;
    hdl->ftPtrDrawMgrDrawPointer(hdl->ptrDrawMgrInstance, DEFAULT_DISPLAY_ID, physicalX, physicalY, (int32_t)mouseStyle);
    MMI_HILOGD("Leave, display:%{public}d,physicalX:%{public}d,physicalY:%{public}d",
        displayId, physicalX, physicalY);
}

void PointerDrawingManager::OpenPointerDrawManagerHdl()
{
    if (ptrDrawMgrHdl_ != nullptr) {
        MMI_HILOGW("has get pointer draw manager handle");
        return;
    }

    std::shared_ptr<PtrDrawMgrHdl> hdl = std::make_shared<PtrDrawMgrHdl>();
    if (hdl == nullptr) {
        MMI_HILOGE("create PtrDrawMgrHdl fail");
        return;
    }

    hdl->dlHandle = dlopen(LIB_POINTER_DRAW, RTLD_NOW | RTLD_LOCAL);
    if (hdl->dlHandle == nullptr) {
        MMI_HILOGE("dlopen fail");
        return;
    }

    // load func symbol
    hdl->ftPtrDrawMgrGetInstance = (FUN_PTR_DRAW_GET_INSTANCE)dlsym(hdl->dlHandle, "FTPtrDrawMgrGetInstance");
    if (hdl->ftPtrDrawMgrGetInstance == nullptr) {
        MMI_HILOGE("load symbol fail, %{public}s", dlerror());
        return;
    }
    hdl->ftPtrDrawMgrInit = (FUN_PTR_DRAW_INIT)dlsym(hdl->dlHandle, "FTPtrDrawMgrInit");
    if (hdl->ftPtrDrawMgrInit == nullptr) {
        MMI_HILOGE("load symbol fail, %{public}s", dlerror());
        return;
    }
    hdl->ftPtrDrawMgrUpdataDispInfo = (FUN_PTR_DRAW_UPDATE_DISPLAY_INFO)dlsym(hdl->dlHandle, "FTPtrDrawMgrUpdataDispInfo");
    if (hdl->ftPtrDrawMgrUpdataDispInfo == nullptr) {
        MMI_HILOGE("load symbol fail, %{public}s", dlerror());
        return;
    }
    hdl->ftPtrDrawMgrDrawPointer = (FUN_PTR_DRAW_DRAWING)dlsym(hdl->dlHandle, "FTPtrDrawMgrDrawPointer");
    if (hdl->ftPtrDrawMgrDrawPointer == nullptr) {
        MMI_HILOGE("load symbol fail, %{public}s", dlerror());
        return;
    }
    hdl->ftPtrDrawMgrFreeInstance = (FUN_PTR_DRAW_FREE_DRAWING)dlsym(hdl->dlHandle, "FTPtrDrawMgrFreeInstance");
    if (hdl->ftPtrDrawMgrFreeInstance == nullptr) {
        MMI_HILOGE("load symbol fail, %{public}s", dlerror());
        return;
    }
    hdl->ftPtrDrawMgrGetScreenSize = (FUN_PTR_DRAW_GET_SCREEN_SIZE)dlsym(hdl->dlHandle, "FTPtrDrawMgrGetScreenSize");
    if (hdl->ftPtrDrawMgrGetScreenSize == nullptr) {
        MMI_HILOGE("load symbol fail, %{public}s", dlerror());
        return;
    }
    hdl->FTPtrDrawMgrSetPointerVisible = (FUN_PTR_DRAW_SET_PTR_VISIBLE)dlsym(hdl->dlHandle, "FTPtrDrawMgrSetPointerVisible");
    if (hdl->FTPtrDrawMgrSetPointerVisible == nullptr) {
        MMI_HILOGE("load symbol fail, %{public}s", dlerror());
        return;
    }

    // get instance
    hdl->ptrDrawMgrInstance = hdl->ftPtrDrawMgrGetInstance();
    if (hdl->ptrDrawMgrInstance == 0) {
        MMI_HILOGE("get pointer draw manager instance fail");
        return;
    }

    ptrDrawMgrHdl_ = hdl;
}

void PointerDrawingManager::AdjustMouseFocus(ICON_TYPE iconType, int32_t &physicalX, int32_t &physicalY)
{
    CALL_DEBUG_ENTER;
    switch (iconType) {
        case ANGLE_SW: {
            physicalY -= imageHeight_;
            break;
        }
        case ANGLE_CENTER: {
            physicalX -= imageWidth_ / 2;
            physicalY -= imageHeight_ / 2;
            break;
        }
        case ANGLE_NW:
        default: {
            MMI_HILOGD("No need adjust mouse focus");
            break;
        }
    }
}

void PointerDrawingManager::FixCursorPosition(int32_t &physicalX, int32_t &physicalY)
{
    if (physicalX < 0) {
        physicalX = 0;
    }

    if (physicalY < 0) {
        physicalY = 0;
    }

    if (displayInfo_.width == 0 && imageWidth_ == 0 && displayInfo_.height == 0 && imageHeight_ == 0) {
        return; // invaild display info, just return
    }

    const int32_t cursorUnit = 16;
    if (displayInfo_.direction == Direction0 || displayInfo_.direction == Direction180) {
        if (physicalX > (displayInfo_.width - imageWidth_ / cursorUnit)) {
            physicalX = displayInfo_.width - imageWidth_ / cursorUnit;
        }
        if (physicalY > (displayInfo_.height - imageHeight_ / cursorUnit)) {
            physicalY = displayInfo_.height - imageHeight_ / cursorUnit;
        }
    } else {
        if (physicalX > (displayInfo_.height - imageHeight_ / cursorUnit)) {
            physicalX = displayInfo_.height - imageHeight_ / cursorUnit;
        }
        if (physicalY > (displayInfo_.width - imageWidth_ / cursorUnit)) {
            physicalY = displayInfo_.width - imageWidth_ / cursorUnit;
        }
    }
}

void PointerDrawingManager::SetMouseDisplayState(bool state)
{
    CALL_DEBUG_ENTER;
    if (mouseDisplayState_ != state) {
        mouseDisplayState_ = state;
        UpdatePointerVisible();
    }
}

bool PointerDrawingManager::GetMouseDisplayState() const
{
    return mouseDisplayState_;
}

void PointerDrawingManager::UpdateDisplayInfo(const DisplayInfo& displayInfo)
{
    CALL_DEBUG_ENTER;
    hasDisplay_ = true;
    displayInfo_ = displayInfo;

    if ((displayInfo_.width >= PHONE_SCREEN_WIDTH) || (displayInfo_.height >= PHONE_SCREEN_WIDTH)) {
        if ((displayInfo_.width == PAD_SCREEN_WIDTH) || (displayInfo_.height == PAD_SCREEN_WIDTH)) {
            imageWidth_ = MIDDLE_ICON_WIDTH;
            imageHeight_ = MIDDLE_ICON_HEIGHT;
        } else {
            imageWidth_ = LARGE_ICON_WIDTH;
            imageHeight_ = LARGE_ICON_HEIGHT;
        }
    } else {
        imageWidth_ = SMALL_ICON_WIDTH;
        imageHeight_ = SMALL_ICON_HEIGHT;
    }
}

void PointerDrawingManager::OnDisplayInfo(const DisplayGroupInfo& displayGroupInfo)
{
    CALL_DEBUG_ENTER;
    for (const auto& item : displayGroupInfo.displaysInfo) {
        if (item.id == displayInfo_.id) {
            UpdateDisplayInfo(item);
            return;
        }
    }
    UpdateDisplayInfo(displayGroupInfo.displaysInfo[0]);
    lastPhysicalX_ = displayGroupInfo.displaysInfo[0].width / 2;
    lastPhysicalY_ = displayGroupInfo.displaysInfo[0].height / 2;
    MouseEventHdr->OnDisplayLost(displayInfo_.id);
    MMI_HILOGD("displayId_:%{public}d, displayWidth_:%{public}d, displayHeight_:%{public}d",
        displayInfo_.id, displayInfo_.width, displayInfo_.height);
}

void PointerDrawingManager::OnWindowInfo(const WinInfo &info)
{
    CALL_DEBUG_ENTER;
    windowId_ = info.windowId;
    pid_ = info.windowPid;
}

void PointerDrawingManager::UpdatePointerDevice(bool hasPointerDevice, bool isPointerVisible)
{
    CALL_DEBUG_ENTER;
    hasPointerDevice_ = hasPointerDevice;
    if (hasPointerDevice_) {
        SetPointerVisible(getpid(), isPointerVisible);
    } else {
        DeletePointerVisible(getpid());
    }
}

bool PointerDrawingManager::Init()
{
    CALL_DEBUG_ENTER;
    InputDevMgr->Attach(shared_from_this());
    pidInfos_.clear();
    return true;
}

std::shared_ptr<IPointerDrawingManager> IPointerDrawingManager::GetInstance()
{
    if (iPointDrawMgr_ == nullptr) {
        iPointDrawMgr_ = std::make_shared<PointerDrawingManager>();
    }
    return iPointDrawMgr_;
}

void PointerDrawingManager::UpdatePointerVisible()
{
    CALL_DEBUG_ENTER;
    if (ptrDrawMgrHdl_ == nullptr) {
        MMI_HILOGE("ptrDrawMgrHdl_ null! can not get screen size");
        return;
    }

    std::shared_ptr<PtrDrawMgrHdl> hdl = std::static_pointer_cast<PtrDrawMgrHdl>(ptrDrawMgrHdl_);
    if (hdl->FTPtrDrawMgrSetPointerVisible == nullptr) {
        MMI_HILOGE("error!");
        return;
    }

    MMI_HILOGE("FTPtrDrawMgrSetPointerVisible=%{public}d", IsPointerVisible());
    hdl->FTPtrDrawMgrSetPointerVisible(hdl->ptrDrawMgrInstance, 0, IsPointerVisible());
    if (IsPointerVisible()) {
        DrawPointer(DEFAULT_DISPLAY_ID, lastPhysicalX_, lastPhysicalY_, MOUSE_ICON(lastMouseStyle_));
    }
}

bool PointerDrawingManager::IsPointerVisible()
{
    CALL_DEBUG_ENTER;
    if (pidInfos_.empty()) {
        MMI_HILOGD("Visible property is true");
        return true;
    }
    auto info = pidInfos_.back();
    MMI_HILOGD("Visible property:%{public}zu.%{public}d-%{public}d", pidInfos_.size(), info.pid, info.visible);
    return info.visible;
}

void PointerDrawingManager::DeletePointerVisible(int32_t pid)
{
    CALL_DEBUG_ENTER;
    auto it = pidInfos_.begin();
    for (; it != pidInfos_.end(); ++it) {
        if (it->pid == pid) {
            pidInfos_.erase(it);
            break;
        }
    }
    if (it != pidInfos_.end()) {
        UpdatePointerVisible();
    }
}

int32_t PointerDrawingManager::SetPointerVisible(int32_t pid, bool visible)
{
    CALL_DEBUG_ENTER;
    for (auto it = pidInfos_.begin(); it != pidInfos_.end(); ++it) {
        if (it->pid == pid) {
            pidInfos_.erase(it);
            break;
        }
    }
    PidInfo info = { .pid = pid, .visible = visible };
    pidInfos_.push_back(info);
    UpdatePointerVisible();
    return RET_OK;
}

void PointerDrawingManager::SetPointerLocation(int32_t pid, int32_t x, int32_t y)
{
    CALL_DEBUG_ENTER;
    FixCursorPosition(x, y);
    lastPhysicalX_ = x;
    lastPhysicalY_ = y;
}

int32_t PointerDrawingManager::SetPointerStyle(int32_t pid, int32_t windowId, int32_t pointerStyle)
{
    CALL_DEBUG_ENTER;
    if (pointerStyle < 0 || MOUSE_ICON(pointerStyle) > MIDDLE_BTN_NORTH_SOUTH_WEST_EAST) {
        MMI_HILOGE("The param pointerStyle is invalid");
        return RET_ERR;
    }

    int32_t ret = WinMgr->SetPointerStyle(pid, windowId, pointerStyle);
    if (ret != RET_OK) {
        MMI_HILOGE("Set pointer style failed");
        return ret;
    }

    if (!InputDevMgr->HasPointerDevice()) {
        MMI_HILOGD("The pointer device is not exist");
        return RET_OK;
    }

    if (!WinMgr->IsNeedRefreshLayer(windowId)) {
        MMI_HILOGD("Not need refresh layer, window type:%{public}d, pointer style:%{public}d", windowId, pointerStyle);
        return RET_OK;
    }

    DrawPointer(DEFAULT_DISPLAY_ID, lastPhysicalX_, lastPhysicalY_, MOUSE_ICON(pointerStyle));
    UpdatePointerVisible();
    MMI_HILOGD("Window id:%{public}d set pointer style:%{public}d success", windowId, pointerStyle);
    return RET_OK;
}

int32_t PointerDrawingManager::GetPointerStyle(int32_t pid, int32_t windowId, int32_t &pointerStyle)
{
    CALL_DEBUG_ENTER;
    int32_t ret = WinMgr->GetPointerStyle(pid, windowId, pointerStyle);
    if (ret != RET_OK) {
        MMI_HILOGE("Get pointer style failed, pointerStyleInfo is nullptr");
        return ret;
    }
    MMI_HILOGD("Window id:%{public}d get pointer style:%{public}d success", windowId, pointerStyle);
    return RET_OK;
}

void PointerDrawingManager::DrawPointerStyle()
{
    CALL_DEBUG_ENTER;
    if (hasDisplay_ && hasPointerDevice_) {
        int32_t mouseStyle = -1;
        int32_t ret = WinMgr->GetPointerStyle(pid_, windowId_, mouseStyle);
        if (ret != RET_OK) {
            MMI_HILOGE("Draw pointer style failed, pointerStyleInfo is nullptr");
            return;
        }
        if (lastPhysicalX_ == -1 || lastPhysicalY_ == -1) {
            DrawPointer(displayInfo_.id, displayInfo_.width / 2, displayInfo_.height / 2, MOUSE_ICON(mouseStyle));
            MMI_HILOGD("Draw pointer style, mouseStyle:%{public}d", mouseStyle);
            return;
        }

        DrawPointer(displayInfo_.id, lastPhysicalX_, lastPhysicalY_, MOUSE_ICON(mouseStyle));
        MMI_HILOGD("Draw pointer style, mouseStyle:%{public}d", mouseStyle);
    }
}

bool PointerDrawingManager::GetScreenSize(int32_t &width, int32_t &height)
{
    if (ptrDrawMgrHdl_ == nullptr) {
        MMI_HILOGE("ptrDrawMgrHdl_ null! can not get screen size");
        return false;
    }

    std::shared_ptr<PtrDrawMgrHdl> hdl = std::static_pointer_cast<PtrDrawMgrHdl>(ptrDrawMgrHdl_);
    if (hdl->ftPtrDrawMgrGetScreenSize == nullptr) {
        MMI_HILOGE("error, can not get screen size");
        return false;
    }

    int32_t w = -1;
    int32_t h = -1;
    if (!hdl->ftPtrDrawMgrGetScreenSize(hdl->ptrDrawMgrInstance, &w, &h)) {
        MMI_HILOGE("error, get screen size fail");
        return false;
    }
    width = w;
    height = h;
    return true;
}

void PointerDrawingManager::InitStyle()
{
    CALL_DEBUG_ENTER;
    mouseIcons_ = {
        {DEFAULT, ANGLE_NW},
        {EAST, ANGLE_CENTER},
        {WEST, ANGLE_CENTER},
        {SOUTH, ANGLE_CENTER},
        {NORTH, ANGLE_CENTER},
        {WEST_EAST, ANGLE_CENTER},
        {NORTH_SOUTH, ANGLE_CENTER},
        {NORTH_EAST, ANGLE_CENTER},
        {NORTH_WEST, ANGLE_CENTER},
        {SOUTH_EAST, ANGLE_CENTER},
        {SOUTH_WEST, ANGLE_CENTER},
        {NORTH_EAST_SOUTH_WEST, ANGLE_CENTER},
        {NORTH_WEST_SOUTH_EAST, ANGLE_CENTER},
        {CROSS, ANGLE_CENTER},
        {CURSOR_COPY, ANGLE_NW},
        {CURSOR_FORBID, ANGLE_NW},
        {COLOR_SUCKER, ANGLE_SW},
        {HAND_GRABBING, ANGLE_CENTER},
        {HAND_OPEN, ANGLE_CENTER},
        {HAND_POINTING, ANGLE_NW},
        {HELP, ANGLE_NW},
        {CURSOR_MOVE, ANGLE_CENTER},
        {RESIZE_LEFT_RIGHT, ANGLE_CENTER},
        {RESIZE_UP_DOWN, ANGLE_CENTER},
        {SCREENSHOT_CHOOSE, ANGLE_CENTER},
        {SCREENSHOT_CURSOR, ANGLE_CENTER},
        {TEXT_CURSOR, ANGLE_CENTER},
        {ZOOM_IN, ANGLE_CENTER},
        {ZOOM_OUT, ANGLE_CENTER},
        {MIDDLE_BTN_EAST, ANGLE_CENTER},
        {MIDDLE_BTN_WEST, ANGLE_CENTER},
        {MIDDLE_BTN_SOUTH, ANGLE_CENTER},
        {MIDDLE_BTN_NORTH, ANGLE_CENTER},
        {MIDDLE_BTN_NORTH_SOUTH, ANGLE_CENTER},
        {MIDDLE_BTN_NORTH_EAST, ANGLE_CENTER},
        {MIDDLE_BTN_NORTH_WEST, ANGLE_CENTER},
        {MIDDLE_BTN_SOUTH_EAST, ANGLE_CENTER},
        {MIDDLE_BTN_SOUTH_WEST, ANGLE_CENTER},
        {MIDDLE_BTN_NORTH_SOUTH_WEST_EAST, ANGLE_CENTER},
    };
}
} // namespace MMI
} // namespace OHOS
