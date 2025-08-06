This folder contains test scripts to help compare the outputs of various modules used in the official workflow before and after changes.

They were used in the work to reduce the resident memory footprint for truth LArCV image making.

* compare_larflow.py
* compare_instance.py
* compare_segment.py

We also have a driver fcl file and supera fcl files used to run the larflow, instance, and segment modules alone.
The same driver without modification can be used to run both the original and modified code.