#pragma once

#include "AttachmentDummy.h"

namespace ureport
{
namespace test
{
    class AttachmentPropertyStub : public AttachmentDummy
    {
    public:
        std::string GetProperty(const std::string& name) const override
        {
            return m_Property;
        }

    public:
        std::string m_Property;
    };
}
}
