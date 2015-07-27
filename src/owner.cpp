/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#include <Poco/JSON/Parser.h>

#include "internal/utils.h"
#include "model/owner.h"

using namespace std;
using namespace Poco::Dynamic;
using namespace Poco::JSON;

namespace galaxy {
namespace fds {

shared_ptr<Owner> Owner::deserialize(std::istream& is) {
  shared_ptr<Owner> res(new Owner());
  Parser parser;
  Var result = parser.parse(is);
  Object::Ptr pObject = result.extract<Object::Ptr>();
  res->setId(Utils::parse(pObject, string("id"), string()));
  res->setDisplayName(Utils::parse(pObject, string("displayName"), string())); 
  return res;
}

string Owner::serialize(const Owner& owner) {
  Object object(true);
  object.set("id", owner.id());
  object.set("displayName", owner.displayName());

  stringstream ss;
  object.stringify(ss);
  return ss.str();
}

} // namespace fds
} // namespace galaxy
