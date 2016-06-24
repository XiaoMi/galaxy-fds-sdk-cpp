#include "gtest/gtest.h"

#include "fds_client_configuration.h"

using namespace std;
using namespace galaxy::fds;

TEST(FDSClientConfigurationTest, Positive) {
  FDSClientConfiguration config;

  // Test flag enableCdnForUpload
  config.setRegionName("regionName");
  config.enableCdnForUpload(false);
  EXPECT_EQ("https://regionName." + FDSClientConfiguration::URI_SUFFIX,
      config.getUploadBaseUri());
  config.enableCdnForUpload(true);
  EXPECT_EQ("https://cdn.regionName." + FDSClientConfiguration::URI_CDN_SUFFIX,
      config.getUploadBaseUri());
  config.enableHttps(false);
  EXPECT_EQ("http://cdn.regionName." + FDSClientConfiguration::URI_CDN_SUFFIX,
      config.getUploadBaseUri());

  // Test flag enableCdnForDownload
  config.enableCdnForDownload(false);
  EXPECT_EQ("http://regionName." + FDSClientConfiguration::URI_SUFFIX,
      config.getDownloadBaseUri());
  config.enableCdnForDownload(true);
  EXPECT_EQ("http://cdn.regionName." + FDSClientConfiguration::URI_CDN_SUFFIX,
      config.getDownloadBaseUri());
  config.enableHttps(true);
  EXPECT_EQ("https://cdn.regionName." + FDSClientConfiguration::URI_CDN_SUFFIX,
      config.getDownloadBaseUri());
}
