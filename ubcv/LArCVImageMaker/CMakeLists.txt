
#needed to compile against ubdl larcv:
add_compile_options(-Wno-deprecated-declarations)
add_compile_options(-Wno-maybe-uninitialized)

cet_make_library(
  SOURCE
  FMWKInterface.cxx
  ImageMetaFromConfig.cxx
  ImageMetaMaker.cxx
  ImageMetaMakerBase.cxx
  ImageMetaMakerFactory.cxx
  Instance2Image.cxx
  LAr2Image.cxx
  LArCVBackTracker.cxx
  LArCVSuperaDriver.cxx
  LArCVWCThrumuImage.cxx
  LArFlowLAr2Image.cxx
  LArPIDInterface.cxx
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
  SuperaWCThrumuImage.cxx
  SuperaWire.cxx
  WireRange3D.cxx
  LIBRARIES
  PUBLIC
  larevt::SpaceChargeService
  lardata::DetectorClocksService
  larcore::Geometry_Geometry_service
  lardataobj::RecoBase
  nusimdata::SimulationBase
  larcv::Base
  larcv::DataFormat
  larcv::Processor
  #larcv::larcv
  larlite::DataFormat
  larlite::LArUtil
  ublarcvapp::MCTools
  ublarcvapp::dbscan
  TorchScatter::TorchScatter
)

cet_build_plugin(
  LArSoftSuperaSriver art::EDAnalyzer
  LIBRARIES
  PRIVATE
  ubcv::LArCVImageMaker
)

cet_build_plugin(
  LArPIDInterfaceTest art::EDAnalyzer
  LIBRARIES
  PRIVATE
  ubcv::LArCVImageMaker
  ubreco::WcpPortedReco_ProducePort_dict
  art_root_io::TFileService_service
  ROOT::Tree
)

cet_build_plugin(
  RSEFilter art::EDFilter
  LIBRARIES
  PRIVATE
  larcore::Geometry_Geometry_service
  lardataobj::RecoBase
  nusimdata::SimulationBase
  larcv::Base
  larcv::DataFormat
  larcv::Processor
  #larcv::larcv
)

cet_build_plugin(
  SSNetHitProducer art::EDProducer
  LIBRARIES
  PRIVATE
  larcore::Geometry_Geometry_service
  lardataobj::RecoBase
  nusimdata::SimulationBase
  larcv::Base
  larcv::DataFormat
  larcv::Processor
  #larcv::larcv
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

# install_headers()
install_fhicl()
install_source()
