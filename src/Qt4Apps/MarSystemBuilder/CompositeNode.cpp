/***********************************
  AggregateMarSystemNode.cpp - IMplementation file.
  Simple enough class represents a group of one or more MarSystemNodes.
  Mar ONly Repesent Fanouts... But Will defintely represent  Fanouts
 ***********************************/
#include "CompositeNode.h"

/**
 * Abstract Constructor I am not sure if it should do anything in
 * particular.
 * Takes in the parameters required to create a marwidget.
 */
CompositeNode::CompositeNode(string type,QString name,QWidget* parent)
  :MarSystemNode(type,name,parent)
{
  //Any Default behaviour?
}

CompositeNode::CompositeNode(MarSystem* msys, QWidget* parent)
  :MarSystemNode(msys,parent)
{
  //Any Default behaviour
}

/**
 * Adds a MarSystemNode to the back of the list.
 * This abstract class has no graphical component so we don't need to
 * worry about anythign else.
 * Returns True if the addtion was a success false otherwise.
 */
bool
CompositeNode::append(MarSystemNode* newTail)
{
  //First make sure we don't already have this MarSystemNode (Adding the
  //same mar widget seems like a mistake
  if(!isMember(newTail)) {
    nodes_.push_back(newTail);
    dynamic_cast<MarSystem*>(represents_)->addMarSystem(newTail->getSystem());
    return true;
  } else {
    return false;
  }
}

/**
 * Adds to a specific index of the Collection (or the end if the index
 * is too large.
 * Returns True if the addition was a success.  False otherwise
 */
bool
CompositeNode::insert(int index, MarSystemNode* newWidget)
{
  if(!isMember(newWidget)) {
    //Comparison is between unsigned and signed int which one is unsigned
    if(index>=nodes_.size()) {
      return false;//Cannot add beyond the end of the
    } else {
      //TODO make sure this works without an issue
      vector<MarSystemNode*>::iterator itr=nodes_.begin();
      int i=0;
      while(i<index && itr!=nodes_.end()) {
        i++;
        itr++;
      }
      nodes_.insert(itr,newWidget);
    }
    return true;
  } else {
    return false;
  }
}

/**
 * Adds to the collection before the given MarSystemNode*
 * If the MarSystemNode* is not in the collection then the function does nothing.
 */
bool
CompositeNode::insert(MarSystemNode* before,MarSystemNode* newWidget)
{
  if(!isMember(newWidget)) {
    vector<MarSystemNode*>::iterator itr=nodes_.begin();
    for(; itr!=nodes_.end(); itr++) {
      if(*itr==before) {
        nodes_.insert(itr,newWidget);
      }
    }
    return true;
  } else {
    return false;
  }
}

/**
 * Checks if the MarSystemNode is a member of the vecotr already
 */
bool
CompositeNode::isMember(MarSystemNode* findMe)
{
  vector<MarSystemNode*>::iterator itr;
  for(itr=nodes_.begin(); itr!=nodes_.end(); itr++) {
    if(findMe==*itr)return true;
  }
  return false;

}

/**
 * All Classes that inherit from this class will be considered Collections
 */
bool
CompositeNode::isCollection()
{
  return true;
}

/**
 * CompositeNodes need to tell additions to them what widget to
 * draw themselvs on.
 */
QWidget*
CompositeNode::getChildrenCanvas()
{
  return dynamic_cast<QWidget*>(parent());
}

void
CompositeNode::handleChildResize(int x,int y, int w, int h)
{
  //This class will do nothing with it
}

/**happened after the constructor of the
derived class finished. At this point the virtual metaObject
method (defined by the Q_OBJECT macro) upon invocation returns
a reference to your derived class' metaobject instance. This way
connect finds slotX as provided by the second metaObject first
and binds that to the signal. If that metaobject wouldn't provide
that slot it would search recursively upwards in the hierarchy of
metaobjects for the given object (and probably find slot
 * CompositeNode sets next instead of marChild.
 * TODO might remove this DOn't think Fanout MarSystemNode is using

//void
//CompositeNode::setChild(MarSystemNode* child)
//{
//  next=child;
//}
 */
