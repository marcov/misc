#!/usr/bin/python

import re
import sys

if len(sys.argv) < 3:
    sys.exit("Please specify wpa_supplicant.conf input and output file paths!")

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


    if not networks_dictionary.has_key(ssid):
        net_to_add = network_block
    else:
        print "Found duplicate for ssid: " + ssid
        print "Choose net to add: "
        print "1 : "
        print networks_dictionary[ssid]
        print "2 : "
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
    print ssid
    print net_to_add
    print "##################"

#################################

try:
    outfile = open(wpa_out_cfg_file_name, "w")
except:
    sys.exit("Cannot create output wpa_supplicant.conf file!")

outfile.write(hdr)

for ssid in networks_dictionary:
    outfile.write(networks_dictionary[ssid])
    outfile.write("\n\n")
