#pragma once

#include "reporter/ReportDraft.h"

#include <vector>

namespace ureport
{
namespace test
{
    class ReportFactSpy : public ureport::ReportDraft
    {
    public:
        std::vector<std::string> m_Facts;

        std::string GetTitle() const
        {
            return std::string();
        }

        void SetTitle(const std::string& title)
        {
        }

        std::string GetTextualDescription() const
        {
            return std::string();
        }

        void SetTextualDescription(const std::string& text)
        {
        }

        std::string GetReporterEmail() const
        {
            return std::string();
        }

        void SetReporterEmail(const std::string& email)
        {
        }

        std::string ReadFact(const FactName& name) const
        {
            return std::string();
        }

        void WriteFact(const FactName& name, const Fact& fact)
        {
            m_Facts.push_back(name);
        }
    };
}
}
