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
const string FDSClientConfiguration::URI_FILES = "files";
const string FDSClientConfiguration::URI_CDN = "cdn";
const string FDSClientConfiguration::URI_FDS_SUFFIX = ".fds.api.xiaomi.com";
const string FDSClientConfiguration::URI_FDS_SSL_SUFFIX
  = ".fds-ssl.api.xiaomi.com";

FDSClientConfiguration::FDSClientConfiguration() {
  _regionName = "";
  _enableHttps = true;
  _enableCdnForUpload = false;
  _enableCdnForDownload = true;
}

string FDSClientConfiguration::buildBaseUri(bool enableCdn) const {
  string res = _enableHttps ? URI_HTTPS_PREFIX : URI_HTTP_PREFIX;
  res += getBaseUriPrefix(enableCdn);
  res += getBaseUriSuffix(enableCdn);
  return res;
}

string FDSClientConfiguration::getBaseUriPrefix(bool enableCdn) const {
  string res = enableCdn ? URI_CDN : URI_FILES;
  if (!_regionName.empty()) {
    res = _regionName + "-" + res;
  }
  return res;
}

string FDSClientConfiguration::getBaseUriSuffix(bool enableCdn) const {
  if (enableCdn && _enableHttps) {
    return URI_FDS_SSL_SUFFIX;
  } else {
    return URI_FDS_SUFFIX;
  }
}

} // namespace fds
} // namespace galaxy
