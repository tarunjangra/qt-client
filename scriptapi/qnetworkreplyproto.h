/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2009 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef __QNETWORKREPLYPROTO_H__
#define __QNETWORKREPLYPROTO_H__

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QSslConfiguration>
#include <QtScript>

class QByteArray;

Q_DECLARE_METATYPE(QNetworkReply*)

void setupQNetworkReplyProto(QScriptEngine *engine);

class QNetworkReplyProto : public QObject, public QScriptable
{
  Q_OBJECT

  public:
    QNetworkReplyProto(QObject *parent);

    Q_INVOKABLE void      abort() const;
    Q_INVOKABLE QVariant  attribute(const QNetworkRequest::Attribute &code) const;
    Q_INVOKABLE void      close();
    Q_INVOKABLE int       error() const;
    Q_INVOKABLE bool      hasRawHeader(const QByteArray &headerName)   const;
    Q_INVOKABLE QVariant  header(QNetworkRequest::KnownHeaders header) const;
    Q_INVOKABLE QNetworkAccessManager           *manager()   const;
    Q_INVOKABLE QNetworkAccessManager::Operation operation() const;
    Q_INVOKABLE QByteArray        rawHeader(const QByteArray &headerName) const;
    Q_INVOKABLE QList<QByteArray> rawHeaderList()    const;
    Q_INVOKABLE qint64            readBufferSize()   const;
    Q_INVOKABLE QNetworkRequest   request()          const;
    Q_INVOKABLE void              setReadBufferSize(qint64 size);
#ifndef QT_NO_OPENSSL
    Q_INVOKABLE void              setSslConfiguration(const QSslConfiguration &config);
    Q_INVOKABLE QSslConfiguration sslConfiguration() const;
#endif
    Q_INVOKABLE QString           toString()         const;
    Q_INVOKABLE QUrl              url()              const;

    // now for the QIODevice API
    Q_INVOKABLE qint64      bytesAvailable()        const;
    Q_INVOKABLE qint64      bytesToWrite()          const;
    Q_INVOKABLE bool        canReadLine()           const;
    Q_INVOKABLE QString     errorString()           const;
    Q_INVOKABLE bool        getChar(char * c);
    Q_INVOKABLE bool        isOpen()                const;
    Q_INVOKABLE bool        isReadable()            const;
    Q_INVOKABLE bool        isSequential()          const;
    Q_INVOKABLE bool        isTextModeEnabled()     const;
    Q_INVOKABLE bool        isWritable()            const;
    Q_INVOKABLE bool        open(int mode);
    Q_INVOKABLE int         openMode()              const;
    Q_INVOKABLE qint64      peek(char * data, qint64 maxSize);
    Q_INVOKABLE QByteArray  peek(qint64 maxSize);
    Q_INVOKABLE qint64      pos()                   const;
    Q_INVOKABLE bool        putChar(char c);
    Q_INVOKABLE qint64      read(char * data, qint64 maxSize);
    Q_INVOKABLE QByteArray  read(qint64 maxSize);
    Q_INVOKABLE QByteArray  readAll();
    Q_INVOKABLE qint64      readLine(char * data, qint64 maxSize);
    Q_INVOKABLE QByteArray  readLine(qint64 maxSize = 0);
    Q_INVOKABLE bool        reset();
    Q_INVOKABLE bool        seek(qint64 pos);
    Q_INVOKABLE void        setTextModeEnabled(bool enabled);
    Q_INVOKABLE qint64      size()                  const;
    Q_INVOKABLE void        ungetChar(char c);
    Q_INVOKABLE bool        waitForBytesWritten(int msecs);
    Q_INVOKABLE bool        waitForReadyRead(int msecs);
    Q_INVOKABLE qint64      write(const char * data, qint64 maxSize);
    Q_INVOKABLE qint64      write(const QByteArray &byteArray);
    Q_INVOKABLE qint64      write(const QString &string);
};

#endif
