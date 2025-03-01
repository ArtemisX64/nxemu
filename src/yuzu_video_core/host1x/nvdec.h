// SPDX-FileCopyrightText: Copyright 2020 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <memory>
#include <vector>
#include "yuzu_common/common_types.h"
#include "yuzu_video_core/host1x/codecs/codec.h"

namespace Tegra {

namespace Host1x {

class Host1x;

class Nvdec {
public:
    explicit Nvdec(Host1x& host1x);
    ~Nvdec();

    /// Writes the method into the state, Invoke Execute() if encountered
    void ProcessMethod(u32 method, u32 argument);

private:
    Host1x& host1x;
    NvdecCommon::NvdecRegisters state;
    std::unique_ptr<Codec> codec;
};

} // namespace Host1x

} // namespace Tegra
