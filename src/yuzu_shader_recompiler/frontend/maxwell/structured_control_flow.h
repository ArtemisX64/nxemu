// SPDX-FileCopyrightText: Copyright 2021 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "yuzu_shader_recompiler/environment.h"
#include "yuzu_shader_recompiler/frontend/ir/abstract_syntax_list.h"
#include "yuzu_shader_recompiler/frontend/ir/basic_block.h"
#include "yuzu_shader_recompiler/frontend/ir/value.h"
#include "yuzu_shader_recompiler/frontend/maxwell/control_flow.h"
#include "yuzu_shader_recompiler/object_pool.h"

namespace Shader {
struct HostTranslateInfo;
namespace Maxwell {

[[nodiscard]] IR::AbstractSyntaxList BuildASL(ObjectPool<IR::Inst>& inst_pool,
                                              ObjectPool<IR::Block>& block_pool, Environment& env,
                                              Flow::CFG& cfg, const HostTranslateInfo& host_info);

} // namespace Maxwell
} // namespace Shader
