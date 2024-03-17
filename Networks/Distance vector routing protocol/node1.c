#include <stdio.h>
#include "project3.h"

extern float clocktime;
extern int TraceLevel;

struct distance_table
{
    int costs[MAX_NODES][MAX_NODES];
};
struct distance_table dt1;
struct NeighborCosts *neighbor1;

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
void printdt1(int MyNodeNumber, struct NeighborCosts *neighbor,
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
} // End of printdt1

void justPrintDt1(struct distance_table dtptr)
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

void rtinit1()
{
    struct NeighborCosts *node1Costs;
    node1Costs = getNeighborCosts(1);

    // fill the dt struct with infinities to begin with.
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            dt1.costs[i][j] = INFINITY;
        }
    }

    for (int i = 0; i < 4; i++)
    {
        dt1.costs[1][i] = node1Costs->NodeCosts[i];
    }

    struct RoutePacket minCostUpdate;

    for (int i = 0; i < 4; i++)
    {
        minCostUpdate.mincost[i] = INFINITY; // default mincost
    }
    for (int i = 0; i < 4; i++)
    {
        if (dt1.costs[1][i] < minCostUpdate.mincost[i])
        {
            minCostUpdate.mincost[i] = dt1.costs[1][i];
        }
    }

    minCostUpdate.sourceid = 1;
    minCostUpdate.destid = 0;
    toLayer2(minCostUpdate); // send update to node 0
    minCostUpdate.destid = 2;
    toLayer2(minCostUpdate); // send update to node 2
    // printdt1(1, node1Costs, &dt1);
}

int isConnected1(int node1, int node2)
{
    struct NeighborCosts *nodeCosts = getNeighborCosts(node1);

    if (nodeCosts->NodeCosts[node2] != INFINITY)
    {
        return 1;
    }

    return 0;
}

void rtupdate1(struct RoutePacket *rcvdpkt)
{
    printf("\nrtpdate1 invoked at time %f \n", clocktime);
    int sourceID = rcvdpkt->sourceid;
    int updateMade = 0;

    for (int i = 0; i < 4; i++)
    {
        if (dt1.costs[i][sourceID] > dt1.costs[1][sourceID] + rcvdpkt->mincost[i])
        {
            dt1.costs[i][sourceID] = dt1.costs[1][sourceID] + rcvdpkt->mincost[i];
            updateMade = 1;
        }
    }

    if (updateMade == 1)
    {
        printf("\nDistance table at node 1 updated: \n");
        printdt1(1, getNeighborCosts(1), &dt1);

        for (int i = 0; i < 4; i++)
        {
            if (i != 1)
            {
                int min_cost = INFINITY;
                for (int j = 0; j < 4; j++)
                {
                    if (!isConnected1(i, j))
                        continue;
                    if (dt1.costs[i][j] < min_cost)
                    {
                        min_cost = dt1.costs[i][j];
                    }
                }
                dt1.costs[i][1] = min_cost;
            }
        }

        struct RoutePacket pkt0, pkt2;
        pkt0.sourceid = 1;
        pkt0.destid = 0;
        pkt2.sourceid = 1;
        pkt2.destid = 2;
        pkt0.mincost[1] = 0;
        pkt2.mincost[1] = 0;

        for (int i = 0; i < 4; i++)
        {
            if (i != 1)
            {
                pkt0.mincost[i] = dt1.costs[i][1];
                pkt2.mincost[i] = dt1.costs[i][1];
            }
        }

        printf("\nSending routing packets to nodes 0, 2, and 3 with following mincost vector: \n");
        for (int i = 0; i < 4; i++)
        {
            printf("%d\t", pkt0.mincost[i]);
        }
        printf("\n");

        for (int i = 0; i < 4; i++)
        {
            if (i == 1)
                continue; // skip itself
            if (isConnected1(1, i))
            {
                struct RoutePacket pkt;
                pkt.sourceid = 1;
                pkt.destid = i;
                pkt.mincost[1] = 0;

                for (int j = 0; j < 4; j++)
                {
                    if (j == 1)
                        continue;
                    pkt.mincost[j] = dt1.costs[j][1];
                }

                toLayer2(pkt);
            }
        }
    }
}