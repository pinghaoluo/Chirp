#ifndef SRC_STORE_DATA_STORE_H_
#define SRC_STORE_DATA_STORE_H_

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

// Model class for data store component of Chirp system. Uses unordered_map to
// store data.
class DataStore {
 public:
  // Default constructor for DataStore class. Will initialize as empty table
  // with lock
  DataStore();

  // Adds key value pair to table_
  // @key: key under which new `val` will be added
  // @val: the value to be added
  // @ret: true on successful insertion
  bool Put(const std::string& key, const std::string& val);

  // Returns vector<string> storing all values from requested key
  // @key: the key to be looked up
  // @ret: the vector at `key` that stores all values associated with `key`
  std::vector<std::string> Get(const std::string& key);

  // Deletes the value at given `key`
  // @key: the key of the key value pair to be removed
  // @ret: true on successful removal
  bool DeleteKey(const std::string& key);

 private:
  // Hashtable to store key value pairs
  std::unordered_map<std::string, std::vector<std::string> > table_;

  // Lock associated with `table_` to make threadsafe
  std::mutex lock_;
};

#endif  // SRC_STORE_DATA_STORE_H_
