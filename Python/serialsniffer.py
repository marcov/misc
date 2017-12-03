#simple serial dump demo application
import serial
import sys
import binascii
import signal
import time

version = { 
            "major"     : "0",
            "minor"     : "1",
            "revision"  : "a" 
          }


LINE_WIDTH  = 16
SERIAL_PORT = "/dev/ttyUSB0"

#
#signal handler for ctrl+c
#
def signal_handler(signal, frame):
  print 'You want to exit!'
  sys.stdout.flush()
  ser.close()
  sys.exit(0)
  
################################################################################

def intro():
  print "\nVery simple serial line sniffer version %s.%s%s by m.v.\n\n" % \
        (version["major"], version["minor"],  version["revision"])

  
################################################################################

def openport(portname):
  try:
    ser = serial.Serial(port=portname,
                        baudrate=1200, 
                        parity=serial.PARITY_ODD,
                        timeout = None)
    #print "Serial port data: %s" % ser
    
  except serial.SerialException:
    print "Cannot open serial port %s." % portname
    return 0
    
  return ser

################################################################################

def print_ascii(str) :
  for i in str :
    if ord(i) >= 0x20:
      sys.stdout.write(i)
    else:
      sys.stdout.write(".")

################################################################################

def main() :
  global ser
  signal.signal(signal.SIGINT, signal_handler)

  intro()
  
  ser = openport(SERIAL_PORT)
  if ser == 0 :
    altport = raw_input("Enter an alternative port name: ")
    ser = openport(altport)
    if ser == 0:
      sys.exit(-1)

  ''' 
  # RTS = 0 HART modem as receiving 
  # DTR = 1 give supply to HART modem
   ser.setRTS(0)
   ser.setDTR(1)
  '''

  print "Sniffing on %s.\n " % ser.port
  
  readline = ""
  
  while 1 :
    readb = ser.read(1)      #This is blocking
    readline += readb

    print "%02X" % ord(readb),
    
    if len(readline) >= LINE_WIDTH:
      print " | ",
      print_ascii(readline)
      print ""
      readline = ""
      
    sys.stdout.flush()
    
  
################################################################################

if __name__ == "__main__":
  main()

