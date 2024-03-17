#include <stdio.h>
#include "project3.h"

extern float clocktime;
extern int TraceLevel;

struct distance_table
{
    int costs[MAX_NODES][MAX_NODES];
};
struct distance_table dt3;
struct NeighborCosts *neighbor3;

/////////////////////////////////////////////////////////////////////
//  printdt
//  This routine is being supplied to you.  It is the same code in
//  each node and is tailored based on the input arguments.
//  Required arguments:
//  MyNodeNumber:  This routine assumes that you know your node
//                 number and supply it when making this call.
//  struct NeighborCosts *neighbor:  A pointer to the structure
//                 that's supplied via a call to getNeighborCosts().
//                 It tells this print routine the configuration
//                 of nodes surrounding the node we're working on.
//  struct distance_table *dtptr: This is the running record of the
//                 current costs as seen by this node.  It is
//                 constantly updated as the node gets new
//                 messages from other nodes.
/////////////////////////////////////////////////////////////////////
void printdt3(int MyNodeNumber, struct NeighborCosts *neighbor,
              struct distance_table *dtptr)
{
    int i, j;
    int TotalNodes = neighbor->NodesInNetwork; // Total nodes in network
    int NumberOfNeighbors = 0;                 // How many neighbors
    int Neighbors[MAX_NODES];                  // Who are the neighbors

    // Determine our neighbors
    for (i = 0; i < TotalNodes; i++)
    {
        if ((neighbor->NodeCosts[i] != INFINITY) && i != MyNodeNumber)
        {
            Neighbors[NumberOfNeighbors] = i;
            NumberOfNeighbors++;
        }
    }
    // Print the header
    printf("                via     \n");
    printf("   D%d |", MyNodeNumber);
    for (i = 0; i < NumberOfNeighbors; i++)
        printf("     %d", Neighbors[i]);
    printf("\n");
    printf("  ----|-------------------------------\n");

    // For each node, print the cost by travelling thru each of our neighbors
    for (i = 0; i < TotalNodes; i++)
    {
        if (i != MyNodeNumber)
        {
            printf("dest %d|", i);
            for (j = 0; j < NumberOfNeighbors; j++)
            {
                printf("  %4d", dtptr->costs[i][Neighbors[j]]);
            }
            printf("\n");
        }
    }
    printf("\n");
} // End of printdt3

void justPrintDt3(struct distance_table dtptr)
{
    printf("         to      \n");
    printf("       0|1|2|3 \n");
    for (int i = 0; i < 4; i++)
    {
        printf("from:%d|", i);
        for (int j = 0; j < 4; j++)
        {
            printf("%d|", dtptr.costs[i][j]);
        }
        printf("\n");
    }
}

/* students to write the following two routines, and maybe some others */

void rtinit3()
{
    struct NeighborCosts *node3Costs;
    node3Costs = getNeighborCosts(3);

    // fill the dt struct with infinities to begin with.
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            dt3.costs[i][j] = INFINITY;
        }
    }

    for (int i = 0; i < 4; i++)
    {
        // printf("%d", node0Costs->NodeCosts[i]);
        dt3.costs[3][i] = node3Costs->NodeCosts[i];
    }

    struct RoutePacket minCostUpdate;

    for (int i = 0; i < 4; i++)
    {
        minCostUpdate.mincost[i] = INFINITY; // default mincost
    }
    for (int i = 0; i < 4; i++)
    {
        if (dt3.costs[3][i] < minCostUpdate.mincost[i])
        {
            minCostUpdate.mincost[i] = dt3.costs[3][i];
        }
    }

    minCostUpdate.sourceid = 3;
    minCostUpdate.destid = 0;
    toLayer2(minCostUpdate); // send update to node 0
    minCostUpdate.destid = 2;
    toLayer2(minCostUpdate); // send update to node 2
    // printdt3(3, node3Costs, &dt3);
}

int isConnected3(int node1, int node2)
{
    struct NeighborCosts *nodeCosts = getNeighborCosts(node1);

    if (nodeCosts->NodeCosts[node2] != INFINITY)
    {
        return 1;
    }

    return 0;
}

void rtupdate3(struct RoutePacket *rcvdpkt)
{
    printf("\nrtpdate3 invoked at time %f \n", clocktime);
    int sourceID = rcvdpkt->sourceid;
    int updateMade = 0;

    for (int i = 0; i < 4; i++)
    {
        if (dt3.costs[i][sourceID] > dt3.costs[3][sourceID] + rcvdpkt->mincost[i])
        {
            dt3.costs[i][sourceID] = dt3.costs[3][sourceID] + rcvdpkt->mincost[i];
            updateMade = 1;
        }
    }

    if (updateMade == 1)
    {
        printf("\nDistance table at node 3 updated: \n");
        printdt3(3, getNeighborCosts(3), &dt3);

        for (int i = 0; i < 4; i++)
        {
            if (i != 3)
            {
                int min_cost = INFINITY;
                for (int j = 0; j < 4; j++)
                {
                    if (!isConnected3(i, j))
                        continue;
                    if (dt3.costs[i][j] < min_cost)
                    {
                        min_cost = dt3.costs[i][j];
                    }
                }
                dt3.costs[i][3] = min_cost;
            }
        }

        struct RoutePacket pkt0, pkt2;
        pkt0.sourceid = 3;
        pkt0.destid = 0;
        // pkt1.sourceid = 3;
        // pkt1.destid = 1;
        pkt2.sourceid = 3;
        pkt2.destid = 2;
        pkt0.mincost[3] = 0;
        // pkt1.mincost[3] = 0;
        pkt2.mincost[3] = 0;

        for (int i = 0; i < 4; i++)
        {
            if (i != 3)
            {
                pkt0.mincost[i] = dt3.costs[i][3];
                // pkt1.mincost[i] = dt3.costs[i][3];
                pkt2.mincost[i] = dt3.costs[i][3];
            }
        }

        printf("\nSending routing packets to nodes 0, 1, and 2 with following mincost vector: \n");
        for (int i = 0; i < 4; i++)
        {
            printf("%d\t", pkt0.mincost[i]);
        }
        printf("\n");

        for (int i = 0; i < 4; i++)
        {
            if (i == 3)
                continue; // skip itself
            if (isConnected3(3, i))
            {
                struct RoutePacket pkt;
                pkt.sourceid = 3;
                pkt.destid = i;
                pkt.mincost[3] = 0;

                for (int j = 0; j < 4; j++)
                {
                    if (j == 3)
                        continue;
                    pkt.mincost[j] = dt3.costs[j][3];
                }

                toLayer2(pkt);
            }
        }
    }
}
