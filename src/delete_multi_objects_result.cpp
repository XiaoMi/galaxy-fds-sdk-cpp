#include <Poco/JSON/Parser.h>

#include "internal/utils.h"
#include "model/delete_multi_objects_result.h"

using namespace std;
using namespace Poco::JSON;
using Poco::Dynamic::Var;
using Poco::JSON::Array;

namespace galaxy {
namespace fds {

std::shared_ptr<FDSObjectsDeleting> FDSObjectsDeleting::deserialize(std::istream &is) {
  std::shared_ptr<FDSObjectsDeleting> res = std::make_shared<FDSObjectsDeleting>();
  Parser jsonparser;
  jsonparser.parse(is);
  Var jsonresult = jsonparser.result();
  Array::Ptr arr = jsonresult.extract<Array::Ptr>();

  for (size_t index = 0; index < arr->size(); index ++) {
    Var value = arr->get(index);
    Object::Ptr pobject = value.extract<Object::Ptr>();

    errorObject errobject;
    value = pobject->get(string("object_name"));
    errobject.objectName = value.toString();

    value = pobject->get(string("error_code"));
    errobject.errorCode = atoi(value.toString().data());

    value = pobject->get(string("error_description"));
    errobject.errorDesc = value.toString();

    if (!errobject.objectName.empty()) {
      res->addFailedObject(std::move(errobject));
    }
  }
  return res;
}

std::string FDSObjectsDeleting::serialize(const FDSObjectsDeleting& fdsObjectsDeleting)
{
  Array::Ptr arr = new Array();
  for(const errorObject& errobject : fdsObjectsDeleting.getErrorList()) {
    Object::Ptr pobject = new Object(true);
    pobject->set(string("object_name"), errobject.objectName);
    pobject->set(std::string("error_code"), errobject.errorCode);
    pobject->set(std::string("error_description"), errobject.errorDesc);

    arr->add(Var(pobject));
  }
  stringstream ss;
  arr->stringify(ss);
  return ss.str();
}

std::string serializeVectorString(const std::vector<std::string> &strs)
{
  Array::Ptr vecArr = new Array();
  for (size_t index = 0; index < strs.size(); index ++) {
    vecArr->add(strs[index]);
  }
  std::stringstream ss;
  vecArr->stringify(ss);
  return ss.str();
}

}   // namespace fds
}   // namespace galaxy