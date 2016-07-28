/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#include "fds_client_configuration.h"

using namespace std;

namespace galaxy {
namespace fds {

const string FDSClientConfiguration::URI_HTTP_PREFIX = "http://";
const string FDSClientConfiguration::URI_HTTPS_PREFIX = "https://";
const string FDSClientConfiguration::URI_CDN = "cdn";
const string FDSClientConfiguration::URI_SUFFIX = "fds.api.xiaomi.com";
const string FDSClientConfiguration::URI_CDN_SUFFIX = "fds.api.mi-img.com";

FDSClientConfiguration::FDSClientConfiguration() {
  _regionName = "cnbj0";
  _enableHttps = true;
  _enableCdnForUpload = false;
  _enableCdnForDownload = true;
}

string FDSClientConfiguration::buildBaseUri(bool enableCdn) const {
  string res = _enableHttps ? URI_HTTPS_PREFIX : URI_HTTP_PREFIX;
  if (!_endpoint.empty()) {
    res += _endpoint;
  } else {
    if (enableCdn) {
      res += URI_CDN + "." + _regionName + "." + URI_CDN_SUFFIX;
    } else {
      res += _regionName + "." + URI_SUFFIX;
    }
  }
  return res;
}

} // namespace fds
} // namespace galaxy
