#ifndef __SUPERA_LARFLOW_LAR2IMAGE_CXX__
#define __SUPERA_LARFLOW_LAR2IMAGE_CXX__

#include "LArFlowLAr2Image.h"
#include "Base/larcv_logger.h"
#include "LArUtil/Geometry.h"

#include "larevt/SpaceChargeServices/SpaceChargeService.h" 
#include "lardata/DetectorInfoServices/DetectorClocksService.h" 
#include "lardata/DetectorInfoServices/DetectorPropertiesService.h" 

namespace supera {

  //
  // SimChannel => PixelFlowMaps
  // 
  std::vector<larcv::Image2D>
    SimCh2LArFlowImages( const std::vector<larcv::ImageMeta>& meta_v,
			 const std::vector<larcv::ROIType_t>& track2type_v,
			 const std::vector<supera::LArSimCh_t>& sch_v,
			 const larcv::EventChStatus& ev_chstatus,
			 const std::vector<float>& row_compression_factor,
			 const std::vector<float>& col_compression_factor,			 
			 const int time_offset, const bool edep_at_anode, const bool tick_backward ) {
    
    LARCV_SINFO() << "Filling Pixel-flow truth image... (with time_offset=" << time_offset << ")" << std::endl;

    // flow enum
    enum { kU2V=0, kU2Y, kV2U, kV2Y, kY2U, kY2V };
    const larutil::Geometry& geo = *(larutil::Geometry::GetME());    
    
    // we create for each plane:
    //  2 images that list column in other images
    //  2 images that give the probability that pixel is visible

    std::vector<larcv::Image2D> flow_v;     // flow values per images
    std::vector<larcv::Image2D> energy_v;  // energy deposition of largest particle

    for ( auto const& meta : meta_v ) {
      //LARCV_SINFO() << meta.dump();      
      larcv::Image2D flow1(meta);
      flow1.paint(-4000);
      flow_v.emplace_back( std::move(flow1) );
      larcv::Image2D flow2(meta);
      flow2.paint(-4000);
      flow_v.emplace_back( std::move(flow2) );
      larcv::Image2D energyimg(meta);
      energyimg.paint(0.0);
      energy_v.emplace_back( std::move(energyimg) );
    }
    //std::cout << "Pixel Flow vector: " << img_v.size() << std::endl;
    //std::cout << " Filling Image shape: (row,col)=(" << meta_v.front().rows() << "," << meta_v.front().cols() << ")" << std::endl;

    // in order to handle overlapping tracks, we have to set a rule to avoid ambiguity
    // we first fill out the y-plane. this plane is used to resolve duplicates

    int numpixfilled_pass0 = 0;
    int numpixfilled_pass1 = 0;
    for (int ipass=0; ipass<2; ipass++) {
      // first pass: Y-only. This is to provide guidance on how to break ambiguities
      // second pass: U,V planes
    
      for (auto const& sch : sch_v) {
	auto ch = sch.Channel();
	auto const& wid   = ::supera::ChannelToWireID(ch);
	auto const& plane = wid.Plane;

	if ( ipass==0 && plane!=2 )
	  continue; // first pass must be Y-plane
	else if ( ipass==1 && plane==2 )
	  continue;
      
	auto& flowmap1    = flow_v.at(2*plane+0);
	auto& flowmap2    = flow_v.at(2*plane+1);
	auto& energyimg  = energy_v.at(plane);
      
	auto const& meta = flowmap1.meta();

	// is the channel inside the meta window
	size_t col = wid.Wire;
	if (col < meta.min_x()) continue;
	if (meta.max_x() <= col) continue;
	if (plane != meta.plane()) continue;

	// remove offset to get position inside image
	col -= (size_t)(meta.min_x());

	for (auto const tick_ides : sch.TDCIDEMap()) {
	  int tick = supera::TPCTDC2Tick((double)(tick_ides.first)) + time_offset; // true deposition tick

	  if (tick <= meta.min_y()) continue;
	  if (tick >= meta.max_y()) continue;

	  // Where is this tick in column vector?
	  int row   = (int)meta.row(tick); // compressed position
	  if ( row<0 || row>=(int)meta.rows() ) continue;

	  // now we loop over the energy depositions in this tick
	  double energy = (double)energyimg.pixel(row,col); // use to keep track the most energetic energy deposition at this point

	  std::vector<double> pos3d(3);
	  ::larcv::ROIType_t roi_type =::larcv::kROIBNB;

	  std::vector<int> imgcoords(4,-1);	  
	  for (auto const& edep : tick_ides.second) {

	    if ( edep.energy<energy ) continue;

	    energy = edep.energy;
	    //trackid = edep.trackID;

	    // we skip filling this position if Y-plane (pass=0) already filled this information during pass=0
	    // if ( ipass==1 ) {
	    //   if ( plane==0 && flow_v.at( kU2Y ).pixel( row, col )>-4000 )
	    // 	continue;
	    //   if ( plane==1 && flow_v.at( kV2Y ).pixel( row, col )>-4000 )
	    // 	continue;
	    // }

	    // set img coords
	    // SCE position
	    double y,z;
	    y = edep.y;
	    z = edep.z;
	    //LARCV_SINFO() << "edep (y,z)=" << "(" << y << "," << z << ") tick=" << tick << " ides=" << tick_ides.first << std::endl;
	    
	    if ( edep_at_anode ) {
	      // the case for wirecell simch
	      pos3d[0] = 150;
	    }
	    else {
	      // old simch
	      double x = edep.x;
	      supera::ApplySCE(x,y,z);
	      pos3d[0] = x;
	    }

	    pos3d[1] = y;
	    pos3d[2] = z;
	    imgcoords[0] = row;		    
	    for (int p=0; p<3; p++) {
	      imgcoords[p+1] = (int)(geo.WireCoordinate( pos3d, p )+0.5);
	    }
	    //LARCV_SINFO() << "   p=" << plane << " col=" << col
	    //<< " wire=(" << imgcoords[1] << "," << imgcoords[2]<< "," << imgcoords[3] << ")" << std::endl;

	    //  PID
	    //roi_type = (::larcv::ROIType_t)temp_roi_type;
	  }//edep loop

	  if ( roi_type!=larcv::kROIUnknown && energy>0) {
	    // we have non-zero energy deposition, valid edep
	  
	    energyimg.set_pixel( row, col, energy );
	    //trackidimg.set_pixel( row, col, trackid );
	    
	    switch( plane ) {
	    case 0:
	      flowmap1.set_pixel( row, col, (float)imgcoords[1+1]-(float)imgcoords[0+1] ); //U->V
	      flowmap2.set_pixel( row, col, (float)imgcoords[2+1]-(float)imgcoords[0+1] ); //U->Y
	      break;
	    case 1:
	      flowmap1.set_pixel( row, col, (float)imgcoords[0+1]-(float)imgcoords[1+1] );
	      flowmap2.set_pixel( row, col, (float)imgcoords[2+1]-(float)imgcoords[1+1] );
	      break;
	    case 2:
	      // when filling the Y-plane, we set the other planes as well to keep everything consistent
	      flowmap1.set_pixel( row, col, (float)imgcoords[0+1]-(float)imgcoords[2+1] ); // Y->U
	      flowmap2.set_pixel( row, col, (float)imgcoords[1+1]-(float)imgcoords[2+1] ); // Y->V
	      // flow_v.at( kU2Y ).set_pixel( row, imgcoords[0+1], (float)imgcoords[2+1]-(float)imgcoords[0+1] ); // U->Y
	      // flow_v.at( kV2Y ).set_pixel( row, imgcoords[1+1], (float)imgcoords[2+1]-(float)imgcoords[1+1] ); // V->Y
	      // flow_v.at( kU2V ).set_pixel( row, imgcoords[0+1], (float)imgcoords[1+1]-(float)imgcoords[0+1] ); // U->V
	      // flow_v.at( kV2U ).set_pixel( row, imgcoords[1+1], (float)imgcoords[0+1]-(float)imgcoords[1+1] ); // V->U
	      // energy_v.at(0).set_pixel( row, imgcoords[0+1], energy );
	      // energy_v.at(1).set_pixel( row, imgcoords[1+1], energy );
	      break;
	    }
	    if (ipass==0)
	      numpixfilled_pass0++;
	    else
	      numpixfilled_pass1++;
	  }//if parti
	  
	  //column[index] = roi_type;
	}
      }
      //break;
    }//end of pass loop
    
    
    LARCV_SINFO() << "LArFlowLAr2Image: num pixels filled pass0=" << numpixfilled_pass0 << " pass1=" << numpixfilled_pass1 << std::endl;

    // max pool compression, we do it ourselves
    //std::cout << "Max pool ourselves: compression factors (row,col)=(" << row_compression_factor.front() << "," << col_compression_factor.front() << ")" << std::endl;
    
    // make output, compressed images
    std::vector<larcv::Image2D> img_out_v;
    std::vector<larcv::Image2D> img_vis_v;
    for ( auto const& img : flow_v ) {
      const larcv::ImageMeta& meta = img.meta();
      float origin_y = meta.min_y();
      if ( tick_backward )
	origin_y = meta.max_y();
      larcv::ImageMeta meta_out(meta.width(), meta.height(), 
				int( meta.rows()/row_compression_factor.at(meta.plane()) ), int( meta.cols()/col_compression_factor.at(meta.plane()) ),
				meta.min_x(), origin_y,
				meta.plane() );
      larcv::Image2D img_out( meta_out );
      img_out.paint(0.0);
      img_out_v.emplace_back( std::move(img_out) );
      larcv::Image2D vis_out( meta_out );
      vis_out.paint(0.0);
      img_vis_v.emplace_back( std::move(vis_out) );
    }

    int compressed_pixels_filled = 0;
    for (size_t iidx=0; iidx<img_out_v.size(); iidx++) {
      const larcv::Image2D& flowimg   = flow_v[iidx];
      larcv::Image2D& imgout          = img_out_v[iidx];
      size_t plane                    = flowimg.meta().plane();
      const larcv::Image2D& energyimg = energy_v.at( plane );

      //std::cout << "outimg (row,col)=" << imgout.meta().rows() << "," << imgout.meta().cols() << ")" << std::endl;

      for (int rout=0; rout<(int)imgout.meta().rows(); rout++) {
	for (int clout=0; clout<(int)imgout.meta().cols(); clout++) {
	  // find max pixel to transfer
	  int rmax = 0;
	  int cmax = 0;
	  float enmax = 0;
	  for (int dr=0; dr<(int)row_compression_factor.at(plane); dr++) {
	    for (int dc=0; dc<(int)col_compression_factor.at(plane); dc++) {
	      
	      int r = rout *int(row_compression_factor.at(plane)) + dr;
	      int c = clout*int(col_compression_factor.at(plane)) + dc;

	      float pixenergy = energyimg.pixel( r, c );
	      float flow      = flowimg.pixel(r,c);

	      if ( pixenergy>enmax && flow>-4000) {
		enmax = pixenergy;
		rmax = r;
		cmax = c;
	      }

	    }
	  }

	  // set the output
	  if (enmax>0 ) {
	    imgout.set_pixel( rout, clout, flowimg.pixel( rmax, cmax ) );
	    compressed_pixels_filled++;
	  }
	}
      }
    }//end of loop over index


    //std::cout << "compressed pixels filled: " << compressed_pixels_filled << std::endl;

    //std::cout << "return image" << std::endl;

    return img_out_v;
  }

