/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2009 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "postStandardJournal.h"

#include <QVariant>
#include <QMessageBox>
#include "glSeries.h"

/*
 *  Constructs a postStandardJournal as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
postStandardJournal::postStandardJournal(QWidget* parent, const char* name, bool modal, Qt::WFlags fl)
    : XDialog(parent, name, modal, fl)
{
  setupUi(this);


  // signals and slots connections
  connect(_post, SIGNAL(clicked()), this, SLOT(sPost()));
  connect(_submit, SIGNAL(clicked()), this, SLOT(sSubmit()));
  
  if (!_metrics->boolean("EnableBatchManager"))
    _submit->hide();

  _captive = false;
  _doSubmit = false;

  _stdjrnl->setAllowNull(TRUE);
  _stdjrnl->populate( "SELECT stdjrnl_id, stdjrnl_name "
                      "FROM stdjrnl "
                      "ORDER BY stdjrnl_name;" );
}

/*
 *  Destroys the object and frees any allocated resources
 */
postStandardJournal::~postStandardJournal()
{
  // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void postStandardJournal::languageChange()
{
  retranslateUi(this);
}

enum SetResponse postStandardJournal::set(const ParameterList &pParams)
{
  _captive = TRUE;

  QVariant param;
  bool     valid;

  param = pParams.value("stdjrnl_id", &valid);
  if (valid)
  {
    _stdjrnl->setId(param.toInt());
    _post->setFocus();
  }

  return NoError;
}


void postStandardJournal::sPost()
{
  if (!_distDate->isValid())
  {
    QMessageBox::critical( this, tr("Cannot Post Standard Journal"),
                           tr("You must enter a Distribution Date before you may post this Standard Journal.") );
    _distDate->setFocus();
    return;
  }

  q.prepare("SELECT postStandardJournal(:stdjrnl_id, :distDate, :reverse) AS result;");
  q.bindValue(":stdjrnl_id", _stdjrnl->id());
  q.bindValue(":distDate", _distDate->date());
  q.bindValue(":reverse", QVariant(_reverse->isChecked()));
  q.exec();
  if (q.first())
  {
    ParameterList params;
    params.append("mode", "postStandardJournal");
    params.append("glSequence", q.value("result"));
    if(_doSubmit)
      params.append("submit");

    glSeries newdlg(this, "", TRUE);
    newdlg.set(params);
    newdlg.exec();
  }
  else
    systemError(this, tr("A System Error occurred at %1::%2.")
                      .arg(__FILE__)
                      .arg(__LINE__) );

  if (_captive)
    accept();
  {
    _stdjrnl->setNull();
    _close->setText(tr("&Close"));
    _stdjrnl->setFocus();
  }
}

void postStandardJournal::sSubmit()
{
  _doSubmit = true;
  sPost();
  _doSubmit = false;
}


