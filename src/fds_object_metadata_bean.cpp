

#include <Poco/JSON/Parser.h>
#include "model/fds_object_metadata_bean.h"


using namespace std;
using namespace Poco::JSON;
using Poco::Dynamic::Var;
using Poco::JSON::Array;

namespace galaxy {
namespace fds {

std::shared_ptr<MetadataBean> MetadataBean::deserialize(istream& is) {
  shared_ptr<MetadataBean> res(new MetadataBean());
  Parser parser;
  parser.parse(is);
  Var result = parser.result();
  Object::Ptr pObject = result.extract<Object::Ptr>();
  string emptyString;

  Object::Ptr pMetaObj = pObject->getObject("rawMeta");
  for (Poco::JSON::Object::ConstIterator iter = pMetaObj->begin(),
      end = pMetaObj->end(); iter != end; iter++) {
    res->add(iter->first, iter->second.toString());
  }
  return res;
}

string MetadataBean::serialize(const MetadataBean& metadataBean) {
  Object::Ptr pObject = new Object(true);
  Object::Ptr pRawMetaObj = new Object(true);
  std::map<std::string, std::string>::const_iterator iter;
  for (iter = metadataBean.metadata().begin(); iter != metadataBean.metadata().end(); iter++) {
    pRawMetaObj->set(iter->first, iter->second);
  }
  pObject->set("rawMeta", pRawMetaObj);
  stringstream ss;
  pObject->stringify(ss);
  return ss.str();
}

} // namespace fds
} // namespace galaxy