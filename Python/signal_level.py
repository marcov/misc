#
# Read and print Enigma2 signal level
# Author: Marco Vedovati
# License: GPLv2
#
import urllib2
import re
from time import sleep
import sys
import cStringIO

URL="http://192.168.10.1/"
#URL="http://10.0.0.100/"
USER="root"
PASS="mypassword"
REALM="enigma"

agcline = "var agc = "
snrline = "var snr = "
berline = "var ber = "


def findline(buffer,word):
	index = -1
	line=buffer.readline()
	while line !='' :
		index = line.find(word)
		if index != -1:
			break
		line=buffer.readline()
	buffer.seek(0)
	return [line,index] 
		


def retrievecontent():
	auth_handler = urllib2.HTTPBasicAuthHandler()
	auth_handler.add_password(realm=REALM,uri=URL, user=USER, passwd=PASS)
	opener = urllib2.build_opener(auth_handler)
	urllib2.install_opener(opener)
	f = urllib2.urlopen(URL+"data")
	buffer = cStringIO.StringIO(f.read())
	return buffer



def main():
	liveindex = 0
	
	lines = [snrline, agcline, berline]
	
	while 1 :
		cachedcontent = retrievecontent()
		
		content = cachedcontent
		
		for l in lines:
			[string,index] = findline(content,l)
			outstring =  string[index+len(l):].split(";")[0]
			print outstring,
		
		print ""
		sleep(1)
		
'''		outstring =  str(liveindex) + ". " + string[index+len(snrline):].split(";")[0]
		print outstring, 
		sys.stdout.flush()
		sleep(.5)
		print "\b" * (2+len(outstring)),
		liveindex+=1
'''	

		

if __name__ == "__main__":
    main()
