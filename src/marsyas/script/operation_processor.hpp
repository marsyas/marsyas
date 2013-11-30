#ifndef MARSYAS_SCRIPT_OPERATION_PROCESSOR
#define MARSYAS_SCRIPT_OPERATION_PROCESSOR

#include <marsyas/system/MarSystem.h>

#include <string>

namespace Marsyas {

class ScriptOperationProcessor : public MarSystem
{
public:
  enum operator_type
  {
    NO_OP = 0,
    PLUS_OP,
    MINUS_OP,
    MULT_OP,
    DIV_OP,
    EQ_OP,
    NEQ_OP,
    LESS_OP,
    MORE_OP,
    LESSEQ_OP,
    MOREEQ_OP
  };

  static operator_type operator_for_text(const std::string op_text);

  struct operation
  {
    operation():
      op(NO_OP),
      left_operand(nullptr),
      right_operand(nullptr)
    {}

    operation(operation *left, operator_type op, operation *right ):
      op(op),
      left_operand(left),
      right_operand(right)
    {}

    ~operation()
    {
      delete left_operand;
      delete right_operand;
    }

    MarControlPtr value;
    operator_type op;
    operation *left_operand;
    operation *right_operand;
  };

  ScriptOperationProcessor(const std::string & name);
  ScriptOperationProcessor( const ScriptOperationProcessor & other );
  virtual ~ScriptOperationProcessor();
  MarSystem *clone() const;

  void setOperation( operation * );

private:
  void myUpdate(MarControlPtr sender);
  void myProcess(realvec&, realvec&);

  void initControls();
  void prepareOperation( operation * );
  MarControlPtr evaluateOperation( operation * );
  void clearOperation();

  operation * m_operation;
  MarControlPtr m_result;
  std::vector<std::string> m_dependencies;
};

class ScriptStateProcessor : public MarSystem
{
public:
  ScriptStateProcessor(const std::string & name);
  ScriptStateProcessor( const ScriptStateProcessor & other );
  virtual ~ScriptStateProcessor();
  MarSystem *clone() const;

  void addMapping( MarControlPtr & dst, MarControlPtr & src );

private:
  void myUpdate(MarControlPtr sender);
  void myProcess(realvec&, realvec&);

  MarControlPtr m_condition;
  MarControlPtr m_inverse;
  std::vector< std::pair<MarControlPtr, MarControlPtr> > m_state;
};

} // namespace Marsyas

#endif // MARSYAS_SCRIPT_OPERATION_PROCESSOR
