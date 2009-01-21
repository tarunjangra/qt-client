/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2009 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#ifndef PRINTWOFORM_H
#define PRINTWOFORM_H

#include "xdialog.h"
#include "ui_printWoForm.h"

class printWoForm : public XDialog, public Ui::printWoForm
{
    Q_OBJECT

public:
    printWoForm(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0);
    ~printWoForm();

    virtual void init();

public slots:
    virtual void sPrint();

protected slots:
    virtual void languageChange();

};

#endif // PRINTWOFORM_H
