#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project2.h"
#include <unistd.h>

/* ***************************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for unidirectional or bidirectional
   data transfer protocols from A to B and B to A.
   Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets may be delivered out of order.

   Compile as gcc -g project2.c student2.c -o p2
**********************************************************************/

// Struct for pkt and whatnot is already defined in project2.h so look there!

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
/*
 * The routines you will write are detailed below. As noted above,
 * such procedures in real-life would be part of the operating system,
 * and would be called by other procedures in the operating system.
 * All these routines are in layer 4.
 */

struct bufferLL {
  char message[20];
  struct bufferLL *next;
};

struct bufferLL *head = NULL;
struct bufferLL *current = NULL;

void insertAtFront(struct msg message){
  //create link
  struct bufferLL *lk = (struct bufferLL*) malloc(sizeof(struct bufferLL));
  strcpy(lk->message, message.data);

   // point it to old first node
   lk->next = head;

   //point first to new first node
   head = lk;
}

void insertAtEnd(struct msg message){

   //create a link
   struct bufferLL *lk = (struct bufferLL*) malloc(sizeof(struct bufferLL));
   strcpy(lk->message, message.data);
   lk->next = NULL;

   if (head == NULL) { // first node to be inserted
       head = lk;
       return;
   }

   struct bufferLL *linkedlist = head;

   // point it to last node
   while(linkedlist->next != NULL)
      linkedlist = linkedlist->next;

   //point last node to new node
   linkedlist->next = lk;
}


void deleteatbegin(){
   head = head->next;
}

void deleteatend(){
   struct bufferLL *linkedlist = head;
   while (linkedlist->next->next != NULL)
      linkedlist = linkedlist->next;
   linkedlist->next = NULL;
}

enum SenderState {
    READYFORNEXT,
    NOTREADYFORNEXT
};

struct Sender {
    enum SenderState state;
    int seq;
    float estimated_rtt;
    struct pkt last_packet;
} A;

struct Receiver {
    int seq;
} B;

int get_checksum(struct pkt *packet) {
    int checksum = 0;
    checksum += packet->seqnum;
    checksum += packet->acknum;
    for (int i = 0; i < 20; ++i)
        checksum += packet->payload[i];
    return checksum;
}

/* called from layer 5, passed the data to be sent to other side */

void A_output(struct msg message) {
    if (A.state == NOTREADYFORNEXT)
    { // make sure we are ready for the next message
    printf("\nThe output for A_output hasn't been acknowledged yet, waiting...\n");
    }

    printf("\nA_output is sending packet with data: %s\n", message.data);
    struct pkt packet;
    packet.seqnum = A.seq;
    memcpy(packet.payload, message.data, 20);
    packet.checksum = get_checksum(&packet);
    A.last_packet = packet;
    A.state = NOTREADYFORNEXT;
    tolayer3(0, packet);
    startTimer(0, A.estimated_rtt);
}

/*
void A_output(struct msg message)
{
  insertAtEnd(message); 
  if (A.state == NOTREADYFORNEXT)
  { // make sure we are ready for the next message
    printf("\nThe output for A_output hasn't been acknowledged yet, waiting...\n");
  }
  else if(A.state == READYFORNEXT){
    struct pkt packet;                         // declare packet space to be used; seqnum, acknum, checksum, payload
    if (head != NULL) {
      memcpy(packet.payload, head->message, 20); // set the packet payload to the first message in the buffer
      packet.seqnum = A.seq;                     // set created packets sequence num
      packet.checksum = get_checksum(&packet); // relies on other data, assign checksum
      A.last_packet = packet;
      A.state = NOTREADYFORNEXT;             // set state of A, needs to wait for an acknowledgment
      tolayer3(0, packet);            // send it down the pipeline
      startTimer(0, A.estimated_rtt); // start the timer for A and store in estimated_rtt
    }
    else{
      printf("\nNo messages in buffer to send.\n");
      return;
    }
  }
  else{
    printf("Shouldn't get here.");
  }
}
*/
void B_output(struct msg message) {
}

