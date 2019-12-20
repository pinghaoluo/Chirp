#include "chirp_obj.h"

ChirpObj::ChirpObj() : username_(), text_(), id_(), parent_id_(), time_() {}

ChirpObj::ChirpObj(const std::string& uname, const std::string& text,
                   const std::optional<std::string>& parent_id)
    : username_(uname), text_(text) {
  parent_id_ = parent_id.value_or("");
  time_ = timestamp::MakeTimeStamp();
  id_ = std::to_string(time_.seconds) + "-" + std::to_string(time_.useconds) +
        "-" + username_;
}

ChirpObj::ChirpObj(const std::string& uname, const std::string& text,
                   const std::string& id, const std::string& parent_id,
                   int seconds, int useconds)
    : username_(uname), text_(text), id_(id), parent_id_(parent_id) {
  TimeStamp ts = {seconds, useconds};
  time_ = ts;
}

std::string ChirpObj::to_string() {
  std::string s;
  s += std::to_string(id_.length()) + "|" + id_;
  s += std::to_string(text_.length()) + "|" + text_;
  s += std::to_string(parent_id_.length()) + "|" + parent_id_;
  return s;
}

const std::string ChirpObj::print_string() const {
  std::string s;
  s += "user: " + username_ + "\n";
  s += "chirp id: " + id_ + "\n";
  s += "time: " + std::to_string(time_.seconds) + "-" +
       std::to_string(time_.useconds) + "\n";
  if (!parent_id_.empty()) {
    s += "reply id: " + parent_id_ + "\n";
  }
  s += "chirp text: " + text_ + "\n";
  return s;
}

TimeStamp timestamp::MakeTimeStamp() {
  TimeStamp ts;
  timeval t;
  gettimeofday(&t, nullptr);
  ts.seconds = t.tv_sec;
  ts.useconds = t.tv_usec;
  return ts;
}
