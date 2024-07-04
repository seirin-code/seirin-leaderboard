#ifndef SEIRIN_DETAIL_HPP
#define SEIRIN_DETAIL_HPP

#include <string>

namespace seirin
{
  namespace detail
  {
    std::string fetchApiData(const std::string& url, const std::string& token);
  }
}

#endif