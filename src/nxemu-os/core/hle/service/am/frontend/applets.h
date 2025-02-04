// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <memory>
#include <queue>

#include "common/swap.h"
#include "core/hle/service/am/applet.h"

union Result;

namespace Core {
class System;
}

namespace Core::Frontend {
class ECommerceApplet;
} // namespace Core::Frontend

namespace Kernel {
class KernelCore;
class KEvent;
class KReadableEvent;
} // namespace Kernel

namespace Service::NFP {
enum class CabinetMode : u8;
} // namespace Service::NFP

namespace Service::AM {

class IStorage;

namespace Frontend {

class FrontendApplet {
public:
    explicit FrontendApplet(Core::System& system_, std::shared_ptr<Applet> applet_,
                            LibraryAppletMode applet_mode_);
    virtual ~FrontendApplet();

    virtual void Initialize();

    virtual Result GetStatus() const = 0;
    virtual void ExecuteInteractive() = 0;
    virtual void Execute() = 0;
    virtual Result RequestExit() = 0;

    LibraryAppletMode GetLibraryAppletMode() const {
        return applet_mode;
    }

    bool IsInitialized() const {
        return initialized;
    }

protected:
    std::shared_ptr<IStorage> PopInData();
    std::shared_ptr<IStorage> PopInteractiveInData();
    void PushOutData(std::shared_ptr<IStorage> storage);
    void PushInteractiveOutData(std::shared_ptr<IStorage> storage);
    void Exit();

protected:
    Core::System& system;
    CommonArguments common_args{};
    std::weak_ptr<Applet> applet{};
    LibraryAppletMode applet_mode{};
    bool initialized{false};
};

struct FrontendAppletSet {
    FrontendAppletSet();
    ~FrontendAppletSet();

    FrontendAppletSet(const FrontendAppletSet&) = delete;
    FrontendAppletSet& operator=(const FrontendAppletSet&) = delete;

    FrontendAppletSet(FrontendAppletSet&&) noexcept;
    FrontendAppletSet& operator=(FrontendAppletSet&&) noexcept;
};

class FrontendAppletHolder {
public:
    explicit FrontendAppletHolder(Core::System& system_);
    ~FrontendAppletHolder();

    const FrontendAppletSet& GetFrontendAppletSet() const;
    NFP::CabinetMode GetCabinetMode() const;
    AppletId GetCurrentAppletId() const;

    void SetFrontendAppletSet(FrontendAppletSet set);
    void SetCabinetMode(NFP::CabinetMode mode);
    void SetCurrentAppletId(AppletId applet_id);
    void SetDefaultAppletsIfMissing();
    void ClearAll();

    std::shared_ptr<FrontendApplet> GetApplet(std::shared_ptr<Applet> applet, AppletId id,
                                              LibraryAppletMode mode) const;

private:
    AppletId current_applet_id{};
    NFP::CabinetMode cabinet_mode{};

    FrontendAppletSet frontend;
    Core::System& system;
};

} // namespace Frontend
} // namespace Service::AM
