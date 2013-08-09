/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#define is_root(_ND) (_ND->m_id==1)
#define odd_id(_ND) (_ND->m_id & 1)
#define is_lchild(_ND) ((_ND!=NULL)&&!(is_root(_ND))&&(!odd_id(_ND)))
#define is_rchild(_ND) ((_ND!=NULL)&&!(is_root(_ND))&&( odd_id(_ND)))

//only  relevant for WIN32 MSVC (and ignored by all other platforms)
//For more info about the reason for this #pragma consult:
//http://msdn2.microsoft.com/en-us/library/sa28fef8.aspx


/* This is heap is designed to sort pointers to objects.
   The heap template requires a Type, a Comparator object for determining Type
   ordering, and a Destructor object for destroying Type objects still in the
   heap once the program finishes.

   Examples of Comparator and Destructor objects for Heap declaration:

    Heap<Object, Comp, Destr> h;

    class Comp {
    public:
        bool operator()(Object* a, Object* b) { return (a->m_id()) < (b->m_id()); }
    };
    class Destr {
    public:
        void operator()(Object* a) { delete(a); }
    };
*/

namespace Marsyas
{
/**
	\ingroup Notmar
	\brief Heap used by the scheduler for sorting scheduled event objects.
	\author Neil Burroughs  inb@cs.uvic.ca
*/

template <typename Type, typename Comparator>
class Heap {
private:
  class Node {
  public:
    Node* parent; Node* lchild; Node* rchild; // tree pointers
    Node* prev; Node* next; // vector list pointers
    unsigned int m_id; // node id, for determining child type
    Type* data;
    Node(unsigned int node_id, Type* d) {
      parent=NULL; lchild=NULL; rchild=NULL;
      prev=NULL; next=NULL;
      data=d; m_id=node_id;
    }
    ~Node() { }
    friend std::ostream& operator<<(std::ostream& o, Node* s) {
      o << "<" << s->m_id << "," << s->data << ",(";
      if (s->parent==NULL) { o<<"0"; } else { o<<s->parent->m_id; } o << ",";
      if (s->lchild==NULL) { o<<"x"; } else { o<<s->lchild->m_id; } o << ",";
      if (s->rchild==NULL) { o<<"x"; } else { o<<s->rchild->m_id; } o << ")>";
      return o;
    };
  };

public:

  Node* first; Node* last; // first and last pointers in heap
  // assigned to a node to identify root, l/r child.
  // An empty tree has a count of 0, while the root node is always 1.
  unsigned int node_count;
  // name declaration for required Comparator object
  Comparator cmp;

  Heap() { first=NULL; last=NULL; node_count=0; }
  virtual ~Heap() {
    while(first!=NULL) {
      last=first->next;
      // use the supplied Destructor object to delete the data
      delete(first->data); delete(first);
      first=last;
    }
  }

  bool empty() { return (node_count==0); };

  Type* top() {
    // on empty heap throw a const char* exception, specified in the contract
    if (first==NULL) { throw "Heap::top()  empty heap exception."; }
    else { return first->data; }
  };
  Type* pop() {
    // on empty heap throw a const char* exception, specified in the contract
    if (first==NULL) { throw "Heap::pop()  empty heap exception."; }
    // save top data
    Type* data = first->data;
    // if it's the root then clear the heap
    if (is_root(last)) {
      delete(last); first=NULL; last=NULL; node_count=0;
    }
    else {
      // swap last element data into top position
      first->data = last->data;
      // extricate the node from its parent
      if (is_lchild(last)) { last->parent->lchild=NULL; }
      else { last->parent->rchild=NULL; }
      // remove the last node and update the pointer to the new last
      last=last->prev; delete(last->next); last->next=NULL;
      // bubble down
      Node* f = first;
      while (1) {
        Node* c = f->lchild;
        // if lchild of c==NULL then c cannot be bubbled down further
        if (c==NULL) { break; }
        // make f point to the smallest of c's children
        if (f->rchild!=NULL && (cmp(f->rchild->data,c->data))) { c=f->rchild; }
        // use the template required Comparator to compare if the
        // smallest child of c is less, if not leave
        if (cmp(f->data,c->data)) { break; }
        // swap data
        Type* sw=c->data; c->data=f->data; f->data=sw;
        // track the bubbling node
        f=c;
      }
      // update node_count ensuring it never goes below 0
      node_count = (node_count>0) ? node_count-1 : 0;
    }
    return data;
  };
  // push
  void push(Type* data) {
    // could throw an exception here...hmm
    if (data==NULL) { return; }
    node_count++;

    Node* n = new Node(node_count,data);
    if (first==NULL) {
      first=n; last=n;
    }
    else {
      // add node to end of list
      last->next=n; n->prev=last;
      // insert node into tree and update parent and parent_children
      if (is_root(last)) {
        n->parent=last;
        last->lchild = n;
      }
      else if (is_lchild(last)) {
        n->parent=last->parent;
        last->parent->rchild=n;
      }
      else {
        n->parent=last->parent->next;
        last->parent->next->lchild=n;
      }
      last=n;
      // bubble up if needed
      while (!is_root(n) && cmp(n->data,n->parent->data)) {
        Type* sw=n->data; n->data=n->parent->data; n->parent->data=sw;
        n = n->parent;
      }
    }
  };
  friend std::ostream& operator<<(std::ostream& o, Heap& s) {
    Node* c = s.first;
    while (c!=NULL) { o << c; c = c->next; } o << "\n";
    return o;
  };
};

}//namespace Marsyas

//only  relevant for WIN32 MSVC (and ignored by all other platforms)
//For more info about the reason for this #pragma consult:
//http://msdn2.microsoft.com/en-us/library/sa28fef8.aspx

