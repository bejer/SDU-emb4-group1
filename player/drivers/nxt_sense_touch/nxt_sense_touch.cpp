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

  path_to_device.assign(cf->ReadFilename(section, DEVICE_PATH, ""));
  if (path_to_device.empty()) {
    std::cerr << "No device path supplied in the config file!\n";
  }
}

int NXT_Sense_touch::MainSetup() {
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
  return 0;
}

void NXT_Sense_touch::Main() {
  for (;;) {
    pthread_testcancel();

    ProcessMessages();

    usleep(100000);
  }
}

extern "C" {
  int player_driver_init(DriverTable *table) {
    std::cout << "NXT_Sense touch driver initialising" << std::endl;
    NXT_Sense_touch_register(table);
    std::cout << "NXT_Sense touch driver done" << std::endl;

    return 0;
  }
}
    
