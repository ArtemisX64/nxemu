// SPDX-FileCopyrightText: Copyright 2023 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <random>

#include "core/core.h"
#include "core/hle/kernel/k_process.h"
#include "core/hle/kernel/k_system_resource.h"
#include "core/hle/service/vi/container.h"
#include "core/hle/service/vi/shared_buffer_manager.h"
#include "core/hle/service/vi/vi_results.h"

namespace Service::VI {

namespace {

Result AllocateSharedBufferMemory(std::unique_ptr<Kernel::KPageGroup>* out_page_group,
                                  Core::System& system, u32 size) {
    using Core::Memory::YUZU_PAGESIZE;

    // Allocate memory for the system shared buffer.
    auto& kernel = system.Kernel();

    // Hold a temporary page group reference while we try to map it.
    auto pg = std::make_unique<Kernel::KPageGroup>(
        kernel, std::addressof(kernel.GetSystemSystemResource().GetBlockInfoManager()));

    // Allocate memory from secure pool.
    R_TRY(kernel.MemoryManager().AllocateAndOpen(
        pg.get(), size / YUZU_PAGESIZE,
        Kernel::KMemoryManager::EncodeOption(Kernel::KMemoryManager::Pool::Secure,
                                             Kernel::KMemoryManager::Direction::FromBack)));

    // Fill the output data with red.
    for (auto& block : *pg) {
        u32* start = system.DeviceMemory().GetPointer<u32>(block.GetAddress());
        u32* end = system.DeviceMemory().GetPointer<u32>(block.GetAddress() + block.GetSize());

        for (; start < end; start++) {
            *start = 0xFF0000FF;
        }
    }

    // Return the mapped page group.
    *out_page_group = std::move(pg);

    // We succeeded.
    R_SUCCEED();
}

Result MapSharedBufferIntoProcessAddressSpace(Common::ProcessAddress* out_map_address,
                                              std::unique_ptr<Kernel::KPageGroup>& pg,
                                              Kernel::KProcess* process, Core::System& system) {
    using Core::Memory::YUZU_PAGESIZE;

    auto& page_table = process->GetPageTable();

    // Get bounds of where mapping is possible.
    const VAddr alias_code_begin = GetInteger(page_table.GetAliasCodeRegionStart());
    const VAddr alias_code_size = page_table.GetAliasCodeRegionSize() / YUZU_PAGESIZE;
    const auto state = Kernel::KMemoryState::IoMemory;
    const auto perm = Kernel::KMemoryPermission::UserReadWrite;
    std::mt19937_64 rng{process->GetRandomEntropy(0)};

    // Retry up to 64 times to map into alias code range.
    Result res = ResultSuccess;
    int i;
    for (i = 0; i < 64; i++) {
        *out_map_address = alias_code_begin + ((rng() % alias_code_size) * YUZU_PAGESIZE);
        res = page_table.MapPageGroup(*out_map_address, *pg, state, perm);
        if (R_SUCCEEDED(res)) {
            break;
        }
    }

    // Return failure, if necessary
    R_UNLESS(i < 64, res);

    // We succeeded.
    R_SUCCEED();
}

constexpr u32 SharedBufferBlockLinearBpp = 4;

constexpr u32 SharedBufferBlockLinearWidth = 1280;
constexpr u32 SharedBufferBlockLinearHeight = 768;
constexpr u32 SharedBufferBlockLinearStride =
    SharedBufferBlockLinearWidth * SharedBufferBlockLinearBpp;
constexpr u32 SharedBufferNumSlots = 7;

constexpr u32 SharedBufferWidth = 1280;
constexpr u32 SharedBufferHeight = 720;
constexpr u32 SharedBufferAsync = false;

constexpr u32 SharedBufferSlotSize =
    SharedBufferBlockLinearWidth * SharedBufferBlockLinearHeight * SharedBufferBlockLinearBpp;
constexpr u32 SharedBufferSize = SharedBufferSlotSize * SharedBufferNumSlots;

constexpr SharedMemoryPoolLayout SharedBufferPoolLayout = [] {
    SharedMemoryPoolLayout layout{};
    layout.num_slots = SharedBufferNumSlots;

    for (u32 i = 0; i < SharedBufferNumSlots; i++) {
        layout.slots[i].buffer_offset = i * SharedBufferSlotSize;
        layout.slots[i].size = SharedBufferSlotSize;
        layout.slots[i].width = SharedBufferWidth;
        layout.slots[i].height = SharedBufferHeight;
    }

    return layout;
}();

void MakeGraphicBuffer(android::BufferQueueProducer& producer, u32 slot, u32 handle) {
    UNIMPLEMENTED();
}

} // namespace

SharedBufferManager::SharedBufferManager(Core::System& system, Container& container,
                                         std::shared_ptr<Nvidia::Module> nvdrv)
    : m_system(system), m_container(container), m_nvdrv(std::move(nvdrv)) {}

SharedBufferManager::~SharedBufferManager() = default;

Result SharedBufferManager::CreateSession(Kernel::KProcess* owner_process, u64* out_buffer_id,
                                          u64* out_layer_handle, u64 display_id,
                                          bool enable_blending) {
    std::scoped_lock lk{m_guard};

    // Ensure we haven't already created.
    const u64 aruid = owner_process->GetProcessId();
    R_UNLESS(!m_sessions.contains(aruid), VI::ResultPermissionDenied);

    // Allocate memory for the shared buffer if needed.
    if (!m_buffer_page_group) {
        R_TRY(AllocateSharedBufferMemory(std::addressof(m_buffer_page_group), m_system,
                                         SharedBufferSize));

        // Record buffer id.
        m_buffer_id = m_next_buffer_id++;

        // Record display id.
        m_display_id = display_id;
    }

    // Map into process.
    Common::ProcessAddress map_address{};
    R_TRY(MapSharedBufferIntoProcessAddressSpace(std::addressof(map_address), m_buffer_page_group,
                                                 owner_process, m_system));

    // Create new session.
    auto [it, was_emplaced] = m_sessions.emplace(aruid, SharedBufferSession{});
    auto& session = it->second;

    UNIMPLEMENTED();

    // Create and open a layer for the display.
    s32 producer_binder_id;
    R_TRY(m_container.CreateStrayLayer(std::addressof(producer_binder_id),
                                       std::addressof(session.layer_id), display_id));

    // Configure blending.
    R_ASSERT(m_container.SetLayerBlending(session.layer_id, enable_blending));

    // Get the producer and set preallocated buffers.
    std::shared_ptr<android::BufferQueueProducer> producer;
    R_TRY(m_container.GetLayerProducerHandle(std::addressof(producer), session.layer_id));
    MakeGraphicBuffer(*producer, 0, session.buffer_nvmap_handle);
    MakeGraphicBuffer(*producer, 1, session.buffer_nvmap_handle);

    // Assign outputs.
    *out_buffer_id = m_buffer_id;
    *out_layer_handle = session.layer_id;

    // We succeeded.
    R_SUCCEED();
}

void SharedBufferManager::DestroySession(Kernel::KProcess* owner_process) {
    std::scoped_lock lk{m_guard};

    if (m_buffer_id == 0) {
        return;
    }

    UNIMPLEMENTED();
}

Result SharedBufferManager::GetSharedBufferMemoryHandleId(u64* out_buffer_size,
                                                          s32* out_nvmap_handle,
                                                          SharedMemoryPoolLayout* out_pool_layout,
                                                          u64 buffer_id,
                                                          u64 applet_resource_user_id) {
    std::scoped_lock lk{m_guard};

    R_UNLESS(m_buffer_id > 0, VI::ResultNotFound);
    R_UNLESS(buffer_id == m_buffer_id, VI::ResultNotFound);
    R_UNLESS(m_sessions.contains(applet_resource_user_id), VI::ResultNotFound);

    *out_pool_layout = SharedBufferPoolLayout;
    *out_buffer_size = SharedBufferSize;
    *out_nvmap_handle = m_sessions[applet_resource_user_id].buffer_nvmap_handle;

    R_SUCCEED();
}

Result SharedBufferManager::CancelSharedFrameBuffer(u64 layer_id, s64 slot) {
    UNIMPLEMENTED();
    R_SUCCEED();
}

Result SharedBufferManager::GetSharedFrameBufferAcquirableEvent(Kernel::KReadableEvent** out_event,
                                                                u64 layer_id) {
    UNIMPLEMENTED();
    R_SUCCEED();
}

Result SharedBufferManager::WriteAppletCaptureBuffer(bool* out_was_written, s32* out_layer_index) {
    UNIMPLEMENTED();
    R_SUCCEED();
}

} // namespace Service::VI
