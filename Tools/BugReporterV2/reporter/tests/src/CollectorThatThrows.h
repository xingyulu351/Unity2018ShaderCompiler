#pragma once

#include <exception>

namespace ureport
{
namespace test
{
    class CollectorThatThrows : public Collector
    {
        void Collect(ReportDraft& report) const
        {
            throw std::exception();
        }
    };
}
}
