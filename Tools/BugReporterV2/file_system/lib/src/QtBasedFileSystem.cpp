#include "FileSystem.h"
#include "DirEntry.h"
#include "FileEntry.h"
#include "EmptyPathException.h"
#include "PathDoesNotExistException.h"
#include "shims/logical/IsEmpty.h"

#include <QFileInfo>
#include <QString>
#include <QDirIterator>
#include <memory>

namespace ureport
{
namespace details
{
    class QtBasedFileSystem : public ureport::FileSystem
    {
        bool HasPath(const std::string& path) const
        {
            QFileInfo info(QString::fromStdString(ResolvePath(path)));
            return info.exists();
        }

        std::list<std::string> GetDirEntries(const std::string& path,
            const std::string& filter) const
        {
            if (IsEmpty(path))
                throw EmptyPathException();
            if (!HasPath(path))
                throw PathDoesNotExistException(path);
            std::list<std::string> entries;
            auto qfilter(QString::fromStdString(filter));
            QDirIterator it(QString::fromStdString(ResolvePath(path)),
                            QDir::Files | QDir::Dirs | QDir::Hidden | QDir::NoDotAndDotDot,
                            QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);
            while (it.hasNext())
            {
                it.next();
                auto fileInfo = it.fileInfo();
                if (qfilter.isEmpty() || (0 == fileInfo.fileName().compare(
                    qfilter, Qt::CaseInsensitive)))
                {
                    entries.push_back(fileInfo.absoluteFilePath().toStdString());
                }
            }
            return entries;
        }

        bool IsDir(const std::string& path) const
        {
            return QFileInfo(QString::fromStdString(ResolvePath(path))).isDir();
        }

        std::string ResolvePath(const std::string& rawPath) const
        {
            if (IsEmpty(rawPath))
                return rawPath;
            auto path = rawPath;
            if (IsUserRelativePath(path))
                path = QDir::homePath().toStdString() + path.substr(1);
            return path;
        }

        bool IsUserRelativePath(const std::string& path) const
        {
            return path.at(0) == '~';
        }

        size_t GetPathDepth(const std::string& path) const override
        {
            if (path.empty())
                return 0;
            auto cleanPath = CleanPath(path);
            if (cleanPath == QDir::rootPath())
                return 0;
            else
            {
                auto count = cleanPath.count('/');
                return count;
            }
        }

        size_t GetSize(const std::string& path) const override
        {
            auto qPath = CleanPath(path);
            QFileInfo pathInfo(qPath);
            size_t size = 0;
            if (pathInfo.isFile())
                return GetFileSize(pathInfo);
            else if (pathInfo.isDir())
                return GetDirectorySize(qPath);
            return 0;
        }

    private:
        size_t GetFileSize(const QFileInfo& pathInfo) const
        {
            return pathInfo.size();
        }

        size_t GetDirectorySize(const QString& path) const
        {
            size_t size = 0;
            QDir dir(path);
            QFileInfoList entries_list = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::Hidden | QDir::NoSymLinks | QDir::NoDotAndDotDot);
            for (int i = 0; i < entries_list.size(); ++i)
            {
                QFileInfo info = entries_list.at(i);
                if (info.isDir())
                    size += GetSize(info.absoluteFilePath().toStdString());
                else
                    size += info.size();
            }

            return size;
        }

        QString CleanPath(const std::string& path) const
        {
            QString qPath = QString::fromStdString(path);
            qPath = QDir::cleanPath(qPath);
            QFileInfo pathInfo(qPath);
            if (pathInfo.exists())
            {
                qPath = pathInfo.canonicalFilePath();
            }
            return qPath;
        }
    };
}

    std::unique_ptr<FileSystem> CreateFileSystem()
    {
        return std::unique_ptr<FileSystem>(new details::QtBasedFileSystem());
    }
}
