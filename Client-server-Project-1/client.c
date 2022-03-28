//  COEN_233_Client

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <errno.h>

#define SERVERPORT 2000
#define CLIENTPORT 2001
#define NUMBER_OF_PACKETS_IN_TEST 5
#define RETRY_COUNT 3
#define END 0XFFFF
#define START END
#define CLIENT_ID 0XFF
#define DATA 0XFFF1
#define REJECT 0XFFF3
#define OUT_OF_SEQUENCE 0XFFF4
#define LENGTH_MISMATCH 0XFFF5
#define END_OF_PACKET_MISSING 0XFFF6
#define DUPLICATE_PACKET 0XFFF7

struct DataPacket {
    int startOfPacketId;
    short int clientId;
    int data;
    short int segmentNo;
    short int length;
    char payload[255];
    int endOfPacketId;
};

struct AckPacket {
    int startOfPacketId;
    short int clientId;
    int ack;
    short int receivedSegmentNo;
    int endOfPacketId;
};

struct RejectPacket {
    int startOfPacketId;
    short int clientId;
    int reject;
    int rejectSubCode;
    short int receivedSegmentNo;
    int endOfPacketId;
};

//Function declarations
int createSocket(void);
void sendNewTestCaseSingalToServer(void);
int getTestCaseNumber(void);
void populateDataPackets(int testCaseNumber, struct DataPacket data_packets[]);
void sendPacketToServer(struct DataPacket dataPacket, int retry_count, int testCaseNumber);
long receiveServerRespose(char notify_buffer[], int notify_buffer_len);
void printReceivedPacket(char notify_buffer[]);

int sockfd;
unsigned int destinationStructLength;
struct sockaddr_in serverAddr;

int main()
{
    sockfd = createSocket();

    destinationStructLength = sizeof(serverAddr);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVERPORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    while(1)
    {
        sendNewTestCaseSingalToServer();

        int testCaseNumber = getTestCaseNumber();
        struct DataPacket dataPackets[NUMBER_OF_PACKETS_IN_TEST];

        populateDataPackets(testCaseNumber, dataPackets);

        for(int i = 0; i < NUMBER_OF_PACKETS_IN_TEST; i++)
        {
            int retryCount;

            for(retryCount = 0; retryCount < RETRY_COUNT; retryCount++)
            {
                //Send data packet to server
                sendPacketToServer(dataPackets[i], retryCount, testCaseNumber);

                //Receive response from server
                char responseBuffer[sizeof(struct RejectPacket)];
                long receiveReturnCode = receiveServerRespose(responseBuffer, sizeof(responseBuffer));

                if(receiveReturnCode < 0)
                {
                    if (errno == EWOULDBLOCK)
                    {
                        printf("Timed out while waiting for response from server");
                    }
                }
                else
                {
                    printReceivedPacket(responseBuffer);
                    break;
                }
                printf("\n\n");
            }

            if(retryCount == RETRY_COUNT)
            {
                printf("Server does not respond\n\n");
                break;
            }
            printf("\n");
        }
    }
    return 0;
}

