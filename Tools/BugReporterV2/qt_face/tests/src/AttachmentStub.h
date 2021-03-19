#pragma once

#include "AttachmentDummy.h"

#include <string>

namespace ureport
{
namespace test
{
    class AttachmentStub : public AttachmentDummy
    {
    public:
        AttachmentStub(std::string nameProperyValue) : m_NameProperyValue(nameProperyValue)
        {
        }

        std::string GetProperty(const std::string& name) const override
        {
            return m_NameProperyValue;
        }

    private:
        std::string m_NameProperyValue;
    };
}
}
