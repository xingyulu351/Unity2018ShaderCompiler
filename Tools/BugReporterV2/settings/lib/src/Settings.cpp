#include "settings/Settings.h"
#include "settings/WindowSettings.h"
#include "common/CustomQtUtils.h"

#include <QByteArray>
#include <QSettings>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

namespace ureport
{
    const std::string Settings::DEFAULT_SERVER_URL = "bugservices.unity3d.com";

namespace details
{
    class Settings : public ureport::Settings
    {
    public:
        Settings() : m_Settings("Unity", "BugReporterV2")
        {
            ReadConfigFile();
        }

    private:
        std::string GetCustomerEmail() const
        {
            return m_Settings.value("CustomerEmail").toString().toStdString();
        }

        void SetCustomerEmail(const std::string& email)
        {
            m_Settings.setValue("CustomerEmail", QString::fromStdString(email));
        }

        std::string GetPublicity() const
        {
            return m_Settings.value("Publicity").toString().toStdString();
        }

        void SetPublicity(const std::string& publicity)
        {
            m_Settings.setValue("Publicity", QString::fromStdString(publicity));
        }

        std::string GetServerUrl()
        {
            auto serverUrlFromSettings = m_Settings.value("Server").toString();
            if (serverUrlFromSettings == "")
            {
                return DEFAULT_SERVER_URL;
            }
            return serverUrlFromSettings.toStdString();
        }

        void SetWindowState(const WindowSettings& windowSettings)
        {
            auto key = GenerateWindowSettingName(windowSettings.m_WindowName);
            auto state = VectorToQByteArray(windowSettings.m_WindowState);
            m_Settings.setValue(QString::fromStdString(key), state);
        }

        WindowSettings GetWindowState(const std::string& windowName)
        {
            auto key = GenerateWindowSettingName(windowName);
            auto state = m_Settings.value(QString::fromStdString(key)).toByteArray();
            auto stateVec = QByteArrayToVector(state);
            return WindowSettings(windowName, stateVec);
        }

        std::string GenerateWindowSettingName(const std::string& windowName)
        {
            return "settings_" + windowName;
        }

        std::string GetSendUrl()
        {
            return m_SendUrl;
        }

    private:
        QSettings m_Settings;
        std::string m_SendUrl;
        std::string m_serviceConfigPath;

        void ReadConfigFile()
        {
            // Temporarily change the app name to be able to use QStandardPaths
            auto appName = QCoreApplication::applicationName();

#if defined(Q_OS_LINUX)
            QCoreApplication::setApplicationName(QString("unity3d"));
#else
            QCoreApplication::setApplicationName(QString("Unity"));
#endif

            // Find config file leveraging QT's abastraction for standard paths (http://doc.qt.io/qt-5/qstandardpaths.html)
            // The actual file will be in:
            //   - C:\ProgramData\Unity (Windows)
            //   - /Library/Application Support/Unity (macOS)
            //   - /usr/share/unity3d (linux)
            auto path = QStandardPaths::locate(QStandardPaths::DataLocation, "config/services-config.json");

            // Restore app name
            QCoreApplication::setApplicationName(appName);

            if (path.isEmpty())
            {
                return;
            }

            // Open config file
            QFile configFile(path);
            if (!configFile.open(QIODevice::ReadOnly))
            {
                return;
            }

            // Parse JSON config
            QByteArray configData = configFile.readAll();
            QJsonDocument configJson(QJsonDocument::fromJson(configData));
            if (configJson.isNull())
            {
                return;
            }

            // Get submit url config
            auto submitUrl = configJson.object()["editorbugs_url"].toString();
            if (!submitUrl.isEmpty())
            {
                m_SendUrl = submitUrl.toStdString();
            }
        }
    };
}

    std::shared_ptr<Settings> GetSettings()
    {
        static auto settings = std::make_shared<details::Settings>();
        return settings;
    }
}
