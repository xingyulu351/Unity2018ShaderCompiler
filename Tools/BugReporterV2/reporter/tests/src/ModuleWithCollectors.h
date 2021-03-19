#pragma once

#include "../src/Module.h"
#include "../src/Collector.h"

#include <vector>
#include <algorithm>

namespace ureport
{
namespace test
{
    class ModuleWithCollectors : public ureport::Module
    {
        typedef std::vector<std::shared_ptr<Collector> > CollectorsVector;

        CollectorsVector m_Collectors;

    public:
        template<unsigned int N>
        ModuleWithCollectors(std::shared_ptr<Collector>(&collectors)[N])
            : m_Collectors(std::vector<std::shared_ptr<Collector> >(&collectors[0], &collectors[N]))
        {
        }

        void Initialize(Workspace& workspace)
        {
            std::for_each(m_Collectors.begin(), m_Collectors.end(), [&] (std::shared_ptr<Collector>& collector) {
                workspace.RegisterCollector(collector);
            });
        }
    };
}
}
