#include <stdio.h>
#include "project3.h"

extern float clocktime;
extern int TraceLevel;

struct distance_table
{
    int costs[MAX_NODES][MAX_NODES];
};
struct distance_table dt2;
struct NeighborCosts *neighbor2;

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
void printdt2(int MyNodeNumber, struct NeighborCosts *neighbor,
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
} // End of printdt2

void justPrintDt2(struct distance_table dtptr)
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

void rtinit2()
{
    struct NeighborCosts *node2Costs;
    node2Costs = getNeighborCosts(2);

    // fill the dt struct with infinities to begin with.
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            dt2.costs[i][j] = INFINITY;
        }
    }

    for (int i = 0; i < 4; i++)
    {
        dt2.costs[2][i] = node2Costs->NodeCosts[i];
    }

    struct RoutePacket minCostUpdate;

    for (int i = 0; i < 4; i++)
    {
        minCostUpdate.mincost[i] = INFINITY; // default mincost
    }
    for (int i = 0; i < 4; i++)
    {
        if (dt2.costs[2][i] < minCostUpdate.mincost[i])
        {
            minCostUpdate.mincost[i] = dt2.costs[2][i];
        }
    }

    minCostUpdate.sourceid = 2;
    minCostUpdate.destid = 0;
    toLayer2(minCostUpdate); // send update to node 0
    minCostUpdate.destid = 1;
    toLayer2(minCostUpdate); // send update to node 1
    minCostUpdate.destid = 3;
    toLayer2(minCostUpdate); // send update to node 3
    // printdt2(2, node2Costs, &dt2);
}

int isConnected2(int node1, int node2)
{
    struct NeighborCosts *nodeCosts = getNeighborCosts(node1);

    if (nodeCosts->NodeCosts[node2] != INFINITY)
    {
        return 1;
    }

    return 0;
}

void rtupdate2(struct RoutePacket *rcvdpkt)
{
    printf("\nrtpdate2 invoked at time %f \n", clocktime);
    int sourceID = rcvdpkt->sourceid;
    int updateMade = 0;

    for (int i = 0; i < 4; i++)
    {
        if (dt2.costs[i][sourceID] > dt2.costs[2][sourceID] + rcvdpkt->mincost[i])
        {
            dt2.costs[i][sourceID] = dt2.costs[2][sourceID] + rcvdpkt->mincost[i];
            updateMade = 1;
        }
    }

    if (updateMade == 1)
    {
        printf("\nDistance table at node 2 updated: \n");
        printdt2(2, getNeighborCosts(2), &dt2);

        for (int i = 0; i < 4; i++)
        {
            if (i != 2)
            {
                int min_cost = INFINITY;
                for (int j = 0; j < 4; j++)
                {
                    if (!isConnected2(i, j))
                        continue;
                    if (dt2.costs[i][j] < min_cost)
                    {
                        min_cost = dt2.costs[i][j];
                    }
                }
                dt2.costs[i][2] = min_cost;
            }
        }

        struct RoutePacket pkt0, pkt1, pkt3;
        pkt0.sourceid = 2;
        pkt0.destid = 0;
        pkt1.sourceid = 2;
        pkt1.destid = 1;
        pkt3.sourceid = 2;
        pkt3.destid = 3;
        pkt0.mincost[2] = 0;
        pkt1.mincost[2] = 0;
        pkt3.mincost[2] = 0;

        for (int i = 0; i < 4; i++)
        {
            if (i != 2)
            {
                pkt0.mincost[i] = dt2.costs[i][2];
                pkt1.mincost[i] = dt2.costs[i][2];
                pkt3.mincost[i] = dt2.costs[i][2];
            }
        }

        printf("\nSending routing packets to nodes 0, 1, and 3 with following mincost vector: \n");
        for (int i = 0; i < 4; i++)
        {
            printf("%d\t", pkt0.mincost[i]);
        }
        printf("\n");

        for (int i = 0; i < 4; i++)
        {
            if (i == 2)
                continue; // skip itself
            if (isConnected2(2, i))
            {
                struct RoutePacket pkt;
                pkt.sourceid = 2;
                pkt.destid = i;
                pkt.mincost[2] = 0;

                for (int j = 0; j < 4; j++)
                {
                    if (j == 2)
                        continue;
                    pkt.mincost[j] = dt2.costs[j][2];
                }

                toLayer2(pkt);
            }
        }
    }
}
