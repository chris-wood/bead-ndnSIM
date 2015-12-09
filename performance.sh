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
PERCENTAGES=( 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0 )
FREQ=( 1 10 100 1000 )

TIME=50

for P in "${PERCENTAGES[@]}"
do
    for F in "${FREQ[@]}"
    do
        fileFOF=att-${FOF}_${P}_${F}
        fileAPPD=att-${APPD}_${P}_${F}
        fileRATE=att-${RATE}_${P}_${F}
        fileDROP=att-${DROP}_${P}_${F}
        echo ./waf --run=\"ndn-bead-att --time=$TIME --fof=$fileFOF --appd=$fileAPPD --rate=$fileRATE --percentage=$P --freq=$F --drop=$fileDROP\"
        ./waf --run="ndn-bead-att --time=$TIME --fof=$fileFOF --appd=$fileAPPD --rate=$fileRATE --percentage=$P --freq=$F --drop=$fileDROP"

        fileFOF=dfn-${FOF}_${P}_${F}
        fileAPPD=dfn-${APPD}_${P}_${F}
        fileRATE=dfn-${RATE}_${P}_${F}
        fileDROP=dfn-${DROP}_${P}_${F}
        echo ./waf --run=\"ndn-bead-dfn --time=$TIME --fof=$fileFOF --appd=$fileAPPD --rate=$fileRATE --percentage=$P --freq=$F --drop=$fileDROP\"
        ./waf --run="ndn-bead-dfn --time=$TIME --fof=$fileFOF --appd=$fileAPPD --rate=$fileRATE --percentage=$P --freq=$F --drop=$fileDROP"
    done
done
