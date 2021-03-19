#include <UnitTest++.h>

#include <QtNetwork>

#include "ProxyUtils.h"

SUITE(ProxyUtilsTests)
{
    using namespace ureport::details;

    TEST(CreateProxyFromUrl_FullUrlSpecified_ProxyProperlyInitialized)
    {
        const QString expectedHost = "192.168.0.1";
        const quint16 expectedPort = 57193;
        const QString expectedUser = "user";
        const QString expectedPassword = "password";

        QNetworkProxy proxy = ProxyUtils::CreateProxyFromUrl(QString("http://%1:%2@%3:%4").arg(expectedUser, expectedPassword, expectedHost, QString::number(expectedPort)));

        CHECK_EQUAL(QNetworkProxy::HttpProxy, proxy.type());
        CHECK_EQUAL(expectedHost.toStdString(), proxy.hostName().toStdString());
        CHECK_EQUAL(expectedPort, proxy.port());
        CHECK_EQUAL(expectedUser.toStdString(), proxy.user().toStdString());
        CHECK_EQUAL(expectedPassword.toStdString(), proxy.password().toStdString());
    }

    TEST(CreateProxyFromUrl_OutOfRangePortSpecified_NoProxyObjectIsReturned)
    {
        QNetworkProxy proxy = ProxyUtils::CreateProxyFromUrl(QString("http://192.168.0.1:128000"));

        CHECK_EQUAL(QNetworkProxy::NoProxy, proxy.type());
    }

    TEST(CreateProxyFromUrl_IllegalPortSpecified_NoProxyIsReturned)
    {
        QNetworkProxy proxy = ProxyUtils::CreateProxyFromUrl(QString("http://192.168.0.1:aaa"));

        CHECK_EQUAL(QNetworkProxy::NoProxy, proxy.type());
    }

    TEST(CreateProxyFromUrl_EmptyUserSpecified_PasswordIsEmpty)
    {
        QNetworkProxy proxy = ProxyUtils::CreateProxyFromUrl(QString("http://:password@192.168.0.1"));

        CHECK_EQUAL("", proxy.user().toStdString());
        CHECK_EQUAL("", proxy.password().toStdString());
    }

    TEST(CreateProxyFromUrl_NoPortIsSpecified_ProxyHasZeroPort)
    {
        QNetworkProxy proxy = ProxyUtils::CreateProxyFromUrl(QString("http://192.168.0.1"));

        CHECK_EQUAL(0, proxy.port());
    }

    TEST(CreateProxyFromUrl_LocalProxyHostNameSpecified_ProxyProperlyInitialized)
    {
        const QString expectedHostName = "hostname";

        QNetworkProxy proxy = ProxyUtils::CreateProxyFromUrl(QString("http://%1").arg(expectedHostName));

        CHECK_EQUAL(expectedHostName.toStdString(), proxy.hostName().toStdString());
    }

    TEST(CreateProxyFromUrl_FullyQualifiedDomainProxyNameSpecified_ProxyProperlyInitialized)
    {
        const QString expectedHostName = "hostname.com.ua";

        QNetworkProxy proxy = ProxyUtils::CreateProxyFromUrl(QString("http://%1").arg(expectedHostName));

        CHECK_EQUAL(expectedHostName.toStdString(), proxy.hostName().toStdString());
    }

    TEST(CreateProxyFromUrl_EmptyProtocolSpecified_NoProxyIsReturned)
    {
        QNetworkProxy proxy = ProxyUtils::CreateProxyFromUrl("192.168.0.1:8888");

        CHECK_EQUAL(QNetworkProxy::NoProxy, proxy.type());
    }

    TEST(CreateProxyFromUrl_EmptyProtocolAndPortSpecified_NoProxyIsReturned)
    {
        QNetworkProxy proxy = ProxyUtils::CreateProxyFromUrl("hostname:8888");

        CHECK_EQUAL(QNetworkProxy::NoProxy, proxy.type());
    }

    TEST(CreateProxyFromUrl_EmptyProtocolAndNoPortSpecified_HttpProtolIsUsedForTheProxy)
    {
        QNetworkProxy proxy = ProxyUtils::CreateProxyFromUrl("hostname");

        CHECK_EQUAL(QNetworkProxy::HttpProxy, proxy.type());
    }

    TEST(CreateProxyFromUrl_IllegalProtocolSpecified_NoProxyIsReturned)
    {
        QNetworkProxy proxy = ProxyUtils::CreateProxyFromUrl(QString("tcp://192.168.0.1:8888"));

        CHECK_EQUAL(QNetworkProxy::NoProxy, proxy.type());
    }

    TEST(QProcessEnvironment_InsertEmptyVariable_ContainsInsertedVariable)
    {
        const QString variableName = "QWERTY";

        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

        env.insert(variableName, QString(""));

        CHECK_EQUAL(true, env.contains(variableName));

        env.remove(variableName);
    }

    TEST(QProcessEnvironment_GetNotExistingValue_EmptyValueIsReturned)
    {
        const QString variableName = "QWERTY";

        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

        QString variableValue = env.value(variableName);

        CHECK_EQUAL(true, variableValue.isEmpty());
        CHECK_EQUAL(false, env.contains(variableName));
    }
}
