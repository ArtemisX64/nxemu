// SPDX-FileCopyrightText: Copyright 2020 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "yuzu_common/yuzu_assert.h"
#include "yuzu_video_core/host1x/host1x.h"
#include "yuzu_video_core/host1x/nvdec.h"

namespace Tegra::Host1x {

#define NVDEC_REG_INDEX(field_name)                                                                \
    (offsetof(NvdecCommon::NvdecRegisters, field_name) / sizeof(u64))

Nvdec::Nvdec(Host1x& host1x_)
    : host1x(host1x_), state{}, codec(std::make_unique<Codec>(host1x, state)) {}

Nvdec::~Nvdec() = default;

void Nvdec::ProcessMethod(u32 method, u32 argument) {
    state.reg_array[method] = static_cast<u64>(argument) << 8;

    switch (method) {
    case NVDEC_REG_INDEX(set_codec_id):
        codec->SetTargetCodec(static_cast<NvdecCommon::VideoCodec>(argument));
        break;
    case NVDEC_REG_INDEX(execute):
        UNIMPLEMENTED();
        break;
    }
}

} // namespace Tegra::Host1x
