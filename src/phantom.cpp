#include "phantom.h"

Phantom::Phantom(QObject *parent)
    : QObject(parent)
    , m_terminated(false)
    , m_returnValue(0)
    , m_converter(0)
    , m_netAccessMan(0)
{
    m_page = new WebPage(this);
    m_pages.append(m_page);

    QString proxyHost;
    QString cookieFile;
    int proxyPort = 1080;
    bool autoLoadImages = true;
    bool pluginsEnabled = false;
    bool diskCacheEnabled = false;
    bool ignoreSslErrors = false;
    bool localAccessRemote = false;

    // second argument: script name
    QStringList args = QApplication::arguments();

    // Skip the first argument, i.e. the application executable (phantomjs).
    args.removeFirst();

    // Handle all command-line options.
    QStringListIterator argIterator(args);
    while (argIterator.hasNext()) {
        const QString &arg = argIterator.next();
        if (arg == "--version") {
            m_terminated = true;
            Registry::terminal().cout(QString("%1 (development)").arg(PHANTOMJS_VERSION_STRING));
            return;
        }
        if (arg == "--load-images=yes") {
            autoLoadImages = true;
            continue;
        }
        if (arg == "--load-images=no") {
            autoLoadImages = false;
            continue;
        }
        if (arg == "--load-plugins=yes") {
            pluginsEnabled = true;
            continue;
        }
        if (arg == "--load-plugins=no") {
            pluginsEnabled = false;
            continue;
        }
        if (arg == "--disk-cache=yes") {
            diskCacheEnabled = true;
            continue;
        }
        if (arg == "--disk-cache=no") {
            diskCacheEnabled = false;
            continue;
        }
        if (arg == "--ignore-ssl-errors=yes") {
            ignoreSslErrors = true;
            continue;
        }
        if (arg == "--ignore-ssl-errors=no") {
            ignoreSslErrors = false;
            continue;
        }
        if (arg == "--local-access-remote=no") {
            localAccessRemote = false;
            continue;
        }
        if (arg == "--local-access-remote=yes") {
            localAccessRemote = true;
            continue;
        }
        if (arg.startsWith("--proxy=")) {
            proxyHost = arg.mid(8).trimmed();
            if (proxyHost.lastIndexOf(':') > 0) {
                bool ok = true;
                int port = proxyHost.mid(proxyHost.lastIndexOf(':') + 1).toInt(&ok);
                if (ok) {
                    proxyHost = proxyHost.left(proxyHost.lastIndexOf(':')).trimmed();
                    proxyPort = port;
                }
            }
            continue;
        }
        if (arg.startsWith("--cookies=")) {
            cookieFile = arg.mid(10).trimmed();
            continue;
        }
        if (arg.startsWith("--output-encoding=")) {
            Registry::terminal().setEncoding(arg.mid(18).trimmed());
            continue;
        }
        if (arg.startsWith("--script-encoding=")) {
            m_scriptFileEnc.setEncoding(arg.mid(18).trimmed());
            continue;
        }
        if (arg.startsWith("--")) {
            Registry::terminal().cerr(QString("Unknown option '%1'").arg(arg));
            m_terminated = true;
            return;
        } else {
            m_scriptFile = arg;
            break;
        }
    }

    if (m_scriptFile.isEmpty()) {
        Utils::showUsage();
        return;
    }

    if (proxyHost.isEmpty()) {
        QNetworkProxyFactory::setUseSystemConfiguration(true);
    } else {
        QNetworkProxy proxy(QNetworkProxy::HttpProxy, proxyHost, proxyPort);
        QNetworkProxy::setApplicationProxy(proxy);
    }

    // The remaining arguments are available for the script.
    while (argIterator.hasNext()) {
        const QString &arg = argIterator.next();
        m_args += arg;
    }

    // Provide WebPage with a non-standard Network Access Manager
    m_netAccessMan = new NetworkAccessManager(this, diskCacheEnabled, cookieFile, ignoreSslErrors);
    m_page->setNetworkAccessManager(m_netAccessMan);

    connect(m_page, SIGNAL(javaScriptConsoleMessageSent(QString, int, QString)),
            SLOT(printConsoleMessage(QString, int, QString)));

    m_defaultPageSettings[PAGE_SETTINGS_LOAD_IMAGES] = QVariant::fromValue(autoLoadImages);
    m_defaultPageSettings[PAGE_SETTINGS_LOAD_PLUGINS] = QVariant::fromValue(pluginsEnabled);
    m_defaultPageSettings[PAGE_SETTINGS_USER_AGENT] = QVariant::fromValue(m_page->userAgent());
    m_defaultPageSettings[PAGE_SETTINGS_LOCAL_ACCESS_REMOTE] = QVariant::fromValue(localAccessRemote);
    m_page->applySettings(m_defaultPageSettings);

    setLibraryPath(QFileInfo(m_scriptFile).dir().absolutePath());

    m_page->mainFrame()->addToJavaScriptWindowObject("phantom", this);
    m_page->mainFrame()->addToJavaScriptWindowObject("fs", &m_filesystem);

    QFile file(":/bootstrap.js");
    if (!file.open(QFile::ReadOnly)) {
        Registry::terminal().cerr("Can not bootstrap!");
        exit(1);
    }
    QString bootstrapper = QString::fromUtf8(file.readAll());
    file.close();
    if (bootstrapper.isEmpty()) {
        Registry::terminal().cerr("Can not bootstrap!");
        exit(1);
    }
    m_page->mainFrame()->evaluateJavaScript(bootstrapper);
}

