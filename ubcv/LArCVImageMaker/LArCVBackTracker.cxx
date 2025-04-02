
#include "LArCVBackTracker.h"

#include <cmath>
#include <unordered_map>


namespace LArCVBackTrack {


  struct SimPartInfo {

    int pdg, node;
    float charge;

    SimPartInfo() : pdg(0), node(-9), charge(-99.) {};
    SimPartInfo(int p, int n, float c) : pdg(p), node(n), charge(c) {};

  };


  TruthMatchResults run_backtracker(const std::vector< std::vector<LArPID::CropPixData_t> >& sparseimg_vv,
                                    const ublarcvapp::mctools::MCPixelPGraph& mcpg,
                                    ublarcvapp::mctools::MCPixelPMap& mcpm,
                                    const std::vector<larcv::Image2D>& adc_v) {

    std::unordered_map<int, float> particleMap;
    std::unordered_map<int, SimPartInfo> simTrackMap;
    float totalPixI = 0.;

    for(unsigned int p = 0; p < 3; ++p){
      for(const auto& pix : sparseimg_vv[p]){
        totalPixI += pix.val;
        auto pixContents = mcpm.getPixContent(p, pix.rawRow, pix.rawCol);
        for(const auto& part : pixContents.particles){
          if(particleMap.find(std::abs(part.pdg)) != particleMap.end()){
            particleMap[std::abs(part.pdg)] += pixContents.pixI;
          }
          else{
            particleMap[std::abs(part.pdg)] = pixContents.pixI;
          }
          if(simTrackMap.find(part.tid) != simTrackMap.end()){
            simTrackMap[part.tid].charge += pixContents.pixI;
          }
          else{
            SimPartInfo newContributor(part.pdg, part.nodeidx, pixContents.pixI);
            simTrackMap[part.tid] = newContributor;
          }
        } //pixContents.particles loop
      } //sparseimg_vv[p] pixel loop
    } //sparseimg_vv plane loop

    TruthMatchResults results;
    int matchedPartNID = -1;
    float matchedPartI = 0.;

    for(const auto& partType : particleMap){
      TruthMatchInfo part(partType.first, partType.second/totalPixI);
      results.allMatches.push_back(part);
    }

    for(const auto& track : simTrackMap){
      if(track.second.charge > matchedPartI){
        matchedPartI = track.second.charge;
        matchedPartNID = track.second.node;
        results.tid = track.first;
        results.pdg = track.second.pdg;
        results.purity = track.second.charge/totalPixI;
      }
    }

    if(matchedPartI > 0.){
      const auto& matchedPartNode = mcpg.node_v[matchedPartNID];
      if(matchedPartNode.tid == results.tid){
        float totNodePixI = 0.;
        for(unsigned int p = 0; p < 3; ++p){
          const auto& pixels = matchedPartNode.pix_vv[p];
          for(unsigned int iP = 0; iP < pixels.size()/2; ++iP){
            int row = (pixels[2*iP] - 2400)/6;
            int col = pixels[2*iP+1];
            totNodePixI += adc_v[p].pixel(row, col);
          } // pixel loop
        } // plane loop
        if(totNodePixI > 0.) results.completeness = matchedPartI/totNodePixI;
      }
    }

    return results;

  }


  
  TruthMatchResults run_backtracker(const std::vector< std::vector<LArPID::CropPixData_t> >& sparseimg_vv,
                                    larcv::IOManager& iolcv,
                                    larlite::storage_manager& ioll,
                                    const std::vector<larcv::Image2D>& adc_v){

    auto mcpg = ublarcvapp::mctools::MCPixelPGraph();
    mcpg.set_adc_treename("wire");
    mcpg.buildgraph(iolcv, ioll);

    auto mcpm = ublarcvapp::mctools::MCPixelPMap();
    mcpm.set_adc_treename("wire");
    mcpm.buildmap(iolcv, mcpg);

    return run_backtracker(sparseimg_vv, mcpg, mcpm, adc_v);

  }


}  // namespace LArCVBackTrack


