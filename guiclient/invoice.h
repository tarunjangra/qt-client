/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2009 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef INVOICE_H
#define INVOICE_H

#include "guiclient.h"
#include "xwidget.h"
#include <parameter.h>

#include "ui_invoice.h"

#include "taxCache.h"

class invoice : public XWidget, public Ui::invoice
{
    Q_OBJECT

public:
    invoice(QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = Qt::Window);
    ~invoice();

    static void newInvoice( int pCustid );
    static void editInvoice( int pId );
    static void viewInvoice( int pId );

public slots:
    virtual SetResponse set( const ParameterList & pParams );
    virtual void sClose();
    virtual void sPopulateCustomerInfo( int pCustid );
    virtual void sShipToList();
    virtual void sParseShipToNumber();
    virtual void populateShipto( int pShiptoid );
    virtual void sCopyToShipto();
    virtual void sSave();
    virtual void sNew();
    virtual void sEdit();
    virtual void sView();
    virtual void sDelete();
    virtual void populate();
    virtual void sFillItemList();
    virtual void sFreightChanged();
    virtual void sCalculateTotal();
    virtual void closeEvent( QCloseEvent * pEvent );
    virtual void setFreeFormShipto( bool pFreeForm );
    virtual void sShipToModified();
    virtual void populateCMInfo();
    virtual void populateCCInfo();
    virtual void sTaxAuthChanged();
    virtual void sHandleShipchrg( int pShipchrgid );

protected:
    virtual void keyPressEvent( QKeyEvent * e );
    virtual void recalculateTax();

protected slots:
    virtual void languageChange();

    virtual void sTaxDetail();


private:
    int		_mode;

    double	_cachedSubtotal;
    int		_custtaxauthid;
    bool	_ffShipto;
    int		_invcheadid;
    int		_shiptoid;
    int		_taxauthidCache;
    int		_taxcurrid;
    bool        _loading;

    taxCache	_taxCache;
};

#endif // INVOICE_H
