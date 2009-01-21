/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2009 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "purgeInvoices.h"

#include <qvariant.h>
#include <qmessagebox.h>

/*
 *  Constructs a purgeInvoices as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
purgeInvoices::purgeInvoices(QWidget* parent, const char* name, bool modal, Qt::WFlags fl)
    : XDialog(parent, name, modal, fl)
{
    setupUi(this);


    // signals and slots connections
    connect(_purge, SIGNAL(clicked()), this, SLOT(sPurge()));
    connect(_close, SIGNAL(clicked()), this, SLOT(reject()));
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
purgeInvoices::~purgeInvoices()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void purgeInvoices::languageChange()
{
    retranslateUi(this);
}


void purgeInvoices::init()
{
}

void purgeInvoices::sPurge()
{
  if (!_cutOffDate->isValid())
  {
    QMessageBox::warning( this, tr("Enter Cutoff Date"),
                          tr("You must enter a valid cutoff date before purging Invoice Records.") );
    return;
  }

  if ( QMessageBox::warning( this, tr("Delete Invoice Records"),
                             tr( "You will not be able to re-print an Invoice if you delete that Invoice's Records.\n"
                                 "Are you sure that you want to delete the selected Invoice Records?" ),
                             tr("Yes"), tr("No"), QString::null, 0, 1) == 0)
  {
    q.prepare("SELECT purgeInvoiceRecords(:cutOffDate) AS result;");
    q.bindValue(":cutOffDate", _cutOffDate->date());
    q.exec();

    accept();
  }
}

