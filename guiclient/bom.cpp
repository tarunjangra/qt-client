/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2009 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "bom.h"

#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QValidator>
#include <QVariant>
#include <QSqlError>

#include <metasql.h>
#include <openreports.h>

#include "bomItem.h"

BOM::BOM(QWidget* parent, const char* name, Qt::WFlags fl)
    : XWidget(parent, name, fl)
{
  setupUi(this);

  connect(_print, SIGNAL(clicked()), this, SLOT(sPrint()));
  connect(_new, SIGNAL(clicked()), this, SLOT(sNew()));
  connect(_edit, SIGNAL(clicked()), this, SLOT(sEdit()));
  connect(_moveUp, SIGNAL(clicked()), this, SLOT(sMoveUp()));
  connect(_moveDown, SIGNAL(clicked()), this, SLOT(sMoveDown()));
  connect(_item, SIGNAL(newId(int)), this, SLOT(sFillList()));
  connect(_revision, SIGNAL(newId(int)), this, SLOT(sFillList()));
  connect(_showExpired, SIGNAL(toggled(bool)), this, SLOT(sFillList()));
  connect(_showFuture, SIGNAL(toggled(bool)), this, SLOT(sFillList()));
  connect(_bomitem, SIGNAL(populateMenu(QMenu*,QTreeWidgetItem*,int)), this, SLOT(sPopulateMenu(QMenu*)));
  connect(_close, SIGNAL(clicked()), this, SLOT(sClose()));
  connect(_view, SIGNAL(clicked()), this, SLOT(sView()));
  connect(_save, SIGNAL(clicked()), this, SLOT(sSave()));
  connect(_expire, SIGNAL(clicked()), this, SLOT(sExpire()));

  _totalQtyPerCache = 0.0;
  
  _item->setType(ItemLineEdit::cGeneralManufactured | ItemLineEdit::cGeneralPurchased | ItemLineEdit::cPlanning | ItemLineEdit::cJob);
  _batchSize->setValidator(omfgThis->qtyVal());
  _requiredQtyPer->setValidator(omfgThis->qtyPerVal());
  _nonPickNumber->setPrecision(omfgThis->qtyVal());
  _nonPickQtyPer->setPrecision(omfgThis->qtyPerVal());
  _pickNumber->setPrecision(omfgThis->qtyVal());
  _pickQtyPer->setPrecision(omfgThis->qtyPerVal());
  _totalNumber->setPrecision(omfgThis->qtyVal());
  _totalQtyPer->setPrecision(omfgThis->qtyPerVal());
  _currentStdCost->setPrecision(omfgThis->costVal());
  _currentActCost->setPrecision(omfgThis->costVal());
  _maxCost->setPrecision(omfgThis->costVal());
  
  _bomitem->addColumn(tr("#"),            _seqColumn,   Qt::AlignCenter, true, "bomitem_seqnumber");
  _bomitem->addColumn(tr("Item Number"),  _itemColumn,  Qt::AlignLeft,   true, "item_number");
  _bomitem->addColumn(tr("Description"),  -1,           Qt::AlignLeft,   true, "item_description");
  _bomitem->addColumn(tr("Issue UOM"),    _uomColumn,   Qt::AlignCenter, true, "issueuom");
  _bomitem->addColumn(tr("Issue Method"), _itemColumn,  Qt::AlignCenter, true, "issuemethod");
  _bomitem->addColumn(tr("Qty. Per"),     _qtyColumn,   Qt::AlignRight,  true, "bomitem_qtyper" );
  _bomitem->addColumn(tr("Scrap %"),      _prcntColumn, Qt::AlignRight,  true, "bomitem_scrap" );
  _bomitem->addColumn(tr("Effective"),    _dateColumn,  Qt::AlignCenter, true, "effective");
  _bomitem->addColumn(tr("Expires"),      _dateColumn,  Qt::AlignCenter, true, "expires");
  _bomitem->addColumn(tr("Notes"),          _itemColumn,  Qt::AlignLeft,  false, "bomitem_notes"   );
  _bomitem->addColumn(tr("Reference"),     _itemColumn,  Qt::AlignLeft,  false, "bomitem_ref"   );
  _bomitem->setDragString("bomid=");
  _bomitem->setAltDragString("itemid=");
  
  if (!_privileges->check("ViewCosts"))
  {
    _currentStdCostLit->hide();
    _currentActCostLit->hide();
    _maxCostLit->hide();
    _currentStdCost->hide();
    _currentActCost->hide();
    _maxCost->hide();
  }
  
  connect(omfgThis, SIGNAL(bomsUpdated(int, bool)), SLOT(sFillList(int, bool)));
  _activate->hide();
  _revision->setMode(RevisionLineEdit::Maintain);
  _revision->setType("BOM");
}

