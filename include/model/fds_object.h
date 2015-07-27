/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#ifndef GALAXY_FDS_OBJECT_H_
#define GALAXY_FDS_OBJECT_H_

#include <istream>
#include <memory>
#include <iostream>

#include <Poco/Net/HTTPClientSession.h>

#include "fds_object_summary.h"
#include "fds_object_metadata.h"

namespace galaxy {
namespace fds {

class FDSObject {
public:
  const FDSObjectSummary& objectSummary() const;

  void setObjectSummary(std::shared_ptr<FDSObjectSummary> pObjectSummary);

  const FDSObjectMetadata& objectMetadata() const;

  void setObjectMetadata(std::shared_ptr<FDSObjectMetadata> pObjectMetadata);
  
  std::istream& objectContent();

  void setObjectContent(std::istream& objectContent);

  void setSession(std::shared_ptr<Poco::Net::HTTPClientSession> session);

private:
  std::shared_ptr<FDSObjectSummary> _pObjectSummary;
  std::shared_ptr<FDSObjectMetadata> _pObjectMetadata;
  std::shared_ptr<Poco::Net::HTTPClientSession> _pSession;
  std::istream* _pObjectContent;
}; // class FDSObject


inline const FDSObjectSummary& FDSObject::objectSummary() const {
  return *_pObjectSummary;
}

inline void FDSObject::setObjectSummary(std::shared_ptr<FDSObjectSummary>
    pObjectSummary) {
  _pObjectSummary = pObjectSummary;
}

inline const FDSObjectMetadata& FDSObject::objectMetadata() const {
  return *_pObjectMetadata;
}

inline void FDSObject::setObjectMetadata(std::shared_ptr<FDSObjectMetadata>
    pObjectMetadata) {
  _pObjectMetadata = pObjectMetadata;
}

inline std::istream& FDSObject::objectContent() {
  return *_pObjectContent;
}

inline void FDSObject::setObjectContent(std::istream& objectContent) {
  _pObjectContent = &objectContent;
}

inline void FDSObject::setSession(std::shared_ptr<Poco::Net::HTTPClientSession>
  pSession) {
  _pSession = pSession;
}

} // namespace fds
} // namespace galaxy

#endif // GALAXY_FDS_OBJECT_H_
