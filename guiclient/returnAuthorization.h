/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2009 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef RETURNAUTHORIZATION_H
#define RETURNAUTHORIZATION_H

#include "guiclient.h"
#include "xwidget.h"
#include <parameter.h>

#include "ui_returnAuthorization.h"

#include "taxCache.h"

class returnAuthorization : public XWidget, public Ui::returnAuthorization
{
    Q_OBJECT

public:
    returnAuthorization(QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = Qt::Window);
    ~returnAuthorization();

    virtual void setNumber();

public slots:
    virtual SetResponse set( const ParameterList & pParams );
    virtual void recalculateTax();
    virtual bool sSave( bool partial );
    virtual void sSaveClick();
    virtual void sShipToList();
    virtual void sParseShipToNumber();
    virtual void populateShipto( int pShiptoid );
    virtual void sPopulateCustomerInfo();
    virtual void sCheckAuthorizationNumber();
    virtual void sClearShiptoNumber();
    virtual void sCopyToShipto();
    virtual void sNew();
    virtual void sEdit();
    virtual void sView();
    virtual void sAction();
    virtual void sDelete();
    virtual void sFillList();
    virtual void sCalculateSubtotal();
    virtual void sCalculateTotal();
    virtual void populate();
    virtual void closeEvent( QCloseEvent * pEvent );
    virtual void sFreightChanged();
    virtual void sTaxAuthChanged();
    virtual void sRecvWhsChanged();
    virtual void sShipWhsChanged();
    virtual void sTaxDetail();
    virtual void sOrigSoChanged();
    virtual void sDispositionChanged();
    virtual void sCreditByChanged();
    virtual void sAuthorizeLine();
    virtual void sClearAuthorization();
    virtual void sAuthorizeAll();
    virtual void sEnterReceipt();
    virtual void sReceiveAll();
    virtual void sHandleEnterReceipt(bool);
    virtual void sHandleAction();
    virtual void sHandleSalesOrderEvent( int pSoheadid, bool );
    virtual void sRefund();
    virtual void sPostReceipts();
    virtual void sPopulateMenu(QMenu*, QTreeWidgetItem *selected);
    virtual void sViewOrigOrder();
    virtual void sEditNewOrder();
    virtual void sViewNewOrder();
    virtual void sEditNewOrderLine();
    virtual void sViewNewOrderLine();
    virtual void sShipment();
    virtual void sShipmentStatus();
    virtual void sCheckNumber();

protected slots:
    virtual void languageChange();

private:
    int _mode;
    int _raheadid;
    int _shiptoid;
    int	_custtaxauthid;
    bool _custEmail;
    bool _ffBillto;
    bool _ffShipto;
    bool _ignoreShiptoSignals;
	  bool _ignoreSoSignals;
	  bool _ignoreWhsSignals;
    double _subtotalCache;
    int _taxauthidCache;
    int _taxcurrid;

    taxCache _taxCache;
};

#endif // RETURNAUTHORIZATION_H
