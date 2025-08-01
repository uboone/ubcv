#ifndef __IMAGEMETAFROMCONFIG_CXX__
#define __IMAGEMETAFROMCONFIG_CXX__

#include "ImageMetaFromConfig.h"

namespace supera {

  void ImageMetaFromConfig::configure(const supera::Config_t& cfg)
  {
    ImageMetaMakerBase::configure(cfg);
    auto min_time = cfg.get<double>("MinTime");
    auto min_wire = cfg.get<double>("MinWire");
    auto image_rows = cfg.get<std::vector<size_t> >("EventImageRows");
    auto image_cols = cfg.get<std::vector<size_t> >("EventImageCols");
    bool tick_backward = cfg.get<bool>("TickBackward",false); // from DLGen1 - deprecated

    auto const& comp_rows = RowCompressionFactor();
    auto const& comp_cols = ColCompressionFactor();

    if(image_rows.size() != comp_rows.size()) {
      std::cerr << "EventImageRows size != EventCompRows..." << std::endl;
      throw std::exception();
    }

    if(image_rows.size() != image_cols.size()) {
      std::cerr << "EventImageRows size != EventImageCols..." << std::endl;
      throw std::exception();
    }

    // construct meta: this is the meta with the FINAL resolution
    for(size_t plane=0; plane<image_rows.size(); ++plane) {
      
      float origin_y = min_time;
      double image_width  = image_cols[plane] * comp_cols[plane];
      double image_height = image_rows[plane] * comp_rows[plane];

      if ( tick_backward )
	origin_y = min_time + image_height;
      larcv::ImageMeta meta(image_width, image_height,
			    image_rows[plane], image_cols[plane],
  			    min_wire, origin_y, plane);
      
      LARCV_INFO() << "Created meta " <<  meta.dump();
      
      _meta_v.emplace_back(std::move(meta));
    }
    
  }  
}
#endif
