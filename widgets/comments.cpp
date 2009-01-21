/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2009 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */


#include <QHBoxLayout>
#include <QLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QList>

#include <parameter.h>
#include <xsqlquery.h>

#include "comment.h"
#include "comments.h"


const Comments::CommentMap Comments::_commentMap[] =
  {
    CommentMap( Uninitialized,     " "   ),
    CommentMap( Address,           "ADDR"),
    CommentMap( BBOMHead,          "BBH" ),
    CommentMap( BBOMItem,          "BBI" ),
    CommentMap( BOMHead,           "BMH" ),
    CommentMap( BOMItem,           "BMI" ),
    CommentMap( BOOHead,           "BOH" ),
    CommentMap( BOOItem,           "BOI" ),
    CommentMap( CRMAccount,        "CRMA"),
    CommentMap( Contact,           "T"   ),
    CommentMap( Customer,          "C"   ),
    CommentMap( Employee,          "EMP" ),
    CommentMap( Incident,          "INCDT"),
    CommentMap( Item,              "I"   ),
    CommentMap( ItemSite,          "IS"  ),
    CommentMap( ItemSource,        "IR"  ),
    CommentMap( Location,          "L"   ),
    CommentMap( LotSerial,         "LS"   ),
    CommentMap( Opportunity,       "OPP" ),
    CommentMap( Project,           "J"   ),
    CommentMap( PurchaseOrder,     "P"   ),
    CommentMap( PurchaseOrderItem, "PI"  ),
    CommentMap( ReturnAuth,        "RA"  ),
    CommentMap( ReturnAuthItem,    "RI"  ),
    CommentMap( Quote,             "Q"   ),
    CommentMap( QuoteItem,         "QI"  ),
    CommentMap( SalesOrder,        "S"   ),
    CommentMap( SalesOrderItem,    "SI"  ),
    CommentMap( TransferOrder,     "TO"  ),
    CommentMap( TransferOrderItem, "TI"  ),
    CommentMap( Vendor,            "V"   ),
    CommentMap( Warehouse,         "WH"  ),
    CommentMap( WorkOrder,         "W"   ),
  };

Comments::Comments(QWidget *pParent, const char *name) :
  QWidget(pParent, name)
{
  _source = Uninitialized;
  _sourceid = -1;

  QHBoxLayout *main = new QHBoxLayout(this);
  main->setMargin(0);
  main->setSpacing(7);

  QWidget *buttons = new QWidget(this);
  QVBoxLayout * buttonsLayout = new QVBoxLayout(buttons);
  buttonsLayout->setMargin(0);
  buttonsLayout->setSpacing(0);

  _comment = new XTreeWidget(this);
  _comment->setObjectName("_comment");
  _comment->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  _comment->addColumn(tr("Date/Time"), _timeDateColumn, Qt::AlignCenter,true, "comment_date");
  _comment->addColumn(tr("Type"),    _itemColumn, Qt::AlignCenter,true, "type");
  _comment->addColumn(tr("User"),    _userColumn, Qt::AlignCenter,true, "comment_user");
  _comment->addColumn(tr("Comment"), -1,          Qt::AlignLeft,  true, "first");
  main->addWidget(_comment);

  _newComment = new QPushButton(tr("New"), buttons, "_newComment");
  buttonsLayout->addWidget(_newComment);

  _viewComment = new QPushButton(tr("View"), buttons, "_viewComment");
  _viewComment->setEnabled(FALSE);
  buttonsLayout->addWidget(_viewComment);

  QSpacerItem *_buttonSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
  buttonsLayout->addItem(_buttonSpacer);
  buttons->setLayout(buttonsLayout);
  main->addWidget(buttons);

  setLayout(main);

  connect(_newComment, SIGNAL(clicked()), this, SLOT( sNew()));
  connect(_viewComment, SIGNAL(clicked()), this, SLOT( sView()));
  connect(_comment, SIGNAL(valid(bool)), _viewComment, SLOT(setEnabled(bool)));
  connect(_comment, SIGNAL(itemSelected(int)), _viewComment, SLOT(animateClick()));

  setFocusProxy(_comment);
}

void Comments::setType(enum CommentSources pSource)
{
  _source = pSource;
}

void Comments::setId(int pSourceid)
{
  _sourceid = pSourceid;
  refresh();
}

void Comments::setReadOnly(bool pReadOnly)
{
  if (pReadOnly)
    _newComment->setEnabled(FALSE);
  else
    _newComment->setEnabled(TRUE);
}

