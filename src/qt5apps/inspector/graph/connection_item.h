#ifndef MARSYAS_INSPECTOR_GRAPH_CONNECTION_ITEM_INCLUDED
#define MARSYAS_INSPECTOR_GRAPH_CONNECTION_ITEM_INCLUDED

#include <QQuickItem>
#include <QSGGeometry>
#include <QSGNode>

namespace MarsyasQml {

class ConnectionItem : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(QPointF p1 READ p1 WRITE setP1 NOTIFY p1Changed)
  Q_PROPERTY(QPointF p2 READ p2 WRITE setP2 NOTIFY p2Changed)
  Q_PROPERTY(qreal breakpoint READ breakpoint WRITE setBreakpoint NOTIFY breakpointChanged )
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
public:
  ConnectionItem(QQuickItem *parent = 0);
  ~ConnectionItem();

  QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);

  QPointF p1() const { return m_p1; }
  QPointF p2() const { return m_p2; }
  qreal breakpoint() const { return m_breakpoint; }
  QColor color() const { return m_color; }

  void setP1(const QPointF &p);
  void setP2(const QPointF &p);
  void setBreakpoint(qreal);
  void setColor(const QColor &);

signals:
  void p1Changed(const QPointF &p);
  void p2Changed(const QPointF &p);
  void breakpointChanged(qreal);
  void colorChanged(const QColor &);

private:
  void updateGeometry(QSGGeometry * geometry);

  QPointF m_p1;
  QPointF m_p2;
  qreal m_breakpoint;
  bool m_use_breakpoint;
  QColor m_color;

  QSGNode::DirtyState m_dirty;
};

} // namespace MarsyasQml

#endif // MARSYAS_INSPECTOR_GRAPH_CONNECTION_ITEM_INCLUDED
