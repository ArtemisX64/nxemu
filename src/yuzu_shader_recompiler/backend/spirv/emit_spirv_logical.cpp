// SPDX-FileCopyrightText: Copyright 2021 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "yuzu_shader_recompiler/backend/spirv/emit_spirv_instructions.h"
#include "yuzu_shader_recompiler/backend/spirv/spirv_emit_context.h"

namespace Shader::Backend::SPIRV {

Id EmitLogicalOr(EmitContext& ctx, Id a, Id b) {
    return ctx.OpLogicalOr(ctx.U1, a, b);
}

Id EmitLogicalAnd(EmitContext& ctx, Id a, Id b) {
    return ctx.OpLogicalAnd(ctx.U1, a, b);
}

Id EmitLogicalXor(EmitContext& ctx, Id a, Id b) {
    return ctx.OpLogicalNotEqual(ctx.U1, a, b);
}

Id EmitLogicalNot(EmitContext& ctx, Id value) {
    return ctx.OpLogicalNot(ctx.U1, value);
}

} // namespace Shader::Backend::SPIRV
