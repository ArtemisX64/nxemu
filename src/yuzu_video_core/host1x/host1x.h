// SPDX-FileCopyrightText: 2021 yuzu Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "yuzu_common/common_types.h"

#include "yuzu_common/address_space.h"
#include "yuzu_video_core/host1x/gpu_device_memory_manager.h"
#include "yuzu_video_core/host1x/syncpoint_manager.h"
#include "yuzu_video_core/memory_manager.h"

namespace Core {
class System;
} // namespace Core

namespace Tegra {

namespace Host1x {

class Host1x {
public:
    explicit Host1x(Core::System& system);
    ~Host1x();

    SyncpointManager& GetSyncpointManager() {
        return syncpoint_manager;
    }

    const SyncpointManager& GetSyncpointManager() const {
        return syncpoint_manager;
    }

    Tegra::MaxwellDeviceMemoryManager& MemoryManager() {
        return memory_manager;
    }

    const Tegra::MaxwellDeviceMemoryManager& MemoryManager() const {
        return memory_manager;
    }

    Tegra::MemoryManager& GMMU() {
        return gmmu_manager;
    }

    const Tegra::MemoryManager& GMMU() const {
        return gmmu_manager;
    }

    Common::FlatAllocator<u32, 0, 32>& Allocator() {
        return *allocator;
    }

    const Common::FlatAllocator<u32, 0, 32>& Allocator() const {
        return *allocator;
    }

private:
    SyncpointManager syncpoint_manager;
    Tegra::MaxwellDeviceMemoryManager memory_manager;
    Tegra::MemoryManager gmmu_manager;
    std::unique_ptr<Common::FlatAllocator<u32, 0, 32>> allocator;
};

} // namespace Host1x

} // namespace Tegra
