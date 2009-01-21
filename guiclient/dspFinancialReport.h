/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2009 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef DSPFINANCIALREPORT_H
#define DSPFINANCIALREPORT_H

class GroupBalances;

#include "guiclient.h"
#include <QWidget>
#include <QMap>
#include <parameter.h>

#include "ui_dspFinancialReport.h"

class dspFinancialReport : public QWidget, public Ui::dspFinancialReport
{
    Q_OBJECT

public:
    dspFinancialReport(QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0);
    ~dspFinancialReport();

public slots:
    virtual SetResponse set( const ParameterList & pParams );
    virtual void sFillList();
    virtual void sPrint();
    virtual void sPopulateMenu(QMenu * pMenu);
    virtual void sFillListStatement();
    virtual void sFillListTrend();
    virtual void sFillPeriods();
    virtual void sEditPeriodLabel();
    virtual void sTogglePeriod();
    virtual void sToggleTrend();
    virtual bool sCheck();

protected slots:
    virtual void languageChange();

    virtual void sCollapsed( QTreeWidgetItem * item );
    virtual void sExpanded( QTreeWidgetItem * item );
    virtual void sReportChanged(int);


private:
    int _mode;
    QMap<int, QString> _columnLabels;
};

#endif // DSPFINANCIALREPORT_H
