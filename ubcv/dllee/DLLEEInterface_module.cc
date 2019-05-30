////////////////////////////////////////////////////////////////////////
// Class:       DLLEEInterface
// Plugin Type: producer (art v2_05_01)
// File:        DLLEEInterface_module.cc
//
// Generated at Wed Aug 29 05:49:09 2018 by Taritree,,, using cetskelgen
// from cetlib version v1_21_00.
////////////////////////////////////////////////////////////////////////

//#include "Python.h"

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <memory>

// opencv
#include "opencv/cv.h"
#include "opencv2/opencv.hpp"

// larsoft
#include "lardataobj/RecoBase/OpFlash.h"
#include "lardataobj/RawData/TriggerData.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusService.h"
#include "larevt/CalibrationDBI/Interface/ChannelStatusProvider.h"

// ublite
#include "DataFormat/trigger.h"
#include "ublite/LiteMaker/ScannerAlgo.h"

// ubcv
#include "ubcv/LArCVImageMaker/LArCVSuperaDriver.h"
#include "ubcv/LArCVImageMaker/ImageMetaMaker.h"
#include "ubcv/LArCVImageMaker/SuperaMetaMaker.h"
#include "ubcv/LArCVImageMaker/SuperaWire.h"
#include "ubcv/LArCVImageMaker/LAr2Image.h"
#include "ubcv/ubdldata/pixeldata.h"

// larcv
#include "Base/larcv_base.h"
#include "Base/PSet.h"
#include "Base/LArCVBaseUtilFunc.h"
#include "DataFormat/EventImage2D.h"
#include "DataFormat/EventPixel2D.h"
#include "ubdllee/FixedCROIFromFlashAlgo.h"
// #include "larcv/core/DataFormat/Image2D.h"
// #include "larcv/core/DataFormat/ImageMeta.h"
// #include "larcv/core/DataFormat/ROI.h"
// #include "larcv/core/json/json_utils.h"

// larlite
#include "DataFormat/opflash.h"
#include "SelectionTool/LEEPreCuts/LEEPreCut.h"

// larlitecv
#include "TaggerCROI/TaggerCROIAlgo.h"
#include "TaggerCROI/TaggerCROIAlgoConfig.h"
#include "TaggerCROI/TaggerCROITypes.h"
#include "GapChs/EmptyChannelAlgo.h"

// ROOT
//#include "TMessage.h"

// zmq-c++
//#include "zmq.hpp"

class DLLEEInterface;

class DLLEEInterface : public art::EDProducer, larcv::larcv_base {

public:

