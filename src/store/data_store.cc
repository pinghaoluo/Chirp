#include "data_store.h"

DataStore::DataStore() : table_(), lock_() {}

bool DataStore::Put(const std::string& key, const std::string& val) {
  std::lock_guard<std::mutex> lg(lock_);

  table_[key].push_back(val);
  return true;
}

std::vector<std::string> DataStore::Get(const std::string& key) {
  std::lock_guard<std::mutex> lg(lock_);
  // Iterator through `table_`
  auto val = table_.find(key);
  if (val == table_.end()) {
    return {};
  }

  return val->second;
}

bool DataStore::DeleteKey(const std::string& key) {
  std::lock_guard<std::mutex> lg(lock_);

  return table_.erase(key);
}
