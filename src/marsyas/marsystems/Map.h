#ifndef MARSYAS_MAP_MARSYSTEM_INCLUDED
#define MARSYAS_MAP_MARSYSTEM_INCLUDED

#include <marsyas/system/MarSystem.h>

namespace Marsyas {

class Map : public MarSystem
{
public:
  Map(const std::string & name);
  Map( const Map & other );
  MarSystem *clone() const { return new Map(*this); }

private:
  void myUpdate(MarControlPtr);
  void myProcess(realvec&, realvec&);

  MarControlPtr m_input_ctl;
  MarControlPtr m_output_ctl;
};

} // namespace Marsyas

#endif // MARSYAS_MAP_MARSYSTEM_INCLUDED
