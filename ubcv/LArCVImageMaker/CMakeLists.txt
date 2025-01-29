set( LARLITE_LIB -L$ENV{LARLITE_LIBDIR} -lLArLite_Analysis -lLArLite_LArUtil -lLArLite_DataFormat -lLArLite_Base )

#set( UBOONECODE_LIB -L$ENV{UBOONECODE_LIB} )

# use with ubdl
include_directories ( $ENV{LARLITE_COREDIR} ${LARCV_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/ubreco ${PYTHON_INCLUDE_DIRS} )
# use with ups larcv
#include_directories ( $ENV{LARCV_INCDIR} $ENV{LARLITE_COREDIR} ${CMAKE_SOURCE_DIR}/ubreco ${PYTHON_INCLUDE_DIRS} )

# use with ups larcv
#link_libraries( ${LIB_NAME} -L$ENV{BOOST_LIB} -lboost_system ${ROOTLIB} ${LARLITE_LIB} -L$ENV{LARCV_LIBDIR} -llarcv ) # All execs need this
# use with ubdl
set(CMAKE_CXX_STANDARD 17)
link_libraries( ${LIB_NAME} -L$ENV{BOOST_LIB} -lboost_system ${ROOTLIB} ${LARLITE_LIB} ) # All execs need this
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DHAS_TORCH")

add_definitions("-Wno-register")
#add_definitions("-DHASPYUTIL")

#add_subdirectory(job)

art_make( EXCLUDE LArCVMetaMaker_service.cc
	  BASENAME_ONLY
	  LIBRARY_NAME     uboonecode_uboone_LArCVImageMaker
	  LIB_LIBRARIES
	  	   larcorealg_Geometry
		   ubcore_Geometry
		   ubobj_MuCS
		   larcore_Geometry_Geometry_service
	   	   nutools_ParticleNavigation lardataobj_Simulation
		   lardataobj_RecoBase
		   nusimdata_SimulationBase
		   lardataobj_MCBase
		   larcoreobj_SummaryData
		   lardataobj_RawData
		   lardata_RecoObjects
		   #${UBOONECODE_LIB}
		   nusimdata_SimulationBase
		   ${LARRECO_LIB}
		   ${LARDATA_LIB}
		   ${ART_FRAMEWORK_CORE}
		   ${ART_FRAMEWORK_PRINCIPAL}
                   ${ART_FRAMEWORK_BASIC}
		   ${ART_FRAMEWORK_SERVICES_REGISTRY}
		   ${ART_FRAMEWORK_SERVICES_OPTIONAL}
		   ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
                   art_Persistency_Common
                   art_Persistency_Provenance
		   art_Utilities canvas
		   ${MF_MESSAGELOGGER}
		   
		   ${FHICLCPP}
		   cetlib cetlib_except
		   ${ROOT_GEOM}
		   ${ROOT_XMLIO}
		   ${ROOT_GDML}
		   ${ROOT_BASIC_LIB_LIST}

          MODULE_LIBRARIES 	  		   
		   uboonecode_uboone_LArCVImageMaker	   
	      	   TFileMetadataMicroBooNE_service
	      	   FileCatalogMetadataMicroBooNE_service
		   LArCVMetaMaker_service
		   ubobj_MuCS
		   larcorealg_Geometry
		   ubcore_Geometry
		   larcore_Geometry_Geometry_service
		   nutools_ParticleNavigation lardataobj_Simulation
                   larevt_CalibrationDBI_IOVData
                   larevt_CalibrationDBI_Providers
		   nusimdata_SimulationBase
		   lardata_Utilities
		   lardataobj_MCBase
		   larcoreobj_SummaryData
		   lardataobj_RawData
		   lardataobj_RecoBase
		   lardata_RecoObjects
                   larevt_Filters
                   larevt_CalibrationDBI_IOVData
                   larevt_CalibrationDBI_Providers
		   #${UBOONECODE_LIB}
		   nusimdata_SimulationBase
		   ${LARRECO_LIB}
		   ${LARDATA_LIB}
		   ${ART_FRAMEWORK_CORE}
		   ${ART_FRAMEWORK_PRINCIPAL}
                   ${ART_FRAMEWORK_BASIC}
		   ${ART_FRAMEWORK_SERVICES_REGISTRY}
		   ${ART_FRAMEWORK_SERVICES_OPTIONAL}
		   ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
                   art_Persistency_Common
                   art_Persistency_Provenance
		   art_Utilities canvas
		   ${MF_MESSAGELOGGER}
		   
		   ${FHICLCPP}
		   cetlib_except
		   ${ROOT_GEOM}
		   ${ROOT_XMLIO}
		   ${ROOT_GDML}
		   ${ROOT_BASIC_LIB_LIST}
		)

simple_plugin(LArCVMetaMaker "service"
	      TFileMetadataMicroBooNE_service
	      FileCatalogMetadataMicroBooNE_service
	      ${ART_FRAMEWORK_SERVICES_SYSTEM_FILECATALOGMETADATA_SERVICE}	      
	      art_Utilities canvas
              art_Framework_Principal
	      ${MF_MESSAGELOGGER}
	      
	      ${FHICLCPP}
	      cetlib cetlib_except
	      ${ROOT_BASIC_LIB_LIST}
              BASENAME_ONLY
)

target_link_libraries( uboonecode_uboone_LArCVImageMaker LArCVCoreBase LArCVCoreDataFormat LArCVCoreProcessor )
target_link_libraries( SSNetHitProducer_module LArCVCoreBase LArCVCoreDataFormat LArCVCoreProcessor )
target_link_libraries( LArSoftSuperaSriver_module LArCVCoreBase LArCVCoreDataFormat LArCVCoreProcessor )
target_link_libraries( RSEFilter_module LArCVCoreBase LArCVCoreDataFormat LArCVCoreProcessor )
target_link_libraries( VPlaneHandler_module LArCVCoreBase LArCVCoreDataFormat LArCVCoreProcessor )
target_link_libraries( LArCVMetaMaker_service LArCVCoreBase LArCVCoreDataFormat LArCVCoreProcessor )
target_link_libraries( LArPIDInterface_module ${TORCH_LIBRARIES} ${PYTHON_LIBRARIES} )

# install_headers()
install_fhicl()
install_source()
