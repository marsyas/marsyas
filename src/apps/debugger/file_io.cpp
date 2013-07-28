#include "file_io.hpp"

namespace Marsyas {

void copy_swap_bytes(char *destination, const char * source, int size )
{
#ifdef MARSYAS_BIGENDIAN
  for (size_t i = 0; i < size; ++i)
    destination[i] = source[size - i - 1];
#else
  std::memcpy(destination, source, size);
#endif
}

}
