//  COEN_233_Server

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define SERVERPORT 2000
#define CLIENTPORT 2001
#define END_OF_PACKET_ID 0XFFFF
#define ACK 0XFFF2
#define REJECT 0XFFF3
#define OUT_OF_SEQUENCE 0XFFF4
#define LENGTH_MISMATCH 0XFFF5
#define END_OF_PACKET_MISSING 0XFFF6
#define DUPLICATE_PACKET 0XFFF7
#define ERROR_BUFFER_SIZE 100

struct DataPacket
{
    int startOfPacketId;
    short int clientId;
    int data;
    short int segmentNo;
    short int length;
    char payload[255];
    int endOfPacketId;
};

struct AckPacket
{
    int startOfPacketId;
    short int clientId;
    int ack;
    short int receivedSegmentNo;
    int endOfPacketId;
};

struct RejectPacket
{
    int startOfPacketId;
    short int clientId;
    int reject;
    int rejectSubCode;
    short int receivedSegmentNo;
    int endOfPacketId;
};

//Function declarations
int createSocket(void);
struct DataPacket getDataPacket(char buffer[]);
short int shouldTimeout(struct DataPacket data_packet);
short int isDuplicate(int next_expected_seg_number, int segmentNo);
void sendDuplicateReject(struct DataPacket data_packet);
void sendSegmentNotInSequence(struct DataPacket data_packet);
void sendEndOfPacketIdMissing(struct DataPacket data_packet);
void sendLengthMismatch(struct DataPacket data_packet) ;
void sendAckPacket(struct DataPacket data_packet);
void sendRejectPacket(int subcode, char* reasonCode, struct DataPacket data_packet);

int sockfd;
unsigned int destinationStructLength;
struct sockaddr_in clientAddr;

int main()
{
    sockfd = createSocket();
    printf("Server ready,waiting for client....\n\n");

    destinationStructLength = sizeof(clientAddr);
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(CLIENTPORT);
    clientAddr.sin_addr.s_addr = INADDR_ANY;

    int next_expected_seg_number = 0;
    for(int i =  0;; i++)
    {
        char buffer[sizeof(struct DataPacket)] = {0};
        if(recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddr, &destinationStructLength) < 0)
        {
            printf("recvfrom error\n");
            exit(EXIT_FAILURE);
        }
        //Setting next_expected_seg_number to 0
        if(strcmp(buffer, "RESET") == 0)
        {
            next_expected_seg_number = 0;
            printf("End of a test case, resetting the next_expected_seg_number to 0\n");
            printf("-------------------------------\n\n");
            continue;
        }
        struct DataPacket data_packet = getDataPacket(buffer);

        //Check for timeout test case
        if(shouldTimeout(data_packet))
        {
            printf("Skipping sending ack or reject packet for timeout test case.\n\n");
            continue;
        }

        short int isduplicate = isDuplicate(next_expected_seg_number, data_packet.segmentNo);
        //send reject response packet to client
        if(data_packet.length != strlen(data_packet.payload))
        {
            sendLengthMismatch(data_packet);
        }
        else if(data_packet.endOfPacketId != END_OF_PACKET_ID)
        {
            sendEndOfPacketIdMissing(data_packet);
        }
        else if(isduplicate)
        {
            sendDuplicateReject(data_packet);
        }
        else if(next_expected_seg_number != data_packet.segmentNo)
        {
            sendSegmentNotInSequence(data_packet);
        }
        else
        {
            //send ack response packet to client
            sendAckPacket(data_packet);
            next_expected_seg_number++;
        }
        printf("\n");
    }
    return 0;
}

