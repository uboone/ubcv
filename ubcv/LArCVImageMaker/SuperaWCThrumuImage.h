/**
 * \file SuperaWCThrumuImage.h
 *
 * \ingroup LArCVImageMaker
 * 
 * \brief Class def header for a class SuperaWCThrumuImage
 *
 * @author Taritree
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __UBCV_LARCVIMAGEMAKER_SUPERA_WC_THRUMU_IMAGE_H__
#define __UBCV_LARCVIMAGEMAKER_SUPERA_WC_THRUMU_IMAGE_H__

#include "SuperaBase.h"
#include "FMWKInterface.h"
#include "ParamsImage2D.h"
#include "ImageMetaMaker.h"

namespace larcv {

  /**
     \class ProcessBase
     User defined class SuperaWire ... these comments are used to generate
     doxygen documentation!
  */
  class SuperaWCThrumuImage : public SuperaBase,
    public supera::ImageMetaMaker {

  public:
    
    /// Default constructor
    SuperaWCThrumuImage(const std::string name="SuperaWCThrumuImage");
    
    /// Default destructor
    ~SuperaWCThrumuImage(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  protected:

    std::string _output_image2d_treename;
    std::string _input_wireimage_treename;
    
  };

  /**
     \class larcv::SuperaWCThrumuImageFactory
     \brief A concrete factory class for larcv::SuperaWCThrumuImage
  */
  class SuperaWCThrumuImageProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    SuperaWCThrumuImageProcessFactory() { ProcessFactory::get().add_factory("SuperaWCThrumuImage",this); }
    /// dtor
    ~SuperaWCThrumuImageProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new SuperaWCThrumuImage(instance_name); }
  };

}
#endif
//#endif
//#endif
/** @} */ // end of doxygen group 

