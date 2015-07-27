/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#include <Poco/JSON/Parser.h>

#include "galaxy_fds_client_exception.h"
#include "internal/utils.h"
#include "model/access_control_list.h"

using namespace std;
using namespace Poco::JSON;
using Poco::Dynamic::Var;
using Poco::JSON::Array;

namespace galaxy {
namespace fds {

const string Permission::READ_STR = "READ";

const string Permission::WRITE_STR = "WRITE";

const string Permission::FULL_CONTROL_STR = "FULL_CONTROL";

vector<Permission> Permission::_allValues = Permission::initAllValues();

vector<Permission> Permission::initAllValues() {
  vector<Permission> res;
  res.push_back(Permission(READ));
  res.push_back(Permission(WRITE));
  res.push_back(Permission(FULL_CONTROL));
  return res;
}

const string& Permission::name() const {
  switch(_value) {
    case READ:
      return READ_STR;
    case WRITE:
      return WRITE_STR;
    case FULL_CONTROL:
      return FULL_CONTROL_STR;
    default:
      throw GalaxyFDSClientException("Unrecognized enum value");
  }
}

Permission Permission::valueOf(const string& text) {
  if (text == READ_STR) {
    return Permission(READ);
  } else if (text == WRITE_STR) {
    return Permission(WRITE);
  } else if (text == FULL_CONTROL_STR) {
    return Permission(FULL_CONTROL);
  } else {
    throw GalaxyFDSClientException("Unrecognized enum text");
  }
}

const string GrantType::USER_STR = "USER";

const string GrantType::GROUP_STR = "GROUP";

const string& GrantType::name() const {
  switch(_value) {
    case USER:
      return USER_STR;
    case GROUP:
      return GROUP_STR;
    default:
      throw GalaxyFDSClientException("Unrecognized enum value");
  }
}

GrantType GrantType::valueOf(const string& text) {
  if (text == USER_STR) {
    return GrantType(USER);
  } else if (text == GROUP_STR) {
    return GrantType(GROUP);
  } else {
    throw GalaxyFDSClientException("Unrecognized enum text");
  }
}

const string UserGroups::ALL_USERS_STR = "ALL_USERS";

const string UserGroups::AUTHENTICATED_USERS_STR = "AUTHENTICATED_USERS";

const string& UserGroups::name() const {
  switch(_value) {
    case ALL_USERS:
      return ALL_USERS_STR;
    case AUTHENTICATED_USERS:
      return AUTHENTICATED_USERS_STR;
    default:
      throw GalaxyFDSClientException("Unrecognized enum value");
  }
}

UserGroups UserGroups::valueOf(const string& text) {
  if (text == ALL_USERS_STR) {
    return UserGroups(ALL_USERS);
  } else if (text == AUTHENTICATED_USERS_STR) {
    return UserGroups(AUTHENTICATED_USERS);
  } else {
    throw GalaxyFDSClientException("Unrecognized enum text");
  }
}

void AccessControlList::addGrant(const Grant& grant) {
  string key = grant.granteeId() + ":" + grant.grantType().name();
  map<string, int>::iterator iter = _acl.find(key);
  if (iter != _acl.end()) {
    _acl[key] = iter->second | grant.permission().value();
  } else {
    _acl[key] = grant.permission().value();
  }
}

vector<Grant> AccessControlList::getGrants() const {
  vector<Grant> grants;
  for (map<string, int>::const_iterator iter = _acl.begin(); iter != _acl.end();
      ++iter) {
    size_t idx = iter->first.find(':');
    string granteeId = iter->first.substr(0, idx);
    string grantType = iter->first.substr(idx + 1);
    if (iter->second == Permission::FULL_CONTROL) {
      grants.push_back(Grant(granteeId, Permission(Permission::FULL_CONTROL),
            GrantType::valueOf(grantType)));
    } else {
      for (vector<Permission>::iterator iterV = Permission::values().begin();
          iterV != Permission::values().end(); ++iterV) {
        if (iterV->value() != Permission::FULL_CONTROL &&
            (iterV->value() & iter->second) > 0) {
          grants.push_back(Grant(granteeId, *iterV, GrantType::valueOf((grantType))));
        }
      }
    }
  }
  return grants;
}

shared_ptr<AccessControlList> AccessControlList::deserialize(istream& is) {
  shared_ptr<AccessControlList> res(new AccessControlList());

  Parser parser;
  parser.parse(is);
  Var result = parser.result();
  Object::Ptr pObject = result.extract<Object::Ptr>();
  Array::Ptr pAclArray = pObject->getArray("accessControlList");
  for (size_t i = 0; i < pAclArray->size(); i++) {
    Object::Ptr pGrantObject = pAclArray->getObject(i);
    string granteeId = pGrantObject->getObject("grantee")->getValue<string>("id"); 
    Permission permission = Permission::valueOf(
        pGrantObject->getValue<string>("permission"));
    GrantType grantType = GrantType::valueOf(
        pGrantObject->getValue<string>("type"));
    res->addGrant(Grant(granteeId, permission, grantType));
  }

  return res;
}

string AccessControlList::serialize(const AccessControlList& acl) {
  vector<Grant> grants = acl.getGrants();

  Array::Ptr pAclArray = new Array();
  for (size_t i = 0; i < grants.size(); i++) {
    Object::Ptr pGrantObject = new Object(true);
    Object::Ptr pOwnerObject = new Object(true);
    pOwnerObject->set("id", grants[i].granteeId());
    pGrantObject->set("grantee", pOwnerObject);
    pGrantObject->set("permission", grants[i].permission().name());
    pGrantObject->set("type", grants[i].grantType().name());
    pAclArray->add(pGrantObject);
  }

  Object::Ptr pResult = new Object(true);
  pResult->set("accessControlList", Var(pAclArray));
  stringstream ss;
  pResult->stringify(ss);
  return ss.str();
}

} // namespace fds
} // namespace galaxy
