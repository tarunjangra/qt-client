/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2009 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "printVendorForm.h"

#include <qvariant.h>
#include <qmessagebox.h>
#include <openreports.h>
#include <parameter.h>
#include "guiclient.h"

/*
 *  Constructs a printVendorForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
printVendorForm::printVendorForm(QWidget* parent, const char* name, bool modal, Qt::WFlags fl)
    : XDialog(parent, name, modal, fl)
{
    setupUi(this);


    // signals and slots connections
    connect(_close, SIGNAL(clicked()), this, SLOT(reject()));
    connect(_print, SIGNAL(clicked()), this, SLOT(sPrint()));
    connect(_vendor, SIGNAL(valid(bool)), _print, SLOT(setEnabled(bool)));
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
printVendorForm::~printVendorForm()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void printVendorForm::languageChange()
{
    retranslateUi(this);
}


void printVendorForm::init()
{
  _report->populate( "SELECT form_id, form_name "
                     "FROM form "
                     "WHERE (form_key='Vend') "
                     "ORDER BY form_name;" );
}

void printVendorForm::sPrint()
{
  q.prepare( "SELECT report_name "
             "FROM report, form "
             "WHERE ( (form_id=:form_id) "
             " AND (report_id=form_report_id) );" );
  q.bindValue(":form_id", _report->id());
  q.exec();
  if (q.first())
  {
    ParameterList params;
    params.append("vend_id", _vendor->id());

    orReport report(q.value("report_name").toString(), params);
    if (report.isValid())
      report.print();
    else
    {
      report.reportError(this);
      reject();
    }

    _vendor->setId(-1);
    _vendor->setFocus();
  }
  else
    QMessageBox::warning( this, tr("Could not locate report"),
                          tr("Could not locate the report definition the form \"%1\"")
                          .arg(_report->currentText()) );
}
