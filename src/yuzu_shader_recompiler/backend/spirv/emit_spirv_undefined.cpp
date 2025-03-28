// SPDX-FileCopyrightText: Copyright 2021 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "yuzu_shader_recompiler/backend/spirv/emit_spirv_instructions.h"
#include "yuzu_shader_recompiler/backend/spirv/spirv_emit_context.h"

namespace Shader::Backend::SPIRV {

Id EmitUndefU1(EmitContext& ctx) {
    return ctx.OpUndef(ctx.U1);
}

Id EmitUndefU8(EmitContext&) {
    throw NotImplementedException("SPIR-V Instruction");
}

Id EmitUndefU16(EmitContext&) {
    throw NotImplementedException("SPIR-V Instruction");
}

Id EmitUndefU32(EmitContext& ctx) {
    return ctx.OpUndef(ctx.U32[1]);
}

Id EmitUndefU64(EmitContext&) {
    throw NotImplementedException("SPIR-V Instruction");
}

} // namespace Shader::Backend::SPIRV
