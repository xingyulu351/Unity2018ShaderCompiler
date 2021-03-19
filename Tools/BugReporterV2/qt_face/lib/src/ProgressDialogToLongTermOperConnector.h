#pragma once

#include "long_term_operation/LongTermOperController.h"
#include "long_term_operation/LongTermOperProgressListener.h"

#include <QProgressDialog>

namespace Ui
{
    class ProgressDialogToLongTermOperConnector :
        public QObject, public ureport::LongTermOperProgressListener
    {
        Q_OBJECT;

    public:
        ProgressDialogToLongTermOperConnector(ureport::LongTermOperController& controller,
                                              QProgressDialog& dialog)
            : m_Controller(controller)
            , m_Dialog(dialog)
        {
            QObject::connect(&m_Dialog, SIGNAL(canceled()), this, SLOT(Cancel()));
            m_Controller.AddProgressListener(this);
        }

        ~ProgressDialogToLongTermOperConnector()
        {
            m_Controller.RemoveProgressListener(this);
        }

        void UpdateProgress(int current, int total)
        {
            m_Dialog.setMaximum(total);
            m_Dialog.setValue(current);
        }

        void UpdateStatus(const std::string& status)
        {
            m_Dialog.setLabelText(QString::fromStdString(status));
        }

    private slots:
        void Cancel()
        {
            m_Controller.Cancel();
        }

    private:
        ProgressDialogToLongTermOperConnector(const ProgressDialogToLongTermOperConnector&);
        ureport::LongTermOperController& m_Controller;
        QProgressDialog& m_Dialog;
    };
}
