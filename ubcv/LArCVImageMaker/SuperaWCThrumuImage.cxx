#ifndef __UBCV_LARCVIMAGEMAKER_SUPERA_WC_THRUMU_IMAGE_CXX__
#define __UBCV_LARCVIMAGEMAKER_SUPERA_WC_THRUMU_IMAGE_CXX__

#include "SuperaWCThrumuImage.h"
#include "LArCVWCThrumuImage.h"
#include "ImageMetaMakerFactory.h"
#include "larcv/core/DataFormat/EventImage2D.h"

namespace larcv {

  static SuperaWCThrumuImageProcessFactory __global_SuperaWCThrumuImageProcessFactory__;

  SuperaWCThrumuImage::SuperaWCThrumuImage(const std::string name)
    : SuperaBase(name)
  {}

  void SuperaWCThrumuImage::configure(const PSet& cfg)
  {
    
    SuperaBase::configure(cfg); 
    // Inherited in SuperBase is the parameter 'LArHitProducer' which needs to be set to get input Hits
    // Determines what product holding recob::Hit container is used when LArData<supera::LArHit_t>() is called

    supera::ImageMetaMaker::configure(cfg);

    _input_wireimage_treename     = cfg.get<std::string>("InputWireTreeName");
    _output_image2d_treename = cfg.get<std::string>("OutImageLabel","thrumu");

  }

  void SuperaWCThrumuImage::initialize()
  { SuperaBase::initialize(); }

  bool SuperaWCThrumuImage::process(IOManager& mgr)
  {

    SuperaBase::process(mgr);

    auto const& meta_v = Meta();
    
    if(meta_v.empty()) {
      LARCV_CRITICAL() << "Meta not created!" << std::endl;
      throw larbys();
    }
    auto ev_image = (EventImage2D*)(mgr.get_data(kProductImage2D,_output_image2d_treename));
    if(!ev_image) {
      LARCV_CRITICAL() << "Output image could not be created!" << std::endl;
      throw larbys();
    }
    if(!(ev_image->Image2DArray().empty())) {
      LARCV_CRITICAL() << "Output image array not empty!" << std::endl;
      throw larbys();
    }
    
    larcv::EventImage2D* ev_wire = 
      (larcv::EventImage2D*)mgr.get_data(kProductImage2D,_input_wireimage_treename);

    if ( ev_wire->as_vector().size()==0 ) {
      LARCV_CRITICAL() << "Input Wire Images not made yet!" << std::endl;
    }

    std::vector<larcv::Image2D> out_thrumu_v = 
      supera::WCThrumuTaggerHits2Image2D( LArData<supera::LArHit_t>(), ev_wire->as_vector() );

    ev_image->Emplace(std::move(out_thrumu_v));

    return true;
  }

  void SuperaWCThrumuImage::finalize()
  {}


}
#endif