//Function to create and bind server socket
int createSocket()
{
    int sockfd;
    if((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        perror("socket creation failed.");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in myAddr;
    printf("Socket created successfully\n");

    myAddr.sin_family = AF_INET;
    myAddr.sin_port = htons(SERVERPORT);
    myAddr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd,(struct sockaddr*)&myAddr, sizeof(myAddr)) < 0)
    {
        printf("Bind to port failed");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

//Function to copy the received buffer data to struct data_packet and print it
struct DataPacket getDataPacket(char buffer[])
{
    struct DataPacket data_packet;
    sscanf(buffer,
           "%X %hX %X %hd %hX %s %X",
           &data_packet.startOfPacketId,
           &data_packet.clientId,
           &data_packet.data,
           &data_packet.segmentNo,
           &data_packet.length,
           data_packet.payload,
           &data_packet.endOfPacketId);

    printf("received packet data: %X %hX %X %hd %hX %s %X  \n",
           data_packet.startOfPacketId,
           data_packet.clientId,
           data_packet.data,
           data_packet.segmentNo,
           data_packet.length,
           data_packet.payload,
           data_packet.endOfPacketId);

    return data_packet;
}

//Function to test the timeout test case
short int shouldTimeout(struct DataPacket data_packet)
{
    return strcmp(data_packet.payload, "Data:Timeout") == 0;
}

//Function to check duplicate segment number
short int isDuplicate(int next_expected_seg_number, int segmentNo)
{
    if (segmentNo < next_expected_seg_number)
    {
        return 1;
    }

    return 0;
}

//Function to send segment not in sequence reject packet
void sendSegmentNotInSequence(struct DataPacket dataPacket)
{
    sendRejectPacket(OUT_OF_SEQUENCE, "Segment not in sequence", dataPacket);
}

//Function to send length mismatch reject packet
void sendLengthMismatch(struct DataPacket dataPacket)
{
    sendRejectPacket(LENGTH_MISMATCH, "Length mismatch", dataPacket);
}

//Function to send end of packet id missing reject packet
void sendEndOfPacketIdMissing(struct DataPacket dataPacket)
{
    sendRejectPacket(END_OF_PACKET_MISSING, "End of packet id missing", dataPacket);
}

//Function to send duplicate reject packet
void sendDuplicateReject(struct DataPacket dataPacket)
{
    sendRejectPacket(DUPLICATE_PACKET, "Duplicate packet", dataPacket);
}

//Function to send specific reject packet and print it
void sendRejectPacket(int subcode, char* reasonCode, struct DataPacket dataPacket)
{
    struct RejectPacket rejectPacket;
    rejectPacket.reject = REJECT;
    rejectPacket.rejectSubCode = subcode;
    char errorBuffer[ERROR_BUFFER_SIZE];

    printf("[%s] Sending reject packet: %X %hX %X %X %hd %X\n",
           reasonCode,
           dataPacket.startOfPacketId,
           dataPacket.clientId,
           rejectPacket.reject,
           rejectPacket.rejectSubCode,
           dataPacket.segmentNo,
           dataPacket.endOfPacketId);

    sprintf(errorBuffer,
            "%X %hX %X %X %hd %X",
            dataPacket.startOfPacketId,
            dataPacket.clientId,
            rejectPacket.reject,
            rejectPacket.rejectSubCode,
            dataPacket.segmentNo,
            dataPacket.endOfPacketId);

    if(sendto(sockfd, errorBuffer, strlen(errorBuffer), 0, (struct sockaddr *)&clientAddr, destinationStructLength) < 0)
    {
        printf("Unable to send reject packet\n");
    }
}

//Function to send ACK packet to client
void sendAckPacket(struct DataPacket dataPacket)
{
    char ack_buffer[sizeof(struct AckPacket)] = {0};
    struct AckPacket ackPacket;
    ackPacket.ack = ACK;

    printf("Sending ack packet: %X %hX %X %hd %X \n",
           dataPacket.startOfPacketId,
           dataPacket.clientId,
           ackPacket.ack,
           dataPacket.segmentNo,
           dataPacket.endOfPacketId);

    sprintf(ack_buffer,
            "%X %hX %X %hd %X",
            dataPacket.startOfPacketId,
            dataPacket.clientId,
            ackPacket.ack,
            dataPacket.segmentNo,
            dataPacket.endOfPacketId);

    if(sendto(sockfd, ack_buffer, strlen(ack_buffer), 0, (struct sockaddr *)&clientAddr, destinationStructLength) < 0)
    {
        printf("Unable to send ack packet\n");
    }
}
