#ifndef PHANTOM_H
#define PHANTOM_H

#include <QtGui>
#include <QtWebKit>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>

#include "csconverter.h"
#include "networkaccessmanager.h"
#include "filesystem.h"
#include "encoding.h"
#include "webpage.h"
#include "utils.h"
#include "consts.h"
#include "registry.h"

#include "libphantom_global.h"

class WebPage;

class LIBPHANTOMSHARED_EXPORT Phantom: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList args READ args)
    Q_PROPERTY(QVariantMap defaultPageSettings READ defaultPageSettings)
    Q_PROPERTY(QString libraryPath READ libraryPath WRITE setLibraryPath)
    Q_PROPERTY(QString outputEncoding READ outputEncoding WRITE setOutputEncoding)
    Q_PROPERTY(QString scriptName READ scriptName)
    Q_PROPERTY(QVariantMap version READ version)

    QString cookieFile;

public:
    explicit Phantom(QObject *parent = 0,
                     bool autoLoadImages = true,
                     bool allowPlugins = false,
                     bool ignoreSslErrors = false,
                     bool diskCaching = false,
                     bool localContentAccessToRemote = false,
                     QString sysProxy = "",
                     QString cookiesFile = ""
                    );
    virtual ~Phantom();

    QStringList args() const;

    QVariantMap defaultPageSettings() const;

    QString outputEncoding() const;
    void setOutputEncoding(const QString &encoding);

    bool execute();
    int returnValue() const;

    QString libraryPath() const;
    void setLibraryPath(const QString &libraryPath);

    QString scriptName() const;

    QVariantMap version() const;
    /*
    void setAutoLoadImages(bool b);
    void setAllowPlugins(bool b);
    void setDiskCaching(bool b);
    void setIgnoreSslErrors(bool b);
    void setLocalContentAccessToRemote(bool b);
    void setProxy(const QString hostAndPort);
    void setCookiesFile(const QString cookiesFilePath);
    */

public slots:
    QObject *createWebPage();
    bool injectJs(const QString &jsFilePath);
    void exit(int code = 0);
    void _destroy(QObject *page);

private slots:
    void printConsoleMessage(const QString &msg, int lineNumber, const QString &source);

private:
    QString m_scriptFile;
    Encoding m_scriptFileEnc;
    QStringList m_args;
    WebPage *m_page;
    bool m_terminated;
    int m_returnValue;
    QString m_script;
    CSConverter *m_converter;
    NetworkAccessManager *m_netAccessMan;
    QVariantMap m_defaultPageSettings;
    FileSystem m_filesystem;
    QList<WebPage*> m_pages;
};

#endif // PHANTOM_H
