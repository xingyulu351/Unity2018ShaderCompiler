#include "FilesPackerDummy.h"
#include "TextAttachment.h"
#include "attachment_preview/AttachmentTextPreview.h"
#include "file_system/File.h"
#include "shims/logical/IsNull.h"
#include "shims/logical/IsEmpty.h"
#include "shims/attribute/GetSize.h"

#include <UnitTest++.h>
#include <iterator>

SUITE(TextAttachment)
{
    using namespace ureport;
    using namespace ureport::test;

    class ATextAttachment
    {
    public:
        FilesPackerDummy m_Packer;
    };

    TEST(HasPreview_WhenCreated)
    {
        auto const attachment = std::make_shared<TextAttachment>("", "");
        CHECK(!IsNull(attachment->GetTextPreview()));
    }

    TEST(GetNameFromPreview_ReturnsNameOfTheAttachment)
    {
        auto const attachment = std::make_shared<TextAttachment>("name", "");
        CHECK_EQUAL("name", attachment->GetTextPreview()->GetName());
    }

    TEST(GetContentFromPreview_GivenOneLineText_ReturnsTheLine)
    {
        auto const attachment = std::make_shared<TextAttachment>("", "line");
        CHECK_EQUAL("line", attachment->GetTextPreview()->GetContent().front());
    }

    TEST(GetContentFromPreview_GivenTwoLinesText_ReturnsTheLine)
    {
        auto const attachment = std::make_shared<TextAttachment>("", "first\nsecond");
        auto const content = attachment->GetTextPreview()->GetContent();
        CHECK_EQUAL(2, GetSize(content));
        CHECK_EQUAL("first", content.front());
        CHECK_EQUAL("second", *(++content.begin()));
    }

    TEST(GetContentFromPreview_GivenNoText_ReturnsEmptySequence)
    {
        auto const attachment = std::make_shared<TextAttachment>("", "");
        CHECK(IsEmpty(attachment->GetTextPreview()->GetContent()));
    }

    TEST_FIXTURE(ATextAttachment, Pack_WhenCreated_ReturnsNotNullFile)
    {
        auto const attachment = std::make_shared<TextAttachment>("", "");
        CHECK(!IsNull(attachment->Pack(m_Packer, nullptr)));
    }

    TEST_FIXTURE(ATextAttachment, GetPathFromFile_ReturnsNameOfTheAttachment)
    {
        auto const attachment = std::make_shared<TextAttachment>("name", "");
        CHECK_EQUAL("name", attachment->Pack(m_Packer, nullptr)->GetPath());
    }

    TEST_FIXTURE(ATextAttachment, ReadFromFile_ReturnsStreamWithTheText)
    {
        auto const attachment = std::make_shared<TextAttachment>("", "text");
        auto const file = attachment->Pack(m_Packer, nullptr);
        const std::string readText(std::istream_iterator<char>(*file->Read()),
                                   std::istream_iterator<char>());
        CHECK_EQUAL("text", readText);
    }
}
