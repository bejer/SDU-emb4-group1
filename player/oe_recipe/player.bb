DESCRIPTION = "Cross-platform robot device interface and server"
LICENSE = "GPLv2+ and LGPLv2+"
HOMEPAGE = "http://playerstage.sourceforge.net"
DEPENDS = "libtool jpeg guile"
PN = "player"
PV = 3.1.0
PR = "r1"
SRC_URI = "svn://playerstage.svn.sourceforge.net/svnroot/playerstage/code/player/;module=trunk;rev=HEAD;proto=https"
S="${WORKDIR}/trunk"

inherit pkgconfig

do_configure () {
	cmake -DCMAKE_INSTALL_PREFIX=/usr -DBUILD_EXAMPLES=OFF -DBUILD_DOCUMENTATION=OFF -DBUILD_EXAMPLES=OFF -DBUILD_PLAYERCC=OFF -DBUILD_PLAYERCC_BOOST=OFF\
-DBUILD_PYTHONC_BINDINGS=OFF -DBUILD_SHARED_LIBS=OFF -DBUILD_UTILS=OFF -DBUILD_UTILS_LOGSPLITTER=OFF -DBUILD_UTILS_PLAYERCAM=OFF \
-DBUILD_UTILS_PLAYERJOY=OFF -DBUILD_UTILS_PLAYERNAV=OFF -DBUILD_UTILS_PLAYERPRINT=OFF -DBUILD_UTILS_PLAYERPROP=OFF -DBUILD_UTILS_PLAYERV=OFF \
-DBUILD_UTILS_PLAYERVCR=OFF -DBUILD_UTILS_PLAYERWRITEMAP=OFF -DBUILD_UTILS_PMAP=OFF -DBUILD_UTILS_XMMS=OFF -DENABLE_DRIVER_ACCEL_CALIB=OFF \
-DENABLE_DRIVER_ACR120U=OFF -DENABLE_DRIVER_ACTS=OFF -DENABLE_DRIVER_AIOTOSONAR=OFF -DENABLE_DRIVER_ALSA=OFF -DENABLE_DRIVER_AMCL=OFF \
-DENABLE_DRIVER_AMTECM5=OFF -DENABLE_DRIVER_AMTECPOWERCUBE=OFF -DENABLE_DRIVER_AODV=OFF -DENABLE_DRIVER_ARTOOLKITPLUS=OFF -DENABLE_DRIVER_BLOBTODIO=OFF \ 
-DENABLE_DRIVER_BLOBTRACKER=OFF -DENABLE_DRIVER_BUMPER2LASER=OFF -DENABLE_DRIVER_BUMPERSAFE=OFF -DENABLE_DRIVER_BUMPERTODIO=OFF -DENABLE_DRIVER_CAMERA1394=OFF \
-DENABLE_DRIVER_CAMERACOMPRESS=OFF -DENABLE_DRIVER_CAMERAUNCOMPRESS=OFF -DENABLE_DRIVER_CAMERAUVC=OFF -DENABLE_DRIVER_CAMERAV4L=OFF -DENABLE_DRIVER_CAMERAV4L2=OFF \
-DENABLE_DRIVER_CAMFILTER=OFF -DENABLE_DRIVER_CANONVCC4=OFF -DENABLE_DRIVER_CLODBUSTER=OFF -DENABLE_DRIVER_CMDSPLITTER=OFF -DENABLE_DRIVER_CMUCAM2=OFF \
-DENABLE_DRIVER_CMVISION=OFF -DENABLE_DRIVER_CREATE=ON -DENABLE_DRIVER_CVCAM=OFF -DENABLE_DRIVER_DEADSTOP=OFF -DENABLE_DRIVER_DIOCMD=OFF -DENABLE_DRIVER_DIODELAY=OFF \
-DENABLE_DRIVER_DIOLATCH=OFF -DENABLE_DRIVER_DUMMY=OFF -DENABLE_DRIVER_EEDHCONTROLLER=OFF -DENABLE_DRIVER_EPUCK=OFF -DENABLE_DRIVER_ER1=OFF -DENABLE_DRIVER_ERRATIC=OFF \
-DENABLE_DRIVER_FAKELOCALIZE=OFF -DENABLE_DRIVER_FESTIVAL=OFF -DENABLE_DRIVER_FLEXIPORT=OFF -DENABLE_DRIVER_FLOCKOFBIRDS=OFF -DENABLE_DRIVER_GARCIA=OFF \
-DENABLE_DRIVER_GARMINNMEA=OFF -DENABLE_DRIVER_GBXGARMINACFR=OFF -DENABLE_DRIVER_GBXSICKACFR=OFF -DENABLE_DRIVER_GLOBALIZE=OFF -DENABLE_DRIVER_GOTO=OFF \
-DENABLE_DRIVER_GRIDMAP=OFF -DENABLE_DRIVER_GRIPCMD=OFF -DENABLE_DRIVER_HOKUYO_AIST=OFF -DENABLE_DRIVER_IMAGESEQ=OFF -DENABLE_DRIVER_INHIBITOR=OFF \
-DENABLE_DRIVER_INSIDEM300=OFF -DENABLE_DRIVER_ISENSE=OFF -DENABLE_DRIVER_IWSPY=OFF -DENABLE_DRIVER_KARTOWRITER=OFF -DENABLE_DRIVER_KHEPERA=OFF \
-DENABLE_DRIVER_LASERBAR=OFF -DENABLE_DRIVER_LASERBARCODE=OFF -DENABLE_DRIVER_LASERCSPACE=OFF -DENABLE_DRIVER_LASERCUTTER=OFF -DENABLE_DRIVER_LASERPOSEINTERPOLATOR=OFF \
-DENABLE_DRIVER_LASERPTZCLOUD=OFF -DENABLE_DRIVER_LASERRESCAN=OFF -DENABLE_DRIVER_LASERSAFE=OFF -DENABLE_DRIVER_LASERTORANGER=OFF -DENABLE_DRIVER_LASERVISUALBARCODE=OFF \
-DENABLE_DRIVER_LASERVISUALBW=OFF -DENABLE_DRIVER_LINUXJOYSTICK=OFF -DENABLE_DRIVER_LINUXWIFI=OFF -DENABLE_DRIVER_LOCALBB=OFF -DENABLE_DRIVER_MAPCSPACE=OFF \
-DENABLE_DRIVER_MAPFILE=OFF -DENABLE_DRIVER_MAPSCALE=OFF -DENABLE_DRIVER_MBICP=OFF -DENABLE_DRIVER_MICA2=OFF -DENABLE_DRIVER_MICROSTRAIN=OFF -DENABLE_DRIVER_MOTIONMIND=OFF \
-DENABLE_DRIVER_MRICP=OFF -DENABLE_DRIVER_ND=OFF -DENABLE_DRIVER_NIMU=OFF -DENABLE_DRIVER_NOMAD=OFF -DENABLE_DRIVER_OBOT=OFF -DENABLE_DRIVER_OCEANSERVER=OFF \
-DENABLE_DRIVER_P2OS=OFF -DENABLE_DRIVER_PASSTHROUGH=OFF -DENABLE_DRIVER_PBSLASER=OFF -DENABLE_DRIVER_PHIDGETACC=OFF -DENABLE_DRIVER_PHIDGETIFK=OFF \
-DENABLE_DRIVER_PHIDGETRFID=OFF -DENABLE_DRIVER_PORTIO=OFF -DENABLE_DRIVER_POSTGIS=OFF -DENABLE_DRIVER_PTU46=OFF -DENABLE_DRIVER_RANGERPOSEINTERPOLATOR=OFF \
-DENABLE_DRIVER_RANGERTODIO=OFF -DENABLE_DRIVER_RANGERTOLASER=OFF -DENABLE_DRIVER_RCORE_XBRIDGE=OFF -DENABLE_DRIVER_READLOG=OFF -DENABLE_DRIVER_REB=OFF \
-DENABLE_DRIVER_RELAY=OFF -DENABLE_DRIVER_RFLEX=OFF -DENABLE_DRIVER_ROBOTEQ=OFF -DENABLE_DRIVER_ROBOTINO=OFF -DENABLE_DRIVER_ROBOTRACKER=OFF \
-DENABLE_DRIVER_ROOMBA=OFF -DENABLE_DRIVER_RS4LEUZE=OFF -DENABLE_DRIVER_RT3XXX=OFF -DENABLE_DRIVER_SEGWAYRMP=OFF -DENABLE_DRIVER_SEGWAYRMP400=OFF \
-DENABLE_DRIVER_SERIALSTREAM=OFF -DENABLE_DRIVER_SERIO=OFF -DENABLE_DRIVER_SERVICE_ADV_MDNS=OFF -DENABLE_DRIVER_SHAPETRACKER=OFF -DENABLE_DRIVER_SICKLDMRS=OFF \
-DENABLE_DRIVER_SICKLMS200=OFF -DENABLE_DRIVER_SICKLMS400=OFF -DENABLE_DRIVER_SICKNAV200=OFF -DENABLE_DRIVER_SICKRFI341=OFF -DENABLE_DRIVER_SICKS3000=OFF \
-DENABLE_DRIVER_SIMPLESHAPE=OFF -DENABLE_DRIVER_SKYETEKM1=OFF -DENABLE_DRIVER_SND=OFF -DENABLE_DRIVER_SONARTORANGER=OFF -DENABLE_DRIVER_SONYEVID30=OFF \
-DENABLE_DRIVER_SPHERE=OFF -DENABLE_DRIVER_SPHEREPTZ=OFF -DENABLE_DRIVER_SPHINX2=OFF -DENABLE_DRIVER_SR3000=OFF -DENABLE_DRIVER_STALLTODIO=OFF -DENABLE_DRIVER_STATGRAB=OFF \
-DENABLE_DRIVER_STOC=OFF -DENABLE_DRIVER_SUPPRESSOR=OFF -DENABLE_DRIVER_SWISSRANGER=OFF -DENABLE_DRIVER_TCPSTREAM=OFF -DENABLE_DRIVER_UNICAPIMAGE=OFF \
-DENABLE_DRIVER_UPCBARCODE=OFF -DENABLE_DRIVER_VEC2MAP=OFF -DENABLE_DRIVER_VELCMD=OFF -DENABLE_DRIVER_VFH=OFF -DENABLE_DRIVER_VIDEOCANNY=OFF \
-DENABLE_DRIVER_VMAPFILE=OFF -DENABLE_DRIVER_WAVEFRONT=OFF -DENABLE_DRIVER_WBR914=OFF -DENABLE_DRIVER_WRITELOG=OFF -DENABLE_DRIVER_XSENSMT=OFF \
-DENABLE_DRIVER_YARPIMAGE=OFF -DENABLE_DRIVER_CREATE=OFF \
-DENABLE_DRIVER_CAMERA=OFF -DENABLE_DRIVER_DIO=OFF -DENABLE_DRIVER_FIDUCAL=OFF -DENABLE_DRIVER_BLOBFINDER=OFF -DENABLE_DRIVER_NXT=OFF -DENABLE_DRIVER_MBASEDRIVER=OFF .
}

do_compile() {

	oe_runmake

}
	
do_install() {

	oe_runmake install DESTDIR=${D}

}

FILES_${PN} = "/usr/bin/player \ 
	/usr/bin/playerinterfacegen \
	/usr/bin/playerxdrgen \ 
	/usr/include/* \ 
	/usr/lib/* \ 
	/usr/share/*"
