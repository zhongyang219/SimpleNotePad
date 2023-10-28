#include "configdata.h"

ConfigData ConfigData::m_ins;

ConfigData::ConfigData()
{

}

ConfigData &ConfigData::Instance()
{
    return m_ins;
}

void ConfigData::LoadConfig()
{

}

void ConfigData::SaveConfig() const
{

}

MenuSettingsData &ConfigData::MenuSettings()
{
    return m_menu_settings_data;
}

const SettingsData &ConfigData::GetGeneralSettings() const
{
    return m_settings_data;
}

void ConfigData::SetGeneralSettings(const SettingsData &data)
{
    m_settings_data = data;
}

const EditSettingData &ConfigData::GetEditSettings() const
{
    return m_edit_settings_data;
}

void ConfigData::SetEditSettings(const EditSettingData &data)
{
    m_edit_settings_data = data;
}

const CUserDefinedLanguageStyle &ConfigData::GetLanguageSettings() const
{
    return m_lanugage_settings_data;
}

void ConfigData::SetLanguageSettings(const CUserDefinedLanguageStyle &data)
{
    m_lanugage_settings_data = data;
}