QStringList Phantom::args() const
{
    return m_args;
}

QVariantMap Phantom::defaultPageSettings() const
{
    return m_defaultPageSettings;
}

QString Phantom::outputEncoding() const
{
    return Registry::terminal().getEncoding();
}

void Phantom::setOutputEncoding(const QString &encoding)
{
    Registry::terminal().setEncoding(encoding);
}

bool Phantom::execute()
{
    if (m_terminated)
        return false;

    if (m_scriptFile.isEmpty())
        return false;

    if (!Utils::injectJsInFrame(m_scriptFile, m_scriptFileEnc, QDir::currentPath(), m_page->mainFrame(), true)) {
        m_returnValue = -1;
        return false;
    }

    return !m_terminated;
}

int Phantom::returnValue() const
{
    return m_returnValue;
}

QString Phantom::libraryPath() const
{
   return m_page->libraryPath();
}

void Phantom::setLibraryPath(const QString &libraryPath)
{
   m_page->setLibraryPath(libraryPath);
}

QString Phantom::scriptName() const
{
    return QFileInfo(m_scriptFile).fileName();
}

QVariantMap Phantom::version() const
{
    QVariantMap result;
    result["major"] = PHANTOMJS_VERSION_MAJOR;
    result["minor"] = PHANTOMJS_VERSION_MINOR;
    result["patch"] = PHANTOMJS_VERSION_PATCH;
    return result;
}

// public slots:
QObject *Phantom::createWebPage()
{
    WebPage *page = new WebPage(this);
    m_pages.append(page);
    page->applySettings(m_defaultPageSettings);
    page->setNetworkAccessManager(m_netAccessMan);
    page->setLibraryPath(QFileInfo(m_scriptFile).dir().absolutePath());
    return page;
}

bool Phantom::injectJs(const QString &jsFilePath) {
    return Utils::injectJsInFrame(jsFilePath, libraryPath(), m_page->mainFrame());
}

void Phantom::exit(int code)
{
    m_terminated = true;
    m_returnValue = code;
    qDeleteAll(m_pages);
    m_pages.clear();
    m_page = 0;
    QApplication::instance()->exit(code);
}

void Phantom::_destroy(QObject *page) {
    m_pages.removeOne((WebPage*)page);
    delete page;
}

// private slots:
void Phantom::printConsoleMessage(const QString &message, int lineNumber, const QString &source)
{
    QString msg = message;
    if (!source.isEmpty())
        msg = source + ":" + QString::number(lineNumber) + " " + msg;
    Registry::terminal().cout(msg);
}
