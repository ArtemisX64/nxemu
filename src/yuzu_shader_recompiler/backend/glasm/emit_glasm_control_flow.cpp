// SPDX-FileCopyrightText: Copyright 2021 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "yuzu_shader_recompiler/backend/glasm/emit_glasm_instructions.h"
#include "yuzu_shader_recompiler/backend/glasm/glasm_emit_context.h"

namespace Shader::Backend::GLASM {

void EmitJoin(EmitContext&) {
    throw NotImplementedException("Join shouldn't be emitted");
}

void EmitDemoteToHelperInvocation(EmitContext& ctx) {
    ctx.Add("KIL TR.x;");
}

} // namespace Shader::Backend::GLASM
