#include "Phrases.h"
#include <QObject>
#include <map>

namespace ureport
{
namespace ui
{
    QString Phrases::Translate(const std::string& key)
    {
        std::map<std::string, QString> values;
        values["folder"] = QObject::tr("folder");
        values["Manually attached directory"] = QObject::tr("Manually attached directory");
        values["Unity Editor log"] = QObject::tr("Unity Editor log");
        values["Previous Unity Editor log"] = QObject::tr("Previous Unity Editor log");
        values["Standalone player log"] = QObject::tr("Standalone player log");
        values["MonoDevelop logs"] = QObject::tr("MonoDevelop logs");
        values["MonoDevelop configurations"] = QObject::tr("MonoDevelop configurations");
        values["Information about installed Unity modules"] = QObject::tr("Information about installed Unity modules");
        values["System Info"] = QObject::tr("System Info");
        values["Attachment can not be null"] = QObject::tr("Attachment can not be null");
        values["No path specified"] = QObject::tr("No path specified");
        values["Cannot attach root directory"] = QObject::tr("Cannot attach root directory");
        values["Please write a title."] = QObject::tr("Please write a title.");
        values["Please write your email address."] = QObject::tr("Please write your email address.");
        values["Please write a valid email address."] = QObject::tr("Please write a valid email address.");
        values["Please select what the problem is related to."] = QObject::tr("Please select what the problem is related to.");
        values["Please select how often the problem is happening."] = QObject::tr("Please select how often the problem is happening.");
        values["Failed to get path to temporary directory."] = QObject::tr("Failed to get path to temporary directory.");
        values["Failed to get common application data path"] = QObject::tr("Failed to get common application data path");
        values["bytes"] = QObject::tr("bytes");
        values["Kb"] = QObject::tr("Kb");
        values["Mb"] = QObject::tr("Mb");
        values["Gb"] = QObject::tr("Gb");
        values["Public"] = QObject::tr("Public");
        values["Not Public"] = QObject::tr("Not Public");

        auto value = values.find(key);

        return value != values.end() ? value->second : QString::fromStdString(key);
    }
}
}
