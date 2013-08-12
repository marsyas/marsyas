#ifndef MARSYASQT_REALVEC_TABLE_WIDGET_INCLUDED
#define MARSYASQT_REALVEC_TABLE_WIDGET_INCLUDED

#include "realvec_item_model.h"
#include <QTableView>

namespace MarsyasQt {

class RealvecTableWidget : public QTableView
{
  Q_OBJECT
public:
  RealvecTableWidget( QWidget * parent = 0 ):
    QTableView(parent),
    m_model(new RealvecModel(this))
  {
    setModel( m_model );
  }

  void setData( const Marsyas::realvec & data )
  {
    m_model->setData(data);
  }

  const Marsyas::realvec & data() const
  {
    return m_model->data();
  }

public slots:
  void setEditable(bool editable) { m_model->setEditable(editable); }

signals:
  void cellEditingFinished();

protected:
  void commitData(QWidget * editor)
  {
    QTableView::commitData(editor);
    emit cellEditingFinished();
  }
  void keyPressEvent(QKeyEvent *event);

private:
  RealvecModel *m_model;
};

} // namespace MarsyasQt

#endif // MARSYASQT_REALVEC_TABLE_WIDGET_INCLUDED
