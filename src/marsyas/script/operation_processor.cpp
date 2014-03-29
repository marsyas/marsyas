#include "operation_processor.hpp"

#include <algorithm>
#include <vector>
#include <string>

using namespace std;

namespace Marsyas {

bool ScriptOperationProcessor::operation::update(const MarControlPtr & cause)
{
  //MRSWARN("Operation update.");

  if (op == NO_OP)
  {
    assert(!value.isInvalid());
    //MRSWARN("Evaluating terminal:" << value);
    return value() == cause();
  }

  assert(left_operand != nullptr);
  assert(right_operand != nullptr);

  bool reevaluate = false;
  reevaluate |= left_operand->update(cause);
  reevaluate |= right_operand->update(cause);
  reevaluate |= value.isInvalid();

  if (!reevaluate) {
    //MRSWARN("Operator: nothing to do.");
    return false;
  }

  const MarControlPtr & a = left_operand->value;
  const MarControlPtr & b = right_operand->value;
  if (a.isInvalid() || b.isInvalid())
  {
    MRSERR("Missing operand values to operator: " << op);
    value = MarControlPtr();
  }
  else
  {
    try {
      switch(op)
      {
      case PLUS_OP:
        value = a + b; break;
      case MINUS_OP:
        value = a - b; break;
      case MULT_OP:
        value = a * b; break;
      case DIV_OP:
        value = a / b; break;
      case EQ_OP:
        value = a == b; break;
      case NEQ_OP:
        value = a != b; break;
      case LESS_OP:
        // NOTE: MarControlPtr::operator< only compares pointers, not values
        // That was probably intended for usage in std::map and similar...
        value = (*a() < *b()); break;
      case MORE_OP:
        value = !(a == b || (*a() < *b())); break;
      case LESSEQ_OP:
        value = (a == b || (*a() < *b())); break;
      case MOREEQ_OP:
        value = !(*a() < *b()); break;
      case WHEN_OP:
        if (value.isInvalid())
          value = *a(); // create new control
        else if ( b() == cause() && b->to<bool>() )
          *value() = *a(); // copy control value
        else
          reevaluate = false;
        break;
      case ON_OP:
        if (value.isInvalid())
          value = *a();  // create new control
        else if ( b() == cause() )
          *value() = *a(); // copy control value
        else
          reevaluate = false;
        break;
      default:
        MRSERR("Unknown operator: " << op);
        value = MarControlPtr();
      }
    }
    catch(std::exception & e)
    {
      MRSERR("Failed operation: " << e.what());
      value = MarControlPtr();
    }
  }

  //MRSWARN("Operator: done.");
  return reevaluate;
}


ScriptOperationProcessor::operator_type
ScriptOperationProcessor::operator_for_text(const std::string op_text)
{
  static vector<string> op_desc = {
    "",  // NO_OP,
    "+", // PLUS_OP,
    "-", // MINUS_OP,
    "*", // MULT_OP,
    "/", // DIV_OP,
    "==", // EQ_OP,
    "!=", // NEQ_OP,
    "<", // LESS_OP,
    ">", // MORE_OP,
    "<=", // LESSEQ_OP,
    ">=", // MOREEQ_OP,
    "when", // WHEN_OP,
    "on" // ON_OP
  };

  vector<string>::const_iterator pos = std::find(op_desc.begin(), op_desc.end(), op_text);
  if (pos == op_desc.end())
    return NO_OP;

  int idx = (int) (pos - op_desc.begin());
  return (operator_type) idx;
}


ScriptOperationProcessor::ScriptOperationProcessor(const std::string & name):
  MarSystem("ScriptOperationProcessor", name),
  m_operation(nullptr)
{
}

ScriptOperationProcessor::ScriptOperationProcessor( const ScriptOperationProcessor & other ):
  MarSystem(other),
  m_operation(nullptr)
{
}

ScriptOperationProcessor::~ScriptOperationProcessor()
{
  delete m_operation;
}

MarSystem *ScriptOperationProcessor::clone() const
{
  return new ScriptOperationProcessor( *this );
}

void ScriptOperationProcessor::setOperation( operation * opn )
{
  clearOperation();

  m_operation = opn;

  if (!m_operation)
    return;

  prepareOperation(m_operation);

  m_operation->update();
  const MarControlPtr & value = m_operation->value;

  if (!value.isInvalid())
  {
    addControl(value->getType() + '/' + "result", *value, m_result);
  }
}

void ScriptOperationProcessor::myUpdate(MarControlPtr cause)
{
#if 0
  cout << "Processing operation!" << endl;
  if (!cause.isInvalid())
  {
    cout << "Triggered by control: ";
    if (cause->getMarSystem())
      cout << cause->getMarSystem()->getAbsPath();
    cout << cause->getName();
    cout << endl;
  }
#endif

  m_operation->update(cause);
  const MarControlPtr & value = m_operation->value;

  if (!value.isInvalid())
  {
    *m_result = *value;
    //cout << "Result = " << *m_result << endl;
  }
}

void ScriptOperationProcessor::myProcess(realvec&, realvec&)
{
  // no-op
}

void ScriptOperationProcessor::prepareOperation( operation *opn )
{
  if (opn->op)
  {
    prepareOperation(opn->left_operand);
    prepareOperation(opn->right_operand);
  }
  else
  {
    assert(!opn->value.isInvalid());

    if (opn->value->getMarSystem())
    {
      MarControlPtr src_control = opn->value;

      string dst_name =
          src_control->getMarSystem()->getAbsPath()
          + src_control->getName();
      std::replace(dst_name.begin(), dst_name.end(), '/', '_');
      dst_name = src_control->getType() + '/' + dst_name;

      MarControlPtr dst_control;
      addControl(dst_name, *src_control, dst_control);
      dst_control->linkTo(src_control, false);
      dst_control->setState(true);

      opn->value = dst_control;

      m_dependencies.push_back( dst_name );
    }
  }
}

void ScriptOperationProcessor::clearOperation()
{
  delete m_operation;
  m_operation = nullptr;

  for (const auto & dep_name : m_dependencies)
  {
    controls_.erase( dep_name );
  }
  m_dependencies.clear();

  if (!m_result.isInvalid())
  {
    controls_.erase( m_result->getName() );
    m_result = MarControlPtr();
  }
}

ScriptStateProcessor::ScriptStateProcessor(const std::string & name):
  MarSystem("ScriptStateProcessor", name)
{
  addControl("mrs_bool/condition", false, m_condition);
  addControl("mrs_bool/inverse", false, m_inverse);
  m_condition->setState(true);
  m_inverse->setState(true);
}

ScriptStateProcessor::ScriptStateProcessor( const ScriptStateProcessor & other ):
  MarSystem(other)
{
  m_condition = getControl("mrs_bool/condition");
  m_inverse = getControl("mrs_bool/inverse");
}

ScriptStateProcessor::~ScriptStateProcessor()
{}

MarSystem *ScriptStateProcessor::clone() const
{
  return new ScriptStateProcessor(*this);
}

void ScriptStateProcessor::addMapping( MarControlPtr & dst, MarControlPtr & src )
{
  m_state.emplace_back( dst, src );
}

void ScriptStateProcessor::myUpdate(MarControlPtr)
{
  bool condition = m_condition->to<bool>();
  bool inverse = m_inverse->to<bool>();
  bool active = condition != inverse;

  if (!active)
    return;

  //cout << "State activated." << endl;

  for( const auto & mapping : m_state )
  {
    const MarControlPtr & dst = mapping.first;
    const MarControlPtr & src = mapping.second;

    if (dst.isInvalid() || src.isInvalid())
      continue;

#if 0
    cout << "..Applying: " << endl;
    cout << "...." << dst->getMarSystem()->getAbsPath() << dst->getName();
    cout << " <- " << *src;
    if (src->getMarSystem())
      cout << " " << src->getMarSystem()->getAbsPath() << src->getName();
    cout << endl;
#endif

    dst->unlinkFromTarget();
    if (src->getMarSystem())
      dst->linkTo(src);
    else
      *dst = *src;
  }
}

void ScriptStateProcessor::myProcess(realvec&, realvec&)
{}

} // namespace Marsyas
