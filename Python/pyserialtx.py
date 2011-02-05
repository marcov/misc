#
# Author:  marco vedovati
# License: GPL v2
#
# pyserialtx
#
# Very simple Python serial transmitter.
# Originally written for testing PIC with the HD44780-CGRAM write software.
#
# It takes as input two parameters:
# 1. an address for the CGRAM custom symbol.
# 2. a string containing hex values for the CGRAM symbol.
# "0x00,0x0,0x01,0x1,0xff,0xf"
#
# It outputs on the serial port MVBus Commmand 0 with the specified
# CGRAM code and data value.
#
#
import serial,sys,binascii

#Serial port parameters.
COM="COM3"
BAUDRATE=19200

#MVBus protocol
MVBUS_DELIM = "\x7e"

################################################################################
# Opens the serial port
#
#
def openport():
	global ser
	
	try:
		ser = serial.Serial(port=COM,
							baudrate=BAUDRATE)				
		#print ser
		return 0
	except serial.SerialException :
		return -1
	
	

################################################################################
# Takes as input a string holding bytes and prints theirs hexadecimal representation.
# Used to check that conversion went right.
def printable(hexbuffer):
	for i in hexbuffer :
		print "%02X" % ord(i),

################################################################################
#
# Takes as input a string containing bytes.
# String has the format:
# str = ['0x00', '0x0', '0x01', '0x1', '0xff', '0xf']
#
# Returns the string:
#
# hex = "\x00\x00\x01\x01\0xff\x0f"

def str2hex(bytesliststr):

	hexform = ""
	
	for hexstr in bytesliststr :
		
		#Universal conversion from:  # or ## or 0x## or 0x# to '##' (one byte in string format).
		#get2bytestr = lambda x: len(x) == 4 and x[2:] or "0" + x[2:]
		
		get2bytestr = lambda x: len(x)     == 1 and "0" + x     or \
								 len(x[2:]) == 0 and x[:2]       or \
		                         len(x[2:]) == 1 and "0" + x[2:] or \
		                         x[2:]
			
		hexform += binascii.unhexlify( get2bytestr(hexstr) )
	
	return hexform
		
################################################################################

def usage():
	print "\nUsage: " + sys.argv[0],
	print "CMD# [Payload]\n"
	print "\tCMD#:    Command number in decimal format."
	print "\tPayload: List of comma-separated bytes (format recognized 0x## or 0x# or ## or #)."

################################################################################
################################################################################

def main():
	
	#Check for number of parameters.
	if len(sys.argv) >= 2 :
	
		if openport() >= 0:
			#print "Serial port opened."
						
			command = int(sys.argv[1])	
			
			try :
				payload = str2hex( sys.argv[2].split(",") )
			except IndexError:
				payload = ""
				
			txbuffer  = MVBUS_DELIM + chr(command) + chr(len(payload)) + payload
			
			print "Sending: %s " % binascii.hexlify(txbuffer)
			ser.write(txbuffer)
			
			ser.timeout = 1
			print ser.readline()
			ser.timeout = None

		else :
			print "Cannot open serial port."
			
	else :
		usage()
	




################################################################################

if __name__ == '__main__':
    # Main call
    main()
