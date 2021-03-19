#pragma once

#include <QObject>

namespace ureport
{
namespace ui
{
    class SignalBlocker
    {
        std::vector<QObject*> m_Blocked;

    public:
        void Block(QObject* object)
        {
            object->blockSignals(true);
            m_Blocked.push_back(object);
        }

        ~SignalBlocker()
        {
            std::for_each(m_Blocked.begin(), m_Blocked.end(), [] (QObject* object)
            {
                object->blockSignals(false);
            });
        }
    };
}
}
