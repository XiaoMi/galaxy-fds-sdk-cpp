/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: cailiuyang@xiaomi.com
 */
#ifndef DELETE_MULTI_OBJECTS_RESULT_H_
#define DELETE_MULTI_OBJECTS_RESULT_H_

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <istream>

namespace galaxy{
namespace fds{

struct errorObject{
  std::string objectName;
  int         errorCode;
  std::string errorDesc;
};
typedef std::vector<errorObject> errorList;

class FDSObjectsDeleting{
public:
  void addFailedObject(errorObject &&errobject);

  int countFailedObjects() const;

  const errorList& getErrorList() const;

  static std::shared_ptr<FDSObjectsDeleting> deserialize(std::istream& is);

  static std::string serialize(const FDSObjectsDeleting& fdsObjectsDeleting);

private:
  errorList _errorlist;
};

inline void FDSObjectsDeleting::addFailedObject(errorObject &&errobject) {
  _errorlist.emplace_back(errobject);
}

inline int FDSObjectsDeleting::countFailedObjects() const {
  return static_cast<int>(_errorlist.size());
}

inline const errorList& FDSObjectsDeleting::getErrorList() const{
  return _errorlist;
}

// helper function to serialize the objects

extern std::string serializeVectorString(const std::vector<std::string> &strs);

}   // namespace fds
}   // namespace galaxy

#endif // DELETE_MULTI_OBJECTS_RESULT_H_