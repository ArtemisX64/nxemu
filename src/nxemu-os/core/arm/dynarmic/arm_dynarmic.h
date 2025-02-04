// SPDX-FileCopyrightText: Copyright 2023 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "core/arm/arm_interface.h"

namespace Core {


#ifdef __linux__

class ScopedJitExecution {
public:
    explicit ScopedJitExecution(Kernel::KProcess* process);
    ~ScopedJitExecution();
    static void RegisterHandler();
};

#else

class ScopedJitExecution {
public:
    explicit ScopedJitExecution(Kernel::KProcess* process) {}
    ~ScopedJitExecution() {}
    static void RegisterHandler() {}
};

#endif

} // namespace Core
