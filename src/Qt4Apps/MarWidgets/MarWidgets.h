/****************************************************************************
**
** Copyright (C) 2005-2005 Trolltech AS. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef MARWIDGETS_H
#define MARWIDGETS_H

#include "MarSystemQtWrapper.h"
#include "common_source.h"
#include "MarSystemManager.h"
#include "MarControlWidget.h"

#include <QScrollArea>
#include <QPushButton>
#include <QCheckBox>
#include <QtOpenGL>


#include <iostream> 
using namespace std;


class MarWidgets : public QScrollArea
{
  Q_OBJECT

public:
  MarWidgets();

  
  void setup();
  
protected:
  
  
private slots:
  
  
private:

  QString strippedName(const QString &fullFileName);
  
  QString curFile;
  bool isUntitled;
  MarSystem* pnet;
  MarSystemQtWrapper* mwr;
  
  
  QPushButton* playButton;
  QPushButton* stepButton;
  // QPushButton* probeButton;
  QCheckBox* probeButton;
  
  QSlider* slider;
  QScrollArea *glWidgetArea;
  
};

#endif