  explicit DLLEEInterface(fhicl::ParameterSet const & p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  DLLEEInterface(DLLEEInterface const &) = delete;
  DLLEEInterface(DLLEEInterface &&) = delete;
  DLLEEInterface & operator = (DLLEEInterface const &) = delete;
  DLLEEInterface & operator = (DLLEEInterface &&) = delete;

  // Required functions.
  void produce(art::Event & e) override;
  void beginJob() override;
  void endJob() override;
  
private:

  // Declare member data here.
  int _verbosity;                    //< verbosity of module
  
  // Track Entries processed
  int _entries_processed;

  // larlite scanneralgo: to make larlite products used in DL algos
  larlite::ScannerAlgo _litemaker;   //< class used to make larlite products
  std::vector< std::string > _litemaker_opflash_producer_v;
  std::string _litemaker_ophitbeam_producer;
  std::string _litemaker_trigger;
  void runLiteMaker( art::Event const& e, 
		     std::vector< larlite::event_opflash >& opflash_vv,
		     larlite::event_ophit& ophit_v,
		     larlite::trigger& trigger );

  // supera/image formation: to make larcv products used in DL algos
  larcv::LArCVSuperaDriver _supera;  //< class used to make larcv products
  std::string _wire_producer_name;   //< name of wire product in art file
  std::string _supera_config;        //< supera config file
  int runSupera( art::Event& e, std::vector<larcv::Image2D>& wholeview_imgs );

  // ssnet image
  std::string _ssnet_producer;
  bool getSSNetFromArt( art::Event const& e, 
			const std::string ssnet_producer, 
			const std::vector<larcv::Image2D>& wholeview_v,
			std::vector< larcv::Image2D >& shower_v,
			std::vector< larcv::Image2D >& track_v,
			std::vector< larcv::ROI >& roi_v );

  // bad channel
  std::string _chstatus_rawdigit_producer;
  std::vector< larcv::Image2D > makeChStatusImage( const art::Event& e, 
						   const std::vector<larcv::Image2D>& img_v );
  

  void saveLArCVProducts( std::vector<larcv::Image2D>& wholeview_imgs,
			  std::vector<larcv::Image2D>& shower_v,
			  std::vector<larcv::Image2D>& track_v,
			  std::vector<larcv::Image2D>& badch_v );

  // =================
  //  Algorithms
  // =================

  // PMT Precuts
  // -------------
  larlite::LEEPreCut m_precutalgo;

  // ROI
  // ----
  larcv::ubdllee::FixedCROIFromFlashAlgo _croifromflashalgo;
  int runCROIfromFlash( const std::vector<larcv::Image2D>& wholeview_v, 
			const larlite::event_opflash& beam_flash_v,
			std::vector<larcv::ROI>& roi_v );


  // Tagger
  // -------------
  int _tagger_endpoint_limit;
  larlitecv::TaggerCROIAlgoConfig  m_taggeralgo_cfg;
  larlitecv::InputPayload          m_tagger_input;
  larlitecv::EmptyChannelAlgo      m_emptych_algo;
  bool runTagger( const art::Event& e,
		  std::vector<larcv::Image2D>& wholeview_v,
		  std::vector<larcv::Image2D>& badch_v, 
		  std::vector<larlite::event_opflash>& opflash_vv,
		  larlite::trigger& trigger, 
		  larcv::EventPixel2D& ev_thrumu_pixels,
		  larcv::EventPixel2D& ev_stopmu_pixels);
  
  void makeTaggerPixelClusters( const larlitecv::InputPayload& input,
				const larlitecv::TaggerCROIAlgoConfig& config,
				larlitecv::ThruMuPayload& thrumuout,
				larlitecv::StopMuPayload& stopmuout,
				larcv::EventPixel2D& ev_thrumu_pixels,
				larcv::EventPixel2D& ev_stopmu_pixels);

  // Vertexer
  // ---------
  std::string _vertexer_cfg;         //< vertexer config file
  larcv::ProcessDriver* m_vertexer;  //< driver for tracker
  std::vector<float> _threshold_v;   //< thresholds for making shower and track image
  void prepareVertexerData( const std::vector<larcv::Image2D>& wholeview_v, 
			    const std::vector<larcv::Image2D>& shower_v,
			    const std::vector<larcv::Image2D>& track_v,
			    std::vector<larcv::Image2D>& shower_masked_v,
			    std::vector<larcv::Image2D>& track_masked_v );

  // Tracker
  // --------
  std::string _tracker_cfg;         //< tracker config file
  larcv::ProcessDriver* m_tracker;  //< driver for tracker
  
  
};

/**
 * constructor: configuration module
 *
 */
DLLEEInterface::DLLEEInterface(fhicl::ParameterSet const & p)
  : larcv::larcv_base("DLLEEInterface_module")
    // Initialize member data here.
{
  
  // file path utility
  cet::search_path finder("FHICL_FILE_PATH");

  // verbosity
  // ----------
  _verbosity = p.get<int>("Verbosity",2);
  set_verbosity( (::larcv::msg::Level_t) _verbosity );
  _supera.set_verbosity( (::larcv::msg::Level_t) _verbosity );

  // entries counter
  // ----------------
  _entries_processed = 0;

  // -----------------
  // litemaker config
  // -----------------
  _litemaker_opflash_producer_v = p.get<std::vector<std::string> >("OpFlashBeamProducers");
  for ( auto const& opflash_producer : _litemaker_opflash_producer_v )
    _litemaker.Register( opflash_producer, larlite::data::kOpFlash );
  _litemaker_ophitbeam_producer = p.get<std::string>("OpHitBeamProducer");
  _litemaker_trigger            = p.get<std::string>("TriggerProducer");

  // ---------------
  // supera config
  // ---------------

  // product to get recob::wire data from
  _wire_producer_name = p.get<std::string>("WireProducerName");

  // name of supera configuration file
  _supera_config      = p.get<std::string>("SuperaConfigFile");

  // ---------------
  //  ssnet
  // ---------------
  _ssnet_producer = p.get<std::string>("SSNetProducerName");

  // ---------------
  //  LEEPreCuts
  // ---------------
  std::string precut_ftype = p.get<std::string>("PrecutConfig");
  if ( precut_ftype=="BNB" || precut_ftype=="MC" )
    m_precutalgo.setDefaults( larlite::LEEPreCut::kBNB );
  else if ( precut_ftype=="EXTBNB" )
    m_precutalgo.setDefaults( larlite::LEEPreCut::kEXTBNB );
  else if ( precut_ftype=="OVERLAY" )
    m_precutalgo.setDefaults( larlite::LEEPreCut::kOVERLAY );
  else
    throw cet::exception("DLLEEInterface") << "Unrecognized precut config. Choices [BNB,MC,EXTBNB,OVERLAY]" << std::endl;

  // ---------------
  //  Tagger
  // ---------------
  std::string      tagger_cfg;
  if( !finder.find_file( p.get<std::string>("TaggerConfigFile"), tagger_cfg) )
    throw cet::exception("DLLEEInterface") << "Unable to find tagger cfg in "  << finder.to_string() << "\n";
  LARCV_INFO() << "LOADING tagger config: " << tagger_cfg << std::endl;
  //larlitecv::TaggerCROIAlgoConfig taggeralgocfg = larlitecv::TaggerCROIAlgoConfig::makeConfigFromFile( tagger_cfg );
  m_taggeralgo_cfg = larlitecv::TaggerCROIAlgoConfig::makeConfigFromFile( tagger_cfg );
  LARCV_INFO() << "LOADING TaggerCROIAlgo instance" << std::endl;
  _tagger_endpoint_limit = p.get<int>("TaggerEndpointLimit");

  // --------------
  //  Bad Channels
  // --------------  
  _chstatus_rawdigit_producer = p.get<std::string>("ChStatusRawDigitProducer" );

  // ---------------
  //  Vertexer
  // ---------------
  _vertexer_cfg = p.get<std::string>("VertexerConfigFile");
  m_vertexer = new larcv::ProcessDriver( "VertexDriver" );
  m_vertexer->set_verbosity( (::larcv::msg::Level_t)_verbosity );
  _threshold_v = p.get< std::vector<float> >( "SSNetADCthreshold" );

  // ---------------
  //  Tracker
  // ---------------
  _tracker_cfg = p.get<std::string>("TrackerConfigFile");
  m_tracker = new larcv::ProcessDriver( "Tracker3DDriver" );
  m_tracker->set_verbosity( (::larcv::msg::Level_t)_verbosity );


}

/** 
 * produce DL network products
 *
 */
void DLLEEInterface::produce(art::Event & e)
{

  // get larlite products with LiteMaker
  //--------------------------------------
  LARCV_INFO() << "Run the LiteMaker" << std::endl;
  std::vector< larlite::event_opflash > opflash_vv;
  larlite::event_ophit ophit_beam_v;
  larlite::trigger  trigger;
  runLiteMaker( e, opflash_vv, ophit_beam_v, trigger );

  // get larcv products
  // ---------------------
  LARCV_INFO() << "Run SUPERA" << std::endl;
  std::vector<larcv::Image2D> wholeview_v;
  int nwholeview_imgs = runSupera( e, wholeview_v );
  LARCV_INFO() << "number of wholeview images: " << nwholeview_imgs << std::endl;

  // recover ssnet info
  // --------------------
  LARCV_INFO() << "Get SSNet From Art Event" << std::endl;
  std::vector< larcv::Image2D > shower_v;
  std::vector< larcv::Image2D > track_v;
  std::vector< larcv::ROI >     ssnet_roi_v;
  bool has_ssnet = getSSNetFromArt( e, _ssnet_producer, wholeview_v,
				    shower_v, track_v, ssnet_roi_v );
  LARCV_INFO() << "SSnet loaded: " << has_ssnet << std::endl;
  if ( has_ssnet )  {
    LARCV_INFO() << "  number of trackimgs=" << track_v.size() 
		 << " showerimgs=" << shower_v.size() << std::endl;
  }

  // Badch Inputs
  // --------------------
  LARCV_DEBUG() << "makeChStatusImage()" << std::endl;
  std::vector< larcv::Image2D > badch_v = makeChStatusImage( e, wholeview_v );
  LARCV_INFO() << "Number of BadCh images: " << badch_v.size() << std::endl;

  bool eventpass = true;

  // PMT Precuts
  // -------------
  eventpass = m_precutalgo.apply( ophit_beam_v );
  LARCV_INFO() << "PMT Precut Algo Result: " << eventpass << std::endl;

  // CROI
  // -----------
  std::vector< larcv::ROI > roi_v;
  int nrois = runCROIfromFlash( wholeview_v, 
				opflash_vv.at(0),
				roi_v );
  LARCV_DEBUG() << "RAN CROI from flash finder. NROIS=" << nrois << std::endl;

  // Tagger
  // ------------  
  larcv::EventPixel2D* ev_thrumu_pixels 
    = (larcv::EventPixel2D*)_supera.driver().io_mutable().get_data( larcv::kProductPixel2D, "thrumupixels" );
  larcv::EventPixel2D* ev_stopmu_pixels 
    = (larcv::EventPixel2D*)_supera.driver().io_mutable().get_data( larcv::kProductPixel2D, "stopmupixels" );

  ev_thrumu_pixels->clear();
  ev_stopmu_pixels->clear();

  bool was_tagger_run = runTagger( e, wholeview_v, badch_v, opflash_vv, trigger,
				   *ev_thrumu_pixels, *ev_stopmu_pixels );
  LARCV_INFO() << "RAN Tagger: " << was_tagger_run << std::endl;

  // Vertexer
  // ----------
  
  // prep track and shower image inputs
  std::vector<larcv::Image2D> shower_masked_v;
  std::vector<larcv::Image2D> track_masked_v;
  prepareVertexerData( wholeview_v, shower_v, track_v, shower_masked_v, track_masked_v );
  
  // add products to vertexer iomanager: track/shower images
  larcv::EventImage2D* ev_shower_tpc 
    = (larcv::EventImage2D*)_supera.driver().io_mutable().get_data( larcv::kProductImage2D, "shower_tpc" );
  larcv::EventImage2D* ev_track_tpc 
    = (larcv::EventImage2D*)_supera.driver().io_mutable().get_data( larcv::kProductImage2D, "track_tpc" );
  ev_shower_tpc->Emplace( std::move(shower_masked_v) );
  ev_track_tpc->Emplace(  std::move(track_masked_v)  );

  // add products to vertexer iomanager: croi 
  larcv::EventROI* ev_roi
    = (larcv::EventROI*)_supera.driver().io_mutable().get_data( larcv::kProductROI, "croi" );
  ev_roi->Emplace( std::move(roi_v) );

  // badch image
  larcv::EventImage2D* ev_badch 
    = (larcv::EventImage2D*)_supera.driver().io_mutable().get_data( larcv::kProductImage2D, "dead_wire" );
  ev_badch->Emplace( std::move(badch_v) );

  // thrumu/stopmu pixel2dclusters
  // (already stored from tagger call above)

  // put back the wire image
  auto ev_imgs  = (larcv::EventImage2D*) _supera.driver().io_mutable().get_data( larcv::kProductImage2D, "wire" );
  ev_imgs->Emplace( std::move(wholeview_v) );

  //run the vertexer
  bool autosave_entry = false;
  bool vertexer_ok = false;
  try {
    vertexer_ok = m_vertexer->process_entry(autosave_entry);
  }
  catch( std::exception& e ) {
    LARCV_WARNING() << "failure in running vertexer: "  << e.what() << std::endl;
    vertexer_ok = false;
  }
  LARCV_DEBUG() << "RAN vertexer. ok=" << vertexer_ok << std::endl;

  // Track Reco
  // -----------
  bool tracker_ok = false;
  try {
    tracker_ok = m_tracker->process_entry(autosave_entry);
  }
  catch( std::exception& e ) {
    LARCV_WARNING() << "failure in running tracker: " << e.what() << std::endl;
    tracker_ok = false;
  }
  LARCV_DEBUG() << "RAN tracker. ok=" << tracker_ok << std::endl;
  

  // Shower Reco
  // ------------

  // Likelihood
  // -----------

  // Make ANA tree
  // ---------------

  // Save LArCV Output
  // --------------------
  saveLArCVProducts( wholeview_v, shower_v, track_v, badch_v );

  // Save LArLite Output
  // ---------------------
  
  LARCV_INFO() << "Event Passed: " << eventpass << std::endl;

  _entries_processed += 1;
}

/**
 * retrive SSNet data from the Art file
 *
 */
bool DLLEEInterface::getSSNetFromArt( art::Event const& e, const std::string ssnet_producer, 
				      const std::vector<larcv::Image2D>& wholeview_v,
				      std::vector< larcv::Image2D >& shower_v,
				      std::vector< larcv::Image2D >& track_v,
				      std::vector< larcv::ROI >& roi_v ) 
{
  
  LARCV_DEBUG() << "Get SSNet data" << std::endl;
  art::Handle< std::vector<ubdldata::pixeldata> > ssnet_handle;
  e.getByLabel( ssnet_producer, ssnet_handle );
  if ( !ssnet_handle.isValid() ) {
    std::cerr << "Attempted to load SSNet ubdldata::pixeldata. label=" << ssnet_producer << std::endl;
    throw cet::exception("DLLEEInterface") << "Could not load SSNet data, label=" << ssnet_producer << "." << std::endl;
  }
  LARCV_INFO() << "Successfully retreived art product" << std::endl;
  
  larcv::ROI pixroi( larcv::kROIBNB ); // ROI for pixeldata

  for ( auto const& pixdata : *ssnet_handle ) {
    if ( pixdata.dim_per_point()!=4 ) {
      throw cet::exception("DLLEEInterface") << "Not the expected number of points! expected=4 vs found=" << pixdata.dim_per_point() << std::endl;
    }
    LARCV_DEBUG() << "pixdata has " << pixdata.len() << " points with " << pixdata.dim_per_point() << " values per point" << std::endl;

    // Make the shower image (fill whole view image)
    // ---------------------------------------------
    const larcv::ImageMeta& meta = wholeview_v.at( pixdata.id() ).meta();
    LARCV_DEBUG() << "storing pixdata into " << meta.dump();

    larcv::Image2D showerimg( meta );
    larcv::Image2D trackimg( meta );
    showerimg.paint(0);
    trackimg.paint(0);
    LARCV_DEBUG() << "made blank shower/track images" << std::endl;
    for ( size_t ipt=0; ipt<(size_t)pixdata.len(); ipt++ ) {
      std::vector<float> pix = pixdata.point(ipt);
      try {
	if ( pix[1]==meta.min_y() ) {
	  pix[1] += 0.5;
	}
	size_t col = meta.col( pix[0] );
	size_t row = meta.row( pix[1] );
	float shr = pix[2];
	float trk = pix[3];
	showerimg.set_pixel( row, col, shr );
	trackimg.set_pixel( row, col, trk );
      }
      catch (std::exception& e ) {
	LARCV_DEBUG() << "failure to load pixdata point[" << ipt << "] "  << std::endl;
	LARCV_DEBUG() << " error: " << e.what() << std::endl;
	LARCV_DEBUG() << "   nvalues=" << pix.size() << std::endl;
	LARCV_DEBUG() << "   (wire,tick)=(" << pix[0] << "," << pix[1] << ")" << std::endl;
      }
    }
    shower_v.emplace_back( std::move(showerimg) );
    track_v.emplace_back( std::move(trackimg) );
    
    // Make CROI
    // -----------
    std::vector<float> bbox = pixdata.as_vector_bbox(); // xmin, ymin, xmax, ymax
    float width  = bbox[2]-bbox[0];
    float height = bbox[3]-bbox[1]; 
    int num_rows = height/meta.pixel_height();
    int num_cols = width/meta.pixel_width();
    float origin_x = bbox[0];
    float origin_y = bbox[3]; // tick-backwards
    // float origin_y = bbox[1]; // tick-forwards
    larcv::ImageMeta plane_roi( width, height, num_rows, num_cols, origin_x, origin_y, meta.plane() );
    pixroi.AppendBB( meta );
    LARCV_INFO() << " SSNET cropped ROI: " << meta.dump();
    LARCV_DEBUG() << "finished pixdata plane=" << pixdata.id() << std::endl;
  }
  roi_v.emplace_back( std::move(pixroi) );

  return true;
}

/**
 * convert larsoft products to larlite
 *
 * we have to covert the following products
 *  -- opflash
 *
 * @param[in] e art::Event with wire data
 * @param[inout] opflash_vv vector of opflash event containers to pass back
 * @param[inout] ophit_beam_v vector of ophits to pass back
 * @param[inout] trigger trigger data
 * 
 */
void DLLEEInterface::runLiteMaker( art::Event const& e, 
				   std::vector< larlite::event_opflash >& opflash_vv,
				   larlite::event_ophit& ophit_beam_v,
				   larlite::trigger& trigger ) {

  // clear the event
  _litemaker.EventClear();

  // opflash products
  for ( auto const& opflash_producer : _litemaker_opflash_producer_v ) {
    art::Handle< std::vector<recob::OpFlash> > flash_handle;
    e.getByLabel( opflash_producer, flash_handle );
    if ( !flash_handle.isValid() ) {
      std::cerr << "Attempted to load OpFlash data. producer=" << opflash_producer << std::endl;
      throw cet::exception("DLLEEInterface") << "Could not load OpFlash data, label=" << opflash_producer << "." << std::endl;
    }
    else {
      LARCV_INFO() << "Loaded OpFlash data. label=" << opflash_producer << std::endl;
    }
    larlite::event_opflash ev_flash;
    _litemaker.ScanData( flash_handle, &ev_flash );
    LARCV_INFO() << "  event container " << opflash_producer << " has " << ev_flash.size() << std::endl;
    opflash_vv.emplace_back( std::move(ev_flash) );
  }
  
  // OpHit: beam
  art::Handle< std::vector<recob::OpHit> > ophit_beam_handle;
  e.getByLabel( _litemaker_ophitbeam_producer, ophit_beam_handle );
  if ( !ophit_beam_handle.isValid() ) {
    std::cerr << "Failed to load OpHit (beam) data. producer=" << _litemaker_ophitbeam_producer << std::endl;
    throw cet::exception("DLLEEInterface") << "Could not load OpHit(beam) data, producer=" << _litemaker_ophitbeam_producer << "." << std::endl;
  }
  try {
    _litemaker.ScanData( ophit_beam_handle, &ophit_beam_v );
    LARCV_INFO() << "  number of beam ophits: " << ophit_beam_v.size() << std::endl;
  }
  catch( std::exception& err ) {
    throw cet::exception("DLLEEInterface") << "Failure to convert ophit (beam) data into larlite object" << std::endl;
  }

  // Trigger
  art::Handle< std::vector<raw::Trigger> > trigger_h;
  e.getByLabel( _litemaker_trigger, trigger_h );
  if ( !trigger_h.isValid() ) {
    throw cet::exception("DLEEInterface") << "Failed to load trigger data from art file. producer=" << _litemaker_trigger << std::endl;
  }
  try {
    _litemaker.ScanData( trigger_h, &trigger );
  }
  catch ( std::exception& err ) {
    throw cet::exception("DLLEEInterface") << "Failed to convert trigger data into larlite object" << std::endl;
  }

  // Hit
  
}

/**
 * have supera make larcv images
 *
 * get recob::Wire from art::Event, pass it to supera
 * the _supera object will contain the larcv IOManager which will have images stored
 *
 * @param[in] e art::Event with wire data
 * @param[inout] wholeview_imgs vector of Image2D to pass back images
 * @return int number of images to process by network
 *
 */
int DLLEEInterface::runSupera( art::Event& e, 
			       std::vector<larcv::Image2D>& wholeview_imgs ) {

  //
  // set data product
  // 
  // :wire  
  art::Handle<std::vector<recob::Wire> > data_h;
  //  handle sub-name
  if(_wire_producer_name.find(" ")<_wire_producer_name.size()) {
    e.getByLabel(_wire_producer_name.substr(0,_wire_producer_name.find(" ")),
		 _wire_producer_name.substr(_wire_producer_name.find(" ")+1,_wire_producer_name.size()-_wire_producer_name.find(" ")-1),
		 data_h);
  }else{ e.getByLabel(_wire_producer_name, data_h); }
  if(!data_h.isValid()) { 
    std::cerr<< "Attempted to load recob::Wire data: " << _wire_producer_name << std::endl;
    throw ::cet::exception("DLLEEInterface") << "Could not locate recob::Wire data!" << std::endl;
  }
  _supera.SetDataPointer(*data_h,_wire_producer_name);

  // execute supera
  bool autosave_entry = false;
  LARCV_INFO() << "process event: (" << e.id().run() << "," << e.id().subRun() << "," << e.id().event() << ")" << std::endl;
  _supera.process(e.id().run(),e.id().subRun(),e.id().event(), autosave_entry);

  // get the images
  auto ev_imgs  = (larcv::EventImage2D*) _supera.driver().io_mutable().get_data( larcv::kProductImage2D, "wire" );
  ev_imgs->Move( wholeview_imgs );
  
  return wholeview_imgs.size();
}

/**
 * Fill ChStatus Image
 *
 * @param[in] e art::Event from which we get status data
 * @param[in] img_v vector of images to be used as template for output
 *
 */
std::vector< larcv::Image2D> DLLEEInterface::makeChStatusImage( const art::Event& e,
								const std::vector< larcv::Image2D >& img_v ) {
  
  std::vector< larcv::Image2D > badch_v;
  auto const* geom = ::lar::providerFrom<geo::Geometry>();
  
  std::vector<bool> filled_ch( geom->Nchannels(), false );
  std::map<geo::PlaneID::PlaneID_t,std::vector<short> > status_m;
  
  // If specified check RawDigit pedestal value: if negative this channel is not used by wire (set status=>-2)                                                                                              
  if(!_chstatus_rawdigit_producer.empty()) {
    LARCV_DEBUG() << "Check RawDigit pedestal value" << std::endl;
    
    art::Handle<std::vector<raw::RawDigit> > digit_h;

    std::string label = _chstatus_rawdigit_producer;

    if(label.find("::")<label.size()) {
      e.getByLabel(label.substr(0,label.find("::")),
                   label.substr(label.find("::")+2,label.size()-label.find("::")-2),
                   digit_h);
    }else{ e.getByLabel(_chstatus_rawdigit_producer,digit_h);}

    for(auto const& digit : *digit_h) {
      auto const ch = digit.Channel();
      if(ch >= filled_ch.size()) throw ::larlite::DataFormatException("Found RawDigit > possible channel number!");
      if(digit.GetPedestal()<0.) {
        auto const wid =  geom->ChannelToWire(ch).front();
        auto iter = status_m.find(wid.planeID().Plane);
        if(iter != status_m.end())
          (*iter).second[wid.Wire] = -2;
        else{
	  std::vector<short> status_v(geom->Nwires(wid.planeID()),5);
          status_v[wid.Wire] = -2;
          status_m.emplace(wid.planeID().Plane,status_v);
        }
        filled_ch[ch] = true;
      }
    }
  }
  

  const lariov::ChannelStatusProvider& chanFilt = art::ServiceHandle<lariov::ChannelStatusService>()->GetProvider();
  for(size_t i=0; i < geom->Nchannels(); ++i) {
    if ( filled_ch[i] ) continue;
    auto const wid =  geom->ChannelToWire(i).front();
    short status = 0;
    if (!chanFilt.IsPresent(i)) status = -1;
    else status = (short)(chanFilt.Status(i));
    
    auto iter = status_m.find(wid.planeID().Plane);
    if(iter != status_m.end())
      (*iter).second[wid.Wire] = status;
    else{
      std::vector<short> status_v(geom->Nwires(wid.planeID()),5);
      status_v[wid.Wire] = status;
      status_m.emplace(wid.planeID().Plane,status_v);
    }
  }

  // transfer status to image2d
  for ( size_t p=0; p<img_v.size(); p++ ) {

    const larcv::ImageMeta& meta = img_v.at(p).meta();
    larcv::Image2D badch( meta );
    badch.paint(0.0);
    badch_v.emplace_back( std::move(badch) );
  }
    

  for ( auto& it : status_m ) {

    short p = (short)it.first;
    std::vector<short>& status_v = it.second;
    larcv::Image2D& badch = badch_v.at(p);
    const larcv::ImageMeta& meta = badch.meta();
    
    for ( size_t c=0; c<meta.cols(); c++ ) {
      size_t wire = (size_t)meta.pos_x(c);
      if ( wire>=status_v.size() || status_v.at(wire)<4 ) {
	badch.paint_col( c, 255.0 );
      }
    }

  }

  return badch_v;
}

int DLLEEInterface::runCROIfromFlash( const std::vector<larcv::Image2D>& wholeview_v, 
				      const larlite::event_opflash& beam_flash_v,
				      std::vector<larcv::ROI>& roi_v ) {


  const float usec_min = 190*0.015625;
  const float usec_max = 320*0.015625;

  std::vector<larlite::opflash> intime_flashes_v;
  for ( auto const& flash : beam_flash_v ) {
    if ( flash.Time()<usec_min || flash.Time()>usec_max ) continue;
    intime_flashes_v.push_back( flash );
  }
  LARCV_INFO() << "Number of INTIME flashes: " << intime_flashes_v.size() << std::endl;

  // pass them to the algo to get CROI    
  int ncrops = 0;
  for ( auto& llflash : intime_flashes_v ) {
    std::vector<larcv::ROI> flashrois = _croifromflashalgo.findCROIfromFlash( llflash );
    ncrops += flashrois.size();
    for ( auto& roi : flashrois )
      roi_v.emplace_back( std::move(roi) );
  }
  LARCV_INFO() << "Number of ROIs: " << ncrops << std::endl;

  // done
  return (int)ncrops;
}

/**
 * Run the tagger
 *
 * load the input class and run the tagger
 *
 */
bool DLLEEInterface::runTagger( const art::Event& e,
				std::vector<larcv::Image2D>& wholeview_v,
				std::vector<larcv::Image2D>& badch_v, 
				std::vector<larlite::event_opflash>& opflash_vv,
				larlite::trigger& trigger, 
				larcv::EventPixel2D& ev_thrumu_pixels,
				larcv::EventPixel2D& ev_stopmu_pixels ) {
  
  // fill tagger input: adc image
  for ( auto const& img : wholeview_v )
    m_tagger_input.img_v.push_back( img );

  // fill tagger input: bad channels
  for ( auto const& img : badch_v )
    m_tagger_input.badch_v.push_back( img );

  // fill tagger input: empty channels
  try {
    for ( auto const &img : wholeview_v ) {
      int p = img.meta().plane();
      larcv::Image2D emptyimg = m_emptych_algo.labelEmptyChannels( m_taggeralgo_cfg.emptych_thresh.at(p), img );
      m_tagger_input.gapch_v.emplace_back( std::move(emptyimg) );
    }
  }
  catch (std::exception& e) {
    std::cerr << "DLLEEInterface:: Error making empty channels: " << e.what() << std::endl;
    throw std::runtime_error("DLLEEInterface:  ERROR");
  }

  // fill tagger input: opflashes
  for ( auto& opflash_v : opflash_vv )
    m_tagger_input.opflashes_v.push_back( &opflash_v );

  // fill tagger input: trigger data
  m_tagger_input.p_ev_trigger = &trigger;
  
  // fill tagger input: run, subrun, event, entry
  m_tagger_input.run    = e.id().run();
  m_tagger_input.subrun = e.id().subRun();
  m_tagger_input.event  = e.id().event();
  m_tagger_input.entry  = _entries_processed;

  auto it = ev_thrumu_pixels.Pixel2DClusterArray().find(0);
  LARCV_INFO() << "DERP1" << std::endl;
  
  // run tagger
  larlitecv::TaggerCROIAlgo taggeralgo(m_taggeralgo_cfg);


  it = ev_thrumu_pixels.Pixel2DClusterArray().find(0);
  LARCV_INFO() << "DERP2" << std::endl;
    
  // output payloads
  larlitecv::ThruMuPayload thrumu_results = taggeralgo.runBoundaryPointFinder( m_tagger_input );

  it = ev_thrumu_pixels.Pixel2DClusterArray().find(0);
  LARCV_INFO() << "DERP3" << std::endl;

  int num_boundary_points = 0;
  num_boundary_points += thrumu_results.side_filtered_v.size();
  num_boundary_points += thrumu_results.anode_filtered_v.size();
  num_boundary_points += thrumu_results.cathode_filtered_v.size();
  num_boundary_points += thrumu_results.imgends_filtered_v.size();

  it = ev_thrumu_pixels.Pixel2DClusterArray().find(0);
  LARCV_INFO() << "DERP4" << std::endl;

  if ( num_boundary_points>_tagger_endpoint_limit) {
    // too many endpoints we stop the tagger
    // likely noisy, so sometimes causes code to hang
    return false;
  }

  it = ev_thrumu_pixels.Pixel2DClusterArray().find(0);

  LARCV_DEBUG() << "run the THRUMU tagger" << std::endl;
  taggeralgo.runThruMu( m_tagger_input, thrumu_results );
  it = ev_thrumu_pixels.Pixel2DClusterArray().find(0);
  LARCV_DEBUG() << "RAN the THRUMU tagger" << std::endl;
  
  LARCV_DEBUG() << "run the STOPMU tagger" << std::endl;
  it = ev_thrumu_pixels.Pixel2DClusterArray().find(0);
  larlitecv::StopMuPayload stopmu_results = taggeralgo.runStopMu( m_tagger_input, thrumu_results );
  LARCV_DEBUG() << "RAN the STOPMU tagger" << std::endl;

  //larlitecv::CROIPayload   croi_out = taggeralgo.runCROISelection( m_tagger_input, thrumu_out, stopmu_out );

  it = ev_thrumu_pixels.Pixel2DClusterArray().find(0);
  makeTaggerPixelClusters( m_tagger_input, m_taggeralgo_cfg, 
			   thrumu_results, stopmu_results,
			   ev_thrumu_pixels, ev_stopmu_pixels );

  LARCV_DEBUG() << "Tagger output converted successfully" << std::endl;

  return true;
}

/**
 * convert 3D tracks from the thrumu reco into tagged images
 *
 * @param[in] input class containing thrumu inputs
 * @param[in] config configuration for tagger
 * @param[in] data output products from thrumu data
 * @param[inout] ev_tracks2d resulting pixel clusters passed back
 */
void DLLEEInterface::makeTaggerPixelClusters( const larlitecv::InputPayload& input,
					      const larlitecv::TaggerCROIAlgoConfig& config,
					      larlitecv::ThruMuPayload& thrumuout,
					      larlitecv::StopMuPayload& stopmuout,
					      larcv::EventPixel2D& ev_thrumupix,
					      larcv::EventPixel2D& ev_stopmupix ) {
  
  // save 2D track objects filtered by good 3d tracks
  
  // THRUMU
  LARCV_INFO() << "Save THRUMU: number of tracks=" 
	       << thrumuout.trackcluster3d_v.size()
	       << std::endl;
  for (int i3d=0; i3d<(int)thrumuout.trackcluster3d_v.size(); i3d++) {
    const larlitecv::BMTrackCluster3D& track3d = thrumuout.trackcluster3d_v.at(i3d);
    LARCV_DEBUG() << "  track[" << i3d << "]" << std::endl;
    std::vector< larcv::Pixel2DCluster > trackcluster2d 
      = track3d.getTrackPixelsFromImages( input.img_v, input.badch_v,
					  config.thrumu_tracker_cfg.pixel_threshold, 
					  config.thrumu_tracker_cfg.tag_neighborhood, 0.3 );
    LARCV_DEBUG() << "  trackcluster2d[" << trackcluster2d.size() << "]" << std::endl;
    for (int p=0; p<3; p++) {
      LARCV_DEBUG() << " storing plane[" << p << "] cluster w/ " << trackcluster2d.at(p).size() << " pixels" << std::endl;
      ev_thrumupix.Append( (larcv::PlaneID_t)p, trackcluster2d.at(p), input.img_v.at(p).meta() );
    }
  }

  // STOPMU
  LARCV_INFO() << "Save STOPMU: number of tracks=" 
	       << stopmuout.stopmu_trackcluster_v.size()
	       << std::endl;
  for ( size_t itrack=0; itrack<stopmuout.stopmu_trackcluster_v.size(); itrack++ ) {
    const larlitecv::BMTrackCluster3D& track3d = stopmuout.stopmu_trackcluster_v.at(itrack);
    std::vector< larcv::Pixel2DCluster > trackpixs_v 
      = track3d.getTrackPixelsFromImages( input.img_v, input.badch_v,
					  config.thrumu_tracker_cfg.pixel_threshold,
					  config.thrumu_tracker_cfg.tag_neighborhood, 0.3 );
    for (size_t p=0; p<trackpixs_v.size(); p++) {
      ev_stopmupix.Append( (larcv::PlaneID_t)p, trackpixs_v.at(p), stopmuout.stopmu_v.at(p).meta() );
    }
  }

}

/**
 * prepare data objects for vertexer
 * 
 * @param[in]
 * @param[inout] 
 */
void DLLEEInterface::prepareVertexerData( const std::vector<larcv::Image2D>& wholeview_v, 
					  const std::vector<larcv::Image2D>& shower_v,
					  const std::vector<larcv::Image2D>& track_v,
					  std::vector<larcv::Image2D>& shower_masked_v,
					  std::vector<larcv::Image2D>& track_masked_v )
{
  
  if ( wholeview_v.size()!=shower_v.size() 
       || wholeview_v.size()!=track_v.size() ) {
    LARCV_CRITICAL() << "Number of shower or track images does not match wholeview images" << std::endl;
  }

  for ( size_t p=0; p<wholeview_v.size(); p++ ) {

    const float thresh = _threshold_v[p];

    const larcv::Image2D& adc_image  = wholeview_v.at(p);
    const larcv::Image2D& ssn_shower = shower_v.at(p);
    const larcv::Image2D& ssn_track  = track_v.at(p);

    larcv::Image2D shower_adc( ssn_shower.meta() );
    larcv::Image2D track_adc(  ssn_track.meta() );

    // check the meta is the same
    if ( adc_image.meta()!=ssn_shower.meta()
	 || adc_image.meta()!=ssn_track.meta() ) {
      LARCV_CRITICAL() << "track, shower, and adc image do not have matching meta" << std::endl;
    }

    const std::vector<float>& adc_pix = adc_image.as_vector();
    const std::vector<float>& shr_pix = ssn_shower.as_vector();
    const std::vector<float>& trk_pix = ssn_track.as_vector();
    std::vector<float> shr_out( shr_pix.size(), 0.0 );
    std::vector<float> trk_out( trk_pix.size(), 0.0 );

    for ( size_t i=0; i<adc_pix.size(); i++ ) {
      if ( adc_pix[i]>thresh && (shr_pix[i]>1.0e-4 || trk_pix[i]>1.0e-4) ) {
	if ( shr_pix[i]>trk_pix[i] )
	  shr_out[i] = adc_pix[i];
	else
	  trk_out[i] = adc_pix[i];
      }
    }

    shower_adc.move( std::move(shr_out) );
    track_adc.move(  std::move(trk_out) );

    shower_masked_v.emplace_back( std::move(shower_adc) );
    track_masked_v.emplace_back(  std::move(track_adc) );
  }
}


/**
 * save LArCV products to file
 *
 */
void DLLEEInterface::saveLArCVProducts( std::vector<larcv::Image2D>& wholeview_v,
					std::vector<larcv::Image2D>& shower_v,
					std::vector<larcv::Image2D>& track_v,
					std::vector<larcv::Image2D>& badch_v ) {
  
  larcv::IOManager& io_larcv = _supera.driver().io_mutable();

  // save wholeview ADC images (done earlier)
  //LARCV_DEBUG() << "Save ADC" << std::endl;
  //auto ev_imgs  = (larcv::EventImage2D*) io_larcv.get_data( larcv::kProductImage2D, "wire" );
  //ev_imgs->Emplace( std::move(wholeview_v) );

  // save shower score images
  LARCV_DEBUG() << "Save shower images" << std::endl;
  auto ev_shr  = (larcv::EventImage2D*) io_larcv.get_data( larcv::kProductImage2D, "ssnetshower" );
  ev_shr->Emplace( std::move(shower_v) );
  
  // save track score images
  LARCV_DEBUG() << "Save track images" << std::endl;
  auto ev_trk  = (larcv::EventImage2D*) io_larcv.get_data( larcv::kProductImage2D, "ssnettrack" );
  ev_trk->Emplace( std::move(track_v) );


  // save entry
  LARCV_INFO() << "saving LARCV entry" << std::endl;
  io_larcv.save_entry();
  
  // we clear entries ourselves
  LARCV_INFO() << "clearing entry" << std::endl;
  io_larcv.clear_entry();

}


/**
 * 
 * overridden method: setup class before job is run
 *
 * 
 */
void DLLEEInterface::beginJob()
{

  // --------
  // SUPERA
  // --------
  std::string supera_cfg;
  cet::search_path finder("FHICL_FILE_PATH");
  if( !finder.find_file(_supera_config, supera_cfg) )
    throw cet::exception("DLLEEInterface") << "Unable to find supera cfg in "  << finder.to_string() << "\n";
  std::cout << "LOADING supera config: " << supera_cfg << std::endl;

  // configure supera (convert art::Event::CalMod -> Image2D)
  _supera.configure(supera_cfg);

  LARCV_DEBUG() << "initialize Supera" << std::endl;
  _supera.initialize();

  // --------
  // VERTEX
  // --------
  std::string vertexer_cfg_path;
  if( !finder.find_file( _vertexer_cfg, vertexer_cfg_path) )
    throw cet::exception("DLLEEInterface") << "Unable to find vertexer cfg in "  << finder.to_string() << "\n";
  LARCV_INFO() << "LOADING vertex config: " << vertexer_cfg_path << std::endl;
  // pass the iomanager for supera
  LARCV_DEBUG() << "VERTEXER: pass supera IOManager to vertexer" << std::endl;
  m_vertexer->set_iomanager( &_supera.driver().io_mutable() );
  LARCV_DEBUG() << "VERTEXER: configure" << std::endl;
  m_vertexer->configure( vertexer_cfg_path );
  LARCV_DEBUG() << "VERTEXER: initialize" << std::endl;
  m_vertexer->initialize();
  LARCV_INFO()  << "VERTEXER: Process Driver Loaded" << std::endl;

  // --------
  // TRACKER
  // --------
  std::string tracker_cfg_path;
  if( !finder.find_file( _tracker_cfg, tracker_cfg_path) )
    throw cet::exception("DLLEEInterface") << "Unable to find tracker cfg in "  << finder.to_string() << "\n";
  LARCV_INFO() << "LOADING vertex config: " << tracker_cfg_path << std::endl;
  // pass the iomanager for supera
  LARCV_DEBUG() << "TRACKER: pass supera IOManager to tracker" << std::endl;
  m_tracker->set_iomanager( &_supera.driver().io_mutable() );
  LARCV_DEBUG() << "TRACKER: configure" << std::endl;
  m_tracker->configure( tracker_cfg_path );
  LARCV_DEBUG() << "TRACKER: initialize" << std::endl;
  m_tracker->initialize();
  LARCV_INFO()  << "TRACKER: Process Driver Loaded" << std::endl;


}

/**
 * 
 * overridden method: setup class after all events are run
 *
 * 
 */
void DLLEEInterface::endJob()
{
  LARCV_DEBUG() << "finalize IOmanager" << std::endl;
  _supera.finalize();
}

DEFINE_ART_MODULE(DLLEEInterface)
