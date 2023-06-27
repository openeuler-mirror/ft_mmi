/*
 * Copyright (c) 2021 Huawei Technologies Co., Ltd.
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

#ifndef CANVAS_H
#define CANVAS_H

#include <iostream>
#include <string>
#include "surface.h"
#include <securec.h>
#include "pixel_map.h"

namespace OHOS {
typedef enum {
    PIXEL_FMT_CLUT8 = 0,                 /**< CLUT8 format */
    PIXEL_FMT_CLUT1,                     /**< CLUT1 format */
    PIXEL_FMT_CLUT4,                     /**< CLUT4 format */
    PIXEL_FMT_RGB_565,                   /**< RGB565 format */
    PIXEL_FMT_RGBA_5658,                 /**< RGBA5658 format */
    PIXEL_FMT_RGBX_4444,                 /**< RGBX4444 format */
    PIXEL_FMT_RGBA_4444,                 /**< RGBA4444 format */
    PIXEL_FMT_RGB_444,                   /**< RGB444 format */
    PIXEL_FMT_RGBX_5551,                 /**< RGBX5551 format */
    PIXEL_FMT_RGBA_5551,                 /**< RGBA5551 format */
    PIXEL_FMT_RGB_555,                   /**< RGB555 format */
    PIXEL_FMT_RGBX_8888,                 /**< RGBX8888 format */
    PIXEL_FMT_RGBA_8888,                 /**< RGBA8888 format */
    PIXEL_FMT_RGB_888,                   /**< RGB888 format */
    PIXEL_FMT_BGR_565,                   /**< BGR565 format */
    PIXEL_FMT_BGRX_4444,                 /**< BGRX4444 format */
    PIXEL_FMT_BGRA_4444,                 /**< BGRA4444 format */
    PIXEL_FMT_BGRX_5551,                 /**< BGRX5551 format */
    PIXEL_FMT_BGRA_5551,                 /**< BGRA5551 format */
    PIXEL_FMT_BGRX_8888,                 /**< BGRX8888 format */
    PIXEL_FMT_BGRA_8888,                 /**< BGRA8888 format */
    PIXEL_FMT_YUV_422_I,                 /**< YUV422 interleaved format */
    PIXEL_FMT_YCBCR_422_SP,              /**< YCBCR422 semi-planar format */
    PIXEL_FMT_YCRCB_422_SP,              /**< YCRCB422 semi-planar format */
    PIXEL_FMT_YCBCR_420_SP,              /**< YCBCR420 semi-planar format */
    PIXEL_FMT_YCRCB_420_SP,              /**< YCRCB420 semi-planar format */
    PIXEL_FMT_YCBCR_422_P,               /**< YCBCR422 planar format */
    PIXEL_FMT_YCRCB_422_P,               /**< YCRCB422 planar format */
    PIXEL_FMT_YCBCR_420_P,               /**< YCBCR420 planar format */
    PIXEL_FMT_YCRCB_420_P,               /**< YCRCB420 planar format */
    PIXEL_FMT_YUYV_422_PKG,              /**< YUYV422 packed format */
    PIXEL_FMT_UYVY_422_PKG,              /**< UYVY422 packed format */
    PIXEL_FMT_YVYU_422_PKG,              /**< YVYU422 packed format */
    PIXEL_FMT_VYUY_422_PKG,              /**< VYUY422 packed format */
    PIXEL_FMT_VENDER_MASK = 0X7FFF0000,  /**< vendor mask format */
    PIXEL_FMT_BUTT = 0X7FFFFFFF          /**< Invalid pixel format */
} PixelFormat;
namespace Rosen {
namespace Drawing {
    typedef uint32_t ColorQuad;
    typedef float scalar;
class Color {
public:
    constexpr static ColorQuad COLOR_TRANSPARENT = 0;
    constexpr static ColorQuad COLOR_BLACK = 0xFF000000;
    constexpr static ColorQuad COLOR_DKGRAY = 0xFF444444;
    constexpr static ColorQuad COLOR_GRAY = 0xFF888888;
    constexpr static ColorQuad COLOR_LTGRAY = 0xFFCCCCCC;
    constexpr static ColorQuad COLOR_WHITE = 0xFFFFFFFF;
    constexpr static ColorQuad COLOR_RED = 0xFFFF0000;
    constexpr static ColorQuad COLOR_GREEN = 0xFF00FF00;
    constexpr static ColorQuad COLOR_BLUE = 0xFF0000FF;
    constexpr static ColorQuad COLOR_YELLOW = 0xFFFFFF00;
    constexpr static ColorQuad COLOR_CYAN = 0xFF00FFFF;
    constexpr static ColorQuad COLOR_MAGENTA = 0xFFFF00FF;
};

enum ColorType {
    COLORTYPE_UNKNOWN = 0,
    COLORTYPE_ALPHA_8,
    COLORTYPE_RGB_565,
    COLORTYPE_ARGB_4444,
    COLORTYPE_RGBA_8888,
    COLORTYPE_BGRA_8888,
};

enum AlphaType {
    ALPHATYPE_UNKNOWN = 0,
    ALPHATYPE_OPAQUE,
    ALPHATYPE_PREMUL,
    ALPHATYPE_UNPREMUL,
};

struct BitmapFormat {
    ColorType colorType;
    AlphaType alphaType;
};

class Pen {
public:
    void SetAntiAlias(bool aa) {}
    void SetColor(const Color& c) {}
    void SetColor(int c) {}
    void SetWidth(scalar width){}
};

class Bitmap {
public:
    void* GetPixels() {return nullptr;}
    void Build(const int width, const int height, const BitmapFormat& format){}
    void SetPixels(void* pixel){}
    void ClearWithColor(const ColorQuad& color) const{}
};

class Canvas {
public:
    Canvas() {};
    virtual ~Canvas() {};
    void AttachPen(const Pen& pen) {}
    void DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py) {}
    void DrawBitmap(Media::PixelMap& pixelMap, const scalar px, const scalar py) {}
    void Bind(const Bitmap& bitmap){}
    void Clear(ColorQuad color){}
};

class Image {
public:

};

enum class FilterMode {
    NEAREST,
    LINEAR,
};

enum class MipmapMode {
    NONE,
    NEAREST,
    LINEAR,
};

struct CubicResampler {
    float cubicCoffB = 0;
    float cubicCoffC = 0;
    static constexpr CubicResampler Mitchell()
    {
        return { 1 / 3.0f, 1 / 3.0f };
    }
    static constexpr CubicResampler CatmullRom()
    {
        return { 0.0f, 1 / 2.0f };
    }
};

class SamplingOptions {
public:
    SamplingOptions() {}
    SamplingOptions(FilterMode fm) {}
    SamplingOptions(FilterMode fm, MipmapMode mm) {}
    SamplingOptions(const CubicResampler& c) {}
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif