#pragma once

#include <memory>

namespace ureport
{
    class AttachmentFactory;

namespace launcher
{
    std::unique_ptr<ureport::AttachmentFactory> CreateExternalAttachmentFactory();
}
}
