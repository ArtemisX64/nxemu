// SPDX-FileCopyrightText: Copyright 2020 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "yuzu_common/logging/log.h"
#include "core/core.h"
#include "core/hle/service/caps/caps_manager.h"
#include "core/hle/service/caps/caps_su.h"
#include "core/hle/service/caps/caps_types.h"
#include "core/hle/service/cmif_serialization.h"
#include "core/hle/service/ipc_helpers.h"

namespace Service::Capture {

IScreenShotApplicationService::IScreenShotApplicationService(
    Core::System& system_, std::shared_ptr<AlbumManager> album_manager)
    : ServiceFramework{system_, "caps:su"}, manager{album_manager} {
    // clang-format off
    static const FunctionInfo functions[] = {
        {32, C<&IScreenShotApplicationService::SetShimLibraryVersion>, "SetShimLibraryVersion"},
        {201, nullptr, "SaveScreenShot"},
        {203, C<&IScreenShotApplicationService::SaveScreenShotEx0>, "SaveScreenShotEx0"},
        {205, C<&IScreenShotApplicationService::SaveScreenShotEx1>, "SaveScreenShotEx1"},
        {210, nullptr, "SaveScreenShotEx2"},
    };
    // clang-format on

    RegisterHandlers(functions);
}

IScreenShotApplicationService::~IScreenShotApplicationService() = default;

Result IScreenShotApplicationService::SetShimLibraryVersion(ShimLibraryVersion library_version,
                                                            ClientAppletResourceUserId aruid) {
    LOG_WARNING(Service_Capture, "(STUBBED) called. library_version={}, applet_resource_user_id={}",
                library_version, aruid.pid);
    R_SUCCEED();
}

Result IScreenShotApplicationService::SaveScreenShotEx0(
    Out<ApplicationAlbumEntry> out_entry, const ScreenShotAttribute& attribute,
    AlbumReportOption report_option, ClientAppletResourceUserId aruid,
    InBuffer<BufferAttr_HipcMapTransferAllowsNonSecure | BufferAttr_HipcMapAlias>
        image_data_buffer) {
    LOG_INFO(Service_Capture,
             "called, report_option={}, image_data_buffer_size={}, applet_resource_user_id={}",
             report_option, image_data_buffer.size(), aruid.pid);

    manager->FlipVerticallyOnWrite(false);
    R_RETURN(manager->SaveScreenShot(*out_entry, attribute, report_option, image_data_buffer,
                                     aruid.pid));
}

Result IScreenShotApplicationService::SaveScreenShotEx1(
    Out<ApplicationAlbumEntry> out_entry, const ScreenShotAttribute& attribute,
    AlbumReportOption report_option, ClientAppletResourceUserId aruid,
    const InLargeData<ApplicationData, BufferAttr_HipcMapAlias> app_data_buffer,
    const InBuffer<BufferAttr_HipcMapTransferAllowsNonSecure | BufferAttr_HipcMapAlias>
        image_data_buffer) {
    LOG_INFO(Service_Capture,
             "called, report_option={}, image_data_buffer_size={}, applet_resource_user_id={}",
             report_option, image_data_buffer.size(), aruid.pid);

    manager->FlipVerticallyOnWrite(false);
    R_RETURN(manager->SaveScreenShot(*out_entry, attribute, report_option, *app_data_buffer,
                                     image_data_buffer, aruid.pid));
}

void IScreenShotApplicationService::CaptureAndSaveScreenshot(AlbumReportOption report_option) {
    UNIMPLEMENTED();
}

} // namespace Service::Capture
