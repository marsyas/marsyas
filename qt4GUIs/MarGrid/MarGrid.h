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

#ifndef MARGRID_H
#define MARGRID_H


#include <QList>
#include <QPoint>
#include <QPixmap>
#include <QWidget>
#include "Collection.h"

class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;
#include "MarSystemManager.h"
#include "MarSystemWrapper.h" 

class MarGrid : public QWidget
{
    Q_OBJECT

public:
    MarGrid(QWidget *parent = 0);
    void clear();

public slots: 
  void extract();
  void predict();
  void train();
  
  
protected:
  void mousePressEvent(QMouseEvent *event);
  void paintEvent(QPaintEvent *event);
  void addFile(int grid_x, int grid_y, std::string filename);
  void resetPredict();
  
  
private:
    int findPiece(const QRect &pieceRect) const;
    const QRect targetSquare(const QPoint &position) const;


  Marsyas::MarSystemManager mng;  
  QVector<QList <std::string> > files;
  QVector<int> counters;
  QVector<int> counterSizes;
  QVector<int> labels;
  
  
  QList<QPixmap> piecePixmaps;
  QList<QRect> pieceRects;
  QList<QPoint> pieceLocations;

  QRect highlightedRect;
  QRect metalRec;
  QRect classicalRec;
  int inPlace;
  MarSystemWrapper*  mwr_;
  Marsyas::MarSystem* pnet_;

  int som_height;
  int som_width;
  int cell_size;

  Marsyas::realvec norm_som_fmatrix;
  Marsyas::MarSystem* som_;
  Marsyas::MarSystem* total_;
  Marsyas::MarSystem* norm_;
  
 
  
};

#endif
