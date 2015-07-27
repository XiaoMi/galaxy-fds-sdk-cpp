#include "gtest/gtest.h"

#include "fds_client_configuration.h"

using namespace std;
using namespace galaxy::fds;

TEST(FDSClientConfigurationTest, Positive) {
  FDSClientConfiguration config;

  // Test flag enableCdnForUpload
  config.enableCdnForUpload(false);
  EXPECT_EQ("https://files" + FDSClientConfiguration::URI_FDS_SUFFIX,
      config.getUploadBaseUri());
  config.enableCdnForUpload(true);
  EXPECT_EQ("https://cdn" + FDSClientConfiguration::URI_FDS_SSL_SUFFIX,
      config.getUploadBaseUri());
  config.enableHttps(false);
  EXPECT_EQ("http://cdn" + FDSClientConfiguration::URI_FDS_SUFFIX,
      config.getUploadBaseUri());

  // Test flag enableCdnForDownload
  config.enableCdnForDownload(false);
  EXPECT_EQ("http://files" + FDSClientConfiguration::URI_FDS_SUFFIX,
      config.getDownloadBaseUri());
  config.enableCdnForDownload(true);
  EXPECT_EQ("http://cdn" + FDSClientConfiguration::URI_FDS_SUFFIX,
      config.getDownloadBaseUri());
  config.enableHttps(true);
  EXPECT_EQ("https://cdn" + FDSClientConfiguration::URI_FDS_SSL_SUFFIX,
      config.getDownloadBaseUri());
  
  // Test region name
  config.setRegionName("staging");
  EXPECT_EQ("https://staging-cdn" + FDSClientConfiguration::URI_FDS_SSL_SUFFIX,
      config.getDownloadBaseUri());
}
