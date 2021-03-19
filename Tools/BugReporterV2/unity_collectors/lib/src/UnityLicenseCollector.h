#pragma once

#include "reporter/Collector.h"
#include "reporter/ReportDraft.h"

#include <memory>
#include <istream>

namespace ureport
{
namespace unity_collectors
{
    class UnityLicenseCollector : public ureport::Collector
    {
    public:
        UnityLicenseCollector()
            : k_ProUserLicenseType("Pro")
            , k_FreeLicenseType("Free")
        {
        }

        virtual ~UnityLicenseCollector()
        {
        }

        void Collect(ReportDraft& report) const
        {
            ReadLicenseType();
            ReportLicenseType(report);
        }

    protected:
        virtual std::unique_ptr<std::istream> GetLicense() const
        {
            return nullptr;
        }

    private:
        void ReadLicenseType() const
        {
            std::unique_ptr<std::istream> license = GetLicense();
            std::string line;
            while (license->good() && !license->eof())
            {
                std::getline(*license, line);
                if (HasProUserMark(line))
                    m_LicenseType = k_ProUserLicenseType;
                else if (HasFreeMark(line))
                    m_LicenseType = k_FreeLicenseType;
            }
        }

        bool HasProUserMark(const std::string& text) const
        {
            return text.find("<Feature Value=\"0\"/>") != std::string::npos;
        }

        bool HasFreeMark(const std::string& text) const
        {
            return text.find("<Feature Value=\"62\"/>") != std::string::npos;
        }

        void ReportLicenseType(ReportDraft& report) const
        {
            report.WriteFact("LicenseType", m_LicenseType);
        }

    private:
        const std::string k_ProUserLicenseType;
        const std::string k_FreeLicenseType;
        mutable std::string m_LicenseType;
    };

    std::unique_ptr<UnityLicenseCollector> CreateUnityLicenseCollector();
}
}
