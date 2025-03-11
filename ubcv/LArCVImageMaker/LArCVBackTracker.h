
#ifndef LARCVBACKTRACKER_H
#define LARCVBACKTRACKER_H

#include "ublarcvapp/MCTools/MCPixelPGraph.h"
#include "ublarcvapp/MCTools/MCPixelPMap.h"

#include "larlite/DataFormat/storage_manager.h"

#include "LArPIDInterface.h"

namespace LArCVBackTrack {


  struct TruthMatchInfo {

    int pdg;
    float purity;

    TruthMatchInfo() : pdg(0), purity(-1.) {};
    TruthMatchInfo(int pid, float pur) : pdg(pid), purity(pur) {};

  };


  struct TruthMatchResults {

    int pdg, tid; // for sim particle that deposits most charge in prong
    float purity, completeness; // for sim particle that deposits most charge in prong
    std::vector<TruthMatchInfo> allMatches; // info for all sim particle types that deposit charge in prong

    TruthMatchResults() : pdg(0), tid(-1), purity(-1.), completeness(-1.) {};
    TruthMatchResults(int pid, int id, float pur, float comp) :
      pdg(pid), tid(id), purity(pur), completeness(comp) {};

  };


  TruthMatchResults run_backtracker(const std::vector< std::vector<LArPID::CropPixData_t> >& sparseimg_vv,
                                    const ublarcvapp::mctools::MCPixelPGraph& mcpg,
                                    ublarcvapp::mctools::MCPixelPMap& mcpm,
                                    const std::vector<larcv::Image2D>& adc_v);

  TruthMatchResults run_backtracker(const std::vector< std::vector<LArPID::CropPixData_t> >& sparseimg_vv,
                                    larcv::IOManager& iolcv,
                                    larlite::storage_manager& ioll,
                                    const std::vector<larcv::Image2D>& adc_v);


}  // namespace LArCVBackTrack

#endif  // LARCVBACKTRACKER_H

