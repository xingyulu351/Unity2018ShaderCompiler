#pragma once

#include "reporter/Collector.h"
#include "reporter/ReportDraft.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>

#include <memory>

namespace ureport
{
namespace unity_collectors
{
    class JsonReportCollector : public ureport::Collector
    {
    public:
        void Collect(ReportDraft& report) const
        {
            QJsonDocument doc;
            QJsonObject obj;
            obj.insert("title", QJsonValue(QString::fromStdString(report.GetTitle())));
            obj.insert("description", QJsonValue(QString::fromStdString(report.GetTextualDescription())));
            obj.insert("email", QJsonValue(QString::fromStdString(report.GetReporterEmail())));
            obj.insert("version", QJsonValue(QString::fromStdString(report.ReadFact("UnityVersion"))));
            obj.insert("editorMode", QJsonValue(QString::fromStdString(report.ReadFact("UnityEditorMode"))));
            obj.insert("osName", QJsonValue(QString::fromStdString(report.ReadFact("OSName"))));
            obj.insert("osVersion", QJsonValue(QString::fromStdString(report.ReadFact("OSVersion"))));
            obj.insert("reportType", QJsonValue(report.GetBugTypeID()));
            obj.insert("repro", QJsonValue(report.GetBugReproducibility()));
            doc.setObject(obj);
            std::string data(doc.toJson().constData());
            report.WriteFact("JsonReport", data);
        }
    };
}
}
