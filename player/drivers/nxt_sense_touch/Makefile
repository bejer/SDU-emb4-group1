# ${OVEROTOP} is pointing to where you installed overo-oe / OpenEmbedded (probably ~/overo-oe/)
PLAYER_VERSION=player-3.1.0-r0
CROSS_COMPILE_SYSROOT := ${OVEROTOP}/tmp/sysroots/armv7a-angstrom-linux-gnueabi
PLAYER_INCLUDE_DIR := ${OVEROTOP}/tmp/work/armv7a-angstrom-linux-gnueabi/${PLAYER_VERSION}/package/usr/include/player-3.1
CXX := ${OVEROTOP}/tmp/sysroots/x86_64-linux/usr/armv7a/bin/arm-angstrom-linux-gnueabi-g++

all: libnxt_sense_touch_driver.so

%.o: %.cpp
#	$(CXX) -Wall -fpic -g3 `pkg-config --cflags playercore` -c $<
# Manually inspecting the pkg-config cflags for playercore:
#${OVEROTOP}/tmp/work/armv7a-angstrom-linux-gnueabi/player-3.1.0-r0/package/usr/lib64/pkgconfig/playercore.pc
#${OVEROTOP}/tmp/work/armv7a-angstrom-linux-gnueabi/player-3.1.0-r0/image/usr/lib64/pkgconfig/playercore.pc
	$(CXX) -Wall -fpic -g3 --sysroot=${CROSS_COMPILE_SYSROOT} -I${PLAYER_INCLUDE_DIR} -c $<

libnxt_sense_touch_driver.so: nxt_sense_touch.o
	$(CXX) -shared -fpic --sysroot=${CROSS_COMPILE_SYSROOT} -o $@ $^

clean:
	rm -f *.o *.so
