set( LARCV_LIB -L$ENV{LARCV_LIBDIR} -llarcv )
set( LARLITE_LIB -L$ENV{LARLITE_LIBDIR} -lLArLite_Analysis -lLArLite_LArUtil -lLArLite_DataFormat -lLArLite_Base )

set( UBOONECODE_LIB -L$ENV{UBOONECODE_LIB} )

include_directories ( $ENV{LARCV_COREDIR} $ENV{LARLITE_COREDIR} )

link_libraries( ${LIB_NAME} -L$ENV{BOOST_LIB} -lboost_system ${ROOTLIB} ${LARCV_LIB} ${LARLITE_LIB} ) # All execs need this.

#add_subdirectory(job)

art_make( EXCLUDE LArCVMetaMaker_service.cc
          BASENAME_ONLY
          LIBRARY_NAME     uboonecode_uboone_LArCVImageMaker
          LIB_LIBRARIES
                   larcorealg_Geometry
                   ubcore_Geometry
                   ubobj_MuCS
                   larcore_Geometry_Geometry_service
                   nug4_ParticleNavigation lardataobj_Simulation
                   lardataobj_RecoBase
                   nusimdata::SimulationBase
                   lardataalg_DetectorInfo
                   lardataobj_MCBase
                   larcoreobj_SummaryData
                   lardataobj_RawData
                   lardata_RecoObjects
                   ${UBOONECODE_LIB}
                   ${LARRECO_LIB}
                   ${LARDATA_LIB}
                   art::Framework_Core
                   art::Framework_Principal
                   art::Framework_Services_Registry
                   art_root_io::tfile_support ROOT::Core
                   art_root_io::TFileService_service
                   art::Persistency_Common
                   art::Persistency_Provenance
                   art::Utilities canvas::canvas
                   messagefacility::MF_MessageLogger
                   fhiclcpp::fhiclcpp fhiclcpp::types
                   cetlib::cetlib cetlib_except
                   ROOT::Geom
                   ROOT::XMLIO
                   ROOT::Gdml
          MODULE_LIBRARIES
                   uboonecode_uboone_LArCVImageMaker
                   ubevt::TFileMetadataMicroBooNE_service
                   ubevt::FileCatalogMetadataMicroBooNE_service
                   LArCVMetaMaker_service
                   ubobj_MuCS
                   larcorealg_Geometry
                   ubcore_Geometry
                   larcore_Geometry_Geometry_service
                   nug4_ParticleNavigation lardataobj_Simulation
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
                   ${UBOONECODE_LIB}
                   ${LARRECO_LIB}
                   ${LARDATA_LIB}
                   art::Framework_Core
                   art::Framework_Principal
                   art::Framework_Services_Registry
                   art_root_io::tfile_support ROOT::Core
                   art_root_io::TFileService_service
                   art::Persistency_Common
                   art::Persistency_Provenance
                   art::Utilities canvas::canvas
                   messagefacility::MF_MessageLogger
                   fhiclcpp::fhiclcpp fhiclcpp::types
                   cetlib_except::cetlib_except
                   ROOT::Geom
                   ROOT::XMLIO
                   ROOT::Gdml
                )

simple_plugin(LArCVMetaMaker "service"
              ubevt::TFileMetadataMicroBooNE_service
              ubevt::FileCatalogMetadataMicroBooNE_service
              ${ART_FRAMEWORK_SERVICES_SYSTEM_FILECATALOGMETADATA_SERVICE}
              art::Utilities canvas::canvas
              art::Framework_Principal
              messagefacility::MF_MessageLogger
              fhiclcpp::fhiclcpp fhiclcpp::types
              cetlib::cetlib cetlib_except
              BASENAME_ONLY
)

# install_headers()
install_fhicl()
install_source()
