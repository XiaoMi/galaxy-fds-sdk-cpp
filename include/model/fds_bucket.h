/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#ifndef GALAXY_FDS_BUCKET_H_
#define GALAXY_FDS_BUCKET_H_

#include <string>

#include "owner.h"

namespace galaxy {
namespace fds {

class FDSBucket {
public:
  FDSBucket(const std::string& name) {
    _name = name;
  }

  const std::string& name() const;

  void setName(const std::string& name);
  
  time_t creationDate() const;

  void setCreationDate(time_t creationDate);

  const Owner& owner() const;

  void setOwner(const Owner& owner);

private:
  std::string _name;
  time_t _creationDate;
  Owner _owner;
}; // class FDSBucket

inline const std::string& FDSBucket::name() const {
  return _name;
}

inline void FDSBucket::setName(const std::string& name) {
  _name = name;
}

inline time_t FDSBucket::creationDate() const {
  return _creationDate;
}

inline void FDSBucket::setCreationDate(time_t creationDate) {
  _creationDate = creationDate;
}

inline const Owner& FDSBucket::owner() const {
  return _owner;
}

inline void FDSBucket::setOwner(const Owner& owner) {
  _owner = owner;
}

} // namespace fds
} // namespace galaxy

#endif // GALAXY_FDS_BUCKET_H_
