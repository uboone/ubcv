#ifndef __UBCV_LARCVIMAGEMAKER_LARCV_WC_THRUMU_IMAGEMAKER_H__
#define __UBCV_LARCVIMAGEMAKER_LARCV_WC_THRUMU_IMAGEMAKER_H__

#include <vector>

#include "FMWKInterface.h"
#include "larcv/core/DataFormat/Image2D.h"

namespace supera {

  std::vector<larcv::Image2D> 
    WCThrumuTaggerHits2Image2D( const std::vector<supera::LArHit_t>& thrumu_hits, // type declared in FMWKInterface.h
				const std::vector<larcv::Image2D>& adc_v );

  int remove_from_hit(larcv::Image2D& removed_img, 
		      const supera::LArHit_t& hit,
		      int padding=2);
  
}

#endif
