/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2011-2015  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

// ndn-simple-withl2tracer.cpp

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {
    ofstream delayFile;

    void
    ForwardingDelay(size_t id, ns3::Time eventTime, float delay, double size)
    {
        // std::cout << "FWD DELAY: " << eventTime.GetNanoSeconds() << "\t" << delay * 1000000000 << "\t" << size << "\n";
        delayFile << id << "\t" << eventTime.GetNanoSeconds() << "\t" << delay * 1000000000 << "\t" << size << "\n";
    }

    ofstream dropFile;

    void
    BeadDropCallback(int id, uint64_t hops)
    {
      std::cout << "BEAD DROP: " << id << "\t" << hops << "\n";
      dropFile << id << "\t" << hops << "\n";
    }

int
main(int argc, char* argv[])
{
    std::string fof = "fof.txt";
    std::string dropFileName = "drops.txt";
    std::string topologyFile = "src/ndnSIM/examples/topologies/topo-tree.txt";
    std::string appDelayFile = "app-delays-trace.txt";
    std::string rateTraceFile = "rate-trace.txt";
    std::string percentage = "0.1";
    std::string frequency = "1";
    int simulationTime = 1000;
    int historySize = 100;

    CommandLine cmd;
    cmd.AddValue("fof", "forwarder overhead file", fof);
    cmd.AddValue("time", "simulation time argument", simulationTime);
    cmd.AddValue("top", "topology file", topologyFile);
    cmd.AddValue("drop", "bead drop file", dropFileName);
    cmd.AddValue("appd", "app delay file", appDelayFile);
    cmd.AddValue("rate", "rate trace file", rateTraceFile);
    cmd.AddValue("percentage", "bead percentage", percentage);
    cmd.AddValue("freq", "bead frequency", frequency);
    cmd.AddValue("hsize", "history size", historySize);
    cmd.Parse(argc, argv);

  AnnotatedTopologyReader topologyReader("", 24);
  topologyReader.SetFileName("src/ndnSIM/examples/topologies/topo-tree-25-node.txt");
  topologyReader.Read();

  /****************************************************************************/
  // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
  ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "0");
  ndnHelper.InstallAll();
  /****************************************************************************/
  // Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();
  /****************************************************************************/
  // Getting containers for the consumer/producer
  Ptr<Node> consumer1 = Names::Find<Node>("Src1");
  Ptr<Node> consumer2 = Names::Find<Node>("Src2");
  Ptr<Node> consumer3 = Names::Find<Node>("Src3");
  Ptr<Node> consumer4 = Names::Find<Node>("Src4");
  Ptr<Node> consumer5 = Names::Find<Node>("Src5");
  Ptr<Node> consumer6 = Names::Find<Node>("Src6");
  Ptr<Node> consumer7 = Names::Find<Node>("Src7");
  Ptr<Node> consumer8 = Names::Find<Node>("Src8");

  Ptr<Node> routers[15] = {
    Names::Find<Node>("Rtr1"),
    Names::Find<Node>("Rtr2"),
    Names::Find<Node>("Rtr3"),
    Names::Find<Node>("Rtr4"),
    Names::Find<Node>("Rtr5"),
    Names::Find<Node>("Rtr6"),
    Names::Find<Node>("Rtr7"),
    Names::Find<Node>("Rtr8"),
    Names::Find<Node>("Rtr9"),
    Names::Find<Node>("Rtr10"),
    Names::Find<Node>("Rtr11"),
    Names::Find<Node>("Rtr12"),
    Names::Find<Node>("Rtr13"),
    Names::Find<Node>("Rtr14"),
    Names::Find<Node>("Rtr15")};

  ndn::StackHelper ndnHelperWithCache;
  ndnHelperWithCache.SetDefaultRoutes(true);
  ndnHelperWithCache.SetOldContentStore("ns3::ndn::cs::Freshness::Lru", "MaxSize", "0");
  for (int i = 0; i < 15; i++) {
      ndnHelperWithCache.InstallCallback(routers[i], (size_t)&ForwardingDelay, i);
      ndnHelperWithCache.InstallBeadDropCallback(routers[i], (size_t)&BeadDropCallback, i);
      ndnHelperWithCache.SetUseHistory(routers[i], historySize);
  }

  Ptr<Node> producer1 = Names::Find<Node>("Dst1");
  ndnHelperWithCache.InstallCallback(producer1, (size_t)&ForwardingDelay, 16);
  ndnHelperWithCache.InstallBeadDropCallback(producer1, (size_t)&BeadDropCallback, 16);
  ndnHelperWithCache.SetUseHistory(producer1, historySize);
  /****************************************************************************/
  ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
  consumerHelper.SetAttribute("Frequency", StringValue("10")); // interests per Second
  consumerHelper.SetAttribute("Randomize", StringValue("uniform"));
  /****************************************************************************/
  // on the first to ninth consumer node install a Consumer application
  // that will express interests in /dst1 to /dst9 namespace
  consumerHelper.SetPrefix("/dst1");
  consumerHelper.Install(consumer1);

  consumerHelper.SetPrefix("/dst1");
  consumerHelper.Install(consumer2);

  consumerHelper.SetPrefix("/dst1");
  consumerHelper.Install(consumer3);

  consumerHelper.SetPrefix("/dst1");
  consumerHelper.Install(consumer4);

  consumerHelper.SetPrefix("/dst1");
  consumerHelper.Install(consumer5);

  consumerHelper.SetPrefix("/dst1");
  consumerHelper.Install(consumer6);

  consumerHelper.SetPrefix("/dst1");
  consumerHelper.Install(consumer7);

  consumerHelper.SetPrefix("/dst1");
  consumerHelper.Install(consumer8);

  /****************************************************************************/
  ndn::AppHelper producerHelper("ns3::ndn::Producer");
  producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
  producerHelper.SetAttribute("Frequency", StringValue(frequency)); // 1 BEAD every second
  producerHelper.SetAttribute("Percentage", StringValue(percentage));
  /****************************************************************************/
  // Register /dst1 to /dst9 prefix with global routing controller and
  // install producer that will satisfy Interests in /dst1 to /dst9 namespace
  ndnGlobalRoutingHelper.AddOrigins("/dst1", producer1);
  producerHelper.SetPrefix("/dst1");
  producerHelper.Install(producer1);

  /*****************************************************************************/
  // Calculate and install FIBs
  ndn::GlobalRoutingHelper::CalculateRoutes();

  Simulator::Stop(Seconds(simulationTime));

  /****************************************************************************/
  // Tracer:

  L2RateTracer::InstallAll("drop-trace.txt", Seconds(0.5));

  Simulator::Run();
  Simulator::Destroy();

  delayFile.flush();
  delayFile.close();

  dropFile.flush();
  dropFile.close();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
