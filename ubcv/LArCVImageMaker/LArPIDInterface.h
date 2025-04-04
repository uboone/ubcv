
#ifndef LARPIDINTERFACE_H
#define LARPIDINTERFACE_H

//ClassDef macro from ROOT and libtorch conflict, this is necessary:
#ifdef ClassDef
#undef ClassDef
#endif
#include <torch/script.h>

//Load ROOT ClassDef back in now that we have the torch headers:
#include <Rtypes.h>

#include "larcv/core/DataFormat/IOManager.h"
#include "larcv/core/DataFormat/EventImage2D.h"
#include "larlite/LArUtil/GeometryHelper.h"
#include "larlite/DataFormat/larflowcluster.h"
#include "larlite/DataFormat/larflow3dhit.h"

#include <cmath>


namespace LArPID {

  struct CropPixData_t {
    int row; ///< row of pixel in cropped image
    int col; ///< col of pixel in cropped image
    int rawRow; ///< row of pixel in original image
    int rawCol; ///< col of pixel in original image
    float val; ///< value of pixel
    bool inCrop; ///< pixel is inside crop
    int idx;   ///< index in container

    CropPixData_t()
    : row(0),col(0),rawRow(0),rawCol(0),val(0.0),inCrop(false),idx(0)
    {};

    CropPixData_t( int r, int c, int rr, int rc, float v, bool ic)
    : row(r),col(c),rawRow(rr),rawCol(rc),val(v),inCrop(ic),idx(0) {};

    bool operator==( const CropPixData_t& rhs ) const {
      if(rawRow == rhs.rawRow && rawCol == rhs.rawCol && fabs(val - rhs.val) < 1e-3) return true;
      return false;
    };

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

  std::vector< std::vector<CropPixData_t> >
    make_cropped_initial_sparse_prong_image_reco( const std::vector<larcv::Image2D>& adc_v,
                                                  const std::vector<larcv::Image2D>& thrumu_v,
                                                  const larlite::larflowcluster& prong,
                                                  const TVector3& cropCenter,
                                                  float threshold, int rowSpan, int colSpan );

  void getRecoImageBounds( std::vector< std::vector<int> >& imgBounds,
                           const std::vector<larcv::Image2D>& adc_v,
                           const larlite::larflowcluster& prong,
                           const TVector3& cropCenter, int rowSpan, int colSpan );

  void fillProngImagesFromReco( std::vector< std::vector<CropPixData_t> >& sparseimg_vv,
                                const float& threshold,
                                const std::vector<larcv::Image2D>& adc_v,
                                const std::vector<larcv::Image2D>& thrumu_v,
                                const larlite::larflowcluster& prong,
                                const std::vector< std::vector<int> >& imgBounds );

  void fillContextImages( std::vector< std::vector<CropPixData_t> >& sparseimg_vv,
                          const float& threshold,
                          const std::vector<larcv::Image2D>& adc_v,
                          const std::vector<larcv::Image2D>& thrumu_v,
                          const std::vector< std::vector<int> >& imgBounds );


  struct ModelOutput {

    int pid, process;
    float completeness, purity;
    float electron_score, photon_score, muon_score, pion_score, proton_score;
    float primary_score, neutralParent_score, chargedParent_score;

    ModelOutput() : pid(0), process(-1), completeness(-1.), purity(-1.), electron_score(-99.),
                    photon_score(-99.), muon_score(-99.), pion_score(-99.), proton_score(-99.),
                    primary_score(-99.), neutralParent_score(-99.), chargedParent_score(-99.) {};

    ModelOutput(int pdg, int proc, float comp, float pur, float el, float ph,
                float mu, float pi, float pr, float prim, float ntrl, float chgd) :
                  pid(pdg), process(proc), completeness(comp), purity(pur), electron_score(el),
                  photon_score(ph), muon_score(mu), pion_score(pi), proton_score(pr),
                  primary_score(prim), neutralParent_score(ntrl), chargedParent_score(chgd) {};
  };


  class TorchModel {

    private:
      torch::jit::Module model;
      //torch::Device device;
      torch::Tensor norm_mean;
      torch::Tensor norm_std;
      bool debug_mode;
      int getPID(const int& cnnClass);
      size_t getChannel(const size_t& pixDataIndex);
      void printTensorValues(const torch::Tensor& tensor);

    public:
      //TorchModel(const std::string& model_path, const bool& useGPU=false);
      TorchModel(); //must call Initialize before using model if using this constructor
      TorchModel(const std::string& model_path, const bool& debug=false);
      void Initialize(const std::string& model_path, const bool& debug=false);
      ModelOutput run_inference(const std::vector< std::vector<CropPixData_t> >& pixelData);

  };


}  // namespace LArPID

#endif  // LARPIDINTERFACE_H

