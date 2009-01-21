/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2009 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "xdialog.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QWorkspace>
#include <QSettings>
#include <QCloseEvent>
#include <QShowEvent>
#include <QDebug>

#include "guiclient.h"
#include "scripttoolbox.h"

//
// XDialogPrivate
//
class XDialogPrivate
{
  friend class XDialog;

  public:
    XDialogPrivate();
    ~XDialogPrivate();

    bool _shown;
    QScriptEngine * _engine;
    QAction *_rememberPos;
    QAction *_rememberSize;
};

XDialogPrivate::XDialogPrivate()
{
  _shown = false;
  _engine = 0;
  _rememberPos = 0;
  _rememberSize = 0;
}

XDialogPrivate::~XDialogPrivate()
{
  if(_engine)
    delete _engine;
  if(_rememberPos)
    delete _rememberPos;
  if(_rememberSize)
    delete _rememberSize;
}

XDialog::XDialog(QWidget * parent, Qt::WindowFlags flags)
  : QDialog(parent, flags)
{
  connect(this, SIGNAL(destroyed(QObject*)), omfgThis, SLOT(windowDestroyed(QObject*)));
  _private = new XDialogPrivate();
  ScriptToolbox::setLastWindow(this);
}

XDialog::XDialog(QWidget * parent, const char * name, bool modal, Qt::WindowFlags flags)
  : QDialog(parent, flags)
{
  if(name)
    setObjectName(name);
  if(modal)
    setModal(modal);

  connect(this, SIGNAL(destroyed(QObject*)), omfgThis, SLOT(windowDestroyed(QObject*)));

  _private = new XDialogPrivate();
  ScriptToolbox::setLastWindow(this);
}

XDialog::~XDialog()
{
  if(_private)
    delete _private;
}

void XDialog::done(int r)
{
  QSettings settings(QSettings::UserScope, "OpenMFG.com", "OpenMFG");
  settings.setValue(objectName() + "/geometry/size", size());
  settings.setValue(objectName() + "/geometry/pos", pos());

  QDialog::done(r);
}

void XDialog::showEvent(QShowEvent *event)
{
  if(!_private->_shown)
  {
    _private->_shown = true;

    QRect availableGeometry = QApplication::desktop()->availableGeometry();

    QSettings settings(QSettings::UserScope, "OpenMFG.com", "OpenMFG");
    QString objName = objectName();
    QPoint pos = settings.value(objName + "/geometry/pos").toPoint();
    QSize lsize = settings.value(objName + "/geometry/size").toSize();

    if(lsize.isValid() && settings.value(objName + "/geometry/rememberSize", true).toBool())
      resize(lsize);

    // do I want to do this for a dialog?
    //_windowList.append(w);
    QRect r(pos, size());
    if(!pos.isNull() && availableGeometry.contains(r) && settings.value(objName + "/geometry/rememberPos", true).toBool())
      move(pos);

    _private->_rememberPos = new QAction(tr("Remember Posisition"), this);
    _private->_rememberPos->setCheckable(true);
    _private->_rememberPos->setChecked(settings.value(objectName() + "/geometry/rememberPos", true).toBool());
    connect(_private->_rememberPos, SIGNAL(triggered(bool)), this, SLOT(setRememberPos(bool)));
    _private->_rememberSize = new QAction(tr("Remember Size"), this);
    _private->_rememberSize->setCheckable(true);
    _private->_rememberSize->setChecked(settings.value(objectName() + "/geometry/rememberSize", true).toBool());
    connect(_private->_rememberSize, SIGNAL(triggered(bool)), this, SLOT(setRememberSize(bool)));

    addAction(_private->_rememberPos);
    addAction(_private->_rememberSize);
    setContextMenuPolicy(Qt::ActionsContextMenu);

    QStringList parts = objectName().split(" ");
    QStringList search_parts;
    QString oName;
    while(!parts.isEmpty())
    {
      search_parts.append(parts.takeFirst());
      oName = search_parts.join(" ");

      // load and run an QtScript that applies to this window
      qDebug() << "Looking for a script on window " << oName;
      q.prepare("SELECT script_source, script_order"
                "  FROM script"
                " WHERE((script_name=:script_name)"
                "   AND (script_enabled))"
                " ORDER BY script_order;");
      q.bindValue(":script_name", oName);
      q.exec();
      while(q.next())
      {
        QString script = q.value("script_source").toString();
        if(!_private->_engine)
        {
          _private->_engine = new QScriptEngine();
          omfgThis->loadScriptGlobals(_private->_engine);
          QScriptValue mywindow = _private->_engine->newQObject(this);
          _private->_engine->globalObject().setProperty("mywindow", mywindow);
        }
  
        QScriptValue result = _private->_engine->evaluate(script);
        if (_private->_engine->hasUncaughtException())
        {
          int line = _private->_engine->uncaughtExceptionLineNumber();
          qDebug() << "uncaught exception at line" << line << ":" << result.toString();
        }
      }
    }
  }
  QDialog::showEvent(event);
}

void XDialog::setRememberPos(bool b)
{
  QSettings settings(QSettings::UserScope, "OpenMFG.com", "OpenMFG");
  settings.setValue(objectName() + "/geometry/rememberPos", b);
  if(_private && _private->_rememberPos)
    _private->_rememberPos->setChecked(b);
}

void XDialog::setRememberSize(bool b)
{
  QSettings settings(QSettings::UserScope, "OpenMFG.com", "OpenMFG");
  settings.setValue(objectName() + "/geometry/rememberSize", b);
  if(_private && _private->_rememberSize)
    _private->_rememberSize->setChecked(b);
}
