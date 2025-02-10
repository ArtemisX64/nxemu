// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "core/core_timing.h"
#include "hid_core/frontend/emulated_devices.h"
#include "hid_core/hid_core.h"
#include "hid_core/resources/applet_resource.h"
#include "hid_core/resources/mouse/debug_mouse.h"
#include "hid_core/resources/shared_memory_format.h"

namespace Service::HID {

DebugMouse::DebugMouse(Core::HID::HIDCore& hid_core_) : ControllerBase{hid_core_} {
    emulated_devices = hid_core.GetEmulatedDevices();
}

DebugMouse::~DebugMouse() = default;

void DebugMouse::OnInit() {}
void DebugMouse::OnRelease() {}

void DebugMouse::OnUpdate(const Core::Timing::CoreTiming& core_timing) {
    std::scoped_lock shared_lock{*shared_mutex};
    const u64 aruid = applet_resource->GetActiveAruid();
    auto* data = applet_resource->GetAruidData(aruid);

    if (data == nullptr || !data->flag.is_assigned) {
        return;
    }

    MouseSharedMemoryFormat& shared_memory = data->shared_memory_format->debug_mouse;

    if (!IsControllerActivated()) {
        shared_memory.mouse_lifo.buffer_count = 0;
        shared_memory.mouse_lifo.buffer_tail = 0;
        return;
    }

    next_state = {};

    const auto& last_entry = shared_memory.mouse_lifo.ReadCurrentEntry().state;
    next_state.sampling_number = last_entry.sampling_number + 1;

    if (Settings::values.mouse_enabled) {
        UNIMPLEMENTED();
    }

    shared_memory.mouse_lifo.WriteNextEntry(next_state);
}

} // namespace Service::HID
