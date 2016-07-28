/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#ifndef GALAXY_CONSTANTS_H_
#define GALAXY_CONSTANTS_H_

#include <string>

namespace galaxy {
namespace fds {

class Constants {
public:
  static const std::string CACHE_CONTROL;

  static const std::string CONTENT_ENCODING;

  static const std::string CONTENT_LENGTH;

  static const std::string CONTENT_MD5;

  static const std::string CONTENT_TYPE;

  static const std::string LAST_MODIFIED;

  static const std::string USER_DEFINED_METADATA_PREFIX;

  static const std::string DATE;

  static const std::string AUTHORIZATION;

  static const std::string RANGE;
}; // class Constants

} // namespace fds
} // namespace galaxy

#endif // GALAXY_CONSTANTS_H_