BOM::~BOM()
{
  // no need to delete child widgets, Qt does it all for us
}

void BOM::languageChange()
{
  retranslateUi(this);
}

enum SetResponse BOM::set(const ParameterList &pParams)
{
  QVariant param;
  bool     valid;
  
  param = pParams.value("item_id", &valid);
  if (valid)
    _item->setId(param.toInt());
   {
     param = pParams.value("revision_id", &valid);
     if (valid)
       _revision->setId(param.toInt());
   }
  
  param = pParams.value("mode", &valid);
  if (valid)
  {
    if ( (param.toString() == "new") || (param.toString() == "edit") )
    {
      connect(_bomitem, SIGNAL(valid(bool)), _edit, SLOT(setEnabled(bool)));
      connect(_bomitem, SIGNAL(valid(bool)), _expire, SLOT(setEnabled(bool)));
      connect(_bomitem, SIGNAL(valid(bool)), _moveUp, SLOT(setEnabled(bool)));
      connect(_bomitem, SIGNAL(valid(bool)), _moveDown, SLOT(setEnabled(bool)));
      connect(_bomitem, SIGNAL(valid(bool)), _edit, SLOT(setEnabled(bool)));
      connect(_bomitem, SIGNAL(itemSelected(int)), _edit, SLOT(animateClick()));
    }
    
    if (param.toString() == "new")
    {
      _mode = cNew;
      _item->setFocus();
	  _revision->setId(-1);
    }
    else if (param.toString() == "edit")
    {
      _mode = cEdit;
      _item->setReadOnly(TRUE);
      _save->setFocus();
    }
    else if (param.toString() == "view")
    {
      _mode = cView;
      _item->setReadOnly(TRUE);
      _documentNum->setEnabled(FALSE);
      _revision->setEnabled(FALSE);
      _revisionDate->setEnabled(FALSE);
      _batchSize->setEnabled(FALSE);
      _new->setEnabled(FALSE);
      _edit->setEnabled(FALSE);
      _expire->setEnabled(FALSE);
      _moveUp->setEnabled(FALSE);
      _moveDown->setEnabled(FALSE);
      _doRequireQtyPer->setEnabled(FALSE);
      _requiredQtyPer->setEnabled(FALSE);
      _save->setEnabled(FALSE);
      
      connect(_bomitem, SIGNAL(itemSelected(int)), _view, SLOT(animateClick()));
      
      _close->setFocus();
    }
  }
  
  return NoError;
}

void BOM::sSave()
{
  if(!sCheckRequiredQtyPer())
    return;
  
  q.prepare( "SELECT bomhead_id "
             "FROM bomhead "
             "WHERE ((bomhead_item_id=:item_id) "
			 "AND (bomhead_rev_id=:bomhead_rev_id));" );
  q.bindValue(":item_id", _item->id());
  q.bindValue(":bomhead_rev_id", _revision->id());
  q.exec();
  if (q.first())
  {   
    q.prepare( "UPDATE bomhead "
               "SET bomhead_docnum=:bomhead_docnum,"
               "    bomhead_revision=:bomhead_revision, bomhead_revisiondate=:bomhead_revisiondate,"
               "    bomhead_batchsize=:bomhead_batchsize,"
               "    bomhead_requiredqtyper=:bomhead_requiredqtyper "
               "WHERE ((bomhead_item_id=:bomhead_item_id) "
			   "AND (bomhead_rev_id=:bomhead_rev_id));" );
    q.bindValue(":bomhead_item_id", _item->id());
    q.bindValue(":bomhead_rev_id", _revision->id());
  }
  else
  {
    q.prepare( "INSERT INTO bomhead "
               "( bomhead_item_id, bomhead_docnum,"
               "  bomhead_revision, bomhead_revisiondate,"
               "  bomhead_batchsize, bomhead_requiredqtyper ) "
               "VALUES "
               "( :bomhead_item_id, :bomhead_docnum,"
               "  :bomhead_revision, :bomhead_revisiondate, "
               "  :bomhead_batchsize, :bomhead_requiredqtyper ) " );
    q.bindValue(":bomhead_item_id", _item->id());
  }
  
  q.bindValue(":bomhead_docnum", _documentNum->text());
  q.bindValue(":bomhead_revision", _revision->number());
  q.bindValue(":bomhead_revisiondate", _revisionDate->date());
  q.bindValue(":bomhead_batchsize", _batchSize->toDouble());
  if(_doRequireQtyPer->isChecked())
    q.bindValue(":bomhead_requiredqtyper", _requiredQtyPer->text().toDouble());
  q.exec();
  
  close();
}

