
#include "LArPIDInterface.h"


namespace LArPID {


  std::vector< std::vector<CropPixData_t> >
  make_cropped_initial_sparse_prong_image_reco( const std::vector<larcv::Image2D>& adc_v,
                                                const std::vector<larcv::Image2D>& thrumu_v,
                                                const larlite::larflowcluster& prong,
                                                const TVector3& cropCenter, 
                                                float threshold, int rowSpan, int colSpan ) {

    // sparsify planes: pixels must be above threshold
    std::vector< std::vector<CropPixData_t> > sparseimg_vv(adc_v.size()*2);
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


  void getRecoImageBounds( std::vector< std::vector<int> >& imgBounds,
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


  void fillProngImagesFromReco(std::vector< std::vector<CropPixData_t> >& sparseimg_vv,
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


  void fillContextImages(std::vector< std::vector<CropPixData_t> >& sparseimg_vv,
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



  //TorchModel::TorchModel(const std::string& model_path, const bool& useGPU){
  TorchModel::TorchModel(const std::string& model_path){

    try {
      model = torch::jit::load(model_path);
      //device = (torch::cuda::is_available() && useGPU) ? torch::kCUDA : torch::kCPU;
      //model.to(device);
      model->to(torch::kCPU);
      //model.eval();
    }
    catch (const c10::Error& e) {
      std::cerr << "LArPIDInterface: Error loading the torchscript model: " << e.what() << std::endl;
      throw;
    }

    //norm_mean = torch::tensor({57.8182, 57.8182, 58.1807, 58.1807, 50.5312, 50.5312}, torch::kFloat32).view({1, 6, 1, 1}).to(device);
    //norm_std  = torch::tensor({62.9932, 62.9932, 62.6569, 62.6569, 42.0027, 42.0027}, torch::kFloat32).view({1, 6, 1, 1}).to(device);
    norm_mean = torch::tensor({57.8182, 57.8182, 58.1807, 58.1807, 50.5312, 50.5312}, torch::kFloat32).view({1, 6, 1, 1}).to(torch::kCPU);
    norm_std  = torch::tensor({62.9932, 62.9932, 62.6569, 62.6569, 42.0027, 42.0027}, torch::kFloat32).view({1, 6, 1, 1}).to(torch::kCPU);

  }


  int TorchModel::getPID(const int& cnnClass){
    if(cnnClass == 0) return 11;
    if(cnnClass == 1) return 22;
    if(cnnClass == 2) return 13;
    if(cnnClass == 3) return 211;
    if(cnnClass == 4) return 2212;
    return 0;
  }


  ModelOutput TorchModel::run_inference(const std::vector< std::vector<CropPixData_t> >& pixelData){

    torch::Tensor input_tensor = torch::zeros({1,6,512,512}, torch::kFloat32);
    //input_tensor = input_tensor.to(device);
    input_tensor = input_tensor.to(torch::kCPU);

    for(size_t v = 0; v < pixelData.size(); ++v){
      for(const auto& pix : pixelData[v]){
        input_tensor[0][v][pix.row][pix.col] = pix.val;
      }
    }

    input_tensor = (input_tensor - norm_mean) / norm_std;
    input_tensor = torch::clamp(input_tensor, -std::numeric_limits<float>::infinity(), 4.0);
    std::vector<torch::jit::IValue> inputs;
    inputs.push_back(input_tensor);

    auto output_tuple = model->forward(inputs).toTuple();
    at::Tensor outputT0 = output_tuple->elements()[0].toTensor();
    at::Tensor outputT1 = output_tuple->elements()[1].toTensor();
    at::Tensor outputT2 = output_tuple->elements()[2].toTensor();
    at::Tensor outputT3 = output_tuple->elements()[3].toTensor();

    ModelOutput output( getPID(outputT0.argmax(1).item<int>()), outputT3.argmax(1).item<int>(),
                        outputT1.item<float>(), outputT2.item<float>(), outputT0[0][0].item<float>(),
                        outputT0[0][1].item<float>(), outputT0[0][2].item<float>(),
                        outputT0[0][3].item<float>(), outputT0[0][4].item<float>(),
                        outputT3[0][0].item<float>(), outputT3[0][1].item<float>(),
                        outputT3[0][2].item<float>()
                      );

    return output;

  }

}
