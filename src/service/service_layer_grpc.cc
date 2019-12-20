#include "service_layer_grpc.h"

ServiceLayerObj::ServiceLayerObj()
    : ds_(grpc::CreateChannel("0.0.0.0:50000",
                              grpc::InsecureChannelCredentials())) {}

bool ServiceLayerObj::Register(const std::string& uname) {
  // std::cout<<"testing"<<std::endl;
  std::vector<std::string> entry = ds_.Get(AllUserKey_); 

  if (uname.empty() || !ds_.Get(uname).empty()) {
    return false;
  }
  ds_.Put(AllUserKey_,uname);
  return ds_.Put(uname, "registered");
}

ChirpObj ServiceLayerObj::MakeChirp(
    const std::string& uname, const std::string& text,
    const std::optional<std::string>& reply_id) {
  if (ds_.Get(uname).empty()) {
    return ChirpObj();
  }

  ChirpObj chirp(uname, text, reply_id);
  if (ds_.Put(chirp.id(), chirp.to_string())) {
    if (!chirp.parent_id().empty()) {
      MakeReply(chirp.parent_id(), chirp.to_string());
    }
    CheckForMonitorKey(uname, chirp.to_string());
    return chirp;
  }

  return ChirpObj();
}



void ServiceLayerObj::MakeReply(const std::string& parent_id,
                                const std::string& chirp_string) {
  // Implementation based on feedback from Ralph Chung
  std::string reply_counter_key = parent_id + kReplyCounterKey_;
  auto reply_count = ds_.Get(reply_counter_key);
  int counter = 0;
  if (!reply_count.empty()) {
    counter = std::stoi(reply_count[0]);
  }

  std::string put_reply_key = parent_id + kReplyKey_ + std::to_string(counter);
  ds_.Put(put_reply_key, chirp_string);
  ds_.Put(reply_counter_key, std::to_string(counter + 1));
}

bool ServiceLayerObj::Follow(const std::string& uname,
                             const std::string& follow_uname) {
  // Implementation based on feedback from Ralph Chung
  if (ds_.Get(uname).empty() || ds_.Get(follow_uname).empty()) {
    return false;
  }
  std::string follow_counter_key = uname + kFollowCounterKey_;
  auto follow_count = ds_.Get(follow_counter_key);
  int counter = 0;
  if (!follow_count.empty()) {
    counter = std::stoi(follow_count[0]);
  }

  std::string key = uname + kFollowKey_ + std::to_string(counter);
  bool check = ds_.Put(key, follow_uname);
  if (check) {
    // Only want to update counter on successful Put
    ds_.Put(follow_counter_key, std::to_string(counter + 1));
  }
  return check;
}

std::vector<ChirpObj> ServiceLayerObj::Read(const std::string& id) {
  std::vector<ChirpObj> replies;
  std::string chirp_id = id;
  std::vector<std::string> chirp_entry = ds_.Get(chirp_id);

  if (!chirp_entry.empty()) {
    ChirpObj chirp = ParseChirpString(chirp_entry[0]);
    replies.push_back(chirp);
    std::string reply_key_base = chirp.id() + kReplyKey_;
    ReadDfs(reply_key_base, &replies, 0);
  }

  return replies;
}

void ServiceLayerObj::ReadDfs(const std::string& key_base,
                              std::vector<ChirpObj>* chirps, int counter) {
  std::vector<std::string> chirp_entry =
      ds_.Get(key_base + std::to_string(counter));

  // Base Case
  if (chirp_entry.empty()) {
    return;
  }

  ChirpObj reply = ParseChirpString(chirp_entry[0]);
  chirps->push_back(reply);

  // c has a reply
  std::string reply_key_base = reply.id() + kReplyKey_;
  ReadDfs(reply_key_base, chirps, 0);

  // another reply to same parent
  ReadDfs(key_base, chirps, counter + 1);
}

std::vector<ChirpObj> ServiceLayerObj::HashTag(const std::string& uname,const std::string& hash_tag) {
  std::vector<std::string> users = ds_.Get(AllUserKey_); 
  // std::cout<<uname<<": "<<hash_tag<<std::endl;
  // Setting up DS to store monitor data
  for (const std::string& u : users) {
    PutMonitorKey(uname, u);
  }

  std::vector<ChirpObj> chirps;
  std::string monitor_check_base = uname + kMonitorCheckKey_;
  int counter = 0;
  std::string key = monitor_check_base + std::to_string(counter);
  std::vector<std::string> entry = ds_.Get(key);

  // Loop over all potential monitor keys present
  while (!entry.empty()) {
    ChirpObj c = ParseChirpString(entry[0]);
    chirps.push_back(c);
    // No longer need this key once entry has been stored
    ds_.DeleteKey(key);
    counter++;
    key = monitor_check_base + std::to_string(counter);
    entry = ds_.Get(key);
  }

  CheckTag(chirps,hash_tag);

  return chirps;
}

