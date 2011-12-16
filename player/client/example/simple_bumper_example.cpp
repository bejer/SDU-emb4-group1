#include <iostream>
#include <libplayerc++/playerc++.h>

int main(int argc, char *argv[]) {
  PlayerCc::PlayerClient robot("10.194.63.51", 6665, PLAYERC_TRANSPORT_TCP);
  robot.SetDataMode(PLAYER_DATAMODE_PUSH);
  PlayerCc::BumperProxy bp(&robot, 0); // the bumper is at device index 0

  std::cout << "bp: " << bp << "\n";
  
  for(int i = 0; i < 10; ++i) {
    std::cout << "------------ " << i << " --------------\n";
    robot.Read();

    if (bp.IsFresh()) {
      std::cout << "bp is fresh!\n";
    }

    int counts = bp.GetCount();
    for (int i = 0; i < counts; ++i) {
      std::cout << "bp[" << i << "] : " << bp[i] << "\n";
    }

  }

  // The objects are destroyed automatically and their destructors are shutting down the client nicely.

  return 0;
}
