#include <QObject>
#include <QLabel>
#include "qformlayout.h"
#include <QtXml/QDomDocument>
#include "feedback/Feedback.h"
#include <set>

namespace ureport
{
namespace ui
{
    class Alert
    {
    public:
        static const QString NORMAL_STYLE;
        static const QString LINK_STYLE_SECTION;
        static const QString RATING_TOOLTIP_TEXT;

        Alert(QLabel* misc, QLabel* rating, QFormLayout* form);

        enum AlertLevel { Normal, Warning, Error, Success };

        void SetFeedbackLabel(QLabel *label, std::string name);
        void RegenerateFromTasks(std::map<std::string, Feedback> allFeedback);

    private:
        void AdjustLabelStyle(QLabel * label);
        void PaintRating(int rating);
        QColor PickColorForRating(int rating);

        static const QString m_defaultListStyle;
        QLabel* m_MiscLabel;
        QLabel* m_RatingLabel;
        QFormLayout* m_Form;
        std::map<std::string, QLabel*> m_FeedbackLabels;
    };
}
}
