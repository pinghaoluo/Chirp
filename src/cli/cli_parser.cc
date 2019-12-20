#include "cli_parser.h"

// GFLAG definitions for cmdline args
// DECLARED in cli_parser.h to allow external access
DEFINE_string(reg, "", "username for registering a new user");
DEFINE_string(user, "", "username for user making command");
DEFINE_string(chirp, "", "text to be chirped");
DEFINE_string(reply, "", "reply id for given chirp");
DEFINE_string(follow, "", "user to follow");
DEFINE_string(read, "", "returns chirp thread starting with given chirp id");
DEFINE_bool(monitor, false,
            "starts monitoring for all following accounts' chirps");
DEFINE_string(stream, "","starts monitoring for all tags");
CliParser::CliParser()
    : service_(grpc::CreateChannel("0.0.0.0:50002",
                                   grpc::InsecureChannelCredentials())) {}

std::string CliParser::Parse(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  if (!FLAGS_reg.empty()) {
    std::cout<<"Registering"<<std::endl;
    return ParseRegister(FLAGS_reg);
  }

  if (!FLAGS_chirp.empty()) {
        std::cout<<"Chirping"<<std::endl;
    return ParseChirp(FLAGS_user, FLAGS_chirp, FLAGS_reply);
  }

  if (!FLAGS_follow.empty()) {
    std::cout<<"Following"<<std::endl;
    return ParseFollow(FLAGS_user, FLAGS_follow);
  }

  if (!FLAGS_read.empty()) {
    std::cout<<"Reading"<<std::endl;
    return ParseRead(FLAGS_read);
  }

  if (FLAGS_monitor) {
    std::cout<<"Monitoring"<<std::endl;
    return ParseMonitor(FLAGS_user);
  }

  if (!FLAGS_stream.empty()) {
    std::cout<<"Streaming"<<std::endl;
    return ParseStream(FLAGS_user,FLAGS_stream);
  }

  if (!FLAGS_user.empty()) {
    return "Username cannot be blank.";
  }

  return "";
}

std::string CliParser::ParseRegister(const std::string& uname) {
  if (!(FLAGS_user.empty() && FLAGS_chirp.empty() && FLAGS_reply.empty() &&
        FLAGS_follow.empty() && FLAGS_read.empty() && !FLAGS_monitor&& FLAGS_stream.empty())) {
    return "Cannot perform any other commands with Register.";
  }

  if (service_.Register(uname)) {
    return uname + " successfully registered.";
  }
  return uname + " failed to be registered.";
}

std::string CliParser::ParseChirp(const std::string& uname,
                                  const std::string& text,
                                  const std::string& reply_id) {
  if (!(FLAGS_follow.empty() && FLAGS_read.empty() && !FLAGS_monitor && FLAGS_stream.empty())) {
    return "Cannot Follow, Read, or Monitor with Chirp.";
  }

  if (uname.empty()) {
    return "Must be logged in to perform actions.";
  }

  // Determines if reply_id is optional or nullopt
  auto r_id =
      reply_id.empty() ? std::nullopt : std::optional<std::string>{reply_id};
  ChirpObj chirp = service_.Chirp(uname, text, r_id);
  if (chirp.username().empty()) {
    return "Chirp failed. Please check Service Layer connection, user is "
           "registered, and reply_id is valid.";
  }

  return chirp.print_string();
}

std::string CliParser::ParseFollow(const std::string& uname,
                                   const std::string& to_follow_user) {
  if (!(FLAGS_reg.empty() && FLAGS_chirp.empty() && FLAGS_reply.empty() &&
        FLAGS_read.empty() && !FLAGS_monitor && FLAGS_stream.empty())) {
    return "Cannot Register, Reply, Read, or Monitor with Follow.";
  }

  if (uname.empty()) {
    return "Must be logged in to perform actions.";
  }

  if (to_follow_user.empty()) {
    return "Must enter valid username to follow.";
  }

  if (service_.Follow(uname, to_follow_user)) {
    return uname + " is following " + to_follow_user + ".";
  }

  return uname + " or " + to_follow_user + " are not registered.";
}

std::string CliParser::ParseRead(const std::string& chirp_id) {
  if (!(FLAGS_reg.empty() && FLAGS_chirp.empty() && FLAGS_reply.empty() &&
        FLAGS_follow.empty() && !FLAGS_monitor && FLAGS_stream.empty())) {
    return "Cannot Reigster, Chirp, Reply, Follow, or Monitor with Read.";
  }

  auto replies = service_.Read(chirp_id);
  std::string ret;
  for (const auto r : replies) {
    ret += r.print_string() + "\n";
  }

  return ret;
}

std::string CliParser::ParseMonitor(const std::string& uname) {
  if (!(FLAGS_reg.empty() && FLAGS_chirp.empty() && FLAGS_reply.empty() &&
        FLAGS_read.empty() && FLAGS_follow.empty()) && !FLAGS_stream.empty()) {
    return "Cannot Register, Reply, Read, or Follow with Monitor.";
  }

  if (uname.empty()) {
    return "Must be logged in to perform actions.";
  }

  while (true) {
    std::vector<ChirpObj> chirps = service_.Monitor(uname);
    for (const auto c : chirps) {
      std::cout << c.print_string() << std::endl;
    }

    usleep(kMonitorLoopDelay_);
  }

  return "Monitor complete";
}



std::string CliParser::ParseStream(const std::string& uname,const std::string& hash_tag) {
  if (!(FLAGS_reg.empty() && FLAGS_chirp.empty() && FLAGS_reply.empty() &&
        FLAGS_read.empty() && FLAGS_follow.empty()&& !FLAGS_monitor)) {
    return "Cannot Register, Reply, Read, or Follow with Stream.";
  }

  if (hash_tag.empty()) {
    return "Hash tags is empty.";
  }

  if (uname.empty()) {
    return "Must be logged in to perform actions.";
  }

  while (true) {
    std::vector<ChirpObj> chirps = service_.Stream(uname,hash_tag);
    for (const auto c : chirps) {
      std::cout << c.print_string() << std::endl;
    }

    usleep(kMonitorLoopDelay_);
  }

  return "Stream complete";
}
