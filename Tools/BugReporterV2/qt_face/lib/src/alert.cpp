#include "alert.h"
#include <QDebug>
#include <array>
#include <QStylePainter>
#include "qformlayout.h"

namespace ureport
{
namespace ui
{
    const QString Alert::NORMAL_STYLE = "border: 1px solid; border-radius: 4px; padding: 5px; color: #31708f; background-color: #d9edf7; border-color: #bce8f1;";
    const QString Alert::LINK_STYLE_SECTION = "<head><style>a:link {color: #31708f}</style></head>";
    const QString Alert::RATING_TOOLTIP_TEXT = "We prioritize bug reports according to their rating.\nHigher rated bugs are more likely to be checked.";

    const QString evaluationRed = "rgb(200,70,73);";
    const QString evaluationGrey = "rgb(90,90,93);";
    const QColor red = QColor(244, 67, 54);
    const QColor orange = QColor(255, 152, 0);
    const QColor yellow = QColor(255, 235, 59);
    const QColor lime = QColor(205, 220, 57);
    const QColor green = QColor(138, 194, 73);
    const int ratingBarSize = 100;

    Alert::Alert(QLabel* misc, QLabel* rating, QFormLayout* form)
    {
        m_MiscLabel = misc;
        m_MiscLabel->hide();
        m_MiscLabel->setStyleSheet(NORMAL_STYLE);
        m_MiscLabel->setOpenExternalLinks(true);
        m_RatingLabel = rating;
        PaintRating(0);
        m_Form = form;
    }

    void Alert::SetFeedbackLabel(QLabel* label, std::string name)
    {
        label->hide();
        AdjustLabelStyle(label);
        m_FeedbackLabels[name] = label;
    }

    void Alert::AdjustLabelStyle(QLabel* label)
    {
        QFont font = label->font();
        if (font.pointSize() > 8)
            font.setPointSize(font.pointSize() - 1);
        label->setFont(font);
        label->setStyleSheet("Color: " + evaluationGrey);
    }

    void Alert::RegenerateFromTasks(std::map<std::string, Feedback> allFeedback)
    {
        std::map<std::string, QDomElement> sections;
        for (auto it = allFeedback.begin(); it != allFeedback.end(); it++)
        {
            if (it->first == "Rating")
            {
                int parsedRating = std::stoi(it->second.contents);
                m_RatingLabel->setToolTip(QString::fromStdString(it->second.contents));
                PaintRating(parsedRating);
                continue;
            }
            if (it->first == "ScoreComponents")
            {
                m_RatingLabel->setToolTip(QString::fromStdString(it->second.contents));
                continue;
            }
            if (m_FeedbackLabels.count(it->first) != 0)
            {
                if (it->first == "Email")
                {
                    if (it->second.contents.empty())
                    {
                        m_Form->removeWidget(m_FeedbackLabels[it->first]);
                    }
                    else
                    {
                        m_Form->insertRow(3, nullptr, m_FeedbackLabels[it->first]);
                    }
                }
                if (it->second.contents.empty())
                {
                    m_FeedbackLabels[it->first]->hide();
                    continue;
                }
                m_FeedbackLabels[it->first]->setText(QString::fromStdString(it->second.contents));
                auto labelColor = evaluationGrey;
                if (it->second.severity > 1)
                    labelColor = evaluationRed;
                m_FeedbackLabels[it->first]->setStyleSheet("Color: " + labelColor);
                m_FeedbackLabels[it->first]->show();
            }
            else if (it->first == "Misc")
            {
                if (it->second.contents.empty())
                {
                    m_MiscLabel->hide();
                    continue;
                }
                m_MiscLabel->setText(QString::fromStdString("<html>" + LINK_STYLE_SECTION.toStdString() + "<body>" + it->second.contents + "</body></html>"));
                m_MiscLabel->show();
            }
        }
    }

    void Alert::PaintRating(int rating)
    {
        QPixmap pix(516, 10);
        pix.fill(QColor("transparent"));
        QPainter painter(&pix);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
        painter.setBrush(QBrush(PickColorForRating(rating)));
        painter.setPen(QPen(Qt::NoPen));
        for (int i = 0; i < 5; i++)
        {
            if (i == rating)
                painter.setBrush(QBrush(Qt::gray));
            if (i == 0)
            {
                QRect rec(0, 0, 10, 10);
                painter.drawPie(rec, 90 * 16, 180 * 16);
            }
            if (i == 4)
            {
                QRect rec((i + 1) * ratingBarSize + i * 1, 0, 10, 10);
                painter.drawPie(rec, 270 * 16, 180 * 16);
            }
            painter.drawRect((ratingBarSize * i + 1 * i) + 5, 0, ratingBarSize, 10);
        }
        m_RatingLabel->setPixmap(pix);
    }

    QColor Alert::PickColorForRating(int rating)
    {
        switch (rating)
        {
            case 1:
                return red;
            case 2:
                return orange;
            case 3:
                return yellow;
            case 4:
                return lime;
            default:
                return green;
        }
    }
}
}
