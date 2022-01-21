#!/bin/bash

# FIT VUTBR ILI Project 2019
# xdvora2t@stud.fit.vutbr.cz

RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
GRAY='\033[0;37m'
NC='\033[0m' # No Color

printOutput(){

	if [ $? -eq 0 ]; then
   	echo -e "${GREEN}DONE${NC}"
	else
   	echo -e "${RED}FAIL${NC} // Already exists"
	fi;
}

printLine(){

	echo -e "${GRAY}--------------------------------------------------${NC}"
}

# 1: Create 4 loop devices
for devi in 0 1 2 3; do
	if [ -b /dev/loop$devi ]; then
			echo "loop$devi already created..."
		else
			echo -e "Creating physical volume /dev/loop$devi...\c"
			dd if=/dev/zero of=disk$devi bs=200M count=1 2>/dev/null
			losetup loop$devi ./disk$devi 2>/dev/null
			pvcreate /dev/loop$devi 1>/dev/null
			eval 'printOutput'
	fi;
done;
eval 'printLine'

# 2: Create software RAIDs
echo -e "Creating Software RAID0...\c"
mdadm --create /dev/md0 --level=0 --raid-devices=2 /dev/loop2 /dev/loop3 2>/dev/null
eval 'printOutput'

echo -e "Creating Software RAID1...\c"
mdadm --create /dev/md1 --level=1 --raid-devices=2 /dev/loop0 /dev/loop1 2>/dev/null <<EOF
yes
EOF
eval 'printOutput'
eval 'printLine'

# 3: Create volume groud on top of them
pvcreate /dev/md1 1>/dev/null
echo -e "Creating volume group on top of both RAIDs...\c"
vgcreate FIT_vg /dev/md1 1>/dev/null
eval 'printOutput'
eval 'printLine'

# add second RAID
echo -e "Adding second RAID...\c"
pvcreate /dev/md0 1>/dev/null
vgextend FIT_vg /dev/md0 1>/dev/null
eval 'printOutput'
eval 'printLine'

# 4: Create 2 logical volumes 100M each
echo -e "Creating logical volume FIT_lv1...\c"
lvcreate FIT_vg -n FIT_lv1 -L100M 1>/dev/null
eval 'printOutput'
echo -e "Creating logical volume FIT_lv2...\c"
lvcreate FIT_vg -n FIT_lv2 -L100M 1>/dev/null
eval 'printOutput'
eval 'printLine'

# 5: Create ext4 on FIT_lv1
echo -e "Creating ext4 on FIT_lv1...\c"
mkfs.ext4 /dev/FIT_vg/FIT_lv1 > /dev/null 2>&1
eval 'printOutput'
eval 'printLine'

# 6: Create XFS on FIT_lv2
echo -e "Creating XFS on FIT_lv2...\c"
mkfs.xfs /dev/FIT_vg/FIT_lv2 > /dev/null 2>&1
eval 'printOutput'
eval 'printLine'

# 7: Mount
echo -e "Creating directory /mnt/test1...\c"
mkdir /mnt/test1 > /dev/null 2>&1
eval 'printOutput'
echo -e "Creating directory /mnt/test2...\c"
mkdir /mnt/test2 > /dev/null 2>&1
eval 'printOutput'

echo -e "Mounting test1 to FIT_LV1...\c"
mount /dev/FIT_vg/FIT_lv1 /mnt/test1 > /dev/null 2>&1
eval 'printOutput'
echo -e "Mounting test2 to FIT_LV2...\c"
mount /dev/FIT_vg/FIT_lv2 /mnt/test2 > /dev/null 2>&1
eval 'printOutput'
eval 'printLine'

# 8: Resize FIT_lv1 filesystem
echo -e "Changing size of filesystem on FIT_LV1...\c"
#umount /dev/FIT_vg/FIT_lv1
lvextend -r -L +300M /dev/FIT_vg/FIT_lv1 > /dev/null 2>&1
#lvresize /dev/FIT_vg/FIT_lv1 +L300M
#resize2fs /dev/FIT_vg/FIT_lv1
eval 'printOutput'
eval 'printLine'

# 9: Create big_file
echo -e "Creating big_file (300M) on /mnt/test1/...\c"
dd if=/dev/urandom of=/mnt/test1/big_file bs=300M count=1 > /dev/null 2>&1
eval 'printOutput'
CHECKSUM=`sha512sum /mnt/test1/big_file | cut -f 1 -d " "`
echo "Checksum of big_file is"
echo "${CHECKSUM}"
eval 'printLine'

# 10: Create 5th loop and switch with another
echo -e "Creating physical volume /dev/loop42...\c"
dd if=/dev/zero of=disk42 bs=200M count=1 2>/dev/null
losetup loop42 ./disk42 2>/dev/null
#pvcreate /dev/loop42 1>/dev/null
eval 'printOutput'
eval 'printLine'

echo -e "Simulation:${RED}Device loop0 is failing!!!${NC}"
echo -e "Creating physical volume /dev/loop42...\c"
mdadm --manage /dev/md1 --fail /dev/loop0 > /dev/null 2>&1
eval 'printOutput'
echo -e "Removing physical volume /dev/loop0...\c"
mdadm --manage /dev/md1 --remove /dev/loop0 > /dev/null 2>&1
eval 'printOutput'
echo -e "Adding physical volume /dev/loop42 to RAID1...\c"
mdadm --manage /dev/md1 --add /dev/loop42 > /dev/null 2>&1
eval 'printOutput'

ENDOUT=`sed -n '2p' /proc/mdstat`
echo -e "${BLUE}${ENDOUT}${NC}"
# the end