bool BOM::setParams(ParameterList &pParams)
{
  pParams.append("item_id",     _item->id());
  pParams.append("revision_id", _revision->id());
  pParams.append("push",        tr("Push"));
  pParams.append("pull",        tr("Pull"));
  pParams.append("mixed",       tr("Mixed"));
  pParams.append("error",       tr("Error"));
  pParams.append("always",      tr("'Always'"));
  pParams.append("never",       tr("'Never'"));

  if (_showExpired->isChecked())
  {
    pParams.append("showExpired");
    pParams.append("expiredDays", 999);
  }
  
  if (_showFuture->isChecked())
  {
    pParams.append("showFuture");
    pParams.append("futureDays", 999);
  }
  
  return true;
}

void BOM::sPrint()
{
  ParameterList params;
  setParams(params);
  orReport report("SingleLevelBOM", params);
  if (report.isValid())
    report.print();
  else
    report.reportError(this);
}

void BOM::sPopulateMenu(QMenu *menuThis)
{
  menuThis->insertItem(tr("View"), this, SLOT(sView()), 0);
  
  if ((_mode == cNew) || (_mode == cEdit))
  {
    menuThis->insertItem(tr("Edit"), this, SLOT(sEdit()), 0);
    menuThis->insertItem(tr("Expire"), this, SLOT(sExpire()), 0);
    menuThis->insertItem(tr("Replace"), this, SLOT(sReplace()), 0);
    
    menuThis->insertSeparator();
    
    menuThis->insertItem(tr("Move Up"),   this, SLOT(sMoveUp()), 0);
    menuThis->insertItem(tr("Move Down"), this, SLOT(sMoveDown()), 0);
  }
}

void BOM::sNew()
{
  ParameterList params;
  params.append("mode", "new");
  params.append("item_id", _item->id());
  params.append("revision_id", _revision->id());
  
  bomItem newdlg(this, "", TRUE);
  newdlg.set(params);
  newdlg.exec();
}

void BOM::sEdit()
{
  ParameterList params;
  params.append("mode", "edit");
  params.append("bomitem_id", _bomitem->id());
  
  bomItem newdlg(this, "", TRUE);
  newdlg.set(params);
  newdlg.exec();
}

void BOM::sView()
{
  ParameterList params;
  params.append("mode", "view");
  params.append("bomitem_id", _bomitem->id());
  
  bomItem newdlg(this, "", TRUE);
  newdlg.set(params);
  newdlg.exec();
}

void BOM::sExpire()
{
  q.prepare( "UPDATE bomitem "
             "SET bomitem_expires=CURRENT_DATE "
             "WHERE (bomitem_id=:bomitem_id);" );
  q.bindValue(":bomitem_id", _bomitem->id());
  q.exec();
  
  omfgThis->sBOMsUpdated(_item->id(), TRUE);
}

void BOM::sReplace()
{
  ParameterList params;
  params.append("mode", "replace");
  params.append("bomitem_id", _bomitem->id());
  params.append("revision_id", _revision->id());
  
  bomItem newdlg(this, "", TRUE);
  newdlg.set(params);
  newdlg.exec();
}

void BOM::sMoveUp()
{
  q.prepare("SELECT moveBomitemUp(:bomitem_id) AS result;");
  q.bindValue(":bomitem_id", _bomitem->id());
  q.exec();
  
  omfgThis->sBOMsUpdated(_item->id(), TRUE);
}

void BOM::sMoveDown()
{
  q.prepare("SELECT moveBomitemDown(:bomitem_id) AS result;");
  q.bindValue(":bomitem_id", _bomitem->id());
  q.exec();
  
  omfgThis->sBOMsUpdated(_item->id(), TRUE);
}

void BOM::sFillList()
{
  sFillList(_item->id(), TRUE);
}

