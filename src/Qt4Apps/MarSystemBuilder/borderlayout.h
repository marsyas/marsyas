/****************************************************************************
**
** Copyright (C) 2004-2005 Trolltech AS. All rights reserved.
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

#ifndef BORDERLAYOUT_H
#define BORDERLAYOUT_H

#include <QLayout>
#include <QRect>
#include <QWidgetItem>

class BorderLayout : public QLayout
{
public:
  enum Position { West, North, South, East, Center };

  BorderLayout(QWidget *parent, int margin = 0, int spacing = -1);
  BorderLayout(int spacing = -1);
  ~BorderLayout();

  void addItem(QLayoutItem *item);
  void addWidget(QWidget *widget, Position position);
  Qt::Orientations expandingDirections() const;
  bool hasHeightForWidth() const;
  int count() const;
  QLayoutItem *itemAt(int index) const;
  QSize minimumSize() const;
  void setGeometry(const QRect &rect);
  QSize sizeHint() const;
  QLayoutItem *takeAt(int index);

  void add(QLayoutItem *item, Position position);

private:
  struct ItemWrapper
  {
    ItemWrapper(QLayoutItem *i, Position p) {
      item = i;
      position = p;
    }

    QLayoutItem *item;
    Position position;
  };

  enum SizeType { MinimumSize, SizeHint };
  QSize calculateSize(SizeType sizeType) const;

  QList<ItemWrapper *> list;
};

#endif
