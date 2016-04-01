/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#ifndef GALAXY_ACCESS_CONTROL_LIST_H_
#define GALAXY_ACCESS_CONTROL_LIST_H_

#include <string>
#include <map>
#include <memory>

namespace galaxy {
namespace fds {

class Permission {
public:
  Permission() {}

  Permission(int value) { _value = value; }

  static const int READ = 0x01;
  static const std::string READ_STR;
  static const int WRITE = 0x02;
  static const std::string WRITE_STR;
  static const int READ_OBJECTS = 0x04;
  static const std::string READ_OBJECTS_STR;
  static const int SSO_WRITE = 0x08;
  static const std::string SSO_WRITE_STR;
  static const int FULL_CONTROL = 0xff;
  static const std::string FULL_CONTROL_STR;

  static Permission valueOf(const std::string& text);

  static std::vector<Permission>& values() { return _allValues; }

  const std::string& name() const;

  int value() const { return _value; }

  void setValue(int value) { _value = value; }

private:
  static std::vector<Permission> initAllValues();
  
private:
  static std::vector<Permission> _allValues;
  int _value;
};

class GrantType {
public:
  GrantType() {}

  GrantType(int value) { _value = value; }

  static const int USER = 0;
  static const std::string USER_STR;
  static const int GROUP = 1;
  static const std::string GROUP_STR;

  static GrantType valueOf(const std::string& text);

  const std::string& name() const;

  int value() const { return _value; }

  void setValue(int value) { _value = value; }

private:
  int _value;
};

class UserGroups {
public:
  UserGroups() {}

  UserGroups(int value) { _value = value; }

  static const int ALL_USERS = 0;
  static const std::string ALL_USERS_STR;
  static const int AUTHENTICATED_USERS = 1;
  static const std::string AUTHENTICATED_USERS_STR;

  static UserGroups valueOf(const std::string& text);

  const std::string& name() const;

  int value() const { return _value; }

  void setValue(int value) { _value = value; }

private:
  int _value;
};

class Grant {
public:
  Grant(const std::string& granteeId, const Permission& permission) {
    Grant(granteeId, permission, GrantType(GrantType::USER));
  }

  Grant(const std::string& granteeId, const Permission& permission,
      const GrantType& type) {
    _granteeId = granteeId;
    _permission = permission;
    _grantType = type;
  }

  const std::string& granteeId() const;

  void setGranteeId(const std::string& granteeId);

  const Permission& permission() const;

  void setPermission(const Permission& permission);

  const GrantType& grantType() const;

  void setGrantType(const GrantType& grantType);

 private:
  std::string _granteeId;
  Permission _permission;
  GrantType _grantType;
}; // class Grant

class AccessControlList {
public:
  void addGrant(const Grant& grant);

  std::vector<Grant> getGrants() const;

  static std::shared_ptr<AccessControlList> deserialize(std::istream& is);

  static std::string serialize(const AccessControlList& acl);

private:
  std::map<std::string, int> _acl;
}; // class AccessControlList

inline const std::string& Grant::granteeId() const {
  return _granteeId;
}

inline void Grant::setGranteeId(const std::string& granteeId) {
  _granteeId = granteeId;
}

inline const Permission& Grant::permission() const {
  return _permission;
}

inline void Grant::setPermission(const Permission& permission) {
  _permission = permission;
}

inline const GrantType& Grant::grantType() const {
  return _grantType;
}

inline void Grant::setGrantType(const GrantType& grantType) {
  _grantType = grantType;
}

} // namespace fds
} // namespace galaxy

#endif // GALAXY_ACCESS_CONTROL_LIST_H_
