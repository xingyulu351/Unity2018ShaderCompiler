#include "QtSenderImpl.h"

namespace ureport
{
    std::unique_ptr<Sender> CreateQtSender()
    {
        return std::unique_ptr<Sender>(new details::QtSender());
    }
}
