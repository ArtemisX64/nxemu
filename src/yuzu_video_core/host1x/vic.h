// SPDX-FileCopyrightText: Copyright 2020 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <memory>

#include "yuzu_common/common_types.h"
#include "yuzu_common/scratch_buffer.h"

struct SwsContext;

namespace Tegra {

namespace Host1x {

class Host1x;
class Nvdec;
union VicConfig;

class Vic {
public:
    enum class Method : u32 {
        Execute = 0xc0,
        SetControlParams = 0x1c1,
        SetConfigStructOffset = 0x1c2,
        SetOutputSurfaceLumaOffset = 0x1c8,
        SetOutputSurfaceChromaOffset = 0x1c9,
        SetOutputSurfaceChromaUnusedOffset = 0x1ca
    };

    explicit Vic(Host1x& host1x, std::shared_ptr<Nvdec> nvdec_processor);

    ~Vic();

    /// Write to the device state.
    void ProcessMethod(Method method, u32 argument);

private:
    void Execute();

    Host1x& host1x;
    std::shared_ptr<Tegra::Host1x::Nvdec> nvdec_processor;

    /// Avoid reallocation of the following buffers every frame, as their
    /// size does not change during a stream
    Common::ScratchBuffer<u8> luma_buffer;
    Common::ScratchBuffer<u8> chroma_buffer;

    GPUVAddr config_struct_address{};
    GPUVAddr output_surface_luma_address{};
    GPUVAddr output_surface_chroma_address{};

    SwsContext* scaler_ctx{};
    s32 scaler_width{};
    s32 scaler_height{};
};

} // namespace Host1x

} // namespace Tegra
