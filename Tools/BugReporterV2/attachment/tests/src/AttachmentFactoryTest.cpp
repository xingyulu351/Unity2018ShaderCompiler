#include "attachment/AttachmentFactory.h"
#include "attachment/Attachment.h"
#include "file_system/FileSystem.h"
#include "file_system/FileGateway.h"
#include "shims/logical/IsNull.h"

#include <UnitTest++.h>

SUITE(AttachmentFactory)
{
    using namespace ureport;

    TEST(CanCreateTextAttachment)
    {
        auto const factory = CreateAttachmentFactory(nullptr, nullptr);
        auto const attachment = factory->CreateTextAttachment("text", "description");
        CHECK(!IsNull(attachment));
    }
}
