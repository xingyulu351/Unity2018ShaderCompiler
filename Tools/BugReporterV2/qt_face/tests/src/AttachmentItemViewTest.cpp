#include "AttachmentItemView.h"
#include "attachment/Attachment.h"
#include "attachment/AttachmentProperties.h"
#include "AttachmentDummy.h"
#include "AttachmentSpy.h"
#include "AttachmentPropertyStub.h"
#include "shims/logical/IsEmpty.h"
#include "shims/attribute/GetSize.h"
#include "shims/attribute/GetRawPtr.h"

#include <UnitTest++.h>
#include <stdexcept>
#include <exception>
#include <memory>
#include <vector>
#include <sstream>

SUITE(AttachmentItemView)
{
    using namespace ureport;
    using namespace ureport::ui;
    using namespace ureport::test;

    TEST(Constructor_GivenNullAttachment_ThrowsInvalidArgument)
    {
        CHECK_THROW(AttachmentItemView(nullptr), std::invalid_argument);
    }

    TEST(GetSize_GivenAttachment_GetsAttachmentsSizeProperty)
    {
        auto attachment = std::make_shared<AttachmentSpy>();
        AttachmentItemView view(attachment);
        view.GetSize();
        CHECK_EQUAL(attachment::kSize, attachment->m_LastPropertyAsked);
    }

    TEST(MakeItems_GivenEmptySetOfAttachments_ReturnsEmptySet)
    {
        std::vector<std::shared_ptr<Attachment> > attachments;
        auto items = AttachmentItemView::MakeItems(attachments);
        CHECK(IsEmpty(items));
    }

    TEST(MakeItems_GivenNonEmptySetOfAttachments_ReturnsTheSameAmountOfItems)
    {
        std::vector<std::shared_ptr<Attachment> > attachments;
        attachments.push_back(std::make_shared<AttachmentDummy>());
        attachments.push_back(std::make_shared<AttachmentDummy>());
        auto items = AttachmentItemView::MakeItems(attachments);
        CHECK_EQUAL(GetSize(attachments), GetSize(items));
    }

    TEST(GetAttachment_ForViewConstructedFromNotNullAttachment_ReturnsReferenceToAttachment)
    {
        auto attachment = std::make_shared<AttachmentDummy>();
        AttachmentItemView view(attachment);
        CHECK_EQUAL(GetRawPtr(attachment), GetRawPtr(view.GetAttachment()));
    }

    class AnAttachmentView
    {
    public:
        AnAttachmentView()
            : m_Attachment(std::make_shared<AttachmentPropertyStub>())
            , m_View(m_Attachment)
        {
        }

        void SetAttachmentSize(size_t size)
        {
            std::stringstream property;
            property << size;
            SetAttachmentSize(property.str());
        }

        void SetAttachmentSize(const std::string& size)
        {
            m_Attachment->m_Property = size;
        }

    public:
        std::shared_ptr<AttachmentPropertyStub> m_Attachment;
        AttachmentItemView m_View;
    };

    TEST_FIXTURE(AnAttachmentView, GetSize_GivenAttachmentWithEmptySize_ReturnsEmptyString)
    {
        m_Attachment->m_Property = "";
        CHECK_EQUAL("", m_View.GetSize());
    }

    TEST_FIXTURE(AnAttachmentView, GetSize_GivenAttachmentOf0BytesSize_Returns0bytes)
    {
        m_Attachment->m_Property = "0";
        CHECK_EQUAL("0 bytes", m_View.GetSize());
    }

    TEST_FIXTURE(AnAttachmentView, GetSize_GivenAttachmentOf100BytesSize_Returns100bytes)
    {
        m_Attachment->m_Property = "100";
        CHECK_EQUAL("100 bytes", m_View.GetSize());
    }

    TEST_FIXTURE(AnAttachmentView, GetSize_GivenAttachmentOf1024BytesSize_Returns1Kb)
    {
        m_Attachment->m_Property = "1024";
        CHECK_EQUAL("1 Kb", m_View.GetSize());
    }

    TEST_FIXTURE(AnAttachmentView, GetSize_GivenAttachmentOf2049BytesSize_Returns2Kb)
    {
        m_Attachment->m_Property = "2049";
        CHECK_EQUAL("2 Kb", m_View.GetSize());
    }

    TEST_FIXTURE(AnAttachmentView, GetSize_GivenAttachmentOf1024x1024BytesSize_Returns1Mb)
    {
        SetAttachmentSize(1024 * 1024);
        CHECK_EQUAL("1 Mb", m_View.GetSize());
    }

    TEST_FIXTURE(AnAttachmentView, GetSize_GivenAttachmentOf1024x1024x2point555BytesSize_Returns2point6Mb)
    {
        SetAttachmentSize(size_t(1024 * 1024 * 2.555));
        CHECK_EQUAL("2.6 Mb", m_View.GetSize());
    }

    TEST_FIXTURE(AnAttachmentView, GetSize_GivenAttachmentOf1024x1024x1024BytesSize_Returns1Gb)
    {
        SetAttachmentSize(1024 * 1024 * 1024);
        CHECK_EQUAL("1 Gb", m_View.GetSize());
    }

    TEST_FIXTURE(AnAttachmentView, GetSize_GivenAttachmentOf1024x1024x1024x2point24BytesSize_Returns2point2Gb)
    {
        SetAttachmentSize(size_t(1024 * 1024 * 1024 * 2.24));
        CHECK_EQUAL("2.2 Gb", m_View.GetSize());
    }

    TEST_FIXTURE(AnAttachmentView, GetSize_RealityCheck1)
    {
        SetAttachmentSize(size_t(10.5 * 1024));
        CHECK_EQUAL("10 Kb", m_View.GetSize());
    }

    TEST_FIXTURE(AnAttachmentView, GetSize_RealityCheck2)
    {
        SetAttachmentSize(size_t(255.55555 * 1024 * 1024));
        CHECK_EQUAL("255.6 Mb", m_View.GetSize());
    }

    TEST_FIXTURE(AnAttachmentView, GetSize_RealityCheck3)
    {
        SetAttachmentSize(size_t(10.5 * 1024 * 1024));
        CHECK_EQUAL("10.5 Mb", m_View.GetSize());
    }

    TEST_FIXTURE(AnAttachmentView, GetSizeInBytes_GivenAttachmentWithEmptySize_ReturnsZero)
    {
        SetAttachmentSize("");
        CHECK_EQUAL(0, m_View.GetSizeInBytes());
    }

    TEST_FIXTURE(AnAttachmentView, GetSizeInBytes_GivenAttachmentOf1ByteSize_ReturnsOne)
    {
        SetAttachmentSize(1);
        CHECK_EQUAL(1, m_View.GetSizeInBytes());
    }

    TEST_FIXTURE(AnAttachmentView, GetSizeInBytes_GivenAttachmentOf1024BytesSize_Returns1024)
    {
        SetAttachmentSize(1024);
        CHECK_EQUAL(1024, m_View.GetSizeInBytes());
    }

    TEST_FIXTURE(AnAttachmentView, GetSizeInBytes_GivenAttachmentOf1024x1024x1024x2point24BytesSize_Returns1024x1024x1024x2point24)
    {
        auto size = size_t(1024 * 1024 * 1024 * 2.24);
        SetAttachmentSize(size);
        CHECK_EQUAL(size, m_View.GetSizeInBytes());
    }

    TEST(GetSizeAndSizeInBytes_CalledMultipleGivenAttachementWithNonEmptySize_GetsSizePropertyOnce)
    {
        auto attachment = std::make_shared<AttachmentSpy>();
        attachment->m_PropertyValue = "0";
        AttachmentItemView view(attachment);
        view.GetSize();
        view.GetSizeInBytes();
        view.GetSize();
        view.GetSizeInBytes();
        CHECK_EQUAL(1, attachment->m_CallsNumber);
    }

    TEST_FIXTURE(AnAttachmentView, GetName_GivenAttachmentWithNonEmptyName_ReturnsAttachmentName)
    {
        m_Attachment->m_Property = "name";
        CHECK_EQUAL("name", m_View.GetName());
    }
}
