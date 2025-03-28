// SPDX-FileCopyrightText: Copyright 2019 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "core/hle/service/glue/errors.h"
#include "core/hle/service/glue/glue_manager.h"

namespace Service::Glue {

struct ARPManager::MapEntry {
    std::vector<u8> control;
};

ARPManager::ARPManager() = default;

ARPManager::~ARPManager() = default;

Result ARPManager::GetControlProperty(std::vector<u8>* out_control_property, u64 title_id) const {
    if (title_id == 0) {
        return Glue::ResultInvalidProcessId;
    }

    const auto iter = entries.find(title_id);
    if (iter == entries.end()) {
        return Glue::ResultProcessIdNotRegistered;
    }

    *out_control_property = iter->second.control;
    return ResultSuccess;
}

Result ARPManager::Unregister(u64 title_id) {
    if (title_id == 0) {
        return Glue::ResultInvalidProcessId;
    }

    const auto iter = entries.find(title_id);
    if (iter == entries.end()) {
        return Glue::ResultProcessIdNotRegistered;
    }

    entries.erase(iter);
    return ResultSuccess;
}

void ARPManager::ResetAll() {
    entries.clear();
}

} // namespace Service::Glue
