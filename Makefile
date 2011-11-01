#OVEROTOP = /home/michael/embedded/emb4/overo-oe-guide/build
OVEROTOP = /home/michael/embedded/emb4/overo-oe-mich/build
device := /dev/sdb
home := /home/michael/Temp/SDU-emb4-group1
#img := $(OVEROTOP)/tmp_clean_myimage/deploy/glibc/images/overo
#img := $(OVEROTOP)/tmp_working_console_image/deploy/glibc/images/overo
img := $(OVEROTOP)/tmp/deploy/glibc/images/overo
mount_place = /mnt/emb4
#kind_of_image := minimalist-image-overo.tar.bz2
kind_of_image := omap3-console-image-overo.tar.bz2
#kind_of_image := minimalist-image_with_wifi-overo.tar.bz2

# umount:
# 	sudo umount $(device)1
# 	sudo umount $(device)2

format:
	read -p "Ready to format $(device)"
	sudo mkfs.vfat -F 32 $(device)1 -n FAT
	sudo mkfs.ext3 $(device)2

build:
	(source $(home)/env/kernel-dev.txt; cd $(home)/kernel_development/leddev; make; make install)
	(source $(home)/env/kernel-dev.txt; cd $(home)/kernel_development/nxtts; make; make install)
	(source $(home)/env/kernel-dev.txt; cd $(home)/kernel_development/adc; make; make install)

archive: build
	(cd $(home); rm export.tgz; cd export; tar --exclude '*.svn*' --exclude '.gitignore' -zcvf ../export.tgz .)

deploy: archive
	echo "Deploying from $(OVEROTOP) to $(device)"
	sudo mkdir -p $(mount_place)
	sudo mount $(device)1 $(mount_place)
	sudo cp $(img)/MLO-overo $(mount_place)/MLO
	sudo cp $(img)/u-boot-overo.bin $(mount_place)/u-boot.bin
	sudo cp $(img)/uImage-overo.bin $(mount_place)/uImage
	sudo umount $(device)1
	sudo mount $(device)2 $(mount_place)
	(cd $(mount_place); sudo tar xvaf $(OVEROTOP)/tmp/deploy/glibc/images/overo/$(kind_of_image))
	(cd $(mount_place); sudo tar -zxvf $(home)/export.tgz)
	sudo umount $(device)2

xport: archive
	sudo mount $(device)2 $(mount_place)
	(cd $(mount_place); sudo rm -rf export; sudo tar -zxvf $(home)/export.tgz)
	sudo umount $(device)2

default:
	echo "Valid targets are archive, build, deploy, format, umount, xport"
