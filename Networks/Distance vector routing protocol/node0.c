#include <stdio.h>
#include "project3.h"

extern float clocktime;
extern int TraceLevel;
// max nodes is 4
struct distance_table
{
    int costs[MAX_NODES][MAX_NODES];
};
struct distance_table dt0; // defined our 4x4 table
struct NeighborCosts *neighbor0;

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
void printdt0(int MyNodeNumber, struct NeighborCosts *neighbor,
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
} // End of printdt0

void justPrintDt0(struct distance_table dtptr)
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

//|0|1|3|7|
//
//  |  0  |  1  |  2  |  3  |
// 0|{0,0}|{0,1}|{0,2}|{0,3}|
// 1|{1,0}|{1,1}|{1,2}|{1,3}|
// 2|{2,0}|{2,1}|{2,2}|{2,3}|
// 3|{3,0}|{3,1}|{3,2}|{3,3}|

void rtinit0()
{
    struct NeighborCosts *node0Costs;
    node0Costs = getNeighborCosts(0);

    // fill the dt struct with infinities to begin with.
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            dt0.costs[i][j] = INFINITY;
        }
    }

    for (int i = 0; i < 4; i++)
    {
        dt0.costs[0][i] = node0Costs->NodeCosts[i];
    }

    struct RoutePacket minCostUpdate;

    for (int i = 0; i < 4; i++)
    {
        minCostUpdate.mincost[i] = INFINITY; // default mincost
    }

    for (int i = 0; i < 4; i++)
    {
        if (dt0.costs[0][i] < minCostUpdate.mincost[i])
        {
            minCostUpdate.mincost[i] = dt0.costs[0][i];
        }
    }

    minCostUpdate.sourceid = 0;
    for (int i = 1; i < 4; i++)
    {
        if (node0Costs->NodeCosts[i] != INFINITY)
        {
            minCostUpdate.destid = i;
            toLayer2(minCostUpdate); // Send update to the connected neighbor node
        }
    }
}

int isConnected0(int node1, int node2)
{
    struct NeighborCosts *nodeCosts = getNeighborCosts(node1);

    if (nodeCosts->NodeCosts[node2] != INFINITY)
    {
        return 1;
    }

    return 0;
}

void rtupdate0(struct RoutePacket *rcvdpkt)
{
    printf("\nrtpdate0 invoked at time %f \n", clocktime);
    int sourceID = rcvdpkt->sourceid;
    int updateMade = 0;

    for (int i = 0; i < 4; i++)
    {
        if (dt0.costs[i][sourceID] > dt0.costs[0][sourceID] + rcvdpkt->mincost[i])
        {
            dt0.costs[i][sourceID] = dt0.costs[0][sourceID] + rcvdpkt->mincost[i];
            updateMade = 1;
        }
    }

    if (updateMade == 1)
    {
        printf("\nDistance table at node 0 updated: \n");
        printdt0(0, getNeighborCosts(0), &dt0);

        for (int i = 1; i < 4; i++)
        {
            int min_cost = INFINITY;
            for (int j = 0; j < 4; j++)
            {
                if (!isConnected0(i, j))
                    continue;
                if (dt0.costs[i][j] < min_cost)
                {
                    min_cost = dt0.costs[i][j];
                }
            }
            dt0.costs[i][0] = min_cost;
        }

        struct RoutePacket pkt1, pkt2, pkt3;
        pkt1.sourceid = 0;
        pkt1.destid = 1;
        pkt2.sourceid = 0;
        pkt2.destid = 2;
        pkt3.sourceid = 0;
        pkt3.destid = 3;
        pkt1.mincost[0] = 0;
        pkt2.mincost[0] = 0;
        pkt3.mincost[0] = 0;

        for (int i = 1; i < 4; i++)
        {
            pkt1.mincost[i] = dt0.costs[i][0];
            pkt2.mincost[i] = dt0.costs[i][0];
            pkt3.mincost[i] = dt0.costs[i][0];
        }

        printf("\nSending routing packets to linked nodes with mincost vector: \n");
        for (int i = 0; i < 4; i++)
        {
            printf("%d\t", pkt1.mincost[i]);
        }
        printf("\n");

        for (int i = 1; i < 4; i++)
        {
            if (isConnected0(0, i))
            {
                struct RoutePacket pkt;
                pkt.sourceid = 0;
                pkt.destid = i;
                pkt.mincost[0] = 0;

                for (int j = 1; j < 4; j++)
                {
                    pkt.mincost[j] = dt0.costs[j][0];
                }

                toLayer2(pkt);
            }
        }
    }
}