//Function to create and bind client socket
int createSocket()
{
    int sockfd;
    if((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket created successfully\n");

    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        perror("Error setting timeout");
    }

    struct sockaddr_in myAddr;
    
    myAddr.sin_family = AF_INET;
    myAddr.sin_port = htons(CLIENTPORT);
    myAddr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd,(struct sockaddr*)&myAddr, sizeof(myAddr)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    return  sockfd;
}

//Function to send next test signal to server
void sendNewTestCaseSingalToServer() {
    printf("Sending start new test case signal to server\n\n");
    char resetBuffer[10];
    strcpy(resetBuffer, "RESET");
    if(sendto(sockfd, resetBuffer, strlen(resetBuffer), 0, (struct sockaddr*)&serverAddr, destinationStructLength) < 0)
    {
        printf("Unable to reset\n");
    }
}

//Fuction to display test cases and accept test case number
int getTestCaseNumber()
{
    int testCaseNumber;

    printf("****** List of test cases ******\n");
    printf("1. No error\n");
    printf("2. 4 incorrect packets with different errors\n");
    printf("3. Out of sequence\n");
    printf("4. Length mismatch\n");
    printf("5. End of packet missing\n");
    printf("6. Duplicate packet\n");
    printf("7. Timeout\n");
    printf("8. Timeout and then acknowledge\n");
    printf("9. End\n");
    printf("Enter the test case number:\n");
    scanf("%d", &testCaseNumber);
    printf("\n");

    return testCaseNumber;
}

//Function to populate data packets
void populateDataPackets(int testCaseNumber, struct DataPacket dataPackets[])
{
    for(int i = 0; i < NUMBER_OF_PACKETS_IN_TEST; i++)
    {
        dataPackets[i].startOfPacketId = START;
        dataPackets[i].clientId = CLIENT_ID;
        dataPackets[i].data = DATA;
        dataPackets[i].segmentNo = i;
        strcpy(dataPackets[i].payload,"message");
        dataPackets[i].length = strlen(dataPackets[i].payload);
        dataPackets[i].endOfPacketId = END;
    }
    switch (testCaseNumber)
    {
    case 1:
        break;
    case 2:
        strcpy(dataPackets[1].payload, "COEN-233");
        dataPackets[2].endOfPacketId = 0XFFAA;
        dataPackets[3].segmentNo = 0;
        dataPackets[4].segmentNo = 20;
        break;
    case 3:
        dataPackets[4].segmentNo = 11;
        break;
    case 4:
        strcpy(dataPackets[4].payload,"COEN-233");
        break;
    case 5:
        dataPackets[2].endOfPacketId = 0XFFDD;
        break;
    case 6:
        dataPackets[4].segmentNo = 1;
        break;
    case 7:
    case 8:
        strcpy(dataPackets[4].payload,"Data:Timeout");
        break;
    case 9:
        exit(0);
    }
}

//Function to send data packets to server
void sendPacketToServer(struct DataPacket dataPacket, int retryCount, int testCaseNumber)
{
    char buffer[sizeof(struct DataPacket)] = {0};

    if (retryCount == 1 && testCaseNumber == 8)
    {
        printf("Changing the content of the payload \n");
        strcpy(dataPacket.payload, "message");
        sprintf(buffer,
                "%X %hX %X %hd %hX %s %X",
                dataPacket.startOfPacketId,
                dataPacket.clientId, dataPacket.data,
                dataPacket.segmentNo, dataPacket.length,
                dataPacket.payload,
                dataPacket.endOfPacketId);
    }
    sprintf(buffer,
            "%X %hX %X %hd %hX %s %X",
            dataPacket.startOfPacketId,
            dataPacket.clientId,
            dataPacket.data,
            dataPacket.segmentNo,
            dataPacket.length,
            dataPacket.payload,
            dataPacket.endOfPacketId);

    if(sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&serverAddr, destinationStructLength) < 0)
    {
        printf("Unable to send message\n");
        exit(EXIT_FAILURE);
    }

    printf("Sending data packet with data: %X %hX %X %hd %hX %s %X\n",
           dataPacket.startOfPacketId,
           dataPacket.clientId,
           dataPacket.data,
           dataPacket.segmentNo,
           dataPacket.length,
           dataPacket.payload,
           dataPacket.endOfPacketId);
}

//Function to receive server response
long receiveServerRespose(char responseBuffer[], int responseBufferLength)
{

    long receiveReturnCode = recvfrom(sockfd, responseBuffer, responseBufferLength, 0, (struct sockaddr *)&serverAddr, &destinationStructLength);
    return receiveReturnCode;
}

//Function to print received server response
void printReceivedPacket(char responseBuffer[])
{
    struct RejectPacket responsePacket;

    sscanf(responseBuffer, "%X %hX %X", &responsePacket.startOfPacketId, &responsePacket.clientId, &responsePacket.reject);

    if (responsePacket.reject == REJECT)
    {
        sscanf(responseBuffer,
               "%X %hX %X %X %hX %X",
               &responsePacket.startOfPacketId,
               &responsePacket.clientId,
               &responsePacket.reject,
               &responsePacket.rejectSubCode,
               &responsePacket.receivedSegmentNo,
               &responsePacket.endOfPacketId);

        char* rejectReason = "unknown";

        switch (responsePacket.rejectSubCode) {
        case OUT_OF_SEQUENCE:
            rejectReason = "Out of sequence";
            break;
        case LENGTH_MISMATCH:
            rejectReason = "Length mismatch";
            break;
        case END_OF_PACKET_MISSING:
            rejectReason = "End of packet missing";
            break;
        case DUPLICATE_PACKET:
            rejectReason = "Duplicate packet";
            break;
        default:
            break;
        }

        printf("[%s] Received reject packet with data:%X %hX %X %X %hX %X\n",
               rejectReason,
               responsePacket.startOfPacketId,
               responsePacket.clientId,
               responsePacket.reject,
               responsePacket.rejectSubCode,
               responsePacket.receivedSegmentNo,
               responsePacket.endOfPacketId);
    }
    else
    {
        sscanf(responseBuffer,
               "%X %hX %X %hX %X",
               &responsePacket.startOfPacketId,
               &responsePacket.clientId,
               &responsePacket.reject,
               &responsePacket.receivedSegmentNo,
               &responsePacket.endOfPacketId);

        printf("Received ack with packet data: %X %hX %X %hX %X\n",
               responsePacket.startOfPacketId,
               responsePacket.clientId,
               responsePacket.reject,
               responsePacket.receivedSegmentNo,
               responsePacket.endOfPacketId);
    }
}
