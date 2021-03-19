#pragma once

#include "reporter/Exporter.h"
#include "reporter/Report.h"
#include "reporter/UnityBugMessage.h"
#include "long_term_operation/LongTermOperObserver.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>

#include <memory>
#include <stdexcept>
#include <limits>

namespace ureport
{
namespace details
{
    class QtExporter : public Exporter
    {
    public:
        QtExporter()
        {
        }

        void Save(const UnityBugMessage& message, LongTermOperObserver* observer, const std::string& path)
        {
            QString source = QString::fromStdString(message.GetAttachedFilePath());
            QString dest = QString::fromStdString(path);
            if (!source.isEmpty() && !dest.isEmpty())
            {
                if (QFile::exists(dest))
                {
                    QFile::remove(dest);
                }
                QFile::copy(source, dest);
            }
        }
    };
}
}
