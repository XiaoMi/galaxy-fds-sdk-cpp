/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#include <Poco/JSON/Parser.h>

#include "galaxy_fds_client_exception.h"
#include "model/quota_policy.h"

using namespace std;
using namespace Poco::JSON;
using Poco::Dynamic::Var;
using Poco::JSON::Array;

namespace galaxy {
namespace fds {

const string QuotaType::QPS_STR = "QPS";

const string QuotaType::THROUGHPUT_STR = "Throughput";

QuotaType QuotaType::valueOf(const string& text) {
  if (text == QPS_STR) {
    return QuotaType(QPS);
  } else if (text == THROUGHPUT_STR) {
    return QuotaType(THROUGHPUT);
  } else {
    throw GalaxyFDSClientException("Unrecognized enum text");
  }
}

string QuotaType::name() const {
  switch(_value) {
    case QPS:
      return QPS_STR;
    case THROUGHPUT:
      return THROUGHPUT_STR;
    default:
      throw GalaxyFDSClientException("Unrecognized enum value");
  }
}

shared_ptr<QuotaPolicy> QuotaPolicy::deserialize(istream& is) {
  shared_ptr<QuotaPolicy> res(new QuotaPolicy());

  Parser parser;
  parser.parse(is);
  Var result = parser.result();
  Object::Ptr pObject = result.extract<Object::Ptr>();
  Array::Ptr pArray = pObject->getArray("quotas");
  for (size_t i = 0; i < pArray->size(); i++) {
    Object::Ptr pQuotaObject = pArray->getObject(i);
    Action action = Action::valueOf(pQuotaObject->getValue<string>("action"));
    QuotaType type = QuotaType::valueOf(pQuotaObject->getValue<string>("type"));
    long value = pQuotaObject->getValue<long>("value");
    res->addQuota(Quota(type, action, value));
  }

  return res;
}

string QuotaPolicy::serialize(const QuotaPolicy& quotaPolicy) {
  const vector<Quota>& quotas = quotaPolicy.quotas();
  Array::Ptr pArray = new Array();
  for (size_t i = 0; i < quotas.size(); i++) {
    Object::Ptr pObject = new Object(true);
    pObject->set("action", quotas[i].action().name());
    pObject->set("type", quotas[i].type().name());
    pObject->set("value", quotas[i].value());
    pArray->add(pObject);
  }

  Object::Ptr pResult = new Object(true);
  pResult->set("quotas", Var(pArray));
  stringstream ss;
  pResult->stringify(ss);
  return ss.str();
}

} // namespace fds
} // namespace galaxy
