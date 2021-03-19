#pragma once

#include "AttachmentDummy.h"

#include <string>

namespace ureport
{
namespace test
{
    class AttachmentSpy : public AttachmentDummy
    {
    public:
        AttachmentSpy()
            : m_CallsNumber(0)
        {
        }

        std::string GetProperty(const std::string& name) const override
        {
            m_LastPropertyAsked = name;
            ++m_CallsNumber;
            return m_PropertyValue;
        }

    public:
        std::string m_PropertyValue;
        mutable std::string m_LastPropertyAsked;
        mutable size_t m_CallsNumber;
    };
}
}
