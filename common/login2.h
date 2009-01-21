/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2009 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef __LOGIN2_H__
#define __LOGIN2_H__

#include "tmp/ui_login2.h"

class QSplashScreen;
class QImage;
class QWidget;

#include <QDialog>
#include <QString>

#include "parameter.h"

class login2 : public QDialog, public Ui::login2
{
  Q_OBJECT

  public:
    login2(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0);
    ~login2();

    QString _databaseURL;
    QString _user;
    int _userid;

    virtual int set( ParameterList & pParams, QSplashScreen * pSplash );
    virtual void populateDatabaseInfo();
    virtual QString username();
    virtual QString password();

  public slots:
    virtual int set( ParameterList & pParams );
    virtual void setLogo( const QImage & );

  protected slots:
    virtual void languageChange();

    virtual void sLogin();
    virtual void sOptions();


  private:
    bool _captive;
    bool _evaluation;
    bool _nonxTupleDB;
    bool _enhancedAuth;
    bool _requireSSL;
    bool _multipleConnections;
    QSplashScreen *_splash;
    QString _cUsername;
    QString _cPassword;
    QString _evalDatabaseURL;
};

#endif

