/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#include <Poco/JSON/Parser.h>

#include "internal/utils.h"
#include "model/put_object_result.h"

using namespace std;
using namespace Poco::JSON;
using Poco::Dynamic::Var;

namespace galaxy {
namespace fds {

shared_ptr<PutObjectResult> PutObjectResult::deserialize(istream& is) {
  shared_ptr<PutObjectResult> res(new PutObjectResult());

  Parser parser;
  parser.parse(is);
  Var result = parser.result();
  Object::Ptr pObject = result.extract<Object::Ptr>();
  string emptyString;
  res->setAccessKeyId(Utils::parse(pObject, string("accessKeyId"), emptyString));
  res->setBucketName(Utils::parse(pObject, string("bucketName"), emptyString));
  res->setExpires(Utils::parse(pObject, string("expires"), 0l));
  res->setObjectName(Utils::parse(pObject, string("objectName"), emptyString));
  res->setSignature(Utils::parse(pObject, string("signature"), emptyString));

  return res;
}

string PutObjectResult::serialize(const PutObjectResult& putObjectResult) {
  Object object(true);
  object.set("accessKeyId", putObjectResult.accessKeyId());
  object.set("bucketName", putObjectResult.bucketName());
  object.set("expires", putObjectResult.expires());
  object.set("objectName", putObjectResult.objectName());
  object.set("signature", putObjectResult.signature());

  stringstream ss;
  object.stringify(ss);
  return ss.str();
}

} // namespace fds
} // namespace galaxy
