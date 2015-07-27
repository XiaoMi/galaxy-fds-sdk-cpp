/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#ifndef GALAXY_PUT_OBJECT_RESULT_H_
#define GALAXY_PUT_OBJECT_RESULT_H_

#include <string>
#include <memory>

namespace Poco {
namespace JSON {
  class Parser;
}
}

namespace galaxy {
namespace fds {

class PutObjectResult {
public:
  const std::string& bucketName() const;

  void setBucketName(const std::string& bucketName);

  const std::string& objectName() const;

  void setObjectName(const std::string& objectName);

  const std::string& accessKeyId() const;

  void setAccessKeyId(const std::string& accessKeyId);

  const std::string& signature() const;

  void setSignature(const std::string& signature);

  long expires() const;

  void setExpires(long expire);

  const std::string& presignedUri() const;

  const std::string& cdnPresignedUri() const;

  static std::shared_ptr<PutObjectResult> deserialize(std::istream& is);

  static std::string serialize(const PutObjectResult& putObjectResult);

private:
  std::string _bucketName;
  std::string _objectName;
  std::string _accessKeyId;
  std::string _signature;
  long _expires;
}; // class PutObjectResult

inline const std::string& PutObjectResult::bucketName() const {
  return _bucketName;
}

inline void PutObjectResult::setBucketName(const std::string& bucketName) {
  _bucketName = bucketName;
}

inline const std::string& PutObjectResult::objectName() const {
  return _objectName;
}

inline void PutObjectResult::setObjectName(const std::string& objectName) {
  _objectName = objectName;
}

inline const std::string& PutObjectResult::accessKeyId() const {
  return _accessKeyId;
}

inline void PutObjectResult::setAccessKeyId(const std::string& accessKeyId) {
  _accessKeyId = accessKeyId;
}

inline const std::string& PutObjectResult::signature() const {
  return _signature;
}

inline void PutObjectResult::setSignature(const std::string& signature) {
  _signature = signature;
}

inline long PutObjectResult::expires() const {
  return _expires;
}

inline void PutObjectResult::setExpires(long expires) {
  _expires = expires;
}

} // namespace fds
} // namespace galaxy

#endif // GALAXY_PUT_OBJECT_RESULT_H_
