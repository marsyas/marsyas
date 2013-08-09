#include "SquareRenderArea.h"


SquareRenderArea::SquareRenderArea(QWidget *parent):QWidget(parent)
{
  squarePaintSize = new double[0];
  squareDensity = new int[0];
  squareSize = 0;
  totalFiles = 0;
}

void SquareRenderArea::updateSquare(int * genreDensity, int numGenres)
{
  totalFiles = 0;
  squareDensity = genreDensity;
  squareSize = numGenres;

  delete squarePaintSize;
  squarePaintSize = new double[squareSize];
  for(int i = 0; i < squareSize; i++)
    totalFiles += squareDensity[i];
  repaint();
}

void SquareRenderArea::paintEvent(QPaintEvent*)
{
  QPainter painter;
  painter.begin(this);
  int height = this->size().height();
  int width = this->size().width();

  // Draw the border
  // x1,y1,x2,y2
  QLine top(0,0,width,0);
  QLine bottom(0,height-1,width,height-1);
  QLine lSide(0,0,0,height);
  QLine rSide(width-1,0,width-1,height);

  painter.drawLine(top);
  painter.drawLine(bottom);
  painter.drawLine(lSide);
  painter.drawLine(rSide);

  //find the % of the square each genre gets
  for(int i = 0; i < squareSize; i++)
  {
    squarePaintSize[i] = squareDensity[i] /  (1.0 * totalFiles );
  }
  int startHeight = 0;

  /*
  * index - genre - color
  * 0 - blues - Qt::blue
  * 1 - classical - Qt::darkRed
  * 2 - country - Qt::green
  * 3 - disco - PURPLE
  * 4 - hiphop - Qt::yellow
  * 5 - jazz - Qt::darkGreen
  * 6 - metal - BROWN
  * 7 - reggae - PINK
  * 8 - rock - ORANGE
  * 9 - pop - Qt::gray
  */

  for(int i = 0; i < squareSize; i++)
  {
    QColor color;
    switch(i)
    {
    case 0:
      color = QColor(Qt::blue);
      break;
    case 1:
      color = QColor(Qt::darkRed);
      break;
    case 2:
      color = QColor(Qt::green);
      break;
    case 3:
      color = QColor(PURPLE);
      break;
    case 4:
      color = QColor(Qt::yellow);
      break;
    case 5:
      color = QColor(Qt::darkGreen);
      break;
    case 6:
      color = QColor(BROWN);
      break;
    case 7:
      color = QColor(PINK);
      break;
    case 8:
      color = QColor(ORANGE);
      break;
    case 9:
      color = QColor(Qt::gray);
      break;
    }

    painter.setBrush(color);
    QRect rect(1, startHeight, width - 2, squarePaintSize[i] * height);
    painter.drawRect(rect);
    startHeight += squarePaintSize[i]* height;
  }

  /*

  QPainter painter;
  painter.begin(this);

  			if(colourMapMode_)
  			{


  				int numSongs = -1;
  				int maxIndex = -1;
  				int * genreDensity = grid_->getDensity(k);
  				for(int m = 0; m < 10; m++)
  				{
  					if( genreDensity[m] > numSongs)
  					{
  						maxIndex = m;
  						numSongs = genreDensity[m];
  					}
  				}
  				QColor * color;
  				switch(maxIndex)
  				{
  				case 0:
  					color = new QColor(Qt::blue);
  					break;
  				case 1:
  					color = new QColor(Qt::darkRed);
  					break;
  				case 2:
  					color = new QColor(Qt::green);
  					break;
  				case 3:
  					color = new QColor(PURPLE);
  					break;
  				case 4:
  					color = new QColor(Qt::yellow);
  					break;
  				case 5:
  					color = new QColor(Qt::darkGreen);
  					break;
  				case 6:
  					color = new QColor(BROWN);
  					break;
  				case 7:
  					color = new QColor(PINK);
  					break;
  				case 8:
  					color = new QColor(ORANGE);
  					break;
  				case 9:
  					color = new QColor(Qt::gray);
  					break;
  				default:
  					cerr << "Problem with colour select" << endl;
  				}
  				painter.setBrush(*color);


  			}
  		}
  		painter.setPen(Qt::NoPen);
  		painter.drawRect(myr);

  		if(colourMapMode_)
  		{
  			painter.setPen(Qt::red);
  		}
  		else
  		{
  			painter.setPen(Qt::black);
  		}
  		painter.drawLine(myl1);
  		painter.drawLine(myl2);

  		painter.setBrush(Qt::red);
  		QRect newr( grid_->getXPos() * _cellSize + _cellSize / 4,
  			grid_->getYPos() * _cellSize + _cellSize / 4,
  			_cellSize - _cellSize / 2,
  			_cellSize-_cellSize / 2);
  		painter.drawRect(newr);
  	}
  }
  // Draw an 'i' in all initilized squares
  for(int i = 0; i < squareHasInitialized.size(); i++)
  {
  	if(squareHasInitialized[i])
  	{
  		int y = i / grid_->getHeight();
  		int x = i % grid_->getHeight();
  		int cellSize = grid_->getCellSize();
  		painter.setBrush(Qt::green);
  		QFont *font = new QFont();
  		font->setPointSize(16);
  		font->setStyleHint(QFont::OldEnglish);
  		painter.setFont(*font);
  		painter.drawText(x * cellSize, y * cellSize, cellSize, cellSize, Qt::AlignCenter, "i");
  	}
  }

  delete map;
  painter.end();

  */
}
