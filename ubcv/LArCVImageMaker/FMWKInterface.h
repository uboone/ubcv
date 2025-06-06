#ifndef __FMWKINTERFACE_H__
#define __FMWKINTERFACE_H__

#include "larcore/Geometry/Geometry.h"
#include "fhiclcpp/ParameterSet.h"
#include "larcv/core/Base/PSet.h"
#include "nusimdata/SimulationBase/MCTruth.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_vectors.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/Wire.h"
#include "lardataobj/RawData/OpDetWaveform.h"
#include "lardataobj/MCBase/MCShower.h"
#include "lardataobj/MCBase/MCTrack.h"
#include "lardataobj/Simulation/SimChannel.h"
namespace detinfo {
  class DetectorClocksData;
  class DetectorPropertiesData;
}
#include "lardataobj/Simulation/SimEnergyDeposit.h"

namespace supera {
  typedef larcv::PSet        Config_t;
  typedef recob::Wire        LArWire_t;
  typedef raw::OpDetWaveform LArOpDigit_t;
  typedef recob::Hit         LArHit_t;
  typedef simb::MCTruth      LArMCTruth_t;
  typedef sim::MCTrack       LArMCTrack_t;
  typedef sim::MCShower      LArMCShower_t;
  typedef sim::SimChannel    LArSimCh_t;
  typedef sim::MCStep        LArMCStep_t;
  typedef sim::SimEnergyDeposit LArSimEdep_t;
}
//
// Utility functions (geometry, lar properties, etc.)
//
namespace supera {
  
  //typedef ::fhicl::ParameterSet Config_t;

  //
  // LArProperties
  //

  // DriftVelocity in cm/us
  double DriftVelocity(detinfo::DetectorPropertiesData const& detProp);

  //
  // Geometry
  //

  /// Channel number to wire ID
  ::geo::WireID ChannelToWireID(unsigned int ch);

  /// Number of channels
  unsigned int Nchannels();
  
  /// Number of planes
  unsigned int Nplanes();
  
  /// Number of wires
  unsigned int Nwires(unsigned int plane);

  /// Nearest wire
  unsigned int NearestWire(const TVector3& xyz, unsigned int plane);

  /// Nearest wire
  unsigned int NearestWire(const double* xyz, unsigned int plane);

  /// Nearest wire
  unsigned int NearestWire(const geo::Point_t& xyz, unsigned int plane);

  /// Angle from z-axis
  double WireAngleToVertical(unsigned int plane);

  /// Wire pitch
  double WirePitch(size_t plane);

  /// Detector height
  double DetHalfHeight();

  /// Detector width
  double DetHalfWidth();

  /// Detector length
  double DetLength();

  //
  // DetectorClockService
  //
  
  // /// Number of time ticks
  // unsigned int NumberTimeSamples();
  
  /// G4 time to TPC tick
  int TPCG4Time2Tick(detinfo::DetectorClocksData const& clockData, double ns);

  /// G4 time to TPC tick
  int TPCG4Time2TDC(detinfo::DetectorClocksData const& clockData, double ns);

  /// per-plane tick offset
  double PlaneTickOffset(detinfo::DetectorClocksData const& clockData,
                         detinfo::DetectorPropertiesData const& detProp,
                         size_t plane0, size_t plane1);
  
  /// TPC TDC to Tick
  double TPCTDC2Tick(detinfo::DetectorClocksData const& clockData, double tdc);

  /// Trigger tick
  double TriggerOffsetTPC(detinfo::DetectorClocksData const& clockData);

  /// TPC sampling period
  double TPCTickPeriod(detinfo::DetectorClocksData const& clockData);

  //
  // SpaceChargeService
  // 
  /// Truth position to shifted
  void ApplySCE(double& x, double& y, double& z);
  /// Truth position to shifted
  void ApplySCE(double* xyz);
  /// Truth position to shifted
  void ApplySCE(TVector3& xyz);
  
}

#endif
