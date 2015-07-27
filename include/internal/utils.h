/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#ifndef _INTERNAL_UTILS_H_
#define _INTERNAL_UTILS_H_

#include <string>
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

}; // class Utils

} // namespace fds
} // namespace galaxy

#endif // _INTERNAL_UTILS_H_
