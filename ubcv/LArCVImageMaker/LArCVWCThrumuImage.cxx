#include "LArCVWCThrumuImage.h"

#include "larcv/core/Base/larcv_logger.h"

namespace supera {

  std::vector<larcv::Image2D> 
  WCThrumuTaggerHits2Image2D( const std::vector<supera::LArHit_t>& thrumu_hits,
			      const std::vector<larcv::Image2D>& adc_v )
  {
    
    // make output image2d container
    std::vector< larcv::Image2D > outoftime_v; // pixels show us what have been tagged as out-of-time
    //std::vector< larcv::Image2D > intime_v;    // pixels show us what has been tagged as in-time
    
    for (size_t p=0; p<adc_v.size(); p++) {
      larcv::Image2D out_img( adc_v.at(p) );        // copy
      //larcv::Image2D in_img( adc_v.at(p).meta() );  // making blank
      outoftime_v.emplace_back( std::move(out_img) );
      //intime_v.emplace_back( std::move(in_img) );
    }
    
    int nhits_removed_pixels = 0;
    for (size_t idx=0; idx < thrumu_hits.size(); idx++) {
      auto const& hit = thrumu_hits.at(idx);
      int plane = hit.View();
    
      if ( plane<0 || plane>=(int)outoftime_v.size() )
	continue;
      
      auto& outoftime = outoftime_v.at(plane);
      
      int nremoved = remove_from_hit( outoftime, hit, 2 );
      if ( nremoved>0 )
	nhits_removed_pixels++;
      
    }
    
    LARCV_SINFO() << "Number of the total (" << thrumu_hits.size() << ") "
		  << "hits that removed pixels is " << nhits_removed_pixels 
		  << std::endl;
    
    return outoftime_v;
    
  }

  int remove_from_hit(larcv::Image2D& removed_img, 
		      const supera::LArHit_t& hit,
		      int padding){
    /*
      this function takes in an image and a hit, and removes charge in the Image
      based on the hit's position, with some padding around it (default padding of 2)
    */

    //int row = 0;
    //int plane = 0;
    int col = 0;
    int minrow = 0;
    int maxrow = 0;
    float offset = -3;
    const larcv::ImageMeta& meta = removed_img.meta();

    float maxtick = hit.PeakTime()+2400+hit.SigmaPeakTime();
    float mintick = hit.PeakTime()+2400-hit.SigmaPeakTime();
    //std::cout << "mintick1: " << mintick << " " << maxtick << std::endl;
    if ( maxtick >= meta.max_y() )
      maxtick = meta.max_y()-meta.pixel_height();
    if ( mintick <= meta.min_y() )
      mintick = meta.min_y()+meta.pixel_height();
    //std::cout << "mintick2: " << mintick << " " << maxtick << std::endl;

    if (mintick>=meta.max_y()) {
      return 0; // not removing
    }
    if (maxtick<=meta.min_y()) {
      return 0; // not removing
    }
    
    float peaktick = hit.PeakTime()+2400+offset;
    if ( peaktick>=meta.max_y() ) 
      return 0;
    if ( peaktick<=meta.min_y() ) 
      return 0;

    int channel = (int)hit.Channel();
    if ( hit.View()==2 )
      channel -= 2*2400;
    else if ( hit.View()==1 )
      channel -= 2400;

    minrow = meta.row(mintick,__FILE__,__LINE__);
    maxrow = meta.row(maxtick,__FILE__,__LINE__);
    col = meta.col(channel);
  
    if ( minrow>maxrow ) {
      int tmp = minrow;
      minrow = maxrow;
      maxrow = tmp;
    }
    
    int pixels_zeroed = 0;
    for (int r = minrow-padding; r <= maxrow+padding; r++){
      if (r<0 ) continue;
      if (r>=(int)removed_img.meta().rows()) continue;
      for(int c = col-padding; c <= col+padding; c++){
	if (c<0 ) continue;
	if (c>=(int)removed_img.meta().cols()) continue;
	
	if ( removed_img.pixel(r,c)>10.0 )
	  pixels_zeroed++;
	removed_img.set_pixel(r,c,0);
      }
    }
    
    return pixels_zeroed;
    
  }

//   void overlay_from_hit(larcv::Image2D& new_img, const larcv::Image2D& old_img, const larlite::hit& hit, int padding){
//     /*
//       this image takes in a new image, and an old image, and overlays the old img
//       on top of the new img where the hit is located in row and col with some padding
//       around the hit location
//       if the column is all 0s in the padded region then instead you overlay a
//       value of 50 at the hit location
//   */
//   int row;
//   int col;
//   int plane;
//   float showerscore;
//   float hit_dist;
//   int minrow;
//   int maxrow;
//   float offset = -3;
//   larcv::ImageMeta meta = old_img.meta();

//   float maxtick = hit.PeakTime()+2400+hit.SigmaPeakTime();
//   float mintick = hit.PeakTime()+2400-hit.SigmaPeakTime();
//   if ( maxtick >= meta.max_y() )
//     maxtick = meta.max_y()-meta.pixel_height();
//   if ( mintick <= meta.min_y() )
//     mintick = meta.min_y()+meta.pixel_height();

//   if (mintick>=meta.max_y()) return;
//   if (maxtick<=meta.min_y()) return;

//   float peaktick = hit.PeakTime()+2400+offset;
//   if ( peaktick>=meta.max_y() ) return;
//   if ( peaktick<=meta.min_y() ) return;  
  
//   if(hit.View()==2){
//     plane = 2;
//     minrow = meta.row(mintick,__FILE__,__LINE__);
//     maxrow = meta.row(maxtick,__FILE__,__LINE__);
//     row = meta.row(hit.PeakTime()+2400+offset,__FILE__,__LINE__);
//     col = meta.col(hit.Channel()-2*2400,__FILE__,__LINE__);
//   }
//   else if (hit.View()==0){
//     minrow = meta.row(mintick,__FILE__,__LINE__);
//     maxrow = meta.row(maxtick,__FILE__,__LINE__);
//     row = meta.row(hit.PeakTime()+2400+offset,__FILE__,__LINE__);
//     col = meta.col(hit.Channel(),__FILE__,__LINE__);
//     plane = 0;
//   }
//   else if (hit.View()==1){
//     minrow = meta.row(mintick,__FILE__,__LINE__);
//     maxrow = meta.row(maxtick,__FILE__,__LINE__);
//     row = meta.row(hit.PeakTime()+2400+offset,__FILE__,__LINE__);
//     col = meta.col(hit.Channel()-2400,__FILE__,__LINE__);
//     plane = 1;
//   }

//   if ( minrow>maxrow ) {
//     int tmp = minrow;
//     minrow = maxrow;
//     maxrow = tmp;
//   }
  
//   for(int c = col-padding; c <= col+padding; c++){
//     double col_sum = 0;
//     if ( c<0 ) continue;
//     if ( c>=(int)new_img.meta().cols() ) continue;
//     for (int r = minrow-padding; r <= maxrow+padding; r++){
//       if ( r<0) continue;
//       if ( r>=new_img.meta().rows() ) continue;
//       double val = old_img.pixel(r,c);
//       col_sum += val;
//       new_img.set_pixel(r,c,val);
//     }
//     if (col_sum == 0) {
//       // std::cout << "FLAG Hit Placed in Dead Region\n";
//       new_img.set_pixel(row,c,50.000);
//     }
//   }
//   return;
// }

}
