/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2009 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */


#ifndef wolist_h
#define wolist_H

#include <qdialog.h>

#include "widgets.h"

class XTreeWidget;
class WarehouseGroup;
class QRadioButton;
class QPushButton;
class ParameterList;

class XTUPLEWIDGETS_EXPORT woList : public QDialog
{
    Q_OBJECT

  public:
    woList(QWidget * = 0, const char * = 0, bool = FALSE, Qt::WFlags = 0);

    WarehouseGroup* _warehouse;
    QPushButton* _close;
    QPushButton* _select;
    XTreeWidget* _wo;

  public slots:
    virtual void set( ParameterList & pParams );

    virtual void sClose();
    virtual void sSelect();
    virtual void sFillList();


private:
    int _woid;
    int _type;
    QString _sql;

};

#endif