void Comments::sNew()
{ 
  ParameterList params;
  params.append("mode", "new");
  params.append("sourceType", _source);
  params.append("source_id", _sourceid);

  comment newdlg(this, "", TRUE);
  newdlg.set(params);

  if (newdlg.exec() != QDialog::Rejected)
  {
    emit commentAdded();
    refresh();
  }
}

void Comments::sView()
{
  ParameterList params;
  params.append("mode", "view");
  params.append("comment_id", _comment->id());
  params.append("commentIDList", _commentIDList);

  comment newdlg(this, "", TRUE);
  newdlg.set(params);
  newdlg.exec();
}

void Comments::refresh()
{
  if(-1 == _sourceid)
  {
    _comment->clear();
    return;
  }

  XSqlQuery comment;
  if(_source != CRMAccount)
  {
    comment.prepare( "SELECT comment_id, comment_date,"
                     "       CASE WHEN (cmnttype_name IS NOT NULL) THEN cmnttype_name"
                     "            ELSE :none"
                     "       END AS type,"
                     "       comment_user,"
                     "       firstLine(detag(comment_text)) AS first "
                     "FROM comment LEFT OUTER JOIN cmnttype ON (comment_cmnttype_id=cmnttype_id) "
                     "WHERE ( (comment_source=:source)"
                     " AND (comment_source_id=:sourceid) ) "
                     "ORDER BY comment_date;" );
  }
  else
  {
    // If it's CRMAccount we want to do some extra joining in our SQL
    comment.prepare( "SELECT comment_id, comment_date,"
                     "       CASE WHEN (cmnttype_name IS NOT NULL) THEN cmnttype_name"
                     "            ELSE :none"
                     "       END AS type,"
                     "       comment_user,"
                     "       firstLine(detag(comment_text)) AS first "
                     "  FROM comment LEFT OUTER JOIN cmnttype ON (comment_cmnttype_id=cmnttype_id) "
                     " WHERE((comment_source=:source)"
                     "   AND (comment_source_id=:sourceid) ) "
                     " UNION "
                     "SELECT comment_id, comment_date,"
                     "       CASE WHEN (cmnttype_name IS NOT NULL) THEN cmnttype_name"
                     "            ELSE :none"
                     "       END,"
                     "       comment_user, firstLine(detag(comment_text)) "
                     "  FROM crmacct, comment LEFT OUTER JOIN cmnttype ON (comment_cmnttype_id=cmnttype_id) "
                     " WHERE((comment_source=:sourceCust)"
                     "   AND (crmacct_id=:sourceid)"
                     "   AND (comment_source_id=crmacct_cust_id) ) "
                     " UNION "
                     "SELECT comment_id, comment_date,"
                     "       CASE WHEN (cmnttype_name IS NOT NULL) THEN cmnttype_name"
                     "            ELSE :none"
                     "       END,"
                     "       comment_user, firstLine(detag(comment_text)) "
                     "  FROM crmacct, comment LEFT OUTER JOIN cmnttype ON (comment_cmnttype_id=cmnttype_id) "
                     " WHERE((comment_source=:sourceVend)"
                     "   AND (crmacct_id=:sourceid)"
                     "   AND (comment_source_id=crmacct_vend_id) ) "
                     " UNION "
                     "SELECT comment_id, comment_date,"
                     "       CASE WHEN (cmnttype_name IS NOT NULL) THEN cmnttype_name"
                     "            ELSE :none"
                     "       END,"
                     "       comment_user, firstLine(detag(comment_text)) "
                     "  FROM cntct, comment LEFT OUTER JOIN cmnttype ON (comment_cmnttype_id=cmnttype_id) "
                     " WHERE((comment_source=:sourceContact)"
                     "   AND (cntct_crmacct_id=:sourceid)"
                     "   AND (comment_source_id=cntct_id) ) "
                     "ORDER BY comment_date;" );
    comment.bindValue(":sourceCust", _commentMap[Customer].ident);
    comment.bindValue(":sourceContact", _commentMap[Contact].ident);
    comment.bindValue(":sourceVend", _commentMap[Vendor].ident);
  }
  comment.bindValue(":none", tr("None"));
  comment.bindValue(":source", _commentMap[_source].ident);
  comment.bindValue(":sourceid", _sourceid);
  comment.exec();

  _commentIDList.clear();
  while(comment.next())
  {
    _commentIDList.push_back(comment.value("comment_id").toInt());
  }

  comment.first();
  _comment->populate(comment);
}
