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

  void setCondition( MarControlPtr condition );
  void addMapping( MarControlPtr & dst, MarControlPtr & src );

private:
  void myUpdate(MarControlPtr sender);
  void myProcess(realvec&, realvec&);

  MarControlPtr m_condition;
  std::vector< std::pair<MarControlPtr, MarControlPtr> > m_state;
};

} // namespace Marsyas

#endif // MARSYAS_SCRIPT_OPERATION_PROCESSOR
