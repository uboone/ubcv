////////////////////////////////////////////////////////////////////////
// Class:       LArPIDInterface
// Plugin Type: analyzer (art v3_01_02)
// File:        LArPIDInterface_module.cc
//
// Generated at Wed Jan 22 08:22:00 2025 by Matthew Rosenberg using cetskelgen
// from cetlib version v3_05_01.
////////////////////////////////////////////////////////////////////////

#include <Python.h> // Python/C API header

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "ubreco/WcpPortedReco/ProducePort/SpacePointStructs.h"
#include "nusimdata/SimulationBase/MCParticle.h"

#include "larcv/core/DataFormat/IOManager.h"
#include "larcv/core/DataFormat/EventImage2D.h"
#include "LArUtil/GeometryHelper.h"
#include "DataFormat/larflowcluster.h"
#include "DataFormat/larflow3dhit.h"

#include <unordered_map>
#include <cmath>

class LArPIDInterface;

class LArPIDInterface : public art::EDAnalyzer {
public:
  explicit LArPIDInterface(fhicl::ParameterSet const& p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  LArPIDInterface(LArPIDInterface const&) = delete;
  LArPIDInterface(LArPIDInterface&&) = delete;
  LArPIDInterface& operator=(LArPIDInterface const&) = delete;
  LArPIDInterface& operator=(LArPIDInterface&&) = delete;

  // Required functions.
  void analyze(art::Event const& e) override;

  void beginJob() override;

private:

  // Declare member data here.
  std::string fLArCVImageFile;
  std::string fPythonScript;
  unsigned int iImg_start;

  struct CropPixData_t {
    int row; ///< row of pixel in cropped image
    int col; ///< col of pixel in cropped image
    int rawRow; ///< row of pixel in original image
    int rawCol; ///< col of pixel in original image
    float val; ///< value of pixel
    int idx;   ///< index in container

    CropPixData_t()
    : row(0),col(0),rawRow(0),rawCol(0),val(0.0),idx(0)
    {};

    /** @brief constructor with row, col, value 
     *  @param[in] r row of pixel
     *  @param[in] c col of pixel
     *  @param[in] v value of pixel
     */
    CropPixData_t( int r, int c, int rr, int rc, float v)
    : row(r),col(c),rawRow(rr),rawCol(rc),val(v),idx(0) {};

    /** @brief equality operator based on row, col, and value */
    bool operator==( const CropPixData_t& rhs ) const {
      if(rawRow == rhs.rawRow && rawCol == rhs.rawCol && val == rhs.val) return true;
      return false;
    };

    /** @brief comparator based on row then col then value */
    bool operator<( const CropPixData_t& rhs ) const {
      if (row<rhs.row) return true;
      if ( row==rhs.row ) {
        if ( col<rhs.col ) return true;
        if ( col==rhs.col ) {
          if ( val<rhs.val ) return true;
        }
      }
      return false;
    };
  };

  struct ParticleInfo {
    TVector3 cropPoint;
    larlite::larflowcluster larflowProng;
    ParticleInfo() {}
    ParticleInfo(TVector3 pt) : cropPoint(pt) {}
  };

  static std::vector< std::vector<LArPIDInterface::CropPixData_t> >
    make_cropped_initial_sparse_prong_image_reco( const std::vector<larcv::Image2D>& adc_v,
                                                  const std::vector<larcv::Image2D>& thrumu_v,
                                                  const larlite::larflowcluster& prong,
                                                  const TVector3& cropCenter,
                                                  float threshold, int rowSpan, int colSpan );

  static void getRecoImageBounds( std::vector< std::vector<int> >& imgBounds,
                             const std::vector<larcv::Image2D>& adc_v,
                             const larlite::larflowcluster& prong,
                             const TVector3& cropCenter, int rowSpan, int colSpan );

  static void fillProngImagesFromReco(std::vector< std::vector<LArPIDInterface::CropPixData_t> >& sparseimg_vv,
                                 const float& threshold,
                                 const std::vector<larcv::Image2D>& adc_v,
                                 const std::vector<larcv::Image2D>& thrumu_v,
                                 const larlite::larflowcluster& prong,
                                 const std::vector< std::vector<int> >& imgBounds);

  static void fillContextImages(std::vector< std::vector<LArPIDInterface::CropPixData_t> >& sparseimg_vv,
                           const float& threshold,
                           const std::vector<larcv::Image2D>& adc_v,
                           const std::vector<larcv::Image2D>& thrumu_v,
                           const std::vector< std::vector<int> >& imgBounds);

};


void LArPIDInterface::beginJob() {
  // Initialize Python interpreter
  Py_Initialize();
  iImg_start = 0;
}


LArPIDInterface::LArPIDInterface(fhicl::ParameterSet const& p)
  : EDAnalyzer{p},
     fLArCVImageFile(p.get<std::string>("LArCVImageFile")),
     fPythonScript(p.get<std::string>("PythonScript"))
  // More initializers here.
{
  // Call appropriate consumes<>() for any products to be retrieved by this module.
}

void LArPIDInterface::analyze(art::Event const& e)
{

  std::cout << "NEW EVENT: run "<<e.id().run()<<", subrun "<<e.id().subRun()<<", event "<<e.id().event() << std::endl;

  larcv::IOManager* iolcv = new larcv::IOManager(larcv::IOManager::kREAD,"larcv",larcv::IOManager::kTickBackward);
  iolcv -> reverse_all_products();
  iolcv -> add_in_file(fLArCVImageFile);
  iolcv -> initialize();

  larcv::EventImage2D* wireImage2D = nullptr;
  larcv::EventImage2D* cosmicImage2D = nullptr;
  for (unsigned int iImg = iImg_start; iImg < iolcv->get_n_entries(); ++iImg){
    iolcv -> read_entry(iImg);
    larcv::EventImage2D* wireImg = (larcv::EventImage2D*)(iolcv->get_data(larcv::kProductImage2D,"wire"));
    if(wireImg->run() == e.id().run() && wireImg->subrun() == e.id().subRun() && wireImg->event() == e.id().event()){
      wireImage2D = wireImg;
      cosmicImage2D = (larcv::EventImage2D*)(iolcv->get_data(larcv::kProductImage2D,"thrumu"));
      iImg_start = iImg+1;
      break;
    }
  }

  if(wireImage2D == nullptr || cosmicImage2D == nullptr){
    std::cout << "MATCHING EVENT FROM LARCV IMAGE FILE NOT FOUND!!! Skipping event..." << std::endl;
    return;
  }
  else{
    std::cout << "Found matching event from larcv image file. We have the images!" << std::endl;
  }

  std::vector<larcv::Image2D> adc_v = wireImage2D->Image2DArray();
  std::vector<larcv::Image2D> thrumu_v = cosmicImage2D->Image2DArray();

  std::unordered_map<int, ParticleInfo> particleMap;

  auto const& particles = *e.getValidHandle<std::vector<simb::MCParticle>>("wirecellPF");
  for (auto const& particle : particles) {
    std::cout << "reco particle: "
              << "trackID = " << particle.TrackId() << ", "
              << "pdg = " << particle.PdgCode() << ", "
              << "start position = ("<<particle.Vx()<<","<<particle.Vy()<<","<<particle.Vz()<<"), "
              << "end position = ("<<particle.EndX()<<","<<particle.EndY()<<","<<particle.EndZ()<<")"
              << std::endl;
    if(std::abs(particle.PdgCode()) == 11 || std::abs(particle.PdgCode()) == 22){
      TVector3 crop(particle.Vx(), particle.Vy(), particle.Vz());
      ParticleInfo newParticle(crop);
      particleMap[particle.TrackId()] = newParticle;
    }
    else{
      TVector3 crop(particle.EndX(), particle.EndY(), particle.EndZ());
      ParticleInfo newParticle(crop);
      particleMap[particle.TrackId()] = newParticle;
    }
  }

  auto const& spacePoints = *e.getValidHandle<std::vector<TrecSpacePoint>>("portedWCSpacePointsTrec");
  for (auto const& point : spacePoints) {
    if(particleMap.count(point.real_cluster_id) < 1) continue;
    larlite::larflow3dhit hit;
    for(unsigned int p = 0; p < adc_v.size(); ++p){
      auto center2D = larutil::GeometryHelper::GetME()->Point_3Dto2D(point.x,point.y,point.z,p);
      if(p == 0) hit.tick = (int)(center2D.t/larutil::GeometryHelper::GetME()->TimeToCm() + 3200.);
      if(p < hit.targetwire.size()) hit.targetwire[p] = (int)(center2D.w/larutil::GeometryHelper::GetME()->WireToCm());
      else hit.targetwire.push_back((int)(center2D.w/larutil::GeometryHelper::GetME()->WireToCm()));
    }
    particleMap[point.real_cluster_id].larflowProng.push_back(hit);
    /*std::cout << "TrecSpacePoint: "
              << "x = " << point.x << ", "
              << "y = " << point.y << ", "
              << "z = " << point.z << ", "
              << "real_cluster_id = " << point.real_cluster_id
              << std::endl;*/
  }


  PyObject* pName = PyUnicode_FromString(fPythonScript.c_str());
  PyObject* pModule = PyImport_Import(pName);
  Py_DECREF(pName);

  if (!pModule) {
    PyErr_Print();
    throw cet::exception("LArPIDInterface") << "Failed to load Python script: " << fPythonScript;
  }

  PyObject* pFunc = PyObject_GetAttrString(pModule, "process_data_test");
  if (!pFunc || !PyCallable_Check(pFunc)) {
    PyErr_Print();
    throw cet::exception("LArPIDInterface") << "Failed to find callable function in python script.";
  }


  for (const auto& partMapPair : particleMap){

    auto prong_vv = make_cropped_initial_sparse_prong_image_reco(adc_v,thrumu_v,partMapPair.second.larflowProng,
                                                                 partMapPair.second.cropPoint,10.,512,512);
    std::cout << "successfully made prong image with "<<prong_vv[0].size()<<" plane 0 pixels, "<<prong_vv[1].size()<<" plane 1 pixels, and "<<prong_vv[2].size()<<" plane 2 pixels!" << std::endl;

    PyObject* pyDataList = PyList_New(prong_vv.size());

    for (size_t i = 0; i < prong_vv.size(); ++i) {

      const std::vector<CropPixData_t>& planeData = prong_vv[i];
      PyObject* pyPlaneDataList = PyList_New(planeData.size());

      for (size_t j = 0; j < planeData.size(); ++j) {
        const CropPixData_t& cropData = planeData[j];
        PyObject* pyTuple = PyTuple_New(6);
        PyTuple_SetItem(pyTuple, 0, PyLong_FromLong(cropData.row));
        PyTuple_SetItem(pyTuple, 1, PyLong_FromLong(cropData.col));
        PyTuple_SetItem(pyTuple, 2, PyLong_FromLong(cropData.rawRow));
        PyTuple_SetItem(pyTuple, 3, PyLong_FromLong(cropData.rawCol));
        PyTuple_SetItem(pyTuple, 4, PyFloat_FromDouble(cropData.val));
        PyTuple_SetItem(pyTuple, 5, PyLong_FromLong(cropData.idx));
        PyList_SetItem(pyPlaneDataList, j, pyTuple);
      }

      PyList_SetItem(pyDataList, i, pyPlaneDataList);

    }

    PyObject* pArgs = PyTuple_New(1);
    PyTuple_SetItem(pArgs, 0, pyDataList);
    PyObject* pResult = PyObject_CallObject(pFunc, pArgs);

    if (pResult != nullptr) {
      std::cout << "Python function executed successfully." << std::endl;
      Py_DECREF(pResult);
    }
    else {
      PyErr_Print();
      throw cet::exception("LArPIDInterface") << "Error in Python function call.";
    }

    Py_DECREF(pArgs);
    Py_DECREF(pyDataList);

  }


  Py_XDECREF(pFunc);
  Py_DECREF(pModule);

}


std::vector< std::vector<LArPIDInterface::CropPixData_t> >
LArPIDInterface::make_cropped_initial_sparse_prong_image_reco( const std::vector<larcv::Image2D>& adc_v,
                                                           const std::vector<larcv::Image2D>& thrumu_v,
                                                           const larlite::larflowcluster& prong,
                                                           const TVector3& cropCenter, 
                                                           float threshold, int rowSpan, int colSpan ) {

  // sparsify planes: pixels must be above threshold
  std::vector< std::vector<LArPIDInterface::CropPixData_t> > sparseimg_vv(adc_v.size()*2);
  for ( size_t p=0; p<adc_v.size(); p++ ) {
    sparseimg_vv[p].reserve( (int)( 0.1 * adc_v[p].as_vector().size() ) ); 
    sparseimg_vv[p+3].reserve( (int)( 0.1 * adc_v[p].as_vector().size() ) ); 
  }    

  std::vector< std::vector<int> > imgBounds;
  getRecoImageBounds(imgBounds, adc_v, prong, cropCenter, rowSpan, colSpan);

  fillProngImagesFromReco(sparseimg_vv, threshold, adc_v, thrumu_v, prong, imgBounds);
  fillContextImages(sparseimg_vv, threshold, adc_v, thrumu_v, imgBounds);

  return sparseimg_vv;
}


void LArPIDInterface::getRecoImageBounds( std::vector< std::vector<int> >& imgBounds,
                                      const std::vector<larcv::Image2D>& adc_v,
                                      const larlite::larflowcluster& prong,
                                      const TVector3& cropCenter, int rowSpan, int colSpan ) {

  std::vector< std::vector<int> > prongBounds;
  for ( size_t p=0; p<adc_v.size(); p++ ) {
    std::vector<int> planeProngBounds{9999999,-9999999,9999999,-9999999};
    for( const auto& hit : prong ){
      int row = (hit.tick - 2400)/6;
      int col = hit.targetwire[p];
      if(row < planeProngBounds[0]) planeProngBounds[0] = row;
      if(row > planeProngBounds[1]) planeProngBounds[1] = row;
      if(col < planeProngBounds[2]) planeProngBounds[2] = col;
      if(col > planeProngBounds[3]) planeProngBounds[3] = col;
    }
    prongBounds.push_back(planeProngBounds);
  }

  bool reCenterAny = false;
  std::vector<bool> reCenter;
  for ( size_t p=0; p<adc_v.size(); p++ ) {
    std::vector<int> imgPlaneBounds{0, 0, 0, 0};
    if( (prongBounds[p][1] - prongBounds[p][0]) < rowSpan &&
        (prongBounds[p][3] - prongBounds[p][2]) < colSpan    ){
      reCenter.push_back(false);
      imgPlaneBounds[0] = (prongBounds[p][0] + prongBounds[p][1])/2 - rowSpan/2;
      imgPlaneBounds[2] = (prongBounds[p][2] + prongBounds[p][3])/2 - colSpan/2;
    } else{
      reCenterAny = true;
      reCenter.push_back(true);
    }
    imgBounds.push_back(imgPlaneBounds);
  }

  if(reCenterAny){

    for ( size_t p=0; p<adc_v.size(); p++ ) {
     if(!reCenter[p]) continue;
     auto center2D = larutil::GeometryHelper::GetME()->Point_3Dto2D(cropCenter.X(),cropCenter.Y(),cropCenter.Z(),p);
     int center2Dr = (int)((center2D.t/larutil::GeometryHelper::GetME()->TimeToCm() + 800.)/6.);
     int center2Dc = (int)(center2D.w/larutil::GeometryHelper::GetME()->WireToCm());
     imgBounds[p][0] = center2Dr - rowSpan/2;
     imgBounds[p][2] = center2Dc - colSpan/2;
    }

  }

  for ( size_t p=0; p<adc_v.size(); p++ ) {
    if(imgBounds[p][0] < 0) imgBounds[p][0] = 0;
    if(imgBounds[p][0] + rowSpan > (int)adc_v[p].meta().rows())
      imgBounds[p][0] = adc_v[p].meta().rows() - rowSpan;
    imgBounds[p][1] = imgBounds[p][0] + rowSpan;
    if(imgBounds[p][2] < 0) imgBounds[p][2] = 0;
    if(imgBounds[p][2] + colSpan > (int)adc_v[p].meta().cols())
      imgBounds[p][2] = adc_v[p].meta().cols() - colSpan;
    imgBounds[p][3] = imgBounds[p][2] + colSpan;
  }

  return;
}


void LArPIDInterface::fillProngImagesFromReco(std::vector< std::vector<LArPIDInterface::CropPixData_t> >& sparseimg_vv,
                                          const float& threshold,
                                          const std::vector<larcv::Image2D>& adc_v,
                                          const std::vector<larcv::Image2D>& thrumu_v,
                                          const larlite::larflowcluster& prong,
                                          const std::vector< std::vector<int> >& imgBounds) {

  for ( size_t p=0; p<adc_v.size(); p++ ) {

    for( const auto& hit : prong ){
      // TO DO: REPLACE HARD-CODED VALUES!!!
      int row = (hit.tick - 2400)/6;
      int col = hit.targetwire[p];
      float val = adc_v[p].pixel(row, col);
      float val_cosmic = thrumu_v[p].pixel(row, col);
        if ( val >= threshold && val_cosmic < threshold &&
             row >= imgBounds[p][0] && row < imgBounds[p][1] &&
             col >= imgBounds[p][2] && col < imgBounds[p][3] ) {
          CropPixData_t cropPixData(row - imgBounds[p][0], col - imgBounds[p][2], row, col, val);
          if( std::find(sparseimg_vv[p].begin(), sparseimg_vv[p].end(), cropPixData) ==
              sparseimg_vv[p].end() )
            sparseimg_vv[p].push_back(cropPixData);
        }
    }

    int idx=0;
    for ( auto& pix : sparseimg_vv[p] ) {
      pix.idx = idx;
      idx++;
    }

  }

  return;
}


void LArPIDInterface::fillContextImages(std::vector< std::vector<LArPIDInterface::CropPixData_t> >& sparseimg_vv,
                                    const float& threshold,
                                    const std::vector<larcv::Image2D>& adc_v,
                                    const std::vector<larcv::Image2D>& thrumu_v,
                                    const std::vector< std::vector<int> >& imgBounds){

  for ( size_t p=0; p<adc_v.size(); p++ ) {

    for ( size_t row=0; row<adc_v[p].meta().rows(); row++ ) {
      for ( size_t col=0; col<adc_v[p].meta().cols(); col++ ) {
        float val = adc_v[p].pixel(row,col);
        float val_cosmic = thrumu_v[p].pixel(row, col);
        if ( val >= threshold && val_cosmic < threshold &&
             (int)row >= imgBounds[p][0] && (int)row < imgBounds[p][1] &&
             (int)col >= imgBounds[p][2] && (int)col < imgBounds[p][3] ) {
          sparseimg_vv[p+3].push_back( CropPixData_t((int)row - imgBounds[p][0],
                                                     (int)col - imgBounds[p][2], (int)row, (int)col, val) );
        }
      }
    }

    int idx=0;
    for ( auto& pix : sparseimg_vv[p+3] ) {
      pix.idx = idx;
      idx++;
    }
  }

  return;
}

DEFINE_ART_MODULE(LArPIDInterface)
