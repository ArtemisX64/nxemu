#include "switch_system.h"
#include "file_format/nacp.h"
#include "file_format/nro.h"
#include "settings/core_settings.h"
#include "settings/identifiers.h"
#include "settings/settings.h"

std::unique_ptr<SwitchSystem> SwitchSystem::m_instance;

bool SwitchSystem::Create()
{
    ShutDown();
    m_instance.reset(new SwitchSystem());
    if (m_instance == nullptr)
    {
        return false;
    }
    return m_instance->Initialize();
}

void SwitchSystem::ShutDown(void)
{
    if (m_instance)
    {
        m_instance.reset();
    }
}

SwitchSystem * SwitchSystem::GetInstance()
{
    return m_instance.get();
}

SwitchSystem::SwitchSystem() :
    m_emulationRunning(false)
{
}

SwitchSystem::~SwitchSystem()
{
    StopEmulation();
}

bool SwitchSystem::Initialize()
{
    if (!m_modules.Initialize())
    {
        return false;
    }
    return true;
}

void SwitchSystem::StartEmulation(void)
{
    m_emulationRunning = true;
    m_modules.StartEmulation();
}

void SwitchSystem::StopEmulation(void)
{
    if (!m_emulationRunning)
    {
        return;
    }
    m_emulationRunning = false;
    m_modules.StopEmulation();
}

bool SwitchSystem::LoadRom(const char * romFile)
{
    bool res = false;
    if (Nro::IsNroFile(romFile))
    {
        res = LoadNRO(romFile);
    }

    if (res)
    {
        Settings::GetInstance().SetString(NXCoreSetting::GameFile, romFile);
    }
    return res;
}

bool SwitchSystem::LoadNRO(const char * nroFile)
{
    Path filePath(nroFile);
    m_nro = std::make_unique<Nro>(filePath);
    if (!m_nro->Valid())
    {
        return false;
    }

    const NACP * Nacp = m_nro->Nacp();
    if (Nacp == nullptr)
    {
        return false;
    }
    Settings::GetInstance().SetString(NXCoreSetting::GameName, Nacp->GetApplicationName().c_str());
    return true;
}

