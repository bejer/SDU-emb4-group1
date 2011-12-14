PKG_CONFIG_PATH:=${HOME}/Temp/player_stage/install_of_player-3.0.2/lib64/pkgconfig

simple_example: simple_example.cpp
	g++ -o simple_example `pkg-config --cflags playerc++` `pkg-config --libs playerc++` simple_example.cpp

.PHONY: clean

clean:
	rm *.o *.so