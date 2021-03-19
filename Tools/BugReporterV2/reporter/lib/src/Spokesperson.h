#pragma once

#include <string>

namespace ureport
{
    class Spokesperson
    {
        std::string m_Email;

    public:
        std::string GetEmail() const
        {
            return m_Email;
        }

        void SetEmail(const std::string email)
        {
            m_Email = email;
        }
    };
}
