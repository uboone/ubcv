# fhicl files for ubdlintegration

A description of those stored in this folder follows:

| fcl  | description |
| ---- | ----------- |
| crt_merge_extra_v06_26_01_13_dlrename.fcl |  Driver fcl. Copy of `crt_merge_extra_v06_26_01_13.fcl` from `uboonecode`. Renames the file, so short enough to not cause an error. For MCC9 production.|
| dlreco2.fcl | Driver fcl. reruns wcopflash and CRT producers in order to incorporate light yield correction and wider CRT merge window. applies to data+MC. For MCC9 production. |
| mcc9_dlreco_driver_data.fcl | Driver fcl. Runs supera and litemaker modules to produce larcv, larlite files. Also produces art-file with SSNet output stored. For MCC9 data production. |
| mcc9_dlreco_driver_overlay_and_mc.fcl | Driver fcl. Runs supera, litemaker modules. reruns optical reco to incorporate gain/light yield calibrations. For MCC9 overlay+full MC production. |
| microboone_ubdl_module.fcl | Configuration fcl for DLInterface module. Setup to run Sparse SSNet. For MCC9 production. |
| supera_ssnetinterface.fcl  | Configuration fcl for Supera module called by DLInterface. Controls how the larcv images are made. |
| standard_dlreco_uboone_metadata.fcl | Driver fcl. Makes metadata json so `merged_dlreco` produced by dlreco script gets renamed with hash and metadata for use with SAM. |
| ubcrop.fcl | configuration fcl for UBSplitDetector, a larcv module. Controls how we split up a full image. Used by dense ssnet networks. |
| infill_split.fcl | configuration fcl for UBSplitDetector, a larcv module. Controls how we split up a full image. Used by infill network. |