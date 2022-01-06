
art_make( BASENAME_ONLY
	  LIBRARY_NAME ubcv_ubdldata
	  LIB_LIBRARIES
		   art::Framework_Core
		   art::Framework_Principal
		   art::Framework_Services_Registry
		   
                   art::Persistency_Common
                   art::Persistency_Provenance
		   art::Utilities canvas
		   messagefacility::MF_MessageLogger
		   
		   fhiclcpp::fhiclcpp
		   cetlib::cetlib
                   cetlib_except::cetlib_except )


install_headers()
install_source()
