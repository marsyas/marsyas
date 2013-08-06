#include "../common/realvec_table_widget.h"

#include <QToolButton>

class RealvecView : public QToolButton
{
  Q_OBJECT
public:
  RealvecView( const QString & title ):
    m_table(new MarsyasQt::RealvecTableWidget(this))
  {
    m_table->setWindowFlags(Qt::Window);
    m_table->setWindowTitle(title);
    m_table->setEditable(false);

    setText("...");
    connect(this, SIGNAL(clicked()), m_table, SLOT(show()));
    connect(m_table, SIGNAL(cellEditingFinished()), this, SIGNAL(cellEditingFinished()));
  }

  void setData( const Marsyas::realvec & d ) { m_table->setData(d); }
  const Marsyas::realvec & data() { return m_table->data(); }

signals:
  void cellEditingFinished();

private:
  MarsyasQt::RealvecTableWidget *m_table;
};
