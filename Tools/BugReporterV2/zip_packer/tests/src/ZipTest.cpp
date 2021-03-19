#include <cstdio>
#include <sstream>
#include <UnitTest++.h>
#include <miniz_zip_ex.h>

SUITE(Zip)
{
    struct OpenFile
    {
        const char* m_Path;
        FILE* m_File;

        OpenFile()
            : m_Path("unity.bugreporter.ziptest.file.zip")
            , m_File(fopen(m_Path, "wb"))
        {
        }

        ~OpenFile()
        {
            auto const closed = fclose(m_File);
            assert(closed == 0);
            auto const deleted = std::remove(m_Path);
            assert(deleted == 0);
        }
    };

    TEST_FIXTURE(OpenFile, mz_zip_writer_init_cfile__GivenValidFile__OpensAndClosesSuccessfully)
    {
        mz_zip_archive zip = {};

        mz_bool open_succeeded = mz_zip_writer_init_cfile(&zip, m_File, MZ_ZIP_FLAG_WRITE_ZIP64);
        mz_bool end_succeeded = mz_zip_writer_end(&zip);

        CHECK_EQUAL(open_succeeded, MZ_TRUE);
        CHECK_EQUAL(end_succeeded, MZ_TRUE);
    }

    struct OpenZip : public OpenFile
    {
        mz_zip_archive m_Zip;

        OpenZip()
            : m_Zip()
        {
            mz_zip_writer_init_cfile(&m_Zip, m_File, MZ_ZIP_FLAG_WRITE_ZIP64);
        }

        ~OpenZip()
        {
            mz_zip_writer_finalize_archive(&m_Zip);
            mz_zip_writer_end(&m_Zip);
        }
    };

    TEST_FIXTURE(OpenZip, mz_zip_writer_add_istream__GivenInputIstream__ReturnsTrue)
    {
        std::string contents("contents");
        std::stringstream contents_stream(contents);

        mz_bool result = mz_zip_writer_add_istream(
            &m_Zip, "entry name", contents_stream,
            contents.size(), nullptr,
            nullptr, 0,
            MZ_ZIP_FLAG_UTF8_FILENAME,
            nullptr, 0,
            nullptr, 0);

        CHECK_EQUAL(result, MZ_TRUE);
    }
}
