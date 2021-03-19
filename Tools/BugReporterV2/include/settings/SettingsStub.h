#pragma once

#include "settings/Settings.h"
#include "settings/WindowSettings.h"

namespace ureport
{
namespace test
{
    class SettingsStub : public ureport::Settings
    {
    public:
        std::string GetCustomerEmail() const
        {
            return m_Email;
        }

        void SetCustomerEmail(const std::string& email)
        {
            m_Email = email;
        }

        std::string GetPublicity() const
        {
            return m_Publicity;
        }

        void SetPublicity(const std::string& publicity)
        {
            m_Publicity = publicity;
        }

        std::string GetServerUrl()
        {
            return "localhost";
        }

        void SetServerUrl(const std::string& serverUrl)
        {
        }

        void SetWindowState(const WindowSettings& WindowSettings)
        {
        }

        WindowSettings GetWindowState(const std::string& windowName)
        {
            std::vector<char> state;
            return WindowSettings("", state);
        }

        std::string GetSendUrl()
        {
            return m_SendUrl;
        }

    public:
        std::string m_Email;
        std::string m_Publicity;
        std::string m_SendUrl;
    };

    const std::shared_ptr<test::SettingsStub> m_SettingsStub = std::make_shared<test::SettingsStub>();
    std::shared_ptr<test::SettingsStub> GetSettingsStub()
    {
        return m_SettingsStub;
    }
}

    std::shared_ptr<Settings> GetSettings()
    {
        return test::m_SettingsStub;
    }
}
