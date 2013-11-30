#include "operation_processor.hpp"

#include <algorithm>

using namespace std;

namespace Marsyas {

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
    case '+':
      return a + b;
    case '-':
      return a - b;
    case '*':
      return a * b;
    case '/':
      return a / b;
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
  m_condition->setState(true);
}

ScriptStateProcessor::ScriptStateProcessor( const ScriptStateProcessor & other ):
  MarSystem(other)
{
  m_condition = getControl("mrs_bool/condition");
}

ScriptStateProcessor::~ScriptStateProcessor()
{}

MarSystem *ScriptStateProcessor::clone() const
{
  return new ScriptStateProcessor(*this);
}

void ScriptStateProcessor::setCondition( MarControlPtr condition )
{
  if (condition.isInvalid() || condition->getType() != "mrs_bool")
  {
    MRSERR("ScriptStateProcessor: invalid condition control");
    return;
  }

  static const bool shall_update = false;
  m_condition->linkTo(condition, shall_update);
}

void ScriptStateProcessor::addMapping( MarControlPtr & dst, MarControlPtr & src )
{
  m_state.emplace_back( dst, src );
}

void ScriptStateProcessor::myUpdate(MarControlPtr)
{
  if (m_condition.isInvalid())
    return;

  //cout << "Valid condition." << endl;

  bool active = m_condition->to<bool>();
  if (!active)
    return;

  //cout << "Active state." << endl;

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
