#include "file_system/FileSystem.h"

#include <string>

#include <UnitTest++.h>

#include <QDir>
#include <QFile>

SUITE(QtBasedFileSystem)
{
    using namespace ureport;

    class QtBasedFileSystemTester
    {
    public:
        QtBasedFileSystemTester()
            : m_FileSystem(CreateFileSystem())
        {
            CreateTempDir();
            InitTempDir();
        }

        ~QtBasedFileSystemTester()
        {
            DeleteTempDir();
        }

        size_t WriteToTxtFile(const std::string& str)
        {
            QFile txtFile(m_TxtFilePath);
            txtFile.open(QIODevice::WriteOnly);
            auto bytes_written = txtFile.write(str.c_str());
            txtFile.close();
            return bytes_written;
        }

        void CreateTempDir()
        {
            m_TmpDirPath = QDir::tempPath() + "/UnityBugReporterTestDir";
            QDir tempDir(m_TmpDirPath);
            if (!tempDir.exists())
                tempDir.mkdir(m_TmpDirPath);
        }

        void InitTempDir()
        {
            m_LinkPath = m_TmpDirPath + "/tmpLink.lnk";
            QFile::link(QDir::rootPath(), m_LinkPath);

            m_TxtFilePath = m_TmpDirPath + "/test.txt";
            QFile txtFile(m_TxtFilePath);
            txtFile.open(QIODevice::WriteOnly);
            txtFile.close();
        }

        void DeleteTempDir()
        {
            QDir tempDir(m_TmpDirPath);
            tempDir.removeRecursively();
        }

        size_t CreateSubDirInTempWithTxtFile(const std::string& str)
        {
            QDir dir(m_TmpDirPath);
            dir.mkdir("test_dir");
            QFile txtFile(m_TmpDirPath + "/test_dir/test2.txt");
            txtFile.open(QIODevice::WriteOnly);
            auto bytes_written = txtFile.write(str.c_str());
            txtFile.close();
            return bytes_written;
        }

        std::shared_ptr<FileSystem> m_FileSystem;
        QString m_TmpDirPath;
        QString m_LinkPath;
        QString m_TxtFilePath;
    };

    TEST_FIXTURE(QtBasedFileSystemTester, GetPathDepth_GivenEmpty_ReturnsZero)
    {
        CHECK_EQUAL(0, m_FileSystem->GetPathDepth(""));
    }

    TEST_FIXTURE(QtBasedFileSystemTester, GetPathDepth_GivenRoot_ReturnsOne)
    {
        CHECK_EQUAL(0, m_FileSystem->GetPathDepth(QDir::rootPath().toStdString()));
    }

    TEST_FIXTURE(QtBasedFileSystemTester, GetPathDepth_GivenDepthTwo_ReturnsTwo)
    {
        CHECK_EQUAL(2, m_FileSystem->GetPathDepth("C:/Program Files/Unity"));
    }

    TEST_FIXTURE(QtBasedFileSystemTester, GetPathDepth_DoNotCountTrailingSlash)
    {
        CHECK_EQUAL(m_FileSystem->GetPathDepth("C:/Program Files/Unity/"),
            m_FileSystem->GetPathDepth("C:/Program Files/Unity"));
    }

    TEST_FIXTURE(QtBasedFileSystemTester, GetPathDepth_GivenDepthThreeWithFile_ReturnsThree)
    {
        CHECK_EQUAL(3, m_FileSystem->GetPathDepth("C:/Program Files/Unity/test.txt"));
    }

    TEST_FIXTURE(QtBasedFileSystemTester, GetPathDepth_GivenLinkToRoot_ReturnsZero)
    {
        CHECK_EQUAL(0, m_FileSystem->GetPathDepth(m_LinkPath.toStdString()));
    }

    TEST_FIXTURE(QtBasedFileSystemTester, GetPathDepth_GivenTxtFileInTempDir_ReturnsTempDirDepthPlusOne)
    {
        CHECK_EQUAL(m_FileSystem->GetPathDepth(m_TmpDirPath.toStdString()) + 1,
            m_FileSystem->GetPathDepth(m_TxtFilePath.toStdString()));
    }

    TEST_FIXTURE(QtBasedFileSystemTester, GetSize_GivenEmptyFile_ReturnsZero)
    {
        CHECK_EQUAL(0, m_FileSystem->GetSize(m_TxtFilePath.toStdString()));
    }

    TEST_FIXTURE(QtBasedFileSystemTester, GetSize_GivenFileWithNBytes_ReturnsN)
    {
        auto written = WriteToTxtFile("TEST");
        CHECK_EQUAL(written, m_FileSystem->GetSize(m_TxtFilePath.toStdString()));
    }

    TEST_FIXTURE(QtBasedFileSystemTester, GetSize_GivenFileWithN2Bytes_ReturnsN2)
    {
        std::string str = "";
        for (int i = 0; i < 1000; ++i)
            str += "TEST ";
        auto written = WriteToTxtFile(str);
        CHECK_EQUAL(written, m_FileSystem->GetSize(m_TxtFilePath.toStdString()));
    }

    TEST_FIXTURE(QtBasedFileSystemTester, GetSize_GivenEmptyDir_ReturnsZero)
    {
        DeleteTempDir();
        CreateTempDir();
        CHECK_EQUAL(0, m_FileSystem->GetSize(m_TmpDirPath.toStdString()));
    }

    TEST_FIXTURE(QtBasedFileSystemTester, GetSize_GivenDirWithFile_ReturnsFileSize)
    {
        DeleteTempDir();
        CreateTempDir();
        auto written = WriteToTxtFile("Sample text");
        CHECK_EQUAL(written, m_FileSystem->GetSize(m_TmpDirPath.toStdString()));
    }

    TEST_FIXTURE(QtBasedFileSystemTester, GetSize_GivenDirWithSubDirs_ReturnsTotalSize)
    {
        DeleteTempDir();
        CreateTempDir();
        auto written = CreateSubDirInTempWithTxtFile("Test");
        written += WriteToTxtFile("Sample text");
        CHECK_EQUAL(written, m_FileSystem->GetSize(m_TmpDirPath.toStdString()));
    }
}
