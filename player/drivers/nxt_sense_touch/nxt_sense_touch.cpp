#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <libplayercore/playercore.h>

#define DEVICE_PATH "device_path"

class NXT_Sense_touch : public ThreadedDriver {
public:
  NXT_Sense_touch(ConfigFile *cf, int section);
  
  virtual int ProcessMessage(QueuePointer &resp_queue,
			     player_msghdr *hdr,
			     void *data);

private:
  virtual void Main();
  virtual int MainSetup();
  virtual void MainQuit();

  player_devaddr_t bumper_id;
  std::string path_to_device;
  std::ifstream device;
};

/* Factory function to create a new instance of NXT_Sense_touch */
Driver *NXT_Sense_touch_init(ConfigFile *cf, int section) {
  return (Driver *) (new NXT_Sense_touch(cf, section));
}

/* Registration function */
void NXT_Sense_touch_register(DriverTable *table) {
  table->AddDriver("nxt_sense_touch", NXT_Sense_touch_init);
}

NXT_Sense_touch::NXT_Sense_touch(ConfigFile *cf, int section)
  : ThreadedDriver(cf, section, true, PLAYER_MSGQUEUE_DEFAULT_MAXLEN, PLAYER_BUMPER_CODE) {

  if (cf->ReadDeviceAddr(&bumper_id, section, "provides", PLAYER_BUMPER_CODE, -1, NULL) != 0) {
    PLAYER_ERROR("Bumper not defined with a device address in the config file!");
  }

  path_to_device.assign(cf->ReadFilename(section, DEVICE_PATH, ""));
  if (path_to_device.empty()) {
    std::cerr << "No device path supplied in the config file!\n";
  }
}

int NXT_Sense_touch::MainSetup() {
  std::cout << "The PLAYER_BUMPER_CODE: " << PLAYER_BUMPER_CODE << std::endl;
  std::cout << "NXT_Sense Touch driver initialising - main setup" << std::endl;

  device.open(path_to_device.c_str(), std::ifstream::in);
  
  if (!device.is_open()) {
    return -1;
  }

  return 0;
}

void NXT_Sense_touch::MainQuit() {
  std::cout << "NXT_Sense Touch driver shutdown" << std::endl;

  device.close();
}

int NXT_Sense_touch::ProcessMessage(QueuePointer &resp_queue,
				    player_msghdr *hdr,
				    void *data) {

  /*
  std::cout << "Process Message info:\n";
  std::cout << "hdr->type: " << hdr->type << "\n";
  std::cout << "hdr->subtype: " << hdr->subtype << "\n";


  player_bumper_data_t bumper_data;
  bumper_data.bumpers_count = 1;
  bumper_data.bumpers = new uint8_t[bumper_data.bumpers_count];

  //  string data;


  if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_REQ, PLAYER_BUMPER_REQ_GET_GEOM, bumper_id)) {
    device >> bumper_data.bumpers[0];
    //    device.getline(&bumper_data.bumpers[0], 4    

    // Read bumper value
    //      Publish(bumper_id, resp_queue, PLAYER_MSGTYPE_RESP_ACK, hdr->subtype, &bumper_data
    Publish(bumper_id, resp_queue, PLAYER_MSGTYPE_RESP_ACK, PLAYER_BUMPER_DATA_STATE, &bumper_data);
  }

  delete [] bumper_data.bumpers;

  std::cout << "End of process message info" << std::endl;
  
  */
  return 0;
}

void NXT_Sense_touch::Main() {
  player_bumper_data_t bumper_data;
  bumper_data.bumpers_count = 1;
  bumper_data.bumpers = new uint8_t[bumper_data.bumpers_count];

  bumper_data.bumpers[0] = 1;

  player_bumper_geom_t geom;
  geom.bumper_def_count = bumper_data.bumpers_count;
  geom.bumper_def = new player_bumper_define_t[geom.bumper_def_count];

  for (;;) {
    pthread_testcancel();

    //std::cout << "Going to process messages" << std::endl;
    ProcessMessages();
    //std::cout << "Done processing messages" << std::endl;    

#if 0
    std::cout << "Publishing player bumper data geom" << std::endl;
    Publish(bumper_id, PLAYER_MSGTYPE_DATA, PLAYER_BUMPER_DATA_GEOM, &geom);
#endif

    std::cout << "Publishing player bumper data state" << std::endl;
    this->Publish(this->bumper_id, PLAYER_MSGTYPE_DATA, PLAYER_BUMPER_DATA_STATE, reinterpret_cast<void *>(&bumper_data));


    usleep(1000000);
  }

  delete [] bumper_data.bumpers;
  delete [] geom.bumper_def;
}

extern "C" {
  int player_driver_init(DriverTable *table) {
    std::cout << "NXT_Sense touch driver initialising" << std::endl;
    NXT_Sense_touch_register(table);
    std::cout << "NXT_Sense touch driver done" << std::endl;

    return 0;
  }
}
    
