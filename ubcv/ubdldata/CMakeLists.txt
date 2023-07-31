cet_make_library(
  SOURCE
  pixeldata.cxx
  LIBRARIES
  PUBLIC
)

art_dictionary(
  DICTIONARY_LIBRARIES
  ubcv::ubdldata
)

install_headers()
install_source()