std::vector<ChirpObj> ServiceLayerObj::Monitor(const std::string& uname) {
  std::vector<std::string> users = GetUsersFollowed(uname);

  // Setting up DS to store monitor data
  for (const std::string& u : users) {
    PutMonitorKey(uname, u);
  }

  std::vector<ChirpObj> chirps;
  std::string monitor_check_base = uname + kMonitorCheckKey_;
  int counter = 0;
  std::string key = monitor_check_base + std::to_string(counter);
  std::vector<std::string> entry = ds_.Get(key);

  // Loop over all potential monitor keys present
  while (!entry.empty()) {
    ChirpObj c = ParseChirpString(entry[0]);
    chirps.push_back(c);
    // No longer need this key once entry has been stored
    ds_.DeleteKey(key);
    counter++;
    key = monitor_check_base + std::to_string(counter);
    entry = ds_.Get(key);
  }

  return chirps;
}

std::vector<ChirpObj> ServiceLayerObj::CheckTag(std::vector<ChirpObj> &chirps,const std::string& hash_tag){
     std::cout<<": "<<hash_tag<<std::endl;
  for(std::vector<ChirpObj>::iterator it = chirps.begin(); it != chirps.end();) {
      bool contain = false;
      std::string text = it->text();
      // holds all the positions that sub occurs within #
      std::vector<size_t> positions; 
      size_t pos = text.find("#", 0);
      while(pos != std::string::npos)
      {
          positions.push_back(pos);
          pos = text.find("#",pos+1);
      }
      if(positions.size() == 0){
        it = chirps.erase(it);
        continue;
      }

      for(auto p:positions){
        std::string sub = text.substr(p);
        std::size_t pos1 = sub.find(" ");
        std::string tag = sub.substr (1,pos1);
        tag.erase( std::remove_if( tag.begin(), tag.end(), ::isspace ), tag.end() );
        if(tag == hash_tag){
          std::cout<<tag<<std::endl;
          std::cout<<hash_tag<<std::endl;
          contain = true;
        }
      }
      if(contain == false){
        it = chirps.erase(it);
        continue;
      }else{
        it++;
      }

    }

  return chirps;
}

void ServiceLayerObj::PutMonitorKey(const std::string& uname,
                                    const std::string& followed_user) {
  std::string monitor_key_base = followed_user + kMonitorKey_;
  int counter = 0;
  std::string key = monitor_key_base + std::to_string(counter);
  std::vector<std::string> check = ds_.Get(key);

  while (!check.empty()) {
    if (check[0] == uname) {
      // Already have Monitor bookkeeping entry
      return;
    }
    counter++;
    key = monitor_key_base + std::to_string(counter);
    check = ds_.Get(key);
  }

  // No matching monitor keys
  ds_.Put(key, uname);
}

void ServiceLayerObj::CheckForMonitorKey(const std::string& uname,
                                         const std::string& chirp_string) {
  std::string monitor_key_base = uname + kMonitorKey_;
  int counter = 0;
  std::string key = monitor_key_base + std::to_string(counter);
  std::vector<std::string> entry = ds_.Get(key);

  while (!entry.empty()) {
    std::string monitorer = entry[0];
    UpdateMonitor(monitorer, chirp_string);
    counter++;
    key = monitor_key_base + std::to_string(counter);
    entry = ds_.Get(key);
  }
}

void ServiceLayerObj::UpdateMonitor(const std::string& uname,
                                    const std::string& chirp_string) {
  std::string monitor_check_base = uname + kMonitorCheckKey_;
  int counter = 0;
  std::string key = monitor_check_base + std::to_string(counter);
  std::vector<std::string> entry = ds_.Get(key);

  while (!entry.empty()) {
    counter++;
    key = monitor_check_base + std::to_string(counter);
    entry = ds_.Get(key);
  }

  ds_.Put(key, chirp_string);
}


std::vector<std::string> ServiceLayerObj::GetUsersFollowed(
    const std::string& uname) {
  std::vector<std::string> users;
  std::string follow_key_base = uname + kFollowKey_;
  int counter = 0;
  std::string key = follow_key_base + std::to_string(counter);
  std::vector<std::string> entry = ds_.Get(key);

  while (!entry.empty()) {
    users.push_back(entry[0]);
    counter++;
    key = follow_key_base + std::to_string(counter);
    entry = ds_.Get(key);
  }

  return users;
}

ChirpObj ServiceLayerObj::ParseChirpString(const std::string& chirp) {
  std::string_view chirp_view(chirp);
  std::size_t id_pos = chirp_view.find("|");
  int id_len = std::stoi(std::string(chirp_view.substr(0, id_pos)));
  std::string id = std::string(chirp_view.substr(id_pos + 1, id_len));
  chirp_view.remove_prefix(id_pos + id_len + 1);

  std::size_t text_pos = chirp_view.find("|");
  int text_len = std::stoi(std::string(chirp_view.substr(0, text_pos)));
  std::string text = std::string(chirp_view.substr(text_pos + 1, text_len));
  chirp_view.remove_prefix(text_pos + text_len + 1);

  std::string parent_id = "";
  if (!chirp_view.empty()) {
    std::size_t parent_pos = chirp_view.find("|");
    int parent_len = std::stoi(std::string(chirp_view.substr(0, parent_pos)));
    parent_id = std::string(chirp_view.substr(parent_pos + 1, parent_len));
  }

  // Parse for timestamp and username from id
  std::vector<std::string> data;
  std::stringstream ss(id);
  std::string token;
  while (std::getline(ss, token, '-')) {
    data.push_back(token);
  }
  // data is now {seconds, useconds, username}

  return ChirpObj(data[2], text, id, parent_id, std::stoi(data[0]),
                  std::stoi(data[1]));
}
