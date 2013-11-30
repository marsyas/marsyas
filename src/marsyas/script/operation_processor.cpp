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


MarSystem *ScriptOperationProcessor::clone() const
{
  return new ScriptOperationProcessor( *this );
}

void ScriptOperationProcessor::initControls()
{
}

void ScriptOperationProcessor::setOperation( operation * opn )
{
  m_operation = opn;

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

} // namespace Marsyas
