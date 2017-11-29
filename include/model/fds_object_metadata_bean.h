#ifndef GALAXY_FDS_OBJECT_METADATA_BEAN_H_
#define GALAXY_FDS_OBJECT_METADATA_BEAN_H_

#include <map>
#include <string>
#include <memory>


namespace galaxy {
namespace fds {
class MetadataBean {
public:
  void add(const std::string& key, const std::string& value);

  const std::map<std::string, std::string>& metadata() const;

  static std::shared_ptr<MetadataBean> deserialize(std::istream& is);

  static std::string serialize(const MetadataBean& metadataBean);


private:
  std::map<std::string, std::string> rawMeta;
};

inline void MetadataBean::add(const std::string& key, const std::string& value) {
  rawMeta.insert(std::pair<std::string, std::string>(key, value));
}

inline const std::map<std::string, std::string>& MetadataBean::metadata() const {
  return rawMeta;
}


}
}

#endif // GALAXY_FDS_OBJECT_METADATA_BEAN_H_