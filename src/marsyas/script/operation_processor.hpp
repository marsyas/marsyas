#ifndef MARSYAS_SCRIPT_OPERATION_PROCESSOR
#define MARSYAS_SCRIPT_OPERATION_PROCESSOR

#include <marsyas/system/MarSystem.h>

#include <string>
#include <map>

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
    MOREEQ_OP,
    WHEN_OP,
    ON_OP
  };

  static operator_type operator_for_text(const std::string op_text);

  struct operation
  {
    operation( const MarControlPtr & value ):
      op(NO_OP),
      parent(nullptr),
      left_operand(nullptr),
      right_operand(nullptr),
      value(value)
    {}

    operation(operation *left, operator_type op, operation *right ):
      op(op),
      parent(nullptr),
      left_operand(left),
      right_operand(right)
    {
      assert(left_operand != nullptr);
      assert(right_operand != nullptr);
      assert(left_operand->parent == nullptr);
      assert(right_operand->parent == nullptr);
      left_operand->parent = this;
      right_operand->parent = this;
    }

    ~operation()
    {
      delete left_operand;
      delete right_operand;
    }

    bool update( const MarControlPtr & cause = MarControlPtr() );

    operator_type op;
    operation *parent;
    operation *left_operand;
    operation *right_operand;
    MarControlPtr value;
  };

  ScriptOperationProcessor(const std::string & name);
  ScriptOperationProcessor( const ScriptOperationProcessor & other );
  virtual ~ScriptOperationProcessor();
  MarSystem *clone() const;

  void setOperation( operation * );

private:
  void myUpdate(MarControlPtr sender);
  void myProcess(realvec&, realvec&);

  void prepareOperation( operation * );
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
