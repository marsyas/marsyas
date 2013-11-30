#include "operation_processor.hpp"

#include <algorithm>
#include <vector>
#include <string>

using namespace std;

namespace Marsyas {


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
    ">=" // MOREEQ_OP
  };

  vector<string>::const_iterator pos = std::find(op_desc.begin(), op_desc.end(), op_text);
  if (pos == op_desc.end())
    return NO_OP;

  int idx = (int) (pos - op_desc.begin());
  return (operator_type) idx;
}


ScriptOperationProcessor::ScriptOperationProcessor(const std::string & name):
  MarSystem("ScriptOperationProcessor", name)
{
  initControls();
}

ScriptOperationProcessor::ScriptOperationProcessor( const ScriptOperationProcessor & other ):
  MarSystem(other)
{
  initControls();
}

ScriptOperationProcessor::~ScriptOperationProcessor()
{
  delete m_operation;
}

MarSystem *ScriptOperationProcessor::clone() const
{
  return new ScriptOperationProcessor( *this );
}

void ScriptOperationProcessor::initControls()
{
}

void ScriptOperationProcessor::setOperation( operation * opn )
{
  clearOperation();

  m_operation = opn;

  if (!m_operation)
    return;

  prepareOperation(m_operation);

  MarControlPtr value = evaluateOperation(m_operation);

  if (!value.isInvalid())
  {
    addControl(value->getType() + '/' + "result", *value, m_result);
    m_result->setState(true);
  }
}

void ScriptOperationProcessor::myUpdate(MarControlPtr)
{
#if 0
  cout << "Processing operation!" << endl;
  if (!sender.isInvalid())
  {
    cout << "Triggered by control: ";
    if (sender->getMarSystem())
      cout << sender->getMarSystem()->getAbsPath();
    cout << sender->getType() << '/' << sender->getName();
    cout << endl;
  }
#endif

  MarControlPtr value = evaluateOperation(m_operation);

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

      m_dependencies.push_back(dst_name);
    }
  }
}

MarControlPtr ScriptOperationProcessor::evaluateOperation( operation *opn )
{
  if (opn->op)
  {
    MarControlPtr a = evaluateOperation(opn->left_operand);
    MarControlPtr b = evaluateOperation(opn->right_operand);
    if (a.isInvalid() || b.isInvalid())
      return MarControlPtr();

    switch(opn->op)
    {
    case PLUS_OP:
      return a + b;
    case MINUS_OP:
      return a - b;
    case MULT_OP:
      return a * b;
    case DIV_OP:
      return a / b;
    case EQ_OP:
      return a == b;
    case NEQ_OP:
      return a != b;
    case LESS_OP:
      return a < b;
#if 1
    case MORE_OP:
      return !(a == b || a < b);
    case LESSEQ_OP:
      return (a == b || a < b);
    case MOREEQ_OP:
      return !(a < b);
#endif
    default:
      MRSERR("Unknown operator: " << opn->op);
    }

    return MarControlPtr();
  }

  return opn->value;
}

void ScriptOperationProcessor::clearOperation()
{
  delete m_operation;
  m_operation = nullptr;

  for (string & name : m_dependencies)
  {
    controls_.erase(name);
  }

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
    //cout << "A mapping: " << dst << " <- " << src << endl;
    if (dst.isInvalid() || src.isInvalid())
      continue;
    //cout << "Applying mapping." << endl;
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
