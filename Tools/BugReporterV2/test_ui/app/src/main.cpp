#include "HumbleComposeReportController.h"
#include "HumbleReport.h"
#include "MainWindow.h"
#include "SimilarCasesSearchFake.h"

#include <QApplication>

int main(int argc, char** argv)
{
    using namespace ureport;
    using namespace ureport::test;
    QApplication app(argc, argv);
    MainWindow window;
    window.SetReportController(std::make_shared<HumbleComposeReportController>());
    window.SetReport(std::make_shared<HumbleReport>());
    window.SetSearch(std::unique_ptr<SimilarCasesSearch>(new SimilarCasesSearchFake()));
    window.show();
    return app.exec();
}
