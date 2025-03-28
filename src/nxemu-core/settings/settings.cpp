#include "settings.h"
#include <common/file.h>
#include <common/json.h>
#include <common/path.h>

std::unique_ptr<Settings> Settings::s_instance;

Settings::Settings()
{
    Path congFilePath(Path::MODULE_DIRECTORY, "NxEmu.config");
    congFilePath.AppendDirectory("config");
    m_configPath = (const char *)congFilePath;
}

bool Settings::Initialize()
{
    m_details = JsonValue();
    for (size_t i = 0; i < 100; i++)
    {
        File configFile;
        if (!configFile.Open(m_configPath.c_str(), IFile::modeRead))
        {
            break;
        }
        uint32_t fileLen = (uint32_t)configFile.GetLength();
        if (fileLen <= 0)
        {
            break;
        }
        std::unique_ptr<char[]> data = std::make_unique<char[]>(fileLen);
        uint32_t Size = configFile.Read(data.get(), fileLen);
        if (Size != fileLen)
        {
            break;
        }

        JsonReader reader;
        JsonValue root;
        if (!reader.Parse(data.get(), data.get() + Size, root))
        {
            return false;
        }
        const JsonValue * value = root.Find("ConfigFile");
        if (value == nullptr)
        {
            m_details = std::move(root);
            break;
        }
        else
        {
            Path newConfigfile(value != nullptr ? value->asString() : "");
            newConfigfile.DirectoryNormalize(Path(Path::MODULE_DIRECTORY));
            m_configPath = (const char *)newConfigfile;
        }
    }
    return true;
}

const char * Settings::GetConfigFile(void) const
{
    return m_configPath.c_str();
}

void Settings::SetChanged(const char * setting, bool changed)
{
    if (setting == nullptr)
    {
        return;
    }
    m_settingsChanged[setting] = changed;
}

JsonValue Settings::GetSettings(const char * section) const
{
    const JsonValue * value = m_details.Find(section);
    if (value != nullptr)
    {
        return *value;
    }
    return JsonValue();
}

void Settings::SetSettings(const char * section, JsonValue & json)
{
    m_details[section] = json;
}

std::string Settings::GetDefaultString(const char * setting) const
{
    SettingsMapString::const_iterator itr = m_settingsDefaultString.find(setting);
    if (itr == m_settingsDefaultString.end())
    {
        return "";
    }
    return itr->second;
}

bool Settings::GetDefaultBool(const char * setting) const
{
    SettingsMapBool::const_iterator itr = m_settingsDefaultBool.find(setting);
    if (itr == m_settingsDefaultBool.end())
    {
        return false;
    }
    return itr->second;
}

std::string Settings::GetString(const char * setting) const
{
    SettingsMapString::const_iterator itr = m_settingsString.find(setting);
    if (itr == m_settingsString.end())
    {
        return GetDefaultString(setting);
    }
    return itr->second;
}

bool Settings::GetBool(const char * setting) const
{
    SettingsMapBool::const_iterator itr = m_settingsBool.find(setting);
    if (itr == m_settingsBool.end())
    {
        return GetDefaultBool(setting);
    }
    return itr->second;
}

bool Settings::GetChanged(const char * setting) const
{
    SettingsMapBool::const_iterator itr = m_settingsChanged.find(setting);
    if (itr == m_settingsChanged.end())
    {
        return false;
    }
    return itr->second;
}

void Settings::SetDefaultString(const char * setting, const char * value)
{
    if (setting == nullptr || value == nullptr)
    {
        return;
    }
    m_settingsDefaultString[setting] = value;
}

void Settings::SetDefaultBool(const char * setting, bool value)
{
    m_settingsDefaultBool[setting] = value;
}

void Settings::SetString(const char * setting, const char * value)
{
    if (setting == nullptr || value == nullptr)
    {
        return;
    }

    SettingsMapString::const_iterator it = m_settingsString.find(setting);
    if (it != m_settingsString.end() && it->second.compare(value) == 0)
    {
        return;
    }
    m_settingsString[setting] = value;
    NotifyChange(setting);
}

void Settings::SetBool(const char * setting, bool value)
{
    if (setting == nullptr)
    {
        return;
    }

    SettingsMapBool::const_iterator it = m_settingsBool.find(setting);
    if (it != m_settingsBool.end() && it->second == value)
    {
        return;
    }
    m_settingsBool[setting] = value;
    NotifyChange(setting);
}

void Settings::Save(void)
{
    std::string jsonStr = JsonStyledWriter().write(m_details);
    Path(m_configPath).DirectoryCreate();
    File configFile;
    if (!configFile.Open(m_configPath.c_str(), IFile::modeWrite | IFile::modeCreate))
    {
        return;
    }
    configFile.Write(jsonStr.c_str(), (uint32_t)jsonStr.length());
}

void Settings::RegisterCallback(const std::string & setting, std::function<void()> callback)
{
    m_notification[setting].emplace_back(callback);
}

Settings & Settings::GetInstance()
{
    if (s_instance == nullptr)
    {
        s_instance = std::make_unique<Settings>();
    }
    return *s_instance;
}

void Settings::CleanUp()
{
    s_instance.reset();
}

void Settings::NotifyChange(const char * setting)
{
    if (setting == nullptr)
    {
        return;
    }
    NotificationMap::const_iterator itr = m_notification.find(setting);
    if (itr != m_notification.end())
    {
        const NotificationCallbacks & callbacks = itr->second;
        for (NotificationCallbacks::const_iterator callItr = callbacks.begin(); callItr != callbacks.end(); callItr++)
        {
            if (*callItr)
            {
                (*callItr)();
            }
        }
    }
}