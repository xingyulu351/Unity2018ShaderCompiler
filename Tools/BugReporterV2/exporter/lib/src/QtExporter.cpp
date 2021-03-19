#include "QtExporterImpl.h"
#include "JsonReportCollector.h"

namespace ureport
{
    std::unique_ptr<Exporter> CreateQtExporter()
    {
        return std::unique_ptr<Exporter>(new details::QtExporter());
    }

    std::unique_ptr<unity_collectors::JsonReportCollector> CreateJsonReportCollector()
    {
        return std::unique_ptr<unity_collectors::JsonReportCollector>(new unity_collectors::JsonReportCollector());
    }
}
