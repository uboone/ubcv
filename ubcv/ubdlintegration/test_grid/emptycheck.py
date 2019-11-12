import sys,os
import larcv
from larcv import larcv

io = larcv.IOManager(larcv.IOManager.kREAD)
io.add_in_file("%s"%sys.argv[1])
io.initialize()

nentries = io.get_n_entries()
print("Number of Entries: ", nentries)

if nentries==0: 
     sys.exit(1)
else:
     sys.exit(0)
