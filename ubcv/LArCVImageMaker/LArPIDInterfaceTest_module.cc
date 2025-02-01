////////////////////////////////////////////////////////////////////////
// Class:       LArPIDInterfaceTest
// Plugin Type: analyzer (art v3_01_02)
// File:        LArPIDInterfaceTest_module.cc
//
// Generated at Wed Jan 22 08:22:00 2025 by Matthew Rosenberg using cetskelgen
// from cetlib version v3_05_01.
////////////////////////////////////////////////////////////////////////

//#include <Python.h> // Python/C API header

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

#include "LArPIDInterface.h"

#include <unordered_map>
#include <cmath>

class LArPIDInterfaceTest;

class LArPIDInterfaceTest : public art::EDAnalyzer {
public:
  explicit LArPIDInterfaceTest(fhicl::ParameterSet const& p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  LArPIDInterfaceTest(LArPIDInterfaceTest const&) = delete;
  LArPIDInterfaceTest(LArPIDInterfaceTest&&) = delete;
  LArPIDInterfaceTest& operator=(LArPIDInterfaceTest const&) = delete;
  LArPIDInterfaceTest& operator=(LArPIDInterfaceTest&&) = delete;

  // Required functions.
  void analyze(art::Event const& e) override;

  void beginJob() override;

private:

  // Declare member data here.
  std::string fLArCVImageFile;
  std::string fModelPath;
  //bool fUseGPU;
  unsigned int fPixelThreshold; // only run model over prongs with at least this many pixels in at least one plane
  bool fAllPlaneThreshold; //if true, only run model over prongs that pass the pixel threshold in all three planes
  //std::string fPythonScript;
  unsigned int iImg_start;

};


void LArPIDInterfaceTest::beginJob() {
  // Initialize Python interpreter
  //Py_Initialize();
  iImg_start = 0;
}


LArPIDInterfaceTest::LArPIDInterfaceTest(fhicl::ParameterSet const& p)
  : EDAnalyzer{p},
     fLArCVImageFile(p.get<std::string>("LArCVImageFile")),
     fModelPath(p.get<std::string>("ModelPath")),
     //fUseGPU(p.get<bool>("UseGPU")),
     fPixelThreshold(p.get<unsigned int>("PixelThreshold")),
     fAllPlaneThreshold(p.get<bool>("AllPlaneThreshold"))
     //fPythonScript(p.get<std::string>("PythonScript"))
  // More initializers here.
{
  // Call appropriate consumes<>() for any products to be retrieved by this module.
}

void LArPIDInterfaceTest::analyze(art::Event const& e)
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

  std::unordered_map<int, LArPID::ParticleInfo> particleMap;

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
      LArPID::ParticleInfo newParticle(crop);
      particleMap[particle.TrackId()] = newParticle;
    }
    else{
      TVector3 crop(particle.EndX(), particle.EndY(), particle.EndZ());
      LArPID::ParticleInfo newParticle(crop);
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


  /*
  PyObject* pName = PyUnicode_FromString(fPythonScript.c_str());
  PyObject* pModule = PyImport_Import(pName);
  Py_DECREF(pName);

  if (!pModule) {
    PyErr_Print();
    throw cet::exception("LArPIDInterfaceTest") << "Failed to load Python script: " << fPythonScript;
  }

  PyObject* pFunc = PyObject_GetAttrString(pModule, "process_data_test");
  if (!pFunc || !PyCallable_Check(pFunc)) {
    PyErr_Print();
    throw cet::exception("LArPIDInterfaceTest") << "Failed to find callable function in python script.";
  }
  */

  //LArPID::TorchModel model(fModelPath, fUseGPU);
  LArPID::TorchModel model(fModelPath);

  for (const auto& partMapPair : particleMap){

    auto prong_vv = LArPID::make_cropped_initial_sparse_prong_image_reco(adc_v, thrumu_v,
     partMapPair.second.larflowProng, partMapPair.second.cropPoint, 10., 512, 512);
    std::cout << "successfully made prong image for trackID "<<partMapPair.first<<" with "<<prong_vv[0].size()<<" plane 0 pixels, "<<prong_vv[1].size()<<" plane 1 pixels, and "<<prong_vv[2].size()<<" plane 2 pixels!" << std::endl;

    bool thresholdPassInOne = false;
    bool thresholdPassInAll = true;
    for(size_t p = 0; p < 3; ++p){
      if(prong_vv[p].size() >= fPixelThreshold) thresholdPassInOne = true;
      else thresholdPassInAll = false;
    }

    if(thresholdPassInAll || (thresholdPassInOne && !fAllPlaneThreshold)){
      LArPID::ModelOutput output = model.run_inference(prong_vv);
      std::cout << "Successfuly ran LArPID! Model outputs:" << std::endl;
      std::cout << "    PID: " << output.pid << std::endl;
      std::cout << "    Production process: " << output.process << std::endl;
      std::cout << "    completeness: " << output.completeness << std::endl;
      std::cout << "    purity: " << output.purity << std::endl;
      std::cout << "    electron_score: " << output.electron_score << std::endl;
      std::cout << "    photon_score: " << output.photon_score << std::endl;
      std::cout << "    muon_score: " << output.muon_score << std::endl;
      std::cout << "    pion_score: " << output.pion_score << std::endl;
      std::cout << "    proton_score: " << output.proton_score << std::endl;
      std::cout << "    primary_score: " << output.primary_score << std::endl;
      std::cout << "    neutralParent_score: " << output.neutralParent_score << std::endl;
      std::cout << "    chargedParent_score: " << output.chargedParent_score << std::endl;
    }

    /*
    PyObject* pyDataList = PyList_New(prong_vv.size());

    for (size_t i = 0; i < prong_vv.size(); ++i) {

      const std::vector<LArPID::CropPixData_t>& planeData = prong_vv[i];
      PyObject* pyPlaneDataList = PyList_New(planeData.size());

      for (size_t j = 0; j < planeData.size(); ++j) {
        const LArPID::CropPixData_t& cropData = planeData[j];
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

    //PyObject* pyProngData = PyROOT::Bind(prong_vv);
    //PyObject* pResult = PyObject_CallFunctionObjArgs(pFunc, pyProngData, NULL);

    if (pResult != nullptr) {
      std::cout << "Python function executed successfully." << std::endl;
      Py_DECREF(pResult);
    }
    else {
      PyErr_Print();
      throw cet::exception("LArPIDInterfaceTest") << "Error in Python function call.";
    }

    Py_DECREF(pArgs);
    Py_DECREF(pyDataList);
    //Py_DECREF(pyProngData);
    */
  }


  //Py_XDECREF(pFunc);
  //Py_DECREF(pModule);

}



DEFINE_ART_MODULE(LArPIDInterfaceTest)
