import ctypes
import playerc

c = playerc.playerc_client(None, '10.194.63.51', 6665)

if c.connect() != 0:
    raise playerc.playerc_error_str()

p = playerc.playerc_bumper(c, 0)

if p.subscribe(playerc.PLAYERC_OPEN_MODE) != 0:
    raise playerc.playerc_error_str()

for i in xrange(0, 10):
    ret = c.read()
    if ret == None:
        print("None received from c.read()")
        raise player.playerc_error_str()
    else:
        print("ID of proxy (ret): {}".format(ret))
        print("Bumper count: {}".format(p.bumper_count))
        for bumper_no in xrange(0, p.bumper_count):
#            print("Bumper: {}".format(p.bumpers[bumper_no]))
            holder = ctypes.c_byte()
            holder = p.bumpers
            lame = ctypes.cast(holder, ctypes.POINTER(ctypes.c_byte))
#            print("Bumper holder: {}".format(holder.own))
            print("Bumper holder: {}".format(lame.content))
#            playerc.playerc_device_get_boolprop(p, "bumpers", byref(holder))
 #           print("Bumper holder: {}".format(holder))

p.unsubscribe()
c.disconnect()
