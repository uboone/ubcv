
#ifndef LARPIDINTERFACE_H
#define LARPIDINTERFACE_H


#include "larcv/core/DataFormat/IOManager.h"
#include "larcv/core/DataFormat/EventImage2D.h"
#include "LArUtil/GeometryHelper.h"
#include "DataFormat/larflowcluster.h"
#include "DataFormat/larflow3dhit.h"

#include <torch/script.h>


namespace LArPID {

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

    CropPixData_t( int r, int c, int rr, int rc, float v)
    : row(r),col(c),rawRow(rr),rawCol(rc),val(v),idx(0) {};

    bool operator==( const CropPixData_t& rhs ) const {
      if(rawRow == rhs.rawRow && rawCol == rhs.rawCol && val == rhs.val) return true;
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
      std::shared_ptr<torch::jit::script::Module> model;
      //torch::Device device;
      torch::Tensor norm_mean;
      torch::Tensor norm_std;
      int getPID(const int& cnnClass);

    public:
      //TorchModel(const std::string& model_path, const bool& useGPU=false);
      TorchModel(const std::string& model_path);
      ModelOutput run_inference(const std::vector< std::vector<CropPixData_t> >& pixelData);

  };


}  // namespace LArPID

#endif  // LARPIDINTERFACE_H

