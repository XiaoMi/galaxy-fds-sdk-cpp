/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#ifndef GALAXY_FDS_OBJECT_SUMMARY_H_
#define GALAXY_FDS_OBJECT_SUMMARY_H_

#include <string> 

#include "owner.h"
#include "model/fds_object_metadata_bean.h"

namespace galaxy {
namespace fds {

class FDSObjectSummary {
public:
  const std::string& bucketName() const;

  void setBucketName(const std::string& bucketName);

  const std::string& objectName() const;

  void setObjectName(const std::string& objectName);

  const Owner& owner() const;

  void setOwner(const Owner& owner);

  long size() const;

  void setSize(long size);

  const MetadataBean& metadataBean() const;

  void setMetadataBean(const MetadataBean& metadataBean);

private:
  std::string _bucketName;
  std::string _objectName;
  Owner _owner;
  long _size;
  MetadataBean _metadataBean;
}; // class FDSObjectSummary

inline const std::string& FDSObjectSummary::bucketName() const {
  return _bucketName;
}

inline void FDSObjectSummary::setBucketName(const std::string& bucketName) {
  _bucketName = bucketName;
}

inline const std::string& FDSObjectSummary::objectName() const {
  return _objectName;
}

inline void FDSObjectSummary::setObjectName(const std::string& objectName) {
  _objectName = objectName;
}

inline const Owner& FDSObjectSummary::owner() const {
  return _owner;
}

inline void FDSObjectSummary::setOwner(const Owner& owner) {
  _owner = owner;
}

inline long FDSObjectSummary::size() const {
  return _size;
}

inline void FDSObjectSummary::setSize(long size) {
  _size = size;
}

inline const MetadataBean& FDSObjectSummary::metadataBean() const {
  return _metadataBean;
}

inline void FDSObjectSummary::setMetadataBean(const MetadataBean& metadataBean) {
  _metadataBean = metadataBean;
}

} // namespace fds
} // namespace galaxy

#endif // GALAXY_FDS_OBJECT_SUMMARY_H_
