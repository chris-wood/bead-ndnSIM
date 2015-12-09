#!/bin/bash

# cmd.AddValue("fof", "forwarder overhead file", fof);
# cmd.AddValue("time", "simulation time argument", simulationTime);
# cmd.AddValue("top", "topology file", topologyFile);
# cmd.AddValue("appd", "app delay file", appDelayFile);
# cmd.AddValue("rate", "rate trace file", rateTraceFile);
# cmd.AddValue("percentage", "bead percentage", percentage);
# cmd.AddValue("freq", "bead frequency", frequency);

FOF="fof"
TOP="src/ndnSIM/examples/topologies/topo-tree.txt"
APPD="app-delays-trace"
RATE="rate-trace"
DROP="bead-drops"
PERCENTAGES=( 0.25 0.5 0.75 1.0 )
FREQ=( 10 100 1000 )
HSIZE=( 512 2048 8192 )

TIME=1000

for P in "${PERCENTAGES[@]}"
do
    for F in "${FREQ[@]}"
    do
        for H in "${HSIZE[@]}"
        do
            fileFOF=pen-${FOF}_${P}_${F}_${H}
            fileAPPD=pen-${APPD}_${P}_${F}_${H}
            fileRATE=pen-${RATE}_${P}_${F}_${H}
            fileDROP=pen-${DROP}_${P}_${F}_${H}

            echo ./waf --run=\"ndn-tree-with-l2tracer --top=src/ndnSIM/examples/topologies/topo-tree-25-node.txt --hsize=$H --time=$TIME --fof=$fileFOF --appd=$fileAPPD --rate=$fileRATE --percentage=$P --freq=$F --drop=$fileDROP\"
            ./waf --run="ndn-tree-with-l2tracer --top=src/ndnSIM/examples/topologies/topo-tree-25-node.txt --hsize=$H --time=$TIME --fof=$fileFOF --appd=$fileAPPD --rate=$fileRATE --percentage=$P --freq=$F --drop=$fileDROP"
        done
    done
done