/* called from layer 3, when a packet arrives for layer 4 */
//I couldn't get this to work with the queue :/
/*
void A_input(struct pkt packet) {
    if (packet.checksum != get_checksum(&packet))
  {// corrupt ack or NAK
    printf("\nPacket corrupt, waiting for timer interrupt...\n");
    //printf("\n--------------------------------------------------------------------------------------\n");
		//printf("Checksum:%d \nGenerated chksum:%d", packet.checksum, get_checksum(&packet));
		//printf("\n--------------------------------------------------------------------------------------\n");
    return;
  }

    if (packet.acknum != A.seq) {
      printf("\nWrong acknum, waiting for timer interrupt...\n");
      //printf("\n--------------------------------------------------------------------------------------\n");
		  //printf("Packet acknum: %d\nExpected acknum: %d", packet.acknum, A.seq);
		  //printf("\n--------------------------------------------------------------------------------------\n");
    return;
    }
    printf("\nSuccessfully received ACK packet!");
    //printf("\n--------------------------------------------------------------------------------------\n");
		//printf("Checksum:%d \nGenerated chksum:%d\nPacket acknum: %d\nExpected acknum: %d", packet.checksum, get_checksum(&packet), packet.acknum, A.seq);
		//printf("\n--------------------------------------------------------------------------------------\n");
    stopTimer(0);
    deleteatbegin(); //head = head->next
    A.seq = 1 - A.seq; //handles the expected and actual ack num we are on
    A.state = READYFORNEXT;
    if (head != NULL) {
      struct pkt newpacket;
      memcpy(newpacket.payload, head->message, 20); // set the packet payload to the first message in the buffer
      newpacket.seqnum = A.seq;                     // set created packets sequence num
      newpacket.checksum = get_checksum(&newpacket); // relies on other data, assign checksum
      A.last_packet = packet;
      A.state = NOTREADYFORNEXT;             // set state of A, needs to wait for an acknowledgment
      tolayer3(0, packet);            // send it down the pipeline
      startTimer(0, A.estimated_rtt); // start the timer for A and store in estimated_rtt
    }
    else{
      printf("\n\nNo more packets to send!");
    }
}
*/


void A_input(struct pkt packet) {
    if (packet.checksum != get_checksum(&packet))
  {// corrupt ack or NAK
    printf("\nPacket corrupt, waiting for timer interrupt...\n");
    //printf("\n--------------------------------------------------------------------------------------\n");
		//printf("Checksum:%d \nGenerated chksum:%d", packet.checksum, get_checksum(&packet));
		//printf("\n--------------------------------------------------------------------------------------\n");
    return;
  }

    if (packet.acknum != A.seq) {
      printf("\nWrong acknum, waiting for timer interrupt...\n");
      //printf("\n--------------------------------------------------------------------------------------\n");
		  //printf("Packet acknum: %d\nExpected acknum: %d", packet.acknum, A.seq);
		  //printf("\n--------------------------------------------------------------------------------------\n");
    return;
    }
    printf("\nSuccessfully received ACK packet!");
    //printf("\n--------------------------------------------------------------------------------------\n");
		//printf("Checksum:%d \nGenerated chksum:%d\nPacket acknum: %d\nExpected acknum: %d", packet.checksum, get_checksum(&packet), packet.acknum, A.seq);
		//printf("\n--------------------------------------------------------------------------------------\n");
    stopTimer(0);
    A.seq = 1 - A.seq; //handles the expected and actual ack num we are on
    A.state = READYFORNEXT;
}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init(void) {
    A.state = READYFORNEXT;
    A.seq = 0;
    A.estimated_rtt = 15;
}


/* called when A's timer goes off */
void A_timerinterrupt(void) {
    if (A.state != NOTREADYFORNEXT)
  {
    printf("ERROR IN A_TIMERINTERRUPT:\nState read as READYFORNEXT when it shouldn't be ready for the next packet.\n\n");
    return;
  }
    //A.estimated_rtt = A.estimated_rtt*1.5; //add more time
    printf("\nA_timerinterrupt called, resending packet!\n");//, resend packet: %s.\n\n", A.last_packet.payload);
    tolayer3(0, A.last_packet);
    startTimer(0, A.estimated_rtt);
}

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet) {
  struct pkt ackpacket;
    if (packet.checksum != get_checksum(&packet)) {
        printf("ERROR IN B_INPUT:\nPacket is corrupted, sending NAK...\n\n");
        ackpacket.acknum = 1-B.seq; //send incremented seqnum
        ackpacket.checksum = get_checksum(&ackpacket);
        tolayer3(1, ackpacket);
        return;
    }
    if (packet.seqnum != B.seq) {
        printf("ERROR IN B_INPUT:\nSequence number not correct, sending NAK...\n\n");
        ackpacket.acknum = 1-B.seq;
        ackpacket.checksum = get_checksum(&ackpacket);
        tolayer3(1, ackpacket);
        return;
    }
    struct msg message;
    strncpy(message.data, packet.payload, MESSAGE_LENGTH);
    //printf("B_input received message: %s.\n\n", packet.payload);
    printf("Sending ACK...\n\n");
    ackpacket.acknum = B.seq;
    ackpacket.checksum = get_checksum(&packet);
    tolayer3(1, ackpacket);//ack
    tolayer5(1, message); //forward message
    B.seq = 1 - B.seq; //alternate seqnum
}

/* called when B's timer goes off */
void B_timerinterrupt(void) {}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init(void) {
    B.seq = 0;
}

