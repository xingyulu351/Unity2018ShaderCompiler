#pragma once

#include "reporter/Exporter.h"
#include "JsonReportCollector.h"

#include <memory>

namespace ureport
{
    std::unique_ptr<Exporter> CreateQtExporter();

    std::unique_ptr<unity_collectors::JsonReportCollector> CreateJsonReportCollector();
}
