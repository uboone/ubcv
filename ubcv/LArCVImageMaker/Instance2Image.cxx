#ifndef __SUPERA_INSTANCE_LAR2IMAGE_CXX__
#define __SUPERA_INSTANCE_LAR2IMAGE_CXX__

#include "Instance2Image.h"
#include "larcv/core/Base/larcv_logger.h"
#include "larlite/LArUtil/Geometry.h"

#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "lardata/DetectorInfoServices/DetectorClocksService.h"

namespace supera {

  //
  // SimChannel => PixelFlowMaps
  // 
  void Instance2Image( const std::vector<larcv::ImageMeta>& meta_v,
		       const std::map<int,int>& trackid2ancestorid,
		       const std::vector<supera::LArSimCh_t>& sch_v,
		       const std::vector<float>& row_compression_factor,
		       const std::vector<float>& col_compression_factor,			 
		       const int time_offset,
		       std::vector<larcv::Image2D>& img_out_v,
		       std::vector<larcv::Image2D>& ancestor_out_v,
		       bool fill_with_uncompressed_image ) 
  {
    
    LARCV_SINFO() << "Instance ID Image ..." << std::endl;

    // we pack truth info about the ancestor particle type
    // we label energy deposition by ancestor track id
    // this groups secondaries with their primary parent
    
    // create images we are going to fill
    // std::vector<larcv::Image2D> img_v;      // Filling track ID per pixel
    // std::vector<larcv::Image2D> ancestor_v; // Filling ancestor ID per pixel

    img_out_v.clear();
    ancestor_out_v.clear();

    for ( auto const& meta : meta_v ) {
      //LARCV_SINFO() << meta.dump() << std::endl;
      larcv::Image2D img1(meta);
      img1.paint(-1.0);
      img_out_v.emplace_back( std::move(img1) ); 
     
      larcv::Image2D img2(meta);
      img2.paint(-1.0);
      ancestor_out_v.emplace_back( std::move(img2) );
    }

    // This represents a pixel (at full resolution for (tick,wire))
    // and its associated labels
    struct PixData_t {

      int col;
      int tick;
      double energy;
      long trackid;
      long ancestorid;
      //std::array<int,4> imgcoord;
      bool operator<( const PixData_t& rhs ) const {
	if ( tick < rhs.tick )
	  return true;
	return false;
      };

    };

    // We group pixels on one column (i.e. wire)
    struct ColumnPixels_t {

      int col;
      std::map<int,PixData_t> pixels;

    };

    // The following stores the column of pixels for a wireplane image
    typedef std::map< int, ColumnPixels_t > ColMap_t;

    // This is the container to stored the image for each wireplane
    std::vector< ColMap_t > planemaps_v(3);

    // loop over sim channel information
    //int num_no_ancestor = 0;
    //std::set<int> noancestorids;
    auto const clockData = art::ServiceHandle<detinfo::DetectorClocksService>()->DataForJob();
    for (auto const& sch : sch_v) {
      auto ch = sch.Channel();
      auto const& wid   = ::supera::ChannelToWireID(ch);
      auto const& plane = wid.Plane;

      
      auto& trackidimg  = img_out_v.at(plane);      // track ID image
      //auto& ancestorimg = ancestor_out_v.at(plane); // ancestor ID (the primary geant4 track ID that origination particle cascade)
      auto const& meta = trackidimg.meta();

      // is the channel inside the meta window
      int col = wid.Wire;
      if (col < meta.min_x()) continue;
      if (meta.max_x() <= col) continue;
      if (plane != meta.plane()) continue;
      // remove offset to get position inside image
      col -= (int)(meta.min_x());

      // get the wire plane pixel container
      auto& planemap = planemaps_v.at(plane);

      // get the pixels for the current column (i.e. wire)
      auto it_colpixels = planemap.find( col );

      // if the container hasnt been made for the column, insert it into the map
      if ( it_colpixels==planemap.end() ) {
	// insert
	planemap[col] = ColumnPixels_t();
	planemap[col].pixels.clear();
	planemap[col].col = col;
	it_colpixels = planemap.find( col ); // have iterator point to new column pixel container
      }
      
      // loop over energy deposition
      for (auto const& tick_ides : sch.TDCIDEMap()) {
        int tick = supera::TPCTDC2Tick(clockData, (double)(tick_ides.first)) + time_offset; // true deposition tick
	if (tick <= meta.min_y()) continue;
	if (tick >= meta.max_y()) continue;
	
	// use the tick to get the pixel struct
	auto it_pixdata = it_colpixels->second.pixels.find( tick );
	double energy = 0.0; //< tracks the highest energy deposit

	// if the (tick,col) pixel not found, make it and then insert it
	if ( it_pixdata==it_colpixels->second.pixels.end() ) {
	  // first time at pixel
	  PixData_t pixdata;
	  pixdata.col = col;
	  pixdata.tick = tick;
	  pixdata.energy = 0.0;
	  it_colpixels->second.pixels[tick] = pixdata;
	  it_pixdata = it_colpixels->second.pixels.find( tick );
	}

	// get the current highest edep assigned to the pixel
	energy = it_pixdata->second.energy;
	
	
	// // Where is this tick in the image
	// int row   = (int)meta.row(tick); // compressed position
	// if ( row<0 || row>=(int)meta.rows() ) continue;
	
	// // now we loop over the energy depositions in this tick
	// double energy = (double)Eimg.pixel(row,col); // use to keep track the most energetic energy deposition at this point
	int ancestorid   = -1;
	
	// energy deposition at tick
	for (auto const& edep : tick_ides.second) {
	  
	  // for the Y-plane (ipass==0), we store the deposition with the highest energy
	  if (edep.energy < energy ) continue; // lower deposition than before
	  // if ( trackid2ancestorid.find( edep.trackID )==trackid2ancestorid.end() ) {
	  //   num_no_ancestor++;
	  //   noancestorids.insert( edep.trackID );
	  //   continue;
	  // }
	  
	  energy = edep.energy;

	  // use the trackid2ancestorid map to get ancestor ID
	  auto it_map = trackid2ancestorid.find( edep.trackID );

	  if ( it_map!=trackid2ancestorid.end() ) {
	    // found ID in ID -> ancestor map	  
	    ancestorid = it_map->second;
	    //ancestormap.set_pixel( row, col, ancestorid );
	  }

	  // save info to pixel object
	  it_pixdata->second.energy     = edep.energy;
	  it_pixdata->second.ancestorid = ancestorid;
	  it_pixdata->second.trackid    = edep.trackID;
	  
	  // regardless if we find ancestor, we store trackID
	  //imgmap1.set_pixel( row, col, edep.trackID ); // raw ID
	  // we have non-zero energy deposition, valid edep
	  //Eimg.set_pixel( row, col, energy );
	  
	}//end of loop over edep objects associated to a ticks
      }//loop over ticks
      
    }//loop over simchannel objects

    // std::cout << "Number of no ancestor pixels: " << num_no_ancestor << std::endl;
    // std::cout << "Number of no ancestor IDs: " << noancestorids.size() << std::endl;
    // std::cout << "   ";
    // for ( auto const& id : noancestorids ) {
    //   std::cout << id << ", ";
    // }
    // std::cout << std::endl;

    int compressed_pixels_filled = 0;

    // loop over list of ImageMeta that define the output images
    for ( auto const& meta : meta_v ) {

      int plane = meta.plane();
      int origin_y = (int)meta.min_y();
      auto& planemap = planemaps_v.at( plane );

      larcv::Image2D& img       = img_out_v[plane];       // track ID image
      larcv::Image2D& ancestor  = ancestor_out_v[plane];  // ancestor ID image
      
      for (int rout=0; rout<(int)meta.rows(); rout++) {
	for (int clout=0; clout<(int)meta.cols(); clout++) {
	  // find pixel with max energy deposit to to transfer
	  float enmax = 0;
	  int maxpix_trackid = -1;
	  int maxpix_ancestorid = -1;
	  for (int dc=0; dc<(int)col_compression_factor.at(plane); dc++) {

	    int c = clout*int(col_compression_factor.at(plane)) + dc;
	    
	    // get column of pixels
	    auto it_colmap = planemap.find( c );
	    if ( it_colmap==planemap.end() )
	      continue; // no such column in this plane
	    
	    for (int dr=0; dr<(int)row_compression_factor.at(plane); dr++) {
	      
	      // look for pixel using tick
	      int r = origin_y + rout *int(row_compression_factor.at(plane)) + dr;

	      auto it_pixel = it_colmap->second.pixels.find( r );
	      if ( it_pixel==it_colmap->second.pixels.end() )
		continue; // no such row in this column of pixels

	      float pixenergy = it_pixel->second.energy;

	      //std::cout << "pixel(" << plane << "," << r << "," << c << ") energy=" << pixenergy 
	      //          << " --> outpixel(" << rout << "," << clout << ", e=" << enmax << ")" << std::endl;
	      
	      if ( pixenergy < enmax )
		continue;

	      // set labels and update max energy
	      enmax = pixenergy;
	      maxpix_trackid    = it_pixel->second.trackid;
	      maxpix_ancestorid = it_pixel->second.ancestorid;
	      
	    }//loop over subrows
	  }//loop over subcols
	  
	  // set the output if we found a pixel following within the output pixel
	  if ( enmax>0 ) {
	    img.set_pixel( rout, clout, maxpix_trackid );
	    ancestor.set_pixel( rout, clout, maxpix_ancestorid );
	    compressed_pixels_filled++;
	  }
	}//end of downsampled column pixels
      }//end of downsampled row pixels
    }//end of loop over index
    
    

    // img_out_v.clear();
    // ancestor_out_v.clear();

    // if ( !fill_with_uncompressed_image ) {
    //   for ( auto& img : img_v ) {
    // 	img_out_v.emplace_back( std::move(img) );
    //   }
    //   for ( auto& a_img : ancestor_v ) {
    // 	ancestor_out_v.emplace_back( std::move(a_img) );
    //   }
    //   return;
    // }
    
    // // make output, compressed images
    // //std::vector<larcv::Image2D> img_out_v;
    // //std::vector<larcv::Image2D> ancestor_out_v;
    // for ( auto const& img : img_v ) {
    //   const larcv::ImageMeta& meta = img.meta();
    //   larcv::ImageMeta meta_out(meta.width(), meta.height(), 
    // 				int( meta.rows()/row_compression_factor.at(meta.plane()) ), 
    // 				int( meta.cols()/col_compression_factor.at(meta.plane()) ),
    // 				meta.min_x(), meta.min_y(), 
    // 				meta.plane() );
    //   LARCV_SINFO() << "output meta: " << std::endl;
    //   LARCV_SINFO() << meta_out.dump() << std::endl;
    //   larcv::Image2D img_out( meta_out );
    //   img_out.paint(-1.0);
    //   img_out_v.emplace_back( std::move(img_out) );
      
    //   larcv::Image2D ancestor_out( meta_out );
    //   ancestor_out.paint(-1.0);
    //   ancestor_out_v.emplace_back( std::move(ancestor_out) );
    // }
    
    // int compressed_pixels_filled = 0;
    // for (size_t iidx=0; iidx<img_out_v.size(); iidx++) {
    //   const larcv::Image2D& img       = img_v[iidx];
    //   const larcv::Image2D& ancestor  = ancestor_v[iidx];
    //   larcv::Image2D& imgout          = img_out_v[iidx];
    //   larcv::Image2D& ancestorout     = ancestor_out_v[iidx];
    //   size_t plane = img.meta().plane();
    //   const larcv::Image2D& energyimg = energy_v.at( plane );
      
    //   for (int rout=0; rout<(int)imgout.meta().rows(); rout++) {
    // 	for (int clout=0; clout<(int)imgout.meta().cols(); clout++) {
    // 	  // find max pixel to transfer
    // 	  int rmax = 0;
    // 	  int cmax = 0;
    // 	  float enmax = 0;
    // 	  for (int dr=0; dr<(int)row_compression_factor.at(plane); dr++) {
    // 	    for (int dc=0; dc<(int)col_compression_factor.at(plane); dc++) {
	      
    // 	      int r = rout *int(row_compression_factor.at(plane)) + dr;
    // 	      int c = clout*int(col_compression_factor.at(plane)) + dc;
	      
    // 	      // no label, we continue
    // 	      if ( img.pixel(r,c)<0 )
    // 		continue;
	      
    // 	      float pixenergy = energyimg.pixel( r, c );
    // 	      if ( pixenergy>enmax ) {
    // 		enmax = pixenergy;
    // 		rmax = r;
    // 		cmax = c;
    // 	      }
	      
    // 	    }
    // 	  }
	  
    // 	  // set the output
    // 	  if (enmax>0 ) {
    // 	    imgout.set_pixel( rout, clout, img.pixel( rmax, cmax ) );
    // 	    ancestorout.set_pixel( rout, clout, ancestor.pixel( rmax, cmax ) );
    // 	    compressed_pixels_filled++;
    // 	  }
    // 	}
    //   }
    // }//end of loop over index
    
    
    //std::cout << "compressed pixels filled: " << compressed_pixels_filled << std::endl;
    
  }
 


}
#endif
