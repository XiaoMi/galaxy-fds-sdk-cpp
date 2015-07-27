/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#ifndef GALAXY_SUB_RESOURCE_H_
#define GALAXY_SUB_RESOURCE_H_

#include <string>
#include <vector>

namespace galaxy {
namespace fds {
 
class SubResource {
public:
  static const std::string ACL;
  static const std::string QUOTA;
  static const std::string UPLOADS;
  static const std::string PART_NUMBER;
  static const std::string UPLOAD_ID;
  static const std::string STORAGE_ACCESS_TOKEN;
  static const std::string METADATA;

  static std::vector<std::string> SUB_RESOURCES;

private:
  static std::vector<std::string> init();
}; // class SubResource

} // namespace fds
} // namespace galaxy

#endif // GALAXY_SUB_RESOURCE_H_
