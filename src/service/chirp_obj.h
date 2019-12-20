#ifndef SRC_SERVICE_CHIRP_OBJ_H_
#define SRC_SERVICE_CHIRP_OBJ_H_

#include <sys/time.h>
#include <optional>
#include <string>

// Struct to handle timestamps for ChirpObjs
struct TimeStamp {
  int seconds;
  int useconds;
};

namespace timestamp {
// Helper function to create TimeStamp struct
TimeStamp MakeTimeStamp();
}  // namespace timestamp

// Wrapper object for Chirp message
class ChirpObj {
 public:
  // Default constructor for Chirp object, used on failure
  ChirpObj();

  // Constructor for Chirp object when sending from user side
  ChirpObj(const std::string& uname, const std::string& text,
           const std::optional<std::string>& parent_id);

  // Constructor for Chirp object when receiving from server side
  ChirpObj(const std::string& uname, const std::string& text,
           const std::string& id, const std::string& parent_id, int seconds,
           int useconds);

  // Getter for `username_`
  inline const std::string& username() const { return username_; }

  // Getter for `text_`
  inline const std::string& text() const { return text_; }

  // Getter for `id_`
  inline const std::string& id() const { return id_; }

  // Getter for `parent_id`
  inline const std::string& parent_id() const { return parent_id_; }

  // Getter for `time_`
  inline const TimeStamp& time() const { return time_; }

  // Helper method to convert Chirp into a string for GRPC transfer
  std::string to_string();

  // Helper method to print Chirp data
  const std::string print_string() const;

 private:
  std::string username_;
  std::string text_;
  std::string id_;
  std::string parent_id_;
  TimeStamp time_;
};

#endif  // SRC_SERVICE_CHIRP_OBJ_H_
