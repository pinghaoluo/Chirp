#ifndef SRC_SERVICE_SERVICE_LAYER_H_
#define SRC_SERVICE_SERVICE_LAYER_H_

#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "chirp_obj.h"
#include "store/data_store.h"

// Model class for service layer component of Chirp system
class ServiceLayer {
 public:
  // Default constructor
  ServiceLayer();

  // Registers a new user with the given username
  // @uname: username of new user
  // @ret: success or failure of registration
  bool Register(const std::string& uname);

  // Signals a new Chirp from given user
  // @uname: username that posted the Chirp
  // @text: text of the Chirp
  // @reply_id: the ID number of the Chirp, this Chirp is replying to
  // @ret: the Chirp created from request
  ChirpObj MakeChirp(const std::string& uname, const std::string& text,
                     const std::optional<std::string>& reply_id);

  // A user wants to follow another user's Chirps
  // @uname: the user that will be following another
  // @follow_uname: the user to be followed
  // @ret: success of follow request
  bool Follow(const std::string& uname, const std::string& follow_uname);

  // Reads a Chirp thread from the given id
  // @id: the Chirp id to begin reading from
  // @ret: vector of Chirps forming the requested thread
  std::vector<ChirpObj> Read(const std::string& id);

  // Streams Chirps from all followed users
  // @uname: the user requesting the monitor
  // @ret: the Chirps of all followed users
  std::vector<ChirpObj> Monitor(const std::string& uname);

 private:
  // Used for follow storage in store
  const std::string kFollowKey_ = "-follow-";

  // Used for counting number of followers for a user in store
  const std::string kFollowCounterKey_ = "-follow-counter";

  // Used for monitor storage in store
  const std::string kMonitorKey_ = "-monitor-";

  // Used for monitor checking in store
  const std::string kMonitorCheckKey_ = "-monitor-check-";

  // Used for reply storage in store
  const std::string kReplyKey_ = "-reply-";

  // Used for counting number of replies to a chirp in store
  const std::string kReplyCounterKey_ = "-reply-counter";

  DataStore ds_;  // private DataStore for testing purposes

  // Helper function to parse data from DataStore
  // @chirp: the string rep of a ChirpObj stored in the DataStore
  // @ret: ChirpObj constructed from `chirp`
  ChirpObj ParseChirpString(const std::string& chirp);

  // Helper function to set up Read data for Chirps replying to another Chirp
  // @parent_id: id of the Chirp being replied to
  // @chirp_string: chirp to_string() of current ChirpObj
  void MakeReply(const std::string& parent_id, const std::string& chirp_string);

  // Helper function to grab all replies for Read. Performs DFS
  // @key: the key to Get() from DataStore
  // @chirps: vector in which to store replies in DFS order
  // @counter: tracker for which reponse to fetch
  void ReadDfs(const std::string& key_base, std::vector<ChirpObj>* chirps,
               int counter);

  // Helper function to find all usernames of people `uname` is following
  // @uname: current user
  // @ret: vector of all users `uname` is following
  std::vector<std::string> GetUsersFollowed(const std::string& uname);

  // Helper fuction to add Monitor bookkeeping key to DS
  // @uname: current user
  // @followed_user: user `uname` user is following and wants to monitor
  void PutMonitorKey(const std::string& uname,
                     const std::string& followed_user);

  // Helper function to check if `uname` is being monitored
  // @uname: user who is being monitored by another user
  // @chirp_string: the chirp `uname` has just made
  void CheckForMonitorKey(const std::string& uname,
                          const std::string& chirp_string);

  // Helper function to store chirp made by user monitored by `uname`
  // @uname: the user who is monitoring another
  // @chirp_string: data of Chirp just made by a user `uname` is following
  void UpdateMonitor(const std::string& uname, const std::string& chirp_string);
};

#endif  // SRC_SERVICE_SERVICE_LAYER_H_
