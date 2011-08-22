/*
  This file is part of the PhantomJS project from Ofi Labs.

  Copyright (C) 2011 Ariya Hidayat <ariya.hidayat@gmail.com>
  Copyright (C) 2010 Ariya Hidayat <ariya.hidayat@gmail.com>

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "cookiejar.h"

CookieJar::CookieJar(QString cookieFile)
        : QNetworkCookieJar()
    {
        m_cookieFile = cookieFile;
    }

bool CookieJar::setCookiesFromUrl(const QList<QNetworkCookie> & cookieList, const QUrl & url) {
    QSettings settings(m_cookieFile, QSettings::IniFormat);

    settings.beginGroup(url.host());
    
    for (QList<QNetworkCookie>::const_iterator i = cookieList.begin(); i != cookieList.end(); i++) {
        settings.setValue((*i).name(), QString((*i).value()));
    }
    
    settings.sync();
    
    return true;
}

QList<QNetworkCookie> CookieJar::cookiesForUrl(const QUrl & url) const {
    QSettings settings(m_cookieFile, QSettings::IniFormat);
    QList<QNetworkCookie> cookieList;

    settings.beginGroup(url.host());

    QStringList keys = settings.childKeys();
    
    for (QStringList::iterator i = keys.begin(); i != keys.end(); i++) {
        cookieList.push_back(QNetworkCookie((*i).toLocal8Bit(), settings.value(*i).toByteArray()));
    }
    
    return cookieList;
}
