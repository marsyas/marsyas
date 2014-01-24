#ifndef MARSYAS_SCRIPT_SYNTAX_TREE_INCLUDED
#define MARSYAS_SCRIPT_SYNTAX_TREE_INCLUDED

#include <vector>
#include <string>

namespace Marsyas {

enum node_tag
{
  GENERIC_NODE,

  INCLUDE_DIRECTIVE,

  ACTOR_NODE,
  PROTOTYPE_NODE,
  CONTROL_DEF_NODE, // { append-spec, access-spec, name, value }
  CONTROL_ASSIGNMENT_NODE, // { path, value }
  STATE_NODE,
  ID_NODE,

  BOOL_NODE,
  INT_NODE,
  REAL_NODE,
  STRING_NODE,
  MATRIX_NODE,
  OPERATION_NODE
};

// This class always treats copying as moving, due to operational semantics
// of parsing: whenever a semantic value is copied, the older value becomes
// obsolete.

struct node
{
  node(): tag(GENERIC_NODE) {}

  node(node_tag t): tag(t) {}

  node( const node & other):
    tag(other.tag),
    v(other.v),
    s(std::move(other.s)),
    components(std::move(other.components))
  {}

  node( const node && other ):
    tag(other.tag),
    v(other.v),
    s(std::move(other.s)),
    components(std::move(other.components))
  {}

  node( bool b ):
    tag(BOOL_NODE)
  {
    v.b = b;
  }

  node ( long i ):
    tag(INT_NODE)
  {
    v.i = i;
  }

  node ( double r ):
    tag(REAL_NODE)
  {
    v.r = r;
  }

  node ( const std::string & other_s ):
    tag(STRING_NODE),
    s(other_s)
  {}

  void operator=( const node & other )
  {
    *this = std::move(other);
  }

  void operator=( const node && other )
  {
    tag = other.tag;
    v = other.v;
    s = std::move(other.s);
    components = std::move(other.components);
  }

  void set_operation( const node & lhs, const std::string & op, const node & rhs )
  {
    s = op;

    components.clear();
    components.push_back(lhs);
    components.push_back(rhs);

    tag = OPERATION_NODE;
  }

  node_tag tag;

  union {
    bool b;
    double r;
    long i;
  } v;

  std::string s;

  std::vector<node> components;
};

} // namespace Marsyas

#endif // MARSYAS_SCRIPT_SYNTAX_TREE_INCLUDED
