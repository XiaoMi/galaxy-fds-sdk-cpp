/**
 * Copyright 2015, Xiaomi.
 * All rights reserved.
 * Author: zhangjunbin@xiaomi.com
 */

#ifndef FDS_CLIENT_CONFIGURATION_H_
#define FDS_CLIENT_CONFIGURATION_H_

#include <string>

namespace galaxy {
namespace fds {

class FDSClientConfiguration {
public:
  static const std::string URI_HTTP_PREFIX;

  static const std::string URI_HTTPS_PREFIX;

  static const std::string URI_CDN;

  static const std::string URI_SUFFIX;

  static const std::string URI_CDN_SUFFIX;

  FDSClientConfiguration();

  const std::string& endpoint() const;

  void setEndpoint(const std::string& endpoint);

  const std::string& regionName() const;

  void setRegionName(const std::string& regionName);

  bool isHttpsEnabled() const;

  void enableHttps(bool enableHttps);

  bool isCdnEnabledForUpload() const;

  void enableCdnForUpload(bool enableCdnForUpload);

  bool isCdnEnabledForDownload() const;

  void enableCdnForDownload(bool enableCdnForDownload);

  std::string getBaseUri() const;

  std::string getCdnBaseUri() const;

  std::string getUploadBaseUri() const;

  std::string getDownloadBaseUri() const;

private:
  std::string buildBaseUri(bool enableCdn) const;

private:
  std::string _endpoint;
  std::string _regionName;
  bool _enableHttps;
  bool _enableCdnForUpload;
  bool _enableCdnForDownload;
}; // class FDSClientConfiguration

inline const std::string& FDSClientConfiguration::endpoint() const {
  return _endpoint;
}

inline void FDSClientConfiguration::setEndpoint(const std::string& endpoint) {
  _endpoint = endpoint;
}

inline const std::string& FDSClientConfiguration::regionName() const {
  return _regionName;
}

inline void FDSClientConfiguration::setRegionName(const std::string&
    regionName) {
  _regionName = regionName;
}

inline bool FDSClientConfiguration::isHttpsEnabled() const {
  return _enableHttps;
}

inline void FDSClientConfiguration::enableHttps(bool enableHttps) {
  _enableHttps = enableHttps;
}

inline bool FDSClientConfiguration::isCdnEnabledForUpload() const {
  return _enableCdnForUpload;
}

inline void FDSClientConfiguration::enableCdnForUpload(bool enableCdnForUpload) {
  _enableCdnForUpload = enableCdnForUpload;
}

inline bool FDSClientConfiguration::isCdnEnabledForDownload() const {
  return _enableCdnForDownload;
}

inline void FDSClientConfiguration::enableCdnForDownload(
    bool enableCdnForDownload) {
  _enableCdnForDownload = enableCdnForDownload;
}

inline std::string FDSClientConfiguration::getBaseUri() const {
  return buildBaseUri(false);
}

inline std::string FDSClientConfiguration::getCdnBaseUri() const {
  return buildBaseUri(true);
}

inline std::string FDSClientConfiguration::getUploadBaseUri() const {
  return buildBaseUri(_enableCdnForUpload);
}

inline std::string FDSClientConfiguration::getDownloadBaseUri() const {
  return buildBaseUri(_enableCdnForDownload);
}

} // namespace fds
} // namespace galaxy

#endif // FDS_CLIENT_CONFIGURATION_H_
