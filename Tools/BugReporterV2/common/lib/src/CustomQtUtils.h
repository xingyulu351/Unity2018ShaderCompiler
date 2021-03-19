#pragma once

#include <QCoreApplication>
#include <memory>

namespace ureport
{
    inline
    std::shared_ptr<QCoreApplication> GetApplicationInstance()
    {
        QCoreApplication* app = QCoreApplication::instance();
        if (nullptr == app)
        {
            int argc = 0;
            char** argv = nullptr;
            return std::make_shared<QCoreApplication>(argc, argv);
        }
        else
        {
            return std::shared_ptr<QCoreApplication>(app, [] (QCoreApplication*) {});
        }
    }

    inline std::vector<char> QByteArrayToVector(const QByteArray& byteArray)
    {
        std::vector<char> vec;
        std::copy(byteArray.begin(), byteArray.end(), std::back_inserter(vec));
        return vec;
    }

    inline QByteArray VectorToQByteArray(const std::vector<char>& vec)
    {
        QByteArray byteArray;
        std::copy(vec.begin(), vec.end(), std::back_inserter(byteArray));
        return byteArray;
    }
}
