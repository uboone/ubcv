#ifndef __FMWKINTERFACE_CXX__
#define __FMWKINTERFACE_CXX__

#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "Base/larbys.h"
#include "FMWKInterface.h"
#include "lardataalg/DetectorInfo/DetectorPropertiesData.h"
#include "lardataalg/DetectorInfo/DetectorClocksData.h"
#include "larevt/SpaceChargeServices/SpaceChargeService.h"
#include "larcore/Geometry/WireReadout.h"
#include "larcore/CoreUtils/ServiceUtil.h" // lar::providerFrom<>()

namespace {
  constexpr geo::TPCID tpcid{0,0};
}

namespace supera {

  ::geo::WireID ChannelToWireID(unsigned int ch)
  { 
    auto const& channelMap = art::ServiceHandle<geo::WireReadout const>()->Get();
    return channelMap.ChannelToWire(ch).front();
  }
  
  double DriftVelocity(detinfo::DetectorPropertiesData const& detProp)
  { 
    return detProp.DriftVelocity();
  }
  
  unsigned int Nchannels()
  {
    auto const& channelMap = art::ServiceHandle<geo::WireReadout const>()->Get();
    return channelMap.Nchannels();
  }
  
  unsigned int Nplanes()
  { 
    auto const& channelMap = art::ServiceHandle<geo::WireReadout const>()->Get();
    return channelMap.Nplanes({0, 0});
  }
  
  unsigned int Nwires(unsigned int plane)
  { 
    auto const& channelMap = art::ServiceHandle<geo::WireReadout const>()->Get();
    return channelMap.Nwires(geo::PlaneID{tpcid, plane});
  }
  
  unsigned int NearestWire(const geo::Point_t& xyz, unsigned int plane)
  {
    double min_wire=0;
    double max_wire=Nwires(plane)-1;
    
    auto const& channelMap = art::ServiceHandle<geo::WireReadout const>()->Get();
    double wire = channelMap.Plane(geo::PlaneID{tpcid, plane}).WireCoordinate(xyz) + 0.5;
    if(wire<min_wire) wire = min_wire;
    if(wire>max_wire) wire = max_wire;
    
    return (unsigned int)wire;
  }

  unsigned int NearestWire(const TVector3& xyz, unsigned int plane)
  {
    return NearestWire(geo::vect::toPoint(xyz), plane);
  }
    
  unsigned int NearestWire(const double* xyz, unsigned int plane)
  {
    return NearestWire(geo::vect::toPoint(xyz), plane);
  }

  double WireAngleToVertical(unsigned int plane)
  {
    auto const& channelMap = art::ServiceHandle<geo::WireReadout const>()->Get();
    return channelMap.WireAngleToVertical(geo::View_t(plane), tpcid);
  }

  double WirePitch(size_t plane)
  {
    auto const& channelMap = art::ServiceHandle<geo::WireReadout const>()->Get();
    return channelMap.Plane(geo::PlaneID(tpcid, plane)).WirePitch();
  }

  double DetHalfWidth() 
  {
    auto const& tpc = art::ServiceHandle<geo::Geometry>()->TPC();
    return tpc.HalfWidth();
  }

  double DetHalfHeight() 
  {
    auto const& tpc = art::ServiceHandle<geo::Geometry>()->TPC();
    return tpc.HalfHeight();
  }

  double DetLength() 
  {
    auto const& tpc = art::ServiceHandle<geo::Geometry>()->TPC();
    return tpc.Length();
  }
  
  int TPCG4Time2Tick(detinfo::DetectorClocksData const& clockData, double ns)
  { 
    return clockData.TPCG4Time2Tick(ns);
  }

  int TPCG4Time2TDC(detinfo::DetectorClocksData const& clockData, double ns)
  {
    return clockData.TPCG4Time2TDC(ns);
  }
  
  double TPCTDC2Tick(detinfo::DetectorClocksData const& clockData, double tdc)
  { 
    return clockData.TPCTDC2Tick(tdc);
  }

  double TPCTickPeriod(detinfo::DetectorClocksData const& clockData)
  {
    return clockData.TPCClock().TickPeriod();
  }

  double TriggerOffsetTPC(detinfo::DetectorClocksData const& clockData)
  {
    return clockData.TriggerOffsetTPC();
  }
  
  double PlaneTickOffset(detinfo::DetectorClocksData const& clockData,
                         detinfo::DetectorPropertiesData const& detProp,
                         size_t plane0, size_t plane1)
  {
    static double const pitch =
      art::ServiceHandle<geo::WireReadout const>()->Get().PlanePitch({0, 0}, plane0, plane1);
    double tick_period = clockData.TPCClock().TickPeriod();
    return (plane1 - plane0) * pitch / DriftVelocity(detProp) / tick_period;
  }

  void ApplySCE(double& x, double& y, double& z)
  {
    auto xyz = ::lar::providerFrom<spacecharge::SpaceChargeService>()->GetPosOffsets(geo::Point_t(x,y,z));
    x = x - xyz.X() + 0.7;
    y = y + xyz.Y();
    z = z + xyz.Z();
  }

  void ApplySCE(double* xyz)
  {
    double x = xyz[0];
    double y = xyz[1];
    double z = xyz[2];
    ApplySCE(x,y,z);
    xyz[0] = x;
    xyz[1] = y;
    xyz[2] = z;
  }

  void ApplySCE(TVector3& xyz)
  {
    double x = xyz[0];
    double y = xyz[1];
    double z = xyz[2];
    ApplySCE(x,y,z);
    xyz[0] = x;
    xyz[1] = y;
    xyz[2] = z;
  }

}

#endif
