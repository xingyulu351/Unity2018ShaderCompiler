#pragma once

#include "reporter/Sender.h"

#include <memory>

namespace ureport
{
    std::unique_ptr<Sender> CreateQtSender();
}
