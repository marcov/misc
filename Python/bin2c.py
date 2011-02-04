#!/usr/bin/python
#
# file: bin2c.py
# Author: cyberstorm
# License: GPLv2
#
# Usage: bin2c.py <list of files to convert>
# 
# You can customize the source code file name by modifying variable 
# OUTFILE.
#

import sys

OUTFILE = "output"
########################################################################
openedfiles = []
ofc = None
ofh = None

def gotoexit(retcode):
  for f in openedfiles:
    f.close()
  sys.exit(retcode)
  
########################################################################  

def filename_to_variable(filename):
  retvalue = filename
  for c in filename:
    if (ord(c) < ord("0")) or \
       (ord(c) > ord("9") and ord(c) < ord("A")) or \
       (ord(c) > ord("Z") and ord(c) < ord("a")) or \
       (ord(c) > ord("z")) :
       retvalue = retvalue.replace(c, "_")
  return retvalue

########################################################################  

def convertsinglefile(f, filename):
  f.seek(0)
  c = f.read(1)
  
  ofh.write("extern const unsigned char * %s; \n" % filename_to_variable(filename))
  ofc.write("const unsigned char * %s = \"" % filename_to_variable(filename))
  while c != "" :
    ofc.write("\\x%02X" % ord(c))
    c = f.read(1)

  ofc.write("\";\n")
  ofc.write("\n")
  

########################################################################  

try:
  ofc = open(OUTFILE+".c", "wb")
  openedfiles.append(ofc)
  ofh = open(OUTFILE+".h", "wb")
  openedfiles.append(ofh)
except IOError:
  print "Cannot open %s.c/.h" % OUTFILE
  gotoexit(-1)

####

if (len(sys.argv) > 1):
  filelist = sys.argv[1:]
  ofc.write("#include \"%s.h\"" % OUTFILE)
  ofc.write("\n\n")
  
  ofh.write("#ifndef __%s_H__" % OUTFILE.upper())
  ofh.write("\n")
  ofh.write("#define __%s_H__" % OUTFILE.upper())
  ofh.write("\n\n")
  
  for filename in filelist:
    try: 
      f = open(filename, "rb")
      openedfiles.append(f)
      print "Converting file %s" % filename
      convertsinglefile(f, filename)
      f.close()
      openedfiles.remove(f)
    except IOError:
      print "Cannot open %s" % filename
      gotoexit(-1) 
    
  ofh.write("\n#endif")  
  
gotoexit(0)
  

  
  
      
      
       
    

