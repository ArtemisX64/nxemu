// SPDX-FileCopyrightText: Copyright 2021 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "yuzu_common/common_types.h"
#include "yuzu_shader_recompiler/frontend/maxwell/opcodes.h"

namespace Shader::Maxwell {

[[nodiscard]] Opcode Decode(u64 insn);

} // namespace Shader::Maxwell
