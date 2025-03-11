
#set( LARLITE_LIB -L$ENV{LARLITE_LIBDIR} -llarlite_Analysis -llarlite_LArUtil -llarlite_DataFormat -llarlite_Base )
#set( UBLARCVAPP_LIB -L$ENV{UBLARCVAPP_LIBDIR} -lLArCVApp_MCTools -lLArCVApp_dbscan )
#include_directories ( $ENV{LARLITE_BASEDIR}/build/installed/include/larlite $ENV{LARLITE_BASEDIR}/build/installed/include ${LARCV_INCLUDE_DIR} $ENV{UBLARCVAPP_INCDIR} ${CMAKE_SOURCE_DIR}/ubreco ${TORCH_INCLUDE_DIRS} )
#set(CMAKE_CXX_STANDARD 17)
#link_libraries( ${LIB_NAME} -L$ENV{BOOST_LIB} -lboost_system ${ROOTLIB} ${LARLITE_LIB} ${UBLARCVAPP_LIB} ${TORCH_LIBRARIES} )
#set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DHAS_TORCH")
#add_definitions("-Wno-register")

cet_make_library(
  SOURCE
  FMWKInterface.cxx
  ImageMetaFromConfig.cxx
  ImageMetaMaker.cxx
  ImageMetaMakerBase.cxx
  ImageMetaMakerFactory.cxx
  Instance2Image.cxx
  LAr2Image.cxx
  LArCVSuperaDriver.cxx
  LArFlowLAr2Image.cxx
  MCParticleTree.cxx
  MCROIMaker.cxx
  ParamsChStatus.cxx
  ParamsImage2D.cxx
  ParamsPixel2D.cxx
  ParamsROI.cxx
  ParamsVoxel3D.cxx
  PulledPork3DSlicer.cxx
  SuperaBase.cxx
  SuperaChStatus.cxx
  SuperaExampleTemplate.cxx.example
  SuperaHit.cxx
  SuperaInstanceImage.cxx
  SuperaKeyPointCluster.cxx
  SuperaLArFlow.cxx
  SuperaMCPCluster.cxx
  SuperaMCROI.cxx
  SuperaMetaMaker.cxx
  SuperaOpDigit.cxx
  SuperaSimCh.cxx
  SuperaSimVoxel3D.cxx
  SuperaTypes.cxx
  SuperaWire.cxx
  WireRange3D.cxx
  LIBRARIES
  PUBLIC
  larevt::SpaceChargeService
  lardata::DetectorClocksService
  larcore::Geometry_Geometry_service
  lardataobj::RecoBase
  nusimdata::SimulationBase
  larcv::larcv
  larlite::LArUtil
)

cet_build_plugin(
  LArSoftSuperaSriver art::EDAnalyzer
  LIBRARIES
  PRIVATE
  ubcv::LArCVImageMaker
)

cet_build_plugin(
  RSEFilter art::EDFilter
  LIBRARIES
  PRIVATE
  larcore::Geometry_Geometry_service
  lardataobj::RecoBase
  nusimdata::SimulationBase
  larcv::larcv
)

cet_build_plugin(
  SSNetHitProducer art::EDProducer
  LIBRARIES
  PRIVATE
  larcore::Geometry_Geometry_service
  lardataobj::RecoBase
  nusimdata::SimulationBase
  larcv::larcv
)

cet_build_plugin(
  VPlaneHandler art::EDProducer
  LIBRARIES
  PRIVATE
  lardataobj::RecoBase
)

cet_build_plugin(
  LArCVMetaMaker art::service
  LIBRARIES
  PUBLIC
  ubevt::Utilities
  art::Framework_Services_System_FileCatalogMetadata_service
  art::Framework_Principal
  ROOT::Core
)

#target_link_libraries( uboonecode_uboone_LArCVImageMaker LArCVCoreBase LArCVCoreDataFormat LArCVCoreProcessor )
#target_link_libraries( SSNetHitProducer_module LArCVCoreBase LArCVCoreDataFormat LArCVCoreProcessor )
#target_link_libraries( LArSoftSuperaSriver_module LArCVCoreBase LArCVCoreDataFormat LArCVCoreProcessor )
#target_link_libraries( RSEFilter_module LArCVCoreBase LArCVCoreDataFormat LArCVCoreProcessor )
#target_link_libraries( VPlaneHandler_module LArCVCoreBase LArCVCoreDataFormat LArCVCoreProcessor )
#target_link_libraries( LArCVMetaMaker_service LArCVCoreBase LArCVCoreDataFormat LArCVCoreProcessor )

# install_headers()
install_fhicl()
install_source()
