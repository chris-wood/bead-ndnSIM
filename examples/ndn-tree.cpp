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

// ndn-tree-app-delay-tracer.cpp

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {

    ofstream delayFile;

    void
    ForwardingDelay(ns3::Time eventTime, float delay, double size)
    {
        delayFile << eventTime.GetNanoSeconds() << "\t" << delay * 1000000000 << "\t" << size << "\n";
    }

/**
 * This scenario simulates a tree topology (using topology reader module)
 *
 *    /------\      /------\      /------\      /------\
 *    |leaf-1|      |leaf-2|      |leaf-3|      |leaf-4|
 *    \------/      \------/      \------/      \------/
 *         ^          ^                ^           ^
 *         |          |                |           |
 *          \        /                  \         /
 *           \      /                    \       /    10Mbps / 1ms
 *            \    /                      \     /
 *             |  |                        |   |
 *             v  v                        v   v
 *          /-------\                    /-------\
 *          | rtr-1 |                    | rtr-2 |
 *          \-------/                    \-------/
 *                ^                        ^
 *                |                        |
 *                 \                      /  10 Mpbs / 1ms
 *                  +--------+  +--------+
 *                           |  |
 *                           v  v
 *                        /--------\
 *                        |  root  |
 *                        \--------/
 *
 *
 * To run scenario and see what is happening, use the following command:
 *
 *     ./waf --run=ndn-tree
 */

int
main(int argc, char* argv[])
{
  CommandLine cmd;

  std::string fof;
  std::string topologyFile; //  "src/ndnSIM/examples/topologies/topo-tree.txt"
  std::string appDelayFile; // "app-delays-trace.txt"
  std::string rateTraceFile; // "rate-trace.txt"
  int simulationTime = 0;

  cmd.AddValue("fof", "forwarder overhead file", fof);
  cmd.AddValue("time", "simulation time argument", simulationTime);
  cmd.AddValue("top", "topology file", topologyFile);
  cmd.AddValue("appd", "app delay file", appDelayFile);
  cmd.AddValue("rate", "rate trace file", rateTraceFile);

  cmd.Parse(argc, argv);

  delayFile.open(fof);

  AnnotatedTopologyReader topologyReader("", 1);
  topologyReader.SetFileName(topologyFile);
  topologyReader.Read();

  ndn::StackHelper ndnHelper;
  ndnHelper.InstallAll();

  // Getting containers for the consumer/producer
  Ptr<Node> consumers[4] = {Names::Find<Node>("leaf-1"), Names::Find<Node>("leaf-2"),
                            Names::Find<Node>("leaf-3"), Names::Find<Node>("leaf-4")};
  Ptr<Node> routers[2] = {Names::Find<Node>("rtr-1"), Names::Find<Node>("rtr-2")};
  Ptr<Node> producer = Names::Find<Node>("root");

  // Choosing forwarding strategy
  ndn::StrategyChoiceHelper::InstallAll("/root", "/localhost/nfd/strategy/best-route");

  // Installing global routing interface on all nodes
  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();

  // Install NDN on routers
  ndn::StackHelper ndnHelperWithCache;
  ndnHelperWithCache.SetDefaultRoutes(true);
  ndnHelperWithCache.SetOldContentStore("ns3::ndn::cs::Freshness::Lru", "MaxSize", "0");
  ndnHelperWithCache.InstallCallback(routers[0], (size_t)&ForwardingDelay, 0);
  ndnHelperWithCache.InstallCallback(routers[1], (size_t)&ForwardingDelay, 1);

  ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
  // Consumer will request /prefix/0, /prefix/1, ...
  consumerHelper.SetPrefix("/root");
  consumerHelper.SetAttribute("Frequency", StringValue("10")); // 10 interests a second
  consumerHelper.Install(consumers[0]);
  consumerHelper.Install(consumers[1]);
  consumerHelper.Install(consumers[2]);

  ndn::AppHelper producerHelper("ns3::ndn::Producer");
  producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
  producerHelper.SetAttribute("Frequency", StringValue("1")); // 1 BEAD every second

  // Register /root prefix with global routing controller and
  // install producer that will satisfy Interests in /root namespace
  ndnGlobalRoutingHelper.AddOrigins("/root", producer);
  producerHelper.SetPrefix("/root");
  producerHelper.Install(producer).Start(Seconds(9));

  // Calculate and install FIBs
  ndn::GlobalRoutingHelper::CalculateRoutes();

  Simulator::Stop(Seconds(simulationTime));

  ndn::AppDelayTracer::InstallAll(appDelayFile);
  ndn::L3RateTracer::InstallAll(rateTraceFile, Seconds(0.5));

  Simulator::Run();
  Simulator::Destroy();

  delayFile.close();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