void BOM::sFillList(int pItemid, bool)
{
  if (_item->isValid() && (pItemid == _item->id()))
  {
    q.prepare( "SELECT * "
               "FROM bomhead "
               "WHERE ( (bomhead_item_id=:item_id) "
			   "AND (bomhead_rev_id=:revision_id) );" );
    q.bindValue(":item_id", _item->id());
	q.bindValue(":revision_id", _revision->id());
    q.exec();
    if (q.first())
    {
      _documentNum->setText(q.value("bomhead_docnum"));
	  _revision->setNumber(q.value("bomhead_revision").toString());
      _revisionDate->setDate(q.value("bomhead_revisiondate").toDate());
      _batchSize->setDouble(q.value("bomhead_batchsize").toDouble());
      if(q.value("bomhead_requiredqtyper").toDouble()!=0)
      {
        _doRequireQtyPer->setChecked(true);
        _requiredQtyPer->setDouble(q.value("bomhead_requiredqtyper").toDouble());
      }
      if (_revision->description() == "Inactive")
	  {
		  _save->setEnabled(FALSE);
	      _new->setEnabled(FALSE);
		  _documentNum->setEnabled(FALSE);
		  _revisionDate->setEnabled(FALSE);
		  _batchSize->setEnabled(FALSE);
		  _bomitem->setEnabled(FALSE);
	  }

	  if ((_revision->description() == "Pending") || (_revision->description() == "Active"))
	  {
		  _save->setEnabled(TRUE);
	      _new->setEnabled(TRUE);
		  _documentNum->setEnabled(TRUE);
		  _revisionDate->setEnabled(TRUE);
		  _batchSize->setEnabled(TRUE);
		  _bomitem->setEnabled(TRUE);
	  }
    }
    else
    {
      _documentNum->clear();
      _revisionDate->clear();
      _batchSize->clear();
    }
    
    MetaSQLQuery mql( "SELECT bomitem_id, item_id, *,"
                 "       (item_descrip1 || ' ' || item_descrip2) AS item_description,"
                 "       uom_name AS issueuom,"
                 "       CASE WHEN (bomitem_issuemethod = 'S') THEN <? value(\"push\") ?>"
                 "            WHEN (bomitem_issuemethod = 'L') THEN <? value(\"pull\") ?>"
                 "            WHEN (bomitem_issuemethod = 'M') THEN <? value(\"mixed\") ?>"
                 "            ELSE <? value(\"error\") ?>"
                 "       END AS issuemethod,"
                 "       'qtyper' AS bomitem_qtyper_xtnumericrole,"
                 "       'percent' AS bomitem_scrap_xtnumericrole,"
                 "       CASE WHEN (bomitem_effective = startOfTime()) THEN NULL "
                 "            ELSE bomitem_effective END AS effective,"
                 "       CASE WHEN (bomitem_expires = endOfTime()) THEN NULL "
                 "            ELSE bomitem_expires END AS expires,"
                 "       <? literal(\"always\") ?> AS effective_xtnullrole,"
                 "       <? literal(\"never\") ?>  AS expires_xtnullrole,"
                 "       CASE WHEN (bomitem_expires < CURRENT_DATE) THEN 'expired'"
                 "            WHEN (bomitem_effective >= CURRENT_DATE) THEN 'future'"
                 "            WHEN (item_type='M') THEN 'altemphasis'"
                 "       END AS qtforegroundrole "
                 "FROM bomitem(<? value(\"item_id\") ?>,"
                 "             <? value(\"revision_id\") ?>), item, uom "
                 "WHERE ((bomitem_item_id=item_id)"
                 " AND (bomitem_uom_id=uom_id)"
                 "<? if not exists(\"showExpired\") ?>"
                 " AND (bomitem_expires > CURRENT_DATE)"
                 "<? endif ?>"
                 "<? if not exists(\"showFuture\") ?>"
                 " AND (bomitem_effective <= CURRENT_DATE)"
                 "<? endif ?>"
                 ") "
                 "ORDER BY bomitem_seqnumber, bomitem_effective;"
                 );
    ParameterList params;
    setParams(params);
    q = mql.toQuery(params);
    
    _bomitem->populate(q);
    if (q.lastError().type() != QSqlError::NoError)
    {
      systemError(this, q.lastError().databaseText(), __FILE__, __LINE__);
      return;
    }
    
    MetaSQLQuery picklistmql("SELECT item_picklist,"
          "       COUNT(*) AS total,"
          "       COALESCE(SUM(bomitem_qtyper * (1 + bomitem_scrap))) AS qtyper "
          "FROM bomitem(<? value(\"item_id\") ?>,"
          "             <? value(\"revision_id\") ?>), item "
          "WHERE ( (bomitem_item_id=item_id)"
          "<? if not exists(\"showExpired\") ?>"
          " AND (bomitem_expires > CURRENT_DATE)"
          "<? endif ?>"
          "<? if not exists(\"showFuture\") ?>"
          " AND (bomitem_effective <= CURRENT_DATE)"
          "<? endif ?>"
          " ) "
          "GROUP BY item_picklist;");
    q = picklistmql.toQuery(params);
    
    bool   foundPick    = FALSE;
    bool   foundNonPick = FALSE;
    int    totalNumber  = 0;
    double totalQtyPer  = 0.0;
    while (q.next())
    {
      totalNumber += q.value("total").toInt();
      totalQtyPer += q.value("qtyper").toDouble();
      
      if (q.value("item_picklist").toBool())
      {
        foundPick = TRUE;
        _pickNumber->setDouble(q.value("total").toDouble());
        _pickQtyPer->setDouble(q.value("qtyper").toDouble());
      }
      else
      {
        foundNonPick = TRUE;
        _nonPickNumber->setDouble(q.value("total").toDouble());
        _nonPickQtyPer->setDouble(q.value("qtyper").toDouble());
      }
    }
    if (q.lastError().type() != QSqlError::NoError)
    {
      systemError(this, q.lastError().databaseText(), __FILE__, __LINE__);
      return;
    }
    
    if (!foundPick)
    {
      _pickNumber->setDouble(0);
      _pickQtyPer->setDouble(0.0);
    }
    
    if (!foundNonPick)
    {
      _nonPickNumber->setDouble(0);
      _nonPickQtyPer->setDouble(0.0);
    }
    
    _totalNumber->setDouble(totalNumber);
    _totalQtyPer->setDouble(totalQtyPer);
    _totalQtyPerCache = totalQtyPer;
    
    if (_privileges->check("ViewCosts"))
    {
      MetaSQLQuery costsmql("SELECT p.item_maxcost,"
            "       COALESCE(SUM(itemuomtouom(bomitem_item_id, bomitem_uom_id, NULL, bomitem_qtyper * (1 + bomitem_scrap)) * stdCost(c.item_id))) AS stdcost,"
            "       COALESCE(SUM(itemuomtouom(bomitem_item_id, bomitem_uom_id, NULL, bomitem_qtyper * (1 + bomitem_scrap)) * ROUND(actCost(c.item_id),4))) AS actcost "
            "FROM bomitem(<? value(\"item_id\") ?>,"
            "             <? value(\"revision_id\") ?>), item AS c, item AS p "
            "WHERE ( (bomitem_item_id=c.item_id)"
            " AND (p.item_id=<? value(\"item_id\") ?>)"
            "<? if not exists(\"showExpired\") ?>"
            " AND (bomitem_expires > CURRENT_DATE)"
            "<? endif ?>"
            "<? if not exists(\"showFuture\") ?>"
            " AND (bomitem_effective <= CURRENT_DATE)"
            "<? endif ?>"
            " ) "
            "GROUP BY p.item_maxcost;");
      q = costsmql.toQuery(params);
      if (q.first())
      {
        _currentStdCost->setDouble(q.value("stdcost").toDouble());
        _currentActCost->setDouble(q.value("actcost").toDouble());
        _maxCost->setDouble(q.value("item_maxcost").toDouble());
      }
      if (q.lastError().type() != QSqlError::NoError)
      {
        systemError(this, q.lastError().databaseText(), __FILE__, __LINE__);
        return;
      }
    }
  }
  else if (!_item->isValid())
  {
    _documentNum->clear();
    _revision->clear();
    _revisionDate->clear();
    _batchSize->clear();
    
    _bomitem->clear();
  }
}

void BOM::keyPressEvent( QKeyEvent * e )
{
#ifdef Q_WS_MAC
  if(e->key() == Qt::Key_S && e->state() == Qt::ControlModifier)
  {
    _save->animateClick();
    e->accept();
  }
  if(e->isAccepted())
    return;
#endif
  e->ignore();
}

void BOM::sClose()
{
  if(sCheckRequiredQtyPer())
    close();
}

bool BOM::sCheckRequiredQtyPer()
{
  if(cView == _mode || !_doRequireQtyPer->isChecked())
    return true;

  if(_requiredQtyPer->toDouble() != _totalQtyPerCache)
  {
    QMessageBox::warning( this, tr("Total Qty. Per Required"),
      tr("<p>A required total Qty. Per was specified but not met. "
         "Please correct the problem before continuing.") );
    return false;
  }
  
  return true;
}
