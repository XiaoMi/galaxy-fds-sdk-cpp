/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#include <Poco/JSON/Parser.h>

#include "internal/utils.h"
#include "model/fds_object_listing.h"

using namespace std;
using namespace Poco::JSON;
using Poco::Dynamic::Var;
using Poco::JSON::Array;

namespace galaxy {
namespace fds {

shared_ptr<FDSObjectListing> FDSObjectListing::deserialize(istream& is) {
  shared_ptr<FDSObjectListing> res(new FDSObjectListing());
  Parser parser;
  parser.parse(is);
  Var result = parser.result();
  Object::Ptr pObject = result.extract<Object::Ptr>();
  string emptyString;

  res->setBucketName(Utils::parse(pObject, string("name"), emptyString));
  res->setPrefix(Utils::parse(pObject, string("prefix"), emptyString));
  res->setDelimiter(Utils::parse(pObject, string("delimiter"), emptyString));
  res->setMarker(Utils::parse(pObject, string("marker"), emptyString));
  res->setNextMarker(Utils::parse(pObject, string("nextMarker"), emptyString));
  res->setMaxKeys(Utils::parse(pObject, string("maxKeys"), 0));
  res->setTruncated(Utils::parse(pObject, string("truncated"), false));

  Array::Ptr pObjectSummaryArray = pObject->getArray("objects");
  vector<FDSObjectSummary> objectSummaries;
  for (size_t i = 0; i < pObjectSummaryArray->size(); i++) {
    Object::Ptr pObjectSummary = pObjectSummaryArray->getObject(i);
    FDSObjectSummary objectSummary;
    objectSummary.setObjectName(Utils::parse(pObjectSummary, string("name"), emptyString));
    objectSummary.setBucketName(res->bucketName());
    Owner owner;
    owner.setId(Utils::parse(pObjectSummary->getObject("owner"), string("id"), emptyString));
    objectSummary.setOwner(owner);
    MetadataBean metadataBean;
    Object::Ptr metaVar = pObjectSummary->getObject("metadataBean");
    if (metaVar) {
      Object::Ptr pMetaObj = metaVar->getObject("rawMeta");
      for (Poco::JSON::Object::ConstIterator iter = pMetaObj->begin(),
         end = pMetaObj->end(); iter != end; iter++) {
         metadataBean.add(iter->first, iter->second.toString());
      }
      objectSummary.setMetadataBean(metadataBean);
    }
    objectSummary.setSize(Utils::parse(pObjectSummary, string("size"), 0l));
    objectSummaries.push_back(objectSummary);
  }
  res->setObjectSummaries(objectSummaries);

  Array::Ptr pCommonPrefixArray = pObject->getArray("commonPrefixes");
  vector<string> commonPrefixes;
  for (size_t i = 0; i < pCommonPrefixArray->size(); i++) {
    commonPrefixes.push_back(pCommonPrefixArray->getElement<string>(i));
  }
  res->setCommonPrefixes(commonPrefixes);
  return res;
}

string FDSObjectListing::serialize(const FDSObjectListing& objectListing) {
  const vector<FDSObjectSummary>& objectSummaries = objectListing.objectSummaries();
  Array::Ptr pObjectSummaryArray = new Array();
  for (size_t i = 0; i < objectSummaries.size(); i++) {
    Object::Ptr pOwnerObject = new Object(true); 
    pOwnerObject->set("id", objectSummaries[i].owner().id());
    Object::Ptr pObjectSummary = new Object(true);
    pObjectSummary->set("owner", pOwnerObject);
    Object::Ptr pMetaObj = new Object(true);
    Object::Ptr pRawMetaObj = new Object(true);
    std::map<std::string, std::string>::const_iterator iter;
    for (iter = objectSummaries[i].metadataBean().metadata().begin();
      iter != objectSummaries[i].metadataBean().metadata().end(); iter++) {
      pRawMetaObj->set(iter->first, iter->second);
    }
    pMetaObj->set("rawMeta", pRawMetaObj);
    pObjectSummary->set("metadataBean", pMetaObj);
    pObjectSummary->set("name", objectSummaries[i].objectName());
    pObjectSummary->set("size", objectSummaries[i].size());
    pObjectSummaryArray->add(pObjectSummary);
  }

  const vector<string> commonPrefixes = objectListing.commonPrefixes();
  Array::Ptr pCommonPrefixArray = new Array();
  for (size_t i = 0; i < commonPrefixes.size(); i++) {
    pCommonPrefixArray->add(Var(commonPrefixes[i]));
  }

  Object::Ptr pResult = new Object(true);
  pResult->set("objects", pObjectSummaryArray);
  pResult->set("commonPrefixes", pCommonPrefixArray);
  pResult->set("name", objectListing.bucketName());
  pResult->set("prefix", objectListing.prefix());
  pResult->set("delimiter", objectListing.delimiter());
  pResult->set("marker", objectListing.marker());
  pResult->set("nextMarker", objectListing.nextMarker());
  pResult->set("maxKeys", objectListing.maxKeys());
  pResult->set("truncated", objectListing.truncated());
  stringstream ss;
  pResult->stringify(ss);
  return ss.str();
}

} // namespace fds
} // namespace galaxy
