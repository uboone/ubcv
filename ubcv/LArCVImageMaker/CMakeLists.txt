set( LARLITE_LIB -L$ENV{LARLITE_LIBDIR} -lLArLite_Analysis -lLArLite_LArUtil -lLArLite_DataFormat -lLArLite_Base )

# use with ubdl
#include_directories ( $ENV{LARLITE_COREDIR} ${LARCV_INCLUDE_DIR} )
# use with ups larcv
include_directories ( $ENV{LARCV_INCDIR} $ENV{LARLITE_COREDIR} )

# use with ups larcv
link_libraries( ${LIB_NAME} -L$ENV{BOOST_LIB} -lboost_system ${ROOTLIB} ${LARLITE_LIB} -L$ENV{LARCV_LIBDIR} -llarcv ) # All execs need this
# use with ubdl
#link_libraries( ${LIB_NAME} -L$ENV{BOOST_LIB} -lboost_system ${ROOTLIB} ${LARLITE_LIB}) # All execs need this

#add_subdirectory(job)

art_make( EXCLUDE LArCVMetaMaker_service.cc
          BASENAME_ONLY
          LIBRARY_NAME     uboonecode_uboone_LArCVImageMaker
          LIB_LIBRARIES
                   larcorealg_Geometry
                   ubcore_Geometry
                   ubobj_MuCS
                   larcore_Geometry_Geometry_service
                   nug4::ParticleNavigation lardataobj_Simulation
                   lardataobj_RecoBase
                   nusimdata::SimulationBase
                   lardataalg_DetectorInfo
                   lardataobj_MCBase
                   larcoreobj_SummaryData
                   lardataobj_RawData
                   lardata_RecoObjects
		   nusimdata::SimulationBase
		   art::Framework_Core
		   art::Framework_Principal
		   art::Framework_Services_Registry
		   art_root_io::tfile_support ROOT::Core
		   art_root_io::TFileService_service
                   art::Persistency_Common
                   art::Persistency_Provenance
                   art::Utilities canvas::canvas
		   messagefacility::MF_MessageLogger
		   fhiclcpp::fhiclcpp
                   cetlib::cetlib cetlib_except
		   ROOT::Geom
		   ROOT::XMLIO
		   ROOT::Gdml
		   ${ROOT_BASIC_LIB_LIST}
          MODULE_LIBRARIES
                   uboonecode_uboone_LArCVImageMaker
                   ubevt::TFileMetadataMicroBooNE_service
                   ubevt::FileCatalogMetadataMicroBooNE_service
                   LArCVMetaMaker_service
                   ubobj_MuCS
                   larcorealg_Geometry
                   ubcore_Geometry
                   larcore_Geometry_Geometry_service
                   nug4::ParticleNavigation lardataobj_Simulation
                   larevt_CalibrationDBI_IOVData
                   larevt_CalibrationDBI_Providers
                   nusimdata::SimulationBase
                   lardata_Utilities
                   lardataobj_MCBase
                   larcoreobj_SummaryData
                   lardataobj_RawData
                   lardataobj_RecoBase
                   lardata_RecoObjects
                   larevt_Filters
                   larevt_CalibrationDBI_IOVData
                   larevt_CalibrationDBI_Providers
		   nusimdata::SimulationBase
		   art::Framework_Core
		   art::Framework_Principal
		   art::Framework_Services_Registry
		   art_root_io::tfile_support ROOT::Core
		   art_root_io::TFileService_service
                   art::Persistency_Common
                   art::Persistency_Provenance
		   art::Utilities canvas
		   messagefacility::MF_MessageLogger
		   fhiclcpp::fhiclcpp
                   cetlib_except::cetlib_except
		   ROOT::Geom
		   ROOT::XMLIO
		   ROOT::Gdml
		   ${ROOT_BASIC_LIB_LIST}
		)

simple_plugin(LArCVMetaMaker "service"
              ubevt::TFileMetadataMicroBooNE_service
              ubevt::FileCatalogMetadataMicroBooNE_service
              art::Utilities canvas::canvas
              art::Framework_Principal
              messagefacility::MF_MessageLogger
              fhiclcpp::fhiclcpp fhiclcpp::types
              cetlib::cetlib cetlib_except
              BASENAME_ONLY
)

if (USE_DL_INTEGRATION)
  target_link_libraries( uboonecode_uboone_LArCVImageMaker LArCVCoreBase LArCVCoreDataFormat LArCVCoreProcessor )
  target_link_libraries( SSNetHitProducer_module LArCVCoreBase LArCVCoreDataFormat LArCVCoreProcessor )
  target_link_libraries( LArSoftSuperaSriver_module LArCVCoreBase LArCVCoreDataFormat LArCVCoreProcessor )
  target_link_libraries( RSEFilter_module LArCVCoreBase LArCVCoreDataFormat LArCVCoreProcessor )
  target_link_libraries( VPlaneHandler_module LArCVCoreBase LArCVCoreDataFormat LArCVCoreProcessor )
  target_link_libraries( LArCVMetaMaker_service LArCVCoreBase LArCVCoreDataFormat LArCVCoreProcessor )
endif()

# install_headers()
install_fhicl()
install_source()