  //
  // SimEnergyDeposit => PixelFlowMaps
  // 
  std::vector<larcv::Image2D>
  SimEnergyDeposit2LArFlowImages( const std::vector<larcv::ImageMeta>& meta_v,
				  const std::vector<supera::LArSimEdep_t>& sed_v,
				  const std::vector<supera::LArMCTruth_t>& mctruth_v,
				  const std::vector<larcv::Image2D>& adc_v,
				  const larcv::EventChStatus& ev_chstatus,
				  const std::vector<float>& row_compression_factor,
				  const std::vector<float>& col_compression_factor,			 
				  const int time_offset, const bool edep_at_anode, 
				  const bool tick_backward ) {

    std::cout << "SimEnergyDeposit2LArFlowImages: start" << std::endl;
    LARCV_SINFO() << "Filling Pixel-flow truth image..." << std::endl;
    
    // flow enum
    enum { kU2V=0, kU2Y, kV2U, kV2Y, kY2U, kY2V };
    int target_plane[6] = { 1, 2, 0, 2, 0, 1 };
    //const larutil::Geometry& geo = *(larutil::Geometry::GetME());    
    art::ServiceHandle<geo::Geometry> geom;
    //const float driftvelocity = 0.1098; // hand-coded for current mcc9-beta velocity
    const float driftvelocity = supera::DriftVelocity();

    // sce module
    //auto const* SCE = lar::providerFrom<spacecharge::SpaceChargeService>();

    // Get time offset for x space charge correction
    auto const& detProperties = lar::providerFrom<detinfo::DetectorPropertiesService>();
    auto const& detClocks = lar::providerFrom<detinfo::DetectorClocksService>();
    auto const& gen = mctruth_v.at(0);
    double det_xtickoffset = detProperties->GetXTicksOffset(0,0,0);
    double det_trigoffset  = detProperties->TriggerOffset();
    double g4Ticks  = 0;
    if ( gen.NeutrinoSet() )
      g4Ticks = detClocks->TPCG4Time2Tick(gen.GetNeutrino().Nu().T());
    else
      g4Ticks = detClocks->TPCG4Time2Tick(gen.GetParticle(0).T());
    g4Ticks += det_xtickoffset;
    g4Ticks -= det_trigoffset;
    double xtimeoffset = detProperties->ConvertTicksToX(g4Ticks,0,0,0);
    
    // we create for each plane:
    //  2 images that list column in other images
    //  2 images that give the probability that pixel is visible

    // to do:
    // we need a charge map to be made as well
    // need above to do compression itself

    std::vector<larcv::Image2D> flow_v;    // flow images
    std::vector<larcv::Image2D> energy_v;  // energy deposition of largest particle

    //std::vector<larcv::Image2D> trackid_v; // track id of particle leaving deposition
    for ( auto const& meta : meta_v ) {

      // the two flow directions
      LARCV_SINFO() << meta.dump();      
      larcv::Image2D img1(meta);
      img1.paint(0.0);
      flow_v.emplace_back( std::move(img1) );
      larcv::Image2D img2(meta);
      img2.paint(0.0);
      flow_v.emplace_back( std::move(img2) );

      // max energy deposition in bin
      larcv::Image2D energyimg(meta);
      energyimg.paint(-1.0);
      energy_v.emplace_back( std::move(energyimg) );
    }
    //std::cout << "SimEdep: Pixel Flow vector, size=" << flow_v.size() << std::endl;
    //std::cout << "SimEdep: Filling Image shape (row,col)=(" << meta_v.front().rows() << "," << meta_v.front().cols() << ")" << std::endl;

    int edeps_wadc[3]  = {0};
    int edeps_noadc[3] = {0};
    int edeps_indeadch[3] = {0};
    int edeps_outoftime = 0;
    for ( auto const& sed : sed_v ) {
      
      // David Caratelli:
      // much of the code below is taken from the module:
      // https://cdcvs.fnal.gov/redmine/projects/larsim/repository/revisions/develop/entry/larsim/ElectronDrift/SimDriftElectrons_module.cc
      
      // given this SimEnergyDeposit, find the TPC channel information
      // "xyz" is the position of the energy deposit in world
      // coordinates. Note that the units of distance in
      // sim::SimEnergyDeposit are supposed to be cm.
      auto const mp = sed.MidPoint();
      double const xyz[3] = { mp.X(), mp.Y(), mp.Z() };

      //auto sce_offset = SCE->GetPosOffsets(geo::Point_t(xyz[0], xyz[1], xyz[2]));

      double xyz_sce[3] = {0};
      // proper sce correction
      // xyz_sce[0] = (xyz[0] - sce_offset.X() + xtimeoffset)*(1.114/1.098) + 0.6;
      // xyz_sce[1] = xyz[1] + sce_offset.Y();
      // xyz_sce[2] = xyz[2] + sce_offset.Z();

      // reverse sce correction
      // xyz_sce[0] = (xyz[0] + sce_offset.X() + xtimeoffset)*(1.114/1.098) - 0.6;
      // xyz_sce[1] = xyz[1] - sce_offset.Y();
      // xyz_sce[2] = xyz[2] - sce_offset.Z();
      
      // just xtimeoffset
      xyz_sce[0] = (xyz[0]+xtimeoffset)*(1.114/1.098);
      xyz_sce[1] = xyz[1];
      xyz_sce[2] = xyz[2];

      double tedep = sed.Time(); // (function returns midpoint time)
      double tick  = supera::TPCG4Time2Tick( tedep ) + time_offset + xyz_sce[0]/driftvelocity/0.5;

      //std::cout << "sed: (" << xyz[0] << "," << xyz[1] << "," << xyz[2] << ") t=" << tedep << " tick=" << tick << std::endl;

      if ( tick <= meta_v.front().min_y() || tick>= meta_v.front().max_y() ) {
	//std::cout << "skipped sed: (" << xyz[0] << "," << xyz[1] << "," << xyz[2] << ") t=" << tedep << " tick=" << tick << std::endl;
	edeps_outoftime++;
	continue;
      }

      unsigned int row = meta_v.front().row( tick );
      //int track_id = sed.TrackID();
      //int pdg      = sed.PdgCode();


      // From the position in world coordinates, determine the
      // cryostat and tpc. If somehow the step is outside a tpc
      // (e.g., cosmic rays in rock) just move on to the next one.
      unsigned int cryostat = 0;
      try {
	geom->PositionToCryostat(xyz_sce, cryostat);
      }
      catch(cet::exception &e){
	LARCV_SERROR()  << "step "// << energyDeposit << "\n"
			<< "cannot be found in a cryostat\n"
			<< e;
	continue;
      }
      unsigned int tpc = 0;
      try {
	geom->PositionToTPC(xyz_sce, tpc, cryostat);
      }
      catch(cet::exception &e){
	LARCV_SWARNING()  << "step "// << energyDeposit << "\n"
			  << "cannot be found in a TPC\n"
			  << e;
	continue;
      }
      //const geo::TPCGeo& tpcGeo = geom->TPC(tpc, cryostat);
      
      //Define charge drift direction: driftcoordinate (x, y or z) and driftsign (positive or negative). Also define coordinates perpendicular to drift direction.
      // unused int driftcoordinate = std::abs(tpcGeo.DetectDriftDirection())-1;  //x:0, y:1, z:2
        
      // mark the planes
      for(size_t p = 0; p < 3; p++){

	// grab the nearest channel to the fDriftClusterPos position
	// David Caratelli, comment begin:
	// NOTE: the below code works only when the drift coordinate is indeed in x (i.e. 0th coordinate)
	// see code linked above for a much more careful treatment of the coordinate system
	// David Caratelli, comment end.
	float wire = supera::NearestWire( xyz_sce, p );
	int col    = energy_v[p].meta().col( wire );

	// get adc value
	float adcval = adc_v[p].pixel( adc_v[p].meta().row( tick ), adc_v[p].meta().col( wire ) );

	// get channel status
	int chstat = ev_chstatus.Status( p ).Status( (size_t)wire );
	if ( chstat>=4 ) {
	  if ( adcval>1.0 )
	    edeps_wadc[p]++;
	  else
	    edeps_noadc[p]++;
	}
	else {
	  edeps_indeadch[p]++;
	}

	// check past energy dep in corresponding pixel
	float pastedep = energy_v[p].pixel( row, col );
	if ( pastedep < sed.Energy() ) {
	  // we update this position
	  energy_v[p].set_pixel( row, col, sed.Energy() );
      
	  // get wires for each plane
	  std::vector<int> colset(3,0);
	  for(size_t pp = 0; pp < 3; pp++ ) {
	    float w = supera::NearestWire( xyz_sce, pp );
	    colset[pp] = (int)energy_v[pp].meta().col(w);
	  }

	  switch ( p ) {
	  case 0:
	    flow_v[kU2V].set_pixel( row, colset[0], colset[1]-colset[0] );
	    flow_v[kU2Y].set_pixel( row, colset[0], colset[2]-colset[0] );
	    break;
	  case 1:
	    flow_v[kV2U].set_pixel( row, colset[1], colset[0]-colset[1] );
	    flow_v[kV2Y].set_pixel( row, colset[1], colset[2]-colset[1] );
	    break;
	  case 2:
	    flow_v[kY2U].set_pixel( row, colset[2], colset[0]-colset[2] );
	    flow_v[kY2V].set_pixel( row, colset[2], colset[1]-colset[2] );
	    break;
	  default:
	    throw std::runtime_error("LArFlowLAr2Image: flow not defined");
	    break;
	  }//end of plane switch
	}//end of if edep max
      }//end of plane loop
    }// end looping over SimEnergyDeposits

    std::cout << "Edeps with ADC:    " << edeps_wadc[0] << " " << edeps_wadc[1] << " " << edeps_wadc[2] << std::endl;
    std::cout << "Edeps no ADC:      " << edeps_noadc[0] << " " << edeps_noadc[1] << " " << edeps_noadc[2] << std::endl;
    std::cout << "Edeps in deadchs:  " << edeps_indeadch[0] << " " << edeps_indeadch[1] << " " << edeps_indeadch[2] << std::endl;
    std::cout << "Edeps out of time: " << edeps_outoftime << std::endl;

    bool docompression = false;
    for ( size_t p=0; p<3; p++ ) {
      if ( row_compression_factor.at(p)!=1  || col_compression_factor.at(p)!=1 ) docompression = true;
    }

    if ( !docompression )
      return flow_v;

    // max pool compression, we do it ourselves
    //std::cout << "Max pool ourselves: compression factors (row,col)=(" << row_compression_factor.front() << "," << col_compression_factor.front() << ")" << std::endl;

    // make compressed energy images
    // std::vector<larcv::Image2D> comp_eng_v;
    // for ( auto const& eng : energy_v ) {
    //   larcv::Image2D comp_eng( eng );
    //   comp_eng.compress( int( eng.meta().rows()/row_compression_factor.at(eng.meta().plane()) ),
    // 			 int( eng.meta().cols()/col_compression_factor.at(eng.meta().plane()) ) );
    //   comp_eng_v.emplace_back( std::move(comp_eng) );
    // }
    
    // make output, compressed images
    std::vector<larcv::Image2D> comp_out_v;
    for ( auto const& flowimg : flow_v ) {
      const larcv::ImageMeta& meta = flowimg.meta();
      float origin_y = meta.min_y();
      if ( tick_backward )
	origin_y = meta.max_y();
      larcv::ImageMeta meta_out(meta.width(), meta.height(), 
				int( meta.rows()/row_compression_factor.at(meta.plane()) ), int( meta.cols()/col_compression_factor.at(meta.plane()) ),
				meta.min_x(), origin_y,
				meta.plane() );
      larcv::Image2D img_out( meta_out );
      img_out.paint(0.0);
      comp_out_v.emplace_back( std::move(img_out) );
    }
    
    int compressed_pixels_filled = 0;
    for (size_t iidx=0; iidx<comp_out_v.size(); iidx++) {
      const larcv::Image2D& flow      = flow_v[iidx];     // uncompressed flow
      larcv::Image2D& imgout          = comp_out_v[iidx]; // compressed flow image
      size_t plane = flow.meta().plane();
      const larcv::Image2D& energyimg = energy_v.at( plane );
      //const larcv::Image2D& compressed_energy = comp_eng_v.at( target_plane[iidx] );
      const larcv::Image2D& targetadc = adc_v.at( target_plane[iidx] );
      
      //std::cout << "outimg (row,col)=" << imgout.meta().rows() << "," << imgout.meta().cols() << ")" << std::endl;
      
      for (int rout=0; rout<(int)imgout.meta().rows(); rout++) {
	float tickout = imgout.meta().pos_y(rout);
	for (int clout=0; clout<(int)imgout.meta().cols(); clout++) {
	  //float wireout = imgout.meta().pos_x(clout);
	  // find max pixel to transfer
	  // no adc constraint
	  int rmax = 0;
	  int cmax = 0;
	  float enmax = 0;
	  // w/ adc constraint
	  int rmax_wadc = 0;
	  int cmax_wadc = 0;
	  float enmax_wadc = 0;

	  // loop over pre-compressed ticks inside the compressed pixel
	  for (int dr=0; dr<(int)row_compression_factor.at(plane); dr++) {
	    int r = rout *int(row_compression_factor.at(plane)) + dr;

	    for (int dc=0; dc<(int)col_compression_factor.at(plane); dc++) {
	      int c = clout*int(col_compression_factor.at(plane)) + dc;
	      
	      float pixenergy = energyimg.pixel( r, c );
	      float flowout   = flow.pixel( r, c );
	      int   flowcol   = c+flowout;
	      float adcval    = targetadc.pixel( targetadc.meta().row( tickout ), 
						 targetadc.meta().col( flowcol ) );
	      
	      //float flowadc   = compressed_energy.pixel( rout, (int)(c+flowout) ); // this is what the target would look like

	      if ( pixenergy>enmax ) {
		enmax = pixenergy;
		rmax = r;
		cmax = c;
	      }
	      if ( adcval>0.0 && pixenergy>enmax_wadc ) {
		enmax_wadc = pixenergy;
		rmax_wadc  = r;
		cmax_wadc  = c;
	      }
	    }
	  }
	  
	  // set the output: prefer answers that flow to a final pixel with ADC values
	  if ( enmax_wadc>0 )  {
	    imgout.set_pixel( rout, clout, flow.pixel( rmax_wadc, cmax_wadc ) );
	    compressed_pixels_filled++;
	  }
	  else if (enmax>0 ) {
	    imgout.set_pixel( rout, clout, flow.pixel( rmax, cmax ) );
	    compressed_pixels_filled++;
	  }
	}
      }
    }//end of loop over index
    
    
    //LARCV_SINFO() << "compressed pixels filled: " << compressed_pixels_filled << std::endl;
    LARCV_SINFO() << "compressed pixels filled: " << compressed_pixels_filled << std::endl;
    
    //std::cout << "return image" << std::endl;

    return comp_out_v;
  }
 


}
#endif
