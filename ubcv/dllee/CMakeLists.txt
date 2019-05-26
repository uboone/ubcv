set( LARLITE_LIB   -L$ENV{LARLITE_LIBDIR} -lLArLite_Analysis -lLArLite_LArUtil -lLArLite_DataFormat -lLArLite_Base -lSelectionTool_LEEPreCuts -lBasicTool_FhiclLite)
set( LARCV_LIB     -L$ENV{LARCV_LIBDIR}   -llarcv )
set( LARLITECV_LIB -L$ENV{LARLITECV_LIBDIR} -llarlitecv )
set( UBLIST_INC $ENV{UBLITE_DIR}/source )
set( LARLITE_INCLUDES $ENV{LARLITE_BASEDIR}/../ $ENV{LARLITE_COREDIR} $ENV{LARLITE_USERDEVDIR} $ENV{LARLITE_USERDEVDIR}/BasicTool $ENV{LARLITE_USERDEVDIR}/SelectionTool )
#set( UBOONECODE_LIB -L$ENV{UBOONECODE_LIB} )

# use with dllee
include_directories ( ${LARLITE_INCLUDES} $ENV{LARCV_INCDIR} $ENV{LARCV_INCDIR}/../../app/ann_1.1.2/include ${UBLIST_INC} $ENV{LARSIM_INC} $ENV{LARLITECV_INCDIR} $ENV{OPENCV_INCDIR} )

# use with ups larcv
link_libraries( ${LIB_NAME} -L$ENV{BOOST_LIB} -lboost_system ${ROOTLIB} ${LARLITE_LIB} ${LARCV_LIB} -L$ENV{OPENCV_LIBDIR} -lopencv_core ${LARLITECV_LIB} ) 

art_make( BASENAME_ONLY
	  LIBRARY_NAME     ubcv_ubdllee
	  LIB_LIBRARIES
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
		   ${UBOONECODE_LIB}
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
	  	   ubcv_LArCVImageMaker
		   ubcv_ubdldata
	      	   TFileMetadataMicroBooNE_service
	      	   FileCatalogMetadataMicroBooNE_service
		   LArCVMetaMaker_service
		   uboonecode_uboone_LiteMaker
		   ubobj_MuCS
		   ubobj_Trigger
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
		   ${UBOONECODE_LIB}
		   nusimdata_SimulationBase
		   ${UBOONEDAQ_DATATYPES_LIB}
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

		   #${LARCV_LIB} 
		   ${LARLITE_LIB} 
		   #${TORCH_LIBRARIES}
		   
		   ${FHICLCPP}
		   cetlib_except
		   ${ROOT_GEOM}
		   ${ROOT_XMLIO}
		   ${ROOT_GDML}
		   ${ROOT_BASIC_LIB_LIST}
		)

# install_headers()
install_fhicl()
install_source()
