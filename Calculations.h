#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<math.h>
#include<arpa/inet.h>

#define INITIAL_FUEL_LEVEL 50
#define INITIAL_ODO_READING 14563

/*Declarations*/
float Fuel_Calculator(int EngineState,float CurrentFuel);
int Get_Engine_Mode(int current_speed,int new_speed);
int Distance_Travelled(int speed);
int Receive_Data_From_Client(int sockfd);
int Send_Data_To_Socket(int socket, int val1, int val2, int val3);



/**
*@brief:Function that calculates the distance travelled in a second 
*Derived from speed = distance/time
*Distance per second = (Speed in km*1000) / (60*60)
*                    = (Speed in km)*(1000/3600) =(Speed)*5/18
*@params:Speed
*@return:Distance
*/
int Distance_Travelled(int speed)
{
    float distance = 0;

    distance = (speed*5)/18; /*Derived from speed = distance/time*/
    printf("Distance covered with speed %d = %.2f",speed,distance);
    return (int)distance;

}



/**
*@brief:Function that determines the state of the engine 
*
*@params:current and next speed
*@return:State of engine(off,idle,running,accelerating,decelerating)
*
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
        printf("Fuel level = %.2f\n",CurrentFuel);
        return CurrentFuel;

}


/**
*@brief:Function that receives speed from python slider
*
*@params:socket address
*@return:Speed
*
*
*/
int Receive_Data_From_Client(int sockfd) {
    int value = 0;
    char buffer[4];
    int bytes_received = recv(sockfd, buffer, 4, 0); /*Receive 4 bytes,as sizeof(int) = 4 bytes*/
    if (bytes_received == -1) {
        perror("recv failed");
        return -1;  // error: receive failed
    }
    if (bytes_received != 4) {
        return -1;  // error: invalid number of bytes received
    }
    /* combine the four bytes into an integer value in host byte order*/
    value |= (buffer[0] & 0xFF) << 24;
    value |= (buffer[1] & 0xFF) << 16;
    value |= (buffer[2] & 0xFF) << 8;
    value |= buffer[3] & 0xFF;
    if (value < 0 || value > 200) {
        return -1;  // error: value out of range
    }
    return value;
}



/**
*@brief:Function that sends dashboard data to socket
*
*@params:socket descriptor,OdometerReading,EngineMOde,Fuellevel
*@return:SUCCESS OR FAILURE
*
*/
int Send_Data_To_Socket(int socket, int OdometerReading, int EngineMode, int FuelLevel) {
    char buffer[12];
    memcpy(buffer, &OdometerReading, 4);
    memcpy(buffer+4, &EngineMode, 4);
    memcpy(buffer+8, &FuelLevel, 4);
    int bytes_sent = send(socket, buffer, sizeof(buffer), 0);
    if (bytes_sent != sizeof(buffer)) {
        return EXIT_FAILURE; // error: failed to send all bytes
    }
    return EXIT_SUCCESS; // success
}
