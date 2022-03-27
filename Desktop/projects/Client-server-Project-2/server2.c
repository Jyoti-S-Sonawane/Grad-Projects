//  COEN_233_Server2

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define SERVER_PORT 2000
#define CLIENT_PORT 2001
#define NOT_PAID 0XFFF9
#define NOT_EXIST 0XFFFA
#define ACK_OK 0XFFFB
#define MAXFILESIZE 20
#define BUFFER_SIZE 100

struct VerificationDatabase
{
    unsigned int subsriberNumber;
    unsigned int technology;
    unsigned int status;
};

struct PermittedAccessMessagePacket
{
    int startOfPacketId;
    short int clientId;
    int acc_OK;
    short int segmentNo;
    int length;
    char technology;
    unsigned int sourceSubscriberNo;
    int endOfPacketId;
};

struct NotPaidMessagePacket
{
    int startOfPacketId;
    short int clientId;
    int notPaid;
    short int segmentNo;
    int length;
    char technology;
    unsigned int sourceSubscriberNo;
    int endOfPacketId;
};

struct SubsriberNotExistMessagePacket
{
    int startOfPacketId;
    short int clientId;
    int notExist;
    short int segmentNo;
    int length;
    char technology;
    unsigned int sourceSubscriberNo;
    int endOfPacketId;
};

struct AccessPermissionRequestPacket
{
    int startOfPacketId;
    short int clientId;
    int Acc_Per;
    short int segmentNo;
    int length;
    char technology;
    unsigned int sourceSubscriberNo;
    int endOfPacketId;
};

//Function declarations
int createSocket(void);
int readVerificationDatabaseFromFile(struct VerificationDatabase verificationDB[]);
short int shouldTimeout(struct AccessPermissionRequestPacket accessPermissionRequestPacket);
struct AccessPermissionRequestPacket getaccessPermissionRequestPacket(char buffer[]);
void sendResponsePacketToSubscriber(struct AccessPermissionRequestPacket accessPermissionRequestPacket, int numberOfRecordsInFile, struct VerificationDatabase verificationDB[]);
void sendSubscriberPermittedAccessPacket(struct AccessPermissionRequestPacket accessPermissionRequestPacket);
void sendSubscriberHasNotPaidPacket(struct AccessPermissionRequestPacket accessPermissionRequestPacket);
void sendSubscriberDoesNotExistPacket(struct AccessPermissionRequestPacket accessPermissionRequestPacket);

int sockfd;
unsigned int destinationStructLength;
struct sockaddr_in clientAddr;

int main()
{
    sockfd = createSocket();
    printf("Server ready, waiting for client....\n\n");

    destinationStructLength = sizeof(clientAddr);
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(CLIENT_PORT);
    clientAddr.sin_addr.s_addr = INADDR_ANY;

    struct VerificationDatabase verificationDB[MAXFILESIZE] = {0};
    int numberOfRecordsInFile = readVerificationDatabaseFromFile(verificationDB);

    for(int i = 0; ; i++)
    {
        //Receive access request packet from subscriber
        char buffer[BUFFER_SIZE] = {0};
        if (recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddr, &destinationStructLength) < 0)
        {
            printf("recvfrom error\n");
            return 1;
        }
        struct AccessPermissionRequestPacket accessPermissionRequestPacket = getaccessPermissionRequestPacket(buffer);

        //Check received packet for timeout test case
        if(shouldTimeout(accessPermissionRequestPacket))
        {
            printf("Skipping sending response packet\n\n");
            continue;
        }
        //Send specific response packet to subscriber
        sendResponsePacketToSubscriber(accessPermissionRequestPacket, numberOfRecordsInFile, verificationDB);
        printf("____________________________________________________________________________\n\n");
    }
    return 0;
}

