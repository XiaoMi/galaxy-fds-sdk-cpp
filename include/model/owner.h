/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#ifndef GALAXY_OWNER_H_
#define GALAXY_OWNER_H_

#include <memory>
#include <istream>

namespace galaxy {
namespace fds {

class Owner {
public:
  const std::string& id() const;
  
  void setId(const std::string& id);

  const std::string& displayName() const;

  void setDisplayName(const std::string& displayName);

  static std::shared_ptr<Owner> deserialize(std::istream& is);

  static std::string serialize(const Owner& owner);

private:
  std::string _id;
  std::string _displayName;
}; // class Owner

inline const std::string& Owner::id() const {
  return _id;
}

inline void Owner::setId(const std::string& id) {
  _id = id;
}

inline const std::string& Owner::displayName() const {
  return _displayName;
}

inline void Owner::setDisplayName(const std::string& displayName) {
  _displayName = displayName;
}

} // namespace fds
} // namespace galaxy

#endif // GALAXY_OWNER_H_
