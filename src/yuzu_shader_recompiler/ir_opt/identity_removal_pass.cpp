// SPDX-FileCopyrightText: Copyright 2021 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <vector>

#include "yuzu_shader_recompiler/frontend/ir/basic_block.h"
#include "yuzu_shader_recompiler/frontend/ir/value.h"
#include "yuzu_shader_recompiler/ir_opt/passes.h"

namespace Shader::Optimization {

void IdentityRemovalPass(IR::Program& program) {
    std::vector<IR::Inst*> to_invalidate;
    for (IR::Block* const block : program.blocks) {
        for (auto inst = block->begin(); inst != block->end();) {
            const size_t num_args{inst->NumArgs()};
            for (size_t i = 0; i < num_args; ++i) {
                IR::Value arg;
                while ((arg = inst->Arg(i)).IsIdentity()) {
                    inst->SetArg(i, arg.Inst()->Arg(0));
                }
            }
            if (inst->GetOpcode() == IR::Opcode::Identity ||
                inst->GetOpcode() == IR::Opcode::Void) {
                to_invalidate.push_back(&*inst);
                inst = block->Instructions().erase(inst);
            } else {
                ++inst;
            }
        }
    }
    for (IR::Inst* const inst : to_invalidate) {
        inst->Invalidate();
    }
}

} // namespace Shader::Optimization
