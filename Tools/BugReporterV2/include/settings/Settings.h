#pragma once

#include <string>
#include <memory>
#include "WindowSettings.h"

namespace ureport
{
    class Settings
    {
    public:
        virtual std::string GetCustomerEmail() const = 0;
        virtual void SetCustomerEmail(const std::string& email) = 0;
        virtual std::string GetPublicity() const = 0;
        virtual void SetPublicity(const std::string& publicity) = 0;
        virtual void SetWindowState(const WindowSettings& windowSettings) = 0;
        virtual WindowSettings GetWindowState(const std::string& windowName) = 0;
        virtual std::string GetServerUrl() = 0;
        virtual std::string GetSendUrl() = 0;
        virtual ~Settings() {}

        static const std::string DEFAULT_SERVER_URL;
    };

    std::shared_ptr<Settings> GetSettings();

namespace constants
{
    const std::string kDisabled = "DISABLED";
}
}
