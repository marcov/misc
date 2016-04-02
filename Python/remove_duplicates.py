#!/usr/bin/python
#
# Remove duplicate networks from wpa_supplicant.conf for Android phones
# Marco Vedovati, 2015
#
'''
How to use:

adb shell su -c 'cp /data/misc/wifi/wpa_supplicant.conf /mnt/sdcard/wpa_supplicant.conf'
adb shell su -c 'cp /data/misc/wifi/wpa_supplicant.conf /mnt/sdcard/wpa_supplicant.conf'

python remove_duplicates.py wpa_supplicant.conf wpa_new.conf

adb push wpa_new.conf /mnt/sdcard/wpa_supplicant.conf
adb shell su -c 'cp /mnt/sdcard/wpa_supplicant.conf /data/misc/wifi/wpa_supplicant.conf'
adb shell su -c reboot
'''

import re
import sys

def usage():
    base_name = sys.argv[0].split("/")[-1]
    sys.exit("Usage:\n" +
             base_name + " [-a] <input file> <output file>\n"
             "\n"
             "Options:\n"
             "  -a: don't prompt for duplicate choice, always select first occurence\n")

if len(sys.argv) == 3:
    auto_mode = False
elif len(sys.argv) == 4 and sys.argv[1] == "-a":
    auto_mode = True
    sys.argv.pop(1)
else:
    usage()

wpa_cfg_file_name = sys.argv[1]
wpa_out_cfg_file_name = sys.argv[2]

try:
    f = open(wpa_cfg_file_name, "r")
except:
    sys.exit("Invalid wpa_supplicant.conf file!")

hdr_regex  = re.compile(r'^(.*?)network={', re.DOTALL)
nwk_regex  = re.compile(r"^network={.*?^}$", re.MULTILINE | re.DOTALL)
ssid_regex = re.compile(r"[\s\t]*ssid=\"(.*)\".*$", re.MULTILINE)

f_content = f.read()

hdr = (hdr_regex.findall(f_content)).pop()

nwk_matches = nwk_regex.findall(f_content)

networks_dictionary = {}

for network_block in nwk_matches:
    try:
        ssid = ssid_regex.findall(network_block).pop()
    except:
        print "ERROR for network block: ",
        print network_block
        continue


    if (not networks_dictionary.has_key(ssid)):
        net_to_add = network_block
    else:
        print "Found duplicate for ssid: " + ssid
        if auto_mode:
            choice = "1"
        else:
            print "Choose net to add: "
            print "1 : ",
            print networks_dictionary[ssid]
            print "2 : ",
            print network_block
            print ""
            print "Type your choice: ",
            choice=raw_input()

        if int(choice) == 1:
            print "Adding net 1"
            net_to_add = networks_dictionary[ssid]
        else:
            print "Adding net 2"
            net_to_add = network_block

    networks_dictionary[ssid] = net_to_add
    print net_to_add
    print "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"

try:
    outfile = open(wpa_out_cfg_file_name, "w")
except:
    sys.exit("Cannot create output wpa_supplicant.conf file!")

outfile.write(hdr)

for ssid in networks_dictionary:
    outfile.write(networks_dictionary[ssid])
    outfile.write("\n\n")

print "That's all. Remember after pushing file to 'chown system.wifi /data/misc/wifi/wpa_supplicant.conf' and change permissions back to 660."
