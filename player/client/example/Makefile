#Remember to have set up the PKG_CONFIG_PATH and the LD_LIBRARY_PATH environment variables

simple_bumper_example: simple_bumper_example.cpp
	g++ -o simple_bumper_example `pkg-config --cflags playerc++` `pkg-config --libs playerc++` simple_bumper_example.cpp

.PHONY: clean

clean:
	rm *.o simple_bumper_example
