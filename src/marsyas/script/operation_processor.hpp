#ifndef MARSYAS_SCRIPT_OPERATION_PROCESSOR
#define MARSYAS_SCRIPT_OPERATION_PROCESSOR

#include <marsyas/system/MarSystem.h>

#include <string>

namespace Marsyas {

class ScriptOperationProcessor : public MarSystem
{
public:
  struct operation
  {
    operation():
      op(0),
      left_operand(nullptr),
      right_operand(nullptr)
    {}

    ~operation()
    {
      delete left_operand;
      delete right_operand;
    }

    MarControlPtr value;
    char op;
    operation *left_operand;
    operation *right_operand;
  };

  ScriptOperationProcessor(const std::string & name);
  ScriptOperationProcessor( const ScriptOperationProcessor & other );

  MarSystem *clone() const;
  void setOperation( operation * );
  void myUpdate(MarControlPtr sender);
  void myProcess(realvec&, realvec&);

private:
  void initControls();
  void prepareOperation( operation * );
  MarControlPtr evaluateOperation( operation * );

  operation * m_operation;
  MarControlPtr m_result;
};

} // namespace Marsyas

#endif // MARSYAS_SCRIPT_OPERATION_PROCESSOR
