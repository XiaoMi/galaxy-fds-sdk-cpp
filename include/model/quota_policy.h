/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#ifndef GALAXY_QUOTA_POLICY_H_
#define GALAXY_QUOTA_POLICY_H_

#include <string>
#include <vector>
#include <memory>

#include "action.h"

namespace galaxy {
namespace fds {

class QuotaType {
public:
  QuotaType() {}

  QuotaType(int value) { _value = value; }

  static const int QPS = 0;
  static const std::string QPS_STR;
  static const int THROUGHPUT = 1;
  static const std::string THROUGHPUT_STR;

  static QuotaType valueOf(const std::string& text);

  std::string name() const;

  int value() const { return _value; }

  void setValue(int value) { _value = value; }

private:
  int _value;
};

class Quota {
public:
  Quota(QuotaType type, Action action, long value) {
    _type = type;
    _action = action;
    _value = value;
  }

  QuotaType type() const;

  void setType(QuotaType type);

  Action action() const;

  void setAction(Action action);

  long value() const;

  void setValue(long value);

private:
  QuotaType _type;
  Action _action;
  long _value;
}; // class Quota

class QuotaPolicy {
public:
  const std::vector<Quota>& quotas() const;

  void addQuota(const Quota& quota);

  static std::shared_ptr<QuotaPolicy> deserialize(std::istream& is);

  static std::string serialize(const QuotaPolicy& quotaPolicy);
    
private:
  std::vector<Quota> _quotas;
}; // class QuotaPolicy

inline QuotaType Quota::type() const {
  return _type;
}

inline void Quota::setType(QuotaType type) {
  _type = type;
}

inline Action Quota::action() const {
  return _action;
}

inline void Quota::setAction(Action action) {
  _action = action;
}

inline long Quota::value() const {
  return _value;
}

inline void Quota::setValue(long value) { 
  _value = value;
}

inline const std::vector<Quota>& QuotaPolicy::quotas() const {
  return _quotas;
}

inline void QuotaPolicy::addQuota(const Quota& quota) {
  _quotas.push_back(quota);
}

} // namespace fds
} // namespace galaxy

#endif // GALAXY_QUOTA_POLICY_H_
