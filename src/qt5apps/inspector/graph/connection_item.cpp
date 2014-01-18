#include "connection_item.h"

#include <QSGNode>
#include <QSGFlatColorMaterial>

namespace MarsyasQml {

ConnectionItem::ConnectionItem(QQuickItem *parent):
  QQuickItem(parent),
  m_p1(0, 0),
  m_p2(1, 0),
  m_breakpoint(0),
  m_use_breakpoint(false),
  m_color(QColor(0,0,0)),
  m_dirty(0)
{
  setFlag(ItemHasContents, true);
}


ConnectionItem::~ConnectionItem()
{
}

void ConnectionItem::setP1(const QPointF &p)
{
  if (p == m_p1)
    return;

  m_p1 = p;
  emit p1Changed(p);

  m_dirty |= QSGNode::DirtyGeometry;
  update();
}


void ConnectionItem::setP2(const QPointF &p)
{
  if (p == m_p2)
    return;

  m_p2 = p;
  emit p2Changed(p);

  m_dirty |= QSGNode::DirtyGeometry;
  update();
}

void ConnectionItem::setBreakpoint(qreal value)
{
  if (m_use_breakpoint && m_breakpoint == value)
    return;

  m_breakpoint = value;
  m_use_breakpoint = true;
  emit breakpointChanged(value);

  m_dirty |= QSGNode::DirtyGeometry;
  update();
}

void ConnectionItem::setColor(const QColor &color)
{
  if (color == m_color)
    return;

  m_color = color;
  emit colorChanged(color);

  m_dirty |= QSGNode::DirtyMaterial;
  update();
}

QSGNode *ConnectionItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
  QSGGeometryNode *node = 0;
  QSGGeometry *geometry = 0;

  if (!oldNode)
  {
    node = new QSGGeometryNode;

    geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), m_use_breakpoint ? 4 : 2);
    geometry->setLineWidth(2);
    geometry->setDrawingMode(GL_LINE_STRIP);
    updateGeometry(geometry);
    node->setGeometry(geometry);
    node->setFlag(QSGNode::OwnsGeometry);

    QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
    material->setColor(m_color);
    node->setMaterial(material);
    node->setFlag(QSGNode::OwnsMaterial);
  }
  else
  {
    node = static_cast<QSGGeometryNode *>(oldNode);
    if (m_dirty & QSGNode::DirtyGeometry)
    {
      geometry = node->geometry();
      geometry->allocate(m_use_breakpoint ? 4 : 2);
      updateGeometry(geometry);
    }
    if (m_dirty & QSGNode::DirtyMaterial)
    {
      QSGFlatColorMaterial *material = static_cast<QSGFlatColorMaterial*>(node->material());
      material->setColor(m_color);
    }
    node->markDirty(m_dirty);
  }

  m_dirty = 0;

  return node;
}

void ConnectionItem::updateGeometry(QSGGeometry * geometry)
{
  QSGGeometry::Point2D *vertices = geometry->vertexDataAsPoint2D();
  int idx = 0;
  vertices[idx].set(m_p1.x(), m_p1.y());
  if (m_use_breakpoint)
  {
    vertices[++idx].set(m_p1.x(), m_breakpoint);
    vertices[++idx].set(m_p2.x(), m_breakpoint);
  }
  vertices[++idx].set(m_p2.x(), m_p2.y());
}

} // namespace MarsyasQml
