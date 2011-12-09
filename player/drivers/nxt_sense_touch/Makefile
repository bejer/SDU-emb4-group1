# Desc: Example plugin makefile
# CVS: $Id$

CROSS_COMPILE_SYSROOT := /home/michael/embedded/emb4/overo-oe-mich/build/tmp/sysroots/armv7a-angstrom-linux-gnueabi
PLAYER_INCLUDE_DIR := /home/michael/embedded/emb4/overo-oe-mich/build/tmp/work/armv7a-angstrom-linux-gnueabi/player-3.1.0-r1/package/usr/include/player-3.1
CXX := /home/michael/embedded/emb4/overo-oe-mich/build/tmp/sysroots/x86_64-linux/usr/armv7a/bin/arm-angstrom-linux-gnueabi-g++

all: libnxt_sense_touch_driver.so

%.o: %.cpp
#	$(CXX) -Wall -fpic -g3 `pkg-config --cflags playercore` -c $<
# Manually inspecting the pkg-config cflags for playercore:
#/home/michael/embedded/emb4/overo-oe-mich/build/tmp/work/armv7a-angstrom-linux-gnueabi/player-3.1.0-r1/package/usr/lib64/pkgconfig/playercore.pc
#/home/michael/embedded/emb4/overo-oe-mich/build/tmp/work/armv7a-angstrom-linux-gnueabi/player-3.1.0-r1/image/usr/lib64/pkgconfig/playercore.pc
	$(CXX) -Wall -fpic -g3 --sysroot=${CROSS_COMPILE_SYSROOT} -I${PLAYER_INCLUDE_DIR} -c $<

libnxt_sense_touch_driver.so: nxt_sense_touch.o
	$(CXX) -shared -fpic --sysroot=${CROSS_COMPILE_SYSROOT} -o $@ $^

clean:
	rm -f *.o *.so