//Function to create and bind server socket
int createSocket()
{
    int sockfd;
    if((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        printf("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in myAddr;
    printf("Socket created successfully\n\n");

    myAddr.sin_family = AF_INET;
    myAddr.sin_port = htons(SERVER_PORT);
    myAddr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd, (struct sockaddr*)&myAddr, sizeof(myAddr)) < 0)
    {
        printf("Bind to port failed");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

//Function to read the Verification_Database.txt
int readVerificationDatabaseFromFile(struct VerificationDatabase verificationDB[])
{
    FILE *fp;
    int recordIndex = 0;
 
    fp = fopen("Verification_Database.txt", "r");

    // Read header row and skip it
    char temp[100];
    fgets(temp, 100, fp);
    
    while(fscanf(fp, "%u %u %u", &verificationDB[recordIndex].subsriberNumber, &verificationDB[recordIndex].technology, &verificationDB[recordIndex].status) != EOF)
    {
        recordIndex++;
    }

    fclose(fp);
    return recordIndex;
}

//Function to test the timeout test case
short int shouldTimeout(struct AccessPermissionRequestPacket accessPermissionRequestPacket)
{
    return (accessPermissionRequestPacket.sourceSubscriberNo == 0);
}

//Function to copy the received buffer data to struct accessPermissionRequestPacket and print it
struct AccessPermissionRequestPacket getaccessPermissionRequestPacket(char buffer[])
{
    struct AccessPermissionRequestPacket packet;

    sscanf(buffer,
           "%X %hX %X %hX %d %hhu %u %X",
           &packet.startOfPacketId,
           &packet.clientId,
           &packet.Acc_Per,
           &packet.segmentNo,
           &packet.length,
           &packet.technology,
           &packet.sourceSubscriberNo,
           &packet.endOfPacketId);

    printf("Received access request with data:\nStart Of PacketId : %X \n", packet.startOfPacketId);
    printf("client ID : %hX \n", packet.clientId);
    printf("Acc permission : %X \n", packet.Acc_Per);
    printf("Segment No : %hX \n",  packet.segmentNo);
    printf("Length : %d \n", packet.length);
    printf("Technology : %hhu \n", packet.technology);
    printf("Source Subscriber No : %u \n", packet.sourceSubscriberNo);
    printf("End of Packet ID : %X \n\n", packet.endOfPacketId);

    return packet;
}

//Function to send specific response packet to subscriber
void sendResponsePacketToSubscriber(struct AccessPermissionRequestPacket accessPermissionRequestPacket, int numberOfRecordsInFile, struct VerificationDatabase verificationDB[])
{
    _Bool isSubscriberPresentInDB = 0;
    for(int i = 0; i < numberOfRecordsInFile; i++)
    {
        if(accessPermissionRequestPacket.sourceSubscriberNo == verificationDB[i].subsriberNumber)
        {
            isSubscriberPresentInDB = 1;
            if (accessPermissionRequestPacket.technology == verificationDB[i].technology)
            {
                if (verificationDB[i].status == 1) {
                    sendSubscriberPermittedAccessPacket(accessPermissionRequestPacket);
                }
                else if (verificationDB[i].status == 0)
                {
                    sendSubscriberHasNotPaidPacket(accessPermissionRequestPacket);
                }
            }
        }
    }

    if(isSubscriberPresentInDB == 0)
    {
        sendSubscriberDoesNotExistPacket(accessPermissionRequestPacket);
    }
}

//Function to send subscriber permitted access response packet and print it
void sendSubscriberPermittedAccessPacket(struct AccessPermissionRequestPacket accessPermissionRequestPacket)
{
    char buffer[BUFFER_SIZE];
    struct PermittedAccessMessagePacket packet;
    packet.acc_OK = ACK_OK;

    printf("Sending : [Subscriber permitted to access the network message] \n");
    printf("StartOfPacketId : %X \n", accessPermissionRequestPacket.startOfPacketId);
    printf("Client ID : %hX \n", accessPermissionRequestPacket.clientId);
    printf("Acc_Per : %X \n", packet.acc_OK);
    printf("Segment No : %hX \n",  accessPermissionRequestPacket.segmentNo);
    printf("Length : %d \n", accessPermissionRequestPacket.length);
    printf("Technology : %hhu \n", accessPermissionRequestPacket.technology);
    printf("Source Subscriber No : %u \n", accessPermissionRequestPacket.sourceSubscriberNo);
    printf("End of Packet ID : %X \n\n", accessPermissionRequestPacket.endOfPacketId);

    sprintf(buffer,
            "%X %hX %X %hX %d %hhu %u %X",
            accessPermissionRequestPacket.startOfPacketId,
            accessPermissionRequestPacket.clientId,
            packet.acc_OK,
            accessPermissionRequestPacket.segmentNo,
            accessPermissionRequestPacket.length,
            accessPermissionRequestPacket.technology,
            accessPermissionRequestPacket.sourceSubscriberNo,
            accessPermissionRequestPacket.endOfPacketId);

    if(sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&clientAddr, destinationStructLength) < 0)
    {
        printf("Unable to send the message\n");
        exit(EXIT_FAILURE);
    }
}

//Function to send subscriber has not paid response packet and print it
void sendSubscriberHasNotPaidPacket(struct AccessPermissionRequestPacket accessPermissionRequestPacket)
{
    char buffer[BUFFER_SIZE];
    struct NotPaidMessagePacket packet;
    packet.notPaid = NOT_PAID;

    printf("Sending : [Subscriber has not paid] \n");
    printf("StartOfPacketId : %X \n", accessPermissionRequestPacket.startOfPacketId);
    printf("Client ID : %hX \n", accessPermissionRequestPacket.clientId);
    printf("Not paid : %X \n", packet.notPaid);
    printf("Segment No : %hX \n",  accessPermissionRequestPacket.segmentNo);
    printf("Length : %d \n", accessPermissionRequestPacket.length);
    printf("Technology : %hhu \n", accessPermissionRequestPacket.technology);
    printf("Source Subscriber No : %u \n", accessPermissionRequestPacket.sourceSubscriberNo);
    printf("End of Packet ID : %X \n\n", accessPermissionRequestPacket.endOfPacketId);

    sprintf(buffer,
            "%X %hX %X %hX %d %hhu %u %X",
            accessPermissionRequestPacket.startOfPacketId,
            accessPermissionRequestPacket.clientId,
            packet.notPaid,
            accessPermissionRequestPacket.segmentNo,
            accessPermissionRequestPacket.length,
            accessPermissionRequestPacket.technology,
            accessPermissionRequestPacket.sourceSubscriberNo,
            accessPermissionRequestPacket.endOfPacketId);

    if(sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&clientAddr, destinationStructLength) < 0)
    {
        printf("Unable to send the message\n");
        exit(EXIT_FAILURE);
    }
}

//Function to send subscriber does not exist packet and print it
void sendSubscriberDoesNotExistPacket(struct AccessPermissionRequestPacket accessPermissionRequestPacket)
{
    char buffer[BUFFER_SIZE];
    struct SubsriberNotExistMessagePacket packet;
    packet.notExist = NOT_EXIST;

    printf("Sending : [Subscriber does not exist] \n");
    printf("StartOfPacketId : %X \n", accessPermissionRequestPacket.startOfPacketId);
    printf("Client ID : %hX \n", accessPermissionRequestPacket.clientId);
    printf("Not exist : %X \n", packet.notExist);
    printf("Segment No : %hX \n",  accessPermissionRequestPacket.segmentNo);
    printf("Length : %d \n", accessPermissionRequestPacket.length);
    printf("Technology : %hhu \n", accessPermissionRequestPacket.technology);
    printf("Source Subscriber No : %u \n", accessPermissionRequestPacket.sourceSubscriberNo);
    printf("End of Packet ID : %X \n\n", accessPermissionRequestPacket.endOfPacketId);

    sprintf(buffer,
            "%X %hX %X %hX %d %hhu %u %X",
            accessPermissionRequestPacket.startOfPacketId,
            accessPermissionRequestPacket.clientId,
            packet.notExist,
            accessPermissionRequestPacket.segmentNo,
            accessPermissionRequestPacket.length,
            accessPermissionRequestPacket.technology,
            accessPermissionRequestPacket.sourceSubscriberNo,
            accessPermissionRequestPacket.endOfPacketId);

    if(sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&clientAddr, destinationStructLength) < 0)
    {
        printf("Unable to send the message\n");
        exit(EXIT_FAILURE);
    }
}

