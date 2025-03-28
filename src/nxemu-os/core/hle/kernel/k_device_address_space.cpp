// SPDX-FileCopyrightText: Copyright 2023 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "yuzu_common/yuzu_assert.h"
#include "core/core.h"
#include "core/hle/kernel/k_device_address_space.h"
#include "core/hle/kernel/kernel.h"
#include "core/hle/kernel/svc_results.h"

namespace Kernel {

KDeviceAddressSpace::KDeviceAddressSpace(KernelCore& kernel)
    : KAutoObjectWithSlabHeapAndContainer(kernel), m_lock(kernel), m_is_initialized(false) {}
KDeviceAddressSpace::~KDeviceAddressSpace() = default;

void KDeviceAddressSpace::Initialize() {
    // This just forwards to the device page table manager.
    // KDevicePageTable::Initialize();
}

// Member functions.
Result KDeviceAddressSpace::Initialize(u64 address, u64 size) {
    // Initialize the device page table.
    // R_TRY(m_table.Initialize(address, size));

    // Set member variables.
    m_space_address = address;
    m_space_size = size;
    m_is_initialized = true;

    R_SUCCEED();
}

void KDeviceAddressSpace::Finalize() {
    // Finalize the table.
    // m_table.Finalize();
}

Result KDeviceAddressSpace::Attach(Svc::DeviceName device_name) {
    // Lock the address space.
    KScopedLightLock lk(m_lock);

    // Attach.
    // R_RETURN(m_table.Attach(device_name, m_space_address, m_space_size));
    R_SUCCEED();
}

Result KDeviceAddressSpace::Detach(Svc::DeviceName device_name) {
    // Lock the address space.
    KScopedLightLock lk(m_lock);

    // Detach.
    // R_RETURN(m_table.Detach(device_name));
    R_SUCCEED();
}

Result KDeviceAddressSpace::Map(KProcessPageTable* page_table, KProcessAddress process_address,
                                size_t size, u64 device_address, u32 option, bool is_aligned) {
    // Check that the address falls within the space.
    R_UNLESS((m_space_address <= device_address &&
              device_address + size - 1 <= m_space_address + m_space_size - 1),
             ResultInvalidCurrentMemory);

    // Decode the option.
    const Svc::MapDeviceAddressSpaceOption option_pack{option};
    const auto device_perm = option_pack.permission.Value();
    const auto flags = option_pack.flags.Value();
    const auto reserved = option_pack.reserved.Value();

    // Validate the option.
    // TODO: It is likely that this check for flags == none is only on NX board.
    R_UNLESS(flags == Svc::MapDeviceAddressSpaceFlag::None, ResultInvalidEnumValue);
    R_UNLESS(reserved == 0, ResultInvalidEnumValue);

    // Lock the address space.
    KScopedLightLock lk(m_lock);

    // Lock the page table to prevent concurrent device mapping operations.
    // KScopedLightLock pt_lk = page_table->AcquireDeviceMapLock();

    // Lock the pages.
    bool is_io{};
    R_TRY(page_table->LockForMapDeviceAddressSpace(std::addressof(is_io), process_address, size,
                                                   ConvertToKMemoryPermission(device_perm),
                                                   is_aligned, true));

    // Ensure that if we fail, we don't keep unmapped pages locked.
    ON_RESULT_FAILURE {
        ASSERT(page_table->UnlockForDeviceAddressSpace(process_address, size) == ResultSuccess);
    };

    // Check that the io status is allowable.
    if (is_io) {
        R_UNLESS(static_cast<u32>(flags & Svc::MapDeviceAddressSpaceFlag::NotIoRegister) == 0,
                 ResultInvalidCombination);
    }

    // Map the pages.
    {
        // Perform the mapping.
        // R_TRY(m_table.Map(page_table, process_address, size, device_address, device_perm,
        //                   is_aligned, is_io));

        // Ensure that we unmap the pages if we fail to update the protections.
        // NOTE: Nintendo does not check the result of this unmap call.
        // ON_RESULT_FAILURE { m_table.Unmap(device_address, size); };

        // Update the protections in accordance with how much we mapped.
        // R_TRY(page_table->UnlockForDeviceAddressSpacePartialMap(process_address, size));
    }

    // We succeeded.
    R_SUCCEED();
}

Result KDeviceAddressSpace::Unmap(KProcessPageTable* page_table, KProcessAddress process_address,
                                  size_t size, u64 device_address) {
    // Check that the address falls within the space.
    R_UNLESS((m_space_address <= device_address &&
              device_address + size - 1 <= m_space_address + m_space_size - 1),
             ResultInvalidCurrentMemory);

    // Lock the address space.
    KScopedLightLock lk(m_lock);

    // Lock the page table to prevent concurrent device mapping operations.
    // KScopedLightLock pt_lk = page_table->AcquireDeviceMapLock();

    // Lock the pages.
    R_TRY(page_table->LockForUnmapDeviceAddressSpace(process_address, size, true));

    // Unmap the pages.
    {
        // If we fail to unmap, we want to do a partial unlock.
        // ON_RESULT_FAILURE {
        //     ASSERT(page_table->UnlockForDeviceAddressSpacePartialMap(process_address, size) ==
        //            ResultSuccess);
        // };

        // Perform the unmap.
        // R_TRY(m_table.Unmap(page_table, process_address, size, device_address));
    }

    // Unlock the pages.
    ASSERT(page_table->UnlockForDeviceAddressSpace(process_address, size) == ResultSuccess);

    R_SUCCEED();
}

} // namespace Kernel
