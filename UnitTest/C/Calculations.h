#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include <sys/socket.h>
#include <string.h>
#include<math.h>
#include <arpa/inet.h>

#define INITIAL_FUEL_LEVEL 50
#define INITIAL_ODO_READING 234
#define ACCELERATION 10 //set rate of acceleration as 10 m/s

//Declarations
float Fuel_Calculator(int EngineState,float CurrentFuel);
int Get_Engine_Mode(int current_speed,int new_speed);
//int Distance_Travelled(int initial_velocity,int final_velocity);
int Distance_Travelled(int speed);
int Receive_From_Socket(int client_sock);
int Send_Data_To_Socket(int socket, int val1, int val2, int val3);



/*
*@brief:Function that calculates the speed and fuel consumption 
*
*@params:Speed
*@return:Distance
*/
int Distance_Travelled(int speed)
{
    float distance = 0;

    distance = (speed*5)/18;
    printf("Distance covered with speed %d = %.2f",speed,distance);
    return (int)distance;

}



/**
*@brief:Function that determines the state of the engine 
*
*@params:State of engine(off,idle,running,accelerating,decelerating)
*@return:Speed
*!NOTE:Also changes the global time taken for each acc/dec steps
*/
int Get_Engine_Mode(int current_speed,int new_speed)
{
    int mode = 0; /*Off*/
    if (current_speed == 0)
        mode = 1; /*Idling*/
    if ((abs(current_speed) == abs(new_speed)) && (current_speed != 0))
        mode = 2; /*Constant speed*/
    if (abs(current_speed) < abs(new_speed))
        mode = 3; /*Accelerating*/
    if (abs(current_speed) > abs(new_speed))
        mode = 4; /*Decelerating*/
    printf("Engine Mode = %d\n",mode);
    return mode;
}


/**
*@brief:Function that calculates the fuel consumed every second
*
*@params:State of engine[0:off,1:idle,2:running,3:accelerating,4:decelerating],Current Fuel level
*@return:updated fuel level
*TODO:Can merge else if of stte 3 and 4 into a sinle elif if acc and deceleration rates are same
*
*/
float Fuel_Calculator(int EngineState,float CurrentFuel)
{
        if(EngineState == 0)
            CurrentFuel =  CurrentFuel; /*Engine is off*/
        else if(EngineState == 1)
            CurrentFuel =  CurrentFuel - 0.05; /*If idling*/
        else if(EngineState == 2)
            CurrentFuel =  CurrentFuel - 0.1; /*If running at constant speed*/
        else if(EngineState == 3)
            CurrentFuel =  CurrentFuel - 0.2; /*If accelerating*/
        else if(EngineState == 4)
            CurrentFuel =  CurrentFuel - 0.2; /*If running at constant speed*/
        else
            CurrentFuel = 0.0; /*Means empty tank,which will stop the program execution*/
        //printf("Fuel level = %.2f\n",CurrentFuel);
        return CurrentFuel;

}


/**
*@brief:Function that receives speed from python slider
*
*@params:socket address
*@return:Speed
*TODO:change return type to int,return -1 if error occurs while sending
*
*/
int Receive_From_Socket(int client_sock)
{
    int BytesRead;
    char Data[2] = {0}; /*Receives speed as 2 digits->86 is received as ascii 8 and 6,0 as ascii 0 (48) and 0 */

    BytesRead = recv(client_sock, Data, sizeof(Data), 0); /*Will be -1 if error occurs*/
    printf("Received data = %d  %c  %02x\n",Data[0],Data[0],Data[0]);
    printf("Received data = %d  %c  %02x\n",Data[1],Data[1],Data[1]);

    for(int i = 0;i < 2;i++) {
        printf("%d ",Data[i]);
         /*Converting ASCII to decimal values*/
		if(Data[i] >= 'a') {
			Data[i] = Data[i] - 'a' + 10; /*ASCII to decimal  conversion*/
		}
		if(Data[i] >= '0' && Data[i] <= '9') {
			Data[i] = Data[i] - '0'; /*ASCII to decimal  conversion*/
		}
		else {
			printf("Error in ASCII-Decimal conversion\n");
		}
        printf("\nAfter conversion :\n%d ",Data[i]);
    }
    
    unsigned int speed = 0;

    speed = Data[0]*10 + Data[1];
    printf("Speed received from py = %d",speed);
    /*ASCII to decimal*/

    printf("\nBytesReceived = %d\n",BytesRead);
    return speed;

}

/**
*@brief:Function that sends dashboard data to socket
*
*@params:socket descriptor,
*@return:Speed
*TODO:change return type to int,return -1 if error occurs while sending
*
*/
int Send_Data_To_Socket(int socket, int val1, int val2, int val3) {
    char buffer[12];
    memcpy(buffer, &val1, 4);
    memcpy(buffer+4, &val2, 4);
    memcpy(buffer+8, &val3, 4);
    int bytes_sent = send(socket, buffer, sizeof(buffer), 0);
    if (bytes_sent != sizeof(buffer)) {
        return -1; // error: failed to send all bytes
    }
    return 0; // success
}
