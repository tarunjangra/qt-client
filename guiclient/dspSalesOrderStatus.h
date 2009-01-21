/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2009 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef DSPSALESORDERSTATUS_H
#define DSPSALESORDERSTATUS_H

#include "guiclient.h"
#include "xwidget.h"
#include <parameter.h>

#include "ui_dspSalesOrderStatus.h"

class dspSalesOrderStatus : public XWidget, public Ui::dspSalesOrderStatus
{
    Q_OBJECT

public:
    dspSalesOrderStatus(QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = Qt::Window);
    ~dspSalesOrderStatus();

public slots:
    virtual enum SetResponse set( const ParameterList & pParams );
    virtual void sPrint();
    virtual void sSalesOrderList();
    virtual void sFillList( int pSoheadid );

protected slots:
    virtual void languageChange();

};

#endif // DSPSALESORDERSTATUS_H
