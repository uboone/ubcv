# UB DL Integration

The DL reconstruction chain is run in one job, but proceeds within that job, in two stages.

The first stage is the execution of the art producer module, `DLIntegration_module`,
located in the `ubcv` respository.  
This module is responsible for 
  * making `larcv` and `larlite` files used by the DL reconstruction chain, and
  * running convolution neural networks and storing the output into both `larcv` format and into the art file.

The second stage of the job is to execute the DL reconstruction algorithms.
The algorithms use the `larcv` and `larlite` files produced in the first stage.
The results of these algorithms are several different root files, which are merged together to ease 
file book-keeping. 

Note: the DL algorithms are not run in the larsoft environment.
Instead, the algorithms need to be executed using an `endscript` for the job.

Therefore, configuring a job involves specifying a larsoft fcl file for the first stage 
and then the bash script for running the second stage.

# Components needed for a job

Each job needs the following pieces:

* a source script
* a fcl chain which is different for data and MC
* an end-script that runs the DL jobs
* special `jobsub` commands to pick the right kind of machine on the grid.


## source script

The jobs require a source script. The one to use is `init_dlreco.sh`.
If setting up a project.py xml, this should go into the `<initsource>init_dlreco.sh</initsource>` tag.

If you are having troubles, an alternative is `init_dlreco_dev.sh`. 
This one copies the sparsessnet weights to the local machine and uses that there.
It also dumps more info about the CPU used.

## fcl Configurations

We specify the driver fcl files and DL reco. script for each type of data.
Contact Taritree (twongj01@tufts.edu or better on slack), if a data type is missing.

| Data Type    | input stage required     | larsoft driver fcl file                   | DL reco. script                           |
| ------------ | ------------------------ | ----------------------------------------- | ----------------------------------------- |
| Overlay MC   | reco1 (needs simchannel) |  mcc9_dlreco_driver_overlay_and_mc.fcl    |  rundlreco_ssnetvertexonly_mcoverlay.sh   |
| Full MC      | reco1 (needs simchannel) |  mcc9_dlreco_driver_overlay_and_mc.fcl    |  rundlreco_ssnetvertexonly_fullmc.sh      |
| BNB data     | reco1 or reco2           |  mcc9_dlreco_driver_data.fcl              |  rundlreco_ssnetvertexonly_bnb.sh         |
| EXT-BNB data | reco1 or reco2           |  mcc9_dlreco_driver_data.fcl              |  rundlreco_ssnetvertexonly_ext.sh         |
| EXT-unbiased | reco1 or reco2           |  mcc9_dlreco_driver_data.fcl              |  rundlreco_ssnetvertexonly_ext.sh         |

These files are found in `ubcv/ubcv/ubdlintegration` folder.

Note that `dl_driver_dev_data.fcl` and `dl_driver_dev_overlay.fcl` are in this folder for development purposes.

## fcl chain

For data jobs `EXTBNB` and `BNB` data, one can run on Reco 2 files. 
The fcl chain is:

```
  <initsource>init_dlreco.sh</initsource>
  <fcl>mcc9_dlreco_driver_data.fcl</fcl>
  <fcl>standard_dlreco_uboone_metadata.fcl</fcl>
  <endscript>rundlreco_ssnetvertexonly_bnb.sh</endscript>
```

For mc jobs, we want access to simchannels so we can have truth images in the same file.
However, this means we have to run some `reco2` processes.

The fcl chain for `OVERLAY` and `PURE-MC` samples are:

```
  <initsource>init_dlreco.sh</initsource>
  <fcl>run_eventweight_microboone_justSplines.fcl</fcl>
  <fcl>wirecell_detsim_optical_overlay_uboone.fcl</fcl>
  <fcl>standard_overlay_optical_uboone.fcl</fcl>
  <fcl>reco_uboone_mcc9_8_driver_overlay_optical.fcl</fcl>
  <fcl>dlreco2.fcl</fcl>
  <fcl>mcc9_dlreco_driver_overlay_and_mc.fcl</fcl>
  <fcl>standard_dlreco_uboone_metadata.fcl</fcl>
  <endscript>rundlreco_ssnetvertexonly_mcoverlay.sh</endscript>
```

One needs to select a driver fcl file and endscript using the table above.  
One also needs to include `standard_dlreco_uboone_metadata.fcl`. 
Note, this (and `dlreco2.fcl`) lives in `ubcv/ubcv/ubdlintegration/`.

## Special Jobsub commands

These commands ensure that `cvmfs` for microboone can be seen. 
Also ensures that the CPUs have access to AVX instruction sets, which `pytorch` needs.

```
    <jobsub>--expected-lifetime=16h -e IFDH_CP_UNLINK_ON_ERROR=1 --append_condor_requirements='(TARGET.HAS_CVMFS_uboone_opensciencegrid_org==true)&amp;&amp;(TARGET.HAS_CVMFS_uboone_osgstorage_org==true)&amp;&amp;(TARGET.has_avx==true)'</jobsub>
    <jobsub_start>--expected-lifetime=short --append_condor_requirements='(TARGET.HAS_CVMFS_uboone_opensciencegrid_org==true)'</jobsub_start>
```

## An Example XML

You can find an example xml file in `example_project.xml`.