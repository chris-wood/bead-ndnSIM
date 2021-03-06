// ndn-simple.cpp

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"
#include <fstream>
#include <chrono>

#define GROUP_SIZE 10
#define NUM_OF_GROUPS 16
#define NUM_OF_CONSUMERS NUM_OF_GROUPS * GROUP_SIZE
#define NUM_OF_ROUTERS 30
#define NUM_OF_PRODUCER 1
#define TOTAL_NODES NUM_OF_CONSUMERS + NUM_OF_ROUTERS + NUM_OF_PRODUCER

#define IN_ROUTERS_COUNT 13
#define OUT_ROUTERS_COUNT 17

int inRouters[IN_ROUTERS_COUNT] = {4, 7, 9, 13, 14, 15, 16, 19, 21, 22, 23, 25, 27};
int outRouters[OUT_ROUTERS_COUNT] = {0, 1, 2, 3, 5, 6, 8, 10, 11, 12, 17, 18, 20, 24, 26, 28, 29};

namespace ns3 {
  ofstream delayFile;

  void
  ForwardingDelay(size_t id, ns3::Time eventTime, float delay, double size)
  {
    delayFile << id << "\t" << eventTime.GetNanoSeconds() << "\t" << delay * 1000000000 << "\n";
  }

  ofstream dropFile;

  void
  BeadDropCallback(int id, uint64_t hops)
  {
    dropFile << id << "\t" << hops << "\n";
  }

  int
  run(int argc, char* argv[])
  {
    // setting default parameters for PointToPoint links and channels
    Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1000Mbps"));
    Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
    Config::SetDefault("ns3::DropTailQueue::MaxPackets", StringValue("4294967295"));

    std::string fof = "fof.txt";
    std::string dropFileName = "drops.txt";
    std::string topologyFile = "src/ndnSIM/examples/topologies/topo-tree.txt";
    std::string appDelayFile = "app-delays-trace.txt";
    std::string rateTraceFile = "rate-trace.txt";
    std::string percentage = "1.0";
    std::string frequency = "1";
    int simulationTime = 1000;

    CommandLine cmd;
    cmd.AddValue("fof", "forwarder overhead file", fof);
    cmd.AddValue("time", "simulation time argument", simulationTime);
    cmd.AddValue("top", "topology file", topologyFile);
    cmd.AddValue("drop", "bead drop file", dropFileName);
    cmd.AddValue("appd", "app delay file", appDelayFile);
    cmd.AddValue("rate", "rate trace file", rateTraceFile);
    cmd.AddValue("percentage", "bead percentage", percentage);
    cmd.AddValue("freq", "bead frequency", frequency);
    cmd.Parse(argc, argv);

    delayFile.open(fof);
    dropFile.open(dropFileName);

    // Creating nodes
    NodeContainer nodes;
    nodes.Create(NUM_OF_CONSUMERS + NUM_OF_ROUTERS + NUM_OF_PRODUCER);

    // Connecting nodes using two links
    // Connecting nodes using two links
    PointToPointHelper p2p;
    // Connecting consumers to edge routers
    int g = 0;
    for (int i = 0; i < GROUP_SIZE; i++, g++)
      p2p.Install (nodes.Get (g), nodes.Get (0 + NUM_OF_CONSUMERS));      // C0 <--> R0
    for (int i = 0; i < GROUP_SIZE; i++, g++)
      p2p.Install (nodes.Get (g), nodes.Get (1 + NUM_OF_CONSUMERS));      // C1 <--> R1
    for (int i = 0; i < GROUP_SIZE; i++, g++)
      p2p.Install (nodes.Get (g), nodes.Get (3 + NUM_OF_CONSUMERS));      // C2 <--> R3
    for (int i = 0; i < GROUP_SIZE; i++, g++)
      p2p.Install (nodes.Get (g), nodes.Get (5 + NUM_OF_CONSUMERS));      // C3 <--> R5
    for (int i = 0; i < GROUP_SIZE; i++, g++)
      p2p.Install (nodes.Get (g), nodes.Get (6 + NUM_OF_CONSUMERS));      // C4 <--> R6
    for (int i = 0; i < GROUP_SIZE; i++, g++)
      p2p.Install (nodes.Get (g), nodes.Get (10 + NUM_OF_CONSUMERS));     // C5 <--> R10
    for (int i = 0; i < GROUP_SIZE; i++, g++)
      p2p.Install (nodes.Get (g), nodes.Get (8 + NUM_OF_CONSUMERS));      // C6 <--> R8
    for (int i = 0; i < GROUP_SIZE; i++, g++)
      p2p.Install (nodes.Get (g), nodes.Get (11 + NUM_OF_CONSUMERS));     // C7 <--> R11
    for (int i = 0; i < GROUP_SIZE; i++, g++)
      p2p.Install (nodes.Get (g), nodes.Get (12 + NUM_OF_CONSUMERS));     // C8 <--> R12
    for (int i = 0; i < GROUP_SIZE; i++, g++)
      p2p.Install (nodes.Get (g), nodes.Get (18 + NUM_OF_CONSUMERS));     // C9 <--> R18
    for (int i = 0; i < GROUP_SIZE; i++, g++)
      p2p.Install (nodes.Get (g), nodes.Get (17 + NUM_OF_CONSUMERS));    // C10 <--> R17
    for (int i = 0; i < GROUP_SIZE; i++, g++)
      p2p.Install (nodes.Get (g), nodes.Get (20 + NUM_OF_CONSUMERS));    // C11 <--> R20
    for (int i = 0; i < GROUP_SIZE; i++, g++)
      p2p.Install (nodes.Get (g), nodes.Get (24 + NUM_OF_CONSUMERS));    // C12 <--> R24
    for (int i = 0; i < GROUP_SIZE; i++, g++)
      p2p.Install (nodes.Get (g), nodes.Get (29 + NUM_OF_CONSUMERS));    // C13 <--> R2
    for (int i = 0; i < GROUP_SIZE; i++, g++)
      p2p.Install (nodes.Get (g), nodes.Get (28 + NUM_OF_CONSUMERS));    // C14 <--> R28
    for (int i = 0; i < GROUP_SIZE; i++, g++)
      p2p.Install (nodes.Get (g), nodes.Get (21 + NUM_OF_CONSUMERS));    // C15 <--> R21

    // Connect routers
    p2p.Install (nodes.Get (0 + NUM_OF_CONSUMERS), nodes.Get (9 + NUM_OF_CONSUMERS));      // R0 <--> R9
    p2p.Install (nodes.Get (1 + NUM_OF_CONSUMERS), nodes.Get (15 + NUM_OF_CONSUMERS));     // R1 <--> R15
    p2p.Install (nodes.Get (2 + NUM_OF_CONSUMERS), nodes.Get (9 + NUM_OF_CONSUMERS));      // R2 <--> R9
    p2p.Install (nodes.Get (3 + NUM_OF_CONSUMERS), nodes.Get (4 + NUM_OF_CONSUMERS));      // R3 <--> R4
    p2p.Install (nodes.Get (4 + NUM_OF_CONSUMERS), nodes.Get (7 + NUM_OF_CONSUMERS));      // R4 <--> R7
    p2p.Install (nodes.Get (4 + NUM_OF_CONSUMERS), nodes.Get (14 + NUM_OF_CONSUMERS));     // R4 <--> R14
    p2p.Install (nodes.Get (4 + NUM_OF_CONSUMERS), nodes.Get (9 + NUM_OF_CONSUMERS));      // R4 <--> R9
    p2p.Install (nodes.Get (4 + NUM_OF_CONSUMERS), nodes.Get (16 + NUM_OF_CONSUMERS));     // R4 <--> R16
    p2p.Install (nodes.Get (4 + NUM_OF_CONSUMERS), nodes.Get (25 + NUM_OF_CONSUMERS));     // R4 <--> R25
    p2p.Install (nodes.Get (5 + NUM_OF_CONSUMERS), nodes.Get (13 + NUM_OF_CONSUMERS));     // R5 <--> R13
    p2p.Install (nodes.Get (6 + NUM_OF_CONSUMERS), nodes.Get (7 + NUM_OF_CONSUMERS));      // R6 <--> R7
    p2p.Install (nodes.Get (7 + NUM_OF_CONSUMERS), nodes.Get (9 + NUM_OF_CONSUMERS));      // R7 <--> R9
    p2p.Install (nodes.Get (7 + NUM_OF_CONSUMERS), nodes.Get (14 + NUM_OF_CONSUMERS));     // R7 <--> R14
    p2p.Install (nodes.Get (7 + NUM_OF_CONSUMERS), nodes.Get (22 + NUM_OF_CONSUMERS));     // R7 <--> R22
    p2p.Install (nodes.Get (7 + NUM_OF_CONSUMERS), nodes.Get (23 + NUM_OF_CONSUMERS));     // R7 <--> R23
    p2p.Install (nodes.Get (8 + NUM_OF_CONSUMERS), nodes.Get (9 + NUM_OF_CONSUMERS));      // R8 <--> R9
    p2p.Install (nodes.Get (9 + NUM_OF_CONSUMERS), nodes.Get (13 + NUM_OF_CONSUMERS));     // R9 <--> R13
    p2p.Install (nodes.Get (9 + NUM_OF_CONSUMERS), nodes.Get (14 + NUM_OF_CONSUMERS));     // R9 <--> R14
    p2p.Install (nodes.Get (9 + NUM_OF_CONSUMERS), nodes.Get (22 + NUM_OF_CONSUMERS));     // R9 <--> R22
    p2p.Install (nodes.Get (9 + NUM_OF_CONSUMERS), nodes.Get (25 + NUM_OF_CONSUMERS));     // R9 <--> R25
    p2p.Install (nodes.Get (9 + NUM_OF_CONSUMERS), nodes.Get (27 + NUM_OF_CONSUMERS));     // R9 <--> R27
    p2p.Install (nodes.Get (10 + NUM_OF_CONSUMERS), nodes.Get (14 + NUM_OF_CONSUMERS));    // R10 <--> R14
    p2p.Install (nodes.Get (11 + NUM_OF_CONSUMERS), nodes.Get (13 + NUM_OF_CONSUMERS));    // R11 <--> R13
    p2p.Install (nodes.Get (12 + NUM_OF_CONSUMERS), nodes.Get (13 + NUM_OF_CONSUMERS));    // R12 <--> R13
    p2p.Install (nodes.Get (13 + NUM_OF_CONSUMERS), nodes.Get (14 + NUM_OF_CONSUMERS));    // R13 <--> R14
    p2p.Install (nodes.Get (13 + NUM_OF_CONSUMERS), nodes.Get (22 + NUM_OF_CONSUMERS));    // R13 <--> R22
    p2p.Install (nodes.Get (13 + NUM_OF_CONSUMERS), nodes.Get (25 + NUM_OF_CONSUMERS));    // R13 <--> R25
    p2p.Install (nodes.Get (13 + NUM_OF_CONSUMERS), nodes.Get (27 + NUM_OF_CONSUMERS));    // R13 <--> R27
    p2p.Install (nodes.Get (14 + NUM_OF_CONSUMERS), nodes.Get (15 + NUM_OF_CONSUMERS));    // R14 <--> R15
    p2p.Install (nodes.Get (14 + NUM_OF_CONSUMERS), nodes.Get (18 + NUM_OF_CONSUMERS));    // R14 <--> R18
    p2p.Install (nodes.Get (14 + NUM_OF_CONSUMERS), nodes.Get (19 + NUM_OF_CONSUMERS));    // R14 <--> R19
    p2p.Install (nodes.Get (15 + NUM_OF_CONSUMERS), nodes.Get (16 + NUM_OF_CONSUMERS));    // R15 <--> R16
    p2p.Install (nodes.Get (15 + NUM_OF_CONSUMERS), nodes.Get (19 + NUM_OF_CONSUMERS));    // R15 <--> R19
    p2p.Install (nodes.Get (15 + NUM_OF_CONSUMERS), nodes.Get (21 + NUM_OF_CONSUMERS));    // R15 <--> R21
    p2p.Install (nodes.Get (15 + NUM_OF_CONSUMERS), nodes.Get (22 + NUM_OF_CONSUMERS));    // R15 <--> R22
    p2p.Install (nodes.Get (15 + NUM_OF_CONSUMERS), nodes.Get (23 + NUM_OF_CONSUMERS));    // R15 <--> R23
    p2p.Install (nodes.Get (15 + NUM_OF_CONSUMERS), nodes.Get (25 + NUM_OF_CONSUMERS));    // R15 <--> R25
    p2p.Install (nodes.Get (15 + NUM_OF_CONSUMERS), nodes.Get (27 + NUM_OF_CONSUMERS));    // R15 <--> R27
    p2p.Install (nodes.Get (16 + NUM_OF_CONSUMERS), nodes.Get (23 + NUM_OF_CONSUMERS));    // R16 <--> R23
    p2p.Install (nodes.Get (16 + NUM_OF_CONSUMERS), nodes.Get (27 + NUM_OF_CONSUMERS));    // R16 <--> R27
    p2p.Install (nodes.Get (17 + NUM_OF_CONSUMERS), nodes.Get (23 + NUM_OF_CONSUMERS));    // R17 <--> R23
    // 18 done
    p2p.Install (nodes.Get (19 + NUM_OF_CONSUMERS), nodes.Get (22 + NUM_OF_CONSUMERS));    // R19 <--> R22
    p2p.Install (nodes.Get (20 + NUM_OF_CONSUMERS), nodes.Get (25 + NUM_OF_CONSUMERS));    // R20 <--> R25
    p2p.Install (nodes.Get (21 + NUM_OF_CONSUMERS), nodes.Get (22 + NUM_OF_CONSUMERS));    // R21 <--> R22
    p2p.Install (nodes.Get (21 + NUM_OF_CONSUMERS), nodes.Get (27 + NUM_OF_CONSUMERS));    // R21 <--> R27
    p2p.Install (nodes.Get (22 + NUM_OF_CONSUMERS), nodes.Get (23 + NUM_OF_CONSUMERS));    // R22 <--> R23
    p2p.Install (nodes.Get (22 + NUM_OF_CONSUMERS), nodes.Get (28 + NUM_OF_CONSUMERS));    // R22 <--> R28
    p2p.Install (nodes.Get (22 + NUM_OF_CONSUMERS), nodes.Get (29 + NUM_OF_CONSUMERS));    // R22 <--> R29
    p2p.Install (nodes.Get (23 + NUM_OF_CONSUMERS), nodes.Get (24 + NUM_OF_CONSUMERS));    // R23 <--> R24
    p2p.Install (nodes.Get (23 + NUM_OF_CONSUMERS), nodes.Get (25 + NUM_OF_CONSUMERS));    // R23 <--> R25
    p2p.Install (nodes.Get (23 + NUM_OF_CONSUMERS), nodes.Get (27 + NUM_OF_CONSUMERS));    // R23 <--> R27
    // 24 done
    // 25 done
    p2p.Install (nodes.Get (26 + NUM_OF_CONSUMERS), nodes.Get (27 + NUM_OF_CONSUMERS));    // R26 <--> R27
    // 27 done
    // 28 done
    // 29 done

    // Connecting producer(s)
    int producerId = 0 + NUM_OF_CONSUMERS + NUM_OF_ROUTERS;
    p2p.Install (nodes.Get (producerId), nodes.Get (0 + NUM_OF_CONSUMERS));      // P0 <--> R0

    // Install NDN stack without cache
    ndn::StackHelper ndnHelperNoCache;
    // ndnHelperNoCache.SetDefaultRoutes(true);
    ndnHelperNoCache.SetOldContentStore("ns3::ndn::cs::Nocache"); // no cache
    // Install on consumers
    for (int i = 0; i < NUM_OF_CONSUMERS; i++) {
      ndnHelperNoCache.Install(nodes.Get(i));
    }
    // Install on producer(s)
    ndnHelperNoCache.Install(nodes.Get(0 + NUM_OF_CONSUMERS + NUM_OF_ROUTERS));


    // Install NDN stack with cache
    ndn::StackHelper ndnHelperWithCache;
    // ndnHelperWithCache.SetDefaultRoutes(true);
    ndnHelperWithCache.SetOldContentStore("ns3::ndn::cs::Freshness::Lru", "MaxSize", "0");
    // Install on routers
    for (int i = NUM_OF_CONSUMERS; i < NUM_OF_CONSUMERS + NUM_OF_ROUTERS; i++) {
      ndnHelperWithCache.Install(nodes.Get(i));
      ndnHelperWithCache.InstallCallback(nodes.Get(i), (size_t)&ForwardingDelay, i);
      ndnHelperWithCache.InstallBeadDropCallback(nodes.Get(i), (size_t)&BeadDropCallback, i);
    }

    // Consumers
    ndn::AppHelper consumerHelperHonest("ns3::ndn::ConsumerCbr");
    consumerHelperHonest.SetAttribute("Frequency", StringValue("1")); // 10 interests a second
    consumerHelperHonest.SetAttribute("Randomize", StringValue("uniform"));
    consumerHelperHonest.SetAttribute("StartSeq", IntegerValue(0));
    for (int i = 0; i < NUM_OF_CONSUMERS; i++) {
      consumerHelperHonest.SetPrefix("/bead/" + std::to_string(i));
      ApplicationContainer consumer = consumerHelperHonest.Install(nodes.Get(i));
      consumer.Start(Seconds(0));
    }

    // Producer
    ndn::AppHelper producerHelper("ns3::ndn::Producer");
    producerHelper.SetPrefix("/bead");
    producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
    producerHelper.SetAttribute("Frequency", StringValue(frequency)); // 1 BEAD every second
    producerHelper.SetAttribute("Percentage", StringValue(percentage));
    producerHelper.Install(nodes.Get(producerId));

    ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
    ndnGlobalRoutingHelper.Install(nodes);

    std::string prefix = "/bead";
    ndnGlobalRoutingHelper.AddOrigins(prefix, nodes.Get(producerId));

    ndn::GlobalRoutingHelper::CalculateRoutes();

    Simulator::Stop(Seconds(simulationTime));

    ndn::AppDelayTracer::InstallAll(appDelayFile);
    ndn::L3RateTracer::InstallAll(rateTraceFile, Seconds(0.5));

    Simulator::Run();
    Simulator::Destroy();

    delayFile.flush();
    delayFile.close();
    return 0;
  }

} // namespace ns3

int
main(int argc, char* argv[])
{
  ns3::run(argc, argv);
}
