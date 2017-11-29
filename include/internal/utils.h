/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#ifndef _INTERNAL_UTILS_H_
#define _INTERNAL_UTILS_H_

#include <string>
#include <algorithm>
#include <Poco/Exception.h>
#include <Poco/JSON/Object.h>

namespace galaxy {
namespace fds {

class Utils {
public:
  template<typename T>
  static T parse(Poco::JSON::Object::Ptr pObject, const std::string& key,
      const T& defaultValue) {
    try {
      return pObject->getValue<T>(key);
    } catch(Poco::InvalidAccessException e) {
      return defaultValue;
    }
  }

  static std::string toLowerCase(const std::string &original) {
    std::string newString = original;
    std::transform(newString.begin(), newString.end(), newString.begin(), ::tolower);
    return newString;
  }
}; // class Utils

} // namespace fds
} // namespace galaxy

#endif // _INTERNAL_UTILS_H_
