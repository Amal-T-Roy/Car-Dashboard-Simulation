from tkinter import *
import socket
import argparse
import struct
import threading


parser = argparse.ArgumentParser(
    description="Car dash board simulation using IPC with sockets"
)
parser.add_argument(
    "-p",
    "--port",
    type=int,
    help="Port to connect",
    metavar="",
    required=False,
    default=8888
)

args = parser.parse_args()  # Parser


# Handle button press
Engine_status = False
Modes = ["OFF", "IDLE", "RUNNING", "ACCELERATING", "DECELERATING"]


# ! disable slider on stop
def IPC():
    """Function that updates GUI every second,Called inside a thread"""
    while(1):
        global Engine_status
        Speed = SpeedSettingSlider.get()  # Get speed
        if Engine_status == False:
            Speed = -1
        SendDataToServer(client_socket, int(Speed))  # send speed
        # Receive response
        Data = ReceiveDataFromServer(client_socket)
        Distance, CurrentEngineStatus, FuelLevel = Data
        # Update odometer
        OdometerDisplay.delete(0, END)
        OdometerDisplay.insert(0, str(Distance) + " km")  # update val
        # Update fuel
        FuelDisplay.delete(0, END)
        if FuelLevel <= 0:  # If fuel is exhausted
            FuelDisplay.insert(0, "EMPTY")  # update val
            # SpeedSettingSlider.configure(state="disabled", troughcolor="black")
            Engine_status = False  # Stop engine
            return
        else:
            FuelDisplay.insert(0, str(FuelLevel) + " Ltr")  # update val
        # Update Engine Run status
        if Engine_status == True:
            EngineStatusDisplay.delete(0, END)
            EngineStatusDisplay.insert(0, Modes[CurrentEngineStatus])


# Create Thread

network_thread = threading.Thread(target=IPC)

# Function to handle button press


def HandleButtonPress():
    """Call function binded with button press with 500ms delay"""
    root.after(500, StartButtonPress)


# Function to handle start button press
def StartButtonPress():
    """
    Toggles Engine ON/OFF
    """
    global Engine_status
    EngineStatusDisplay.delete(0, END)  # Deletes value already in screen
    if Engine_status == False:
        Engine_status = True
        # Call thread
        network_thread.start()
        EngineStatusDisplay.insert(0, "Running")
        SpeedSettingSlider.configure(state="normal", troughcolor="gray")
    else:
        Engine_status = False  # Turn engine off
        EngineStatusDisplay.insert(0, "OFF")
        # Close the thread
        if network_thread.is_alive():
            network_thread.join()  # Close thread
        SpeedSettingSlider.configure(state="disabled", troughcolor="black")

    return


# Function that calls The updatespeed function with a delay of 200ms
def CallUpdateSpeed(Event):
    """Implement the after method on UpdateSpeed function.
    Avoids event queue overloading.
    NOTE:The parameter is not used,it is a dummy->Slider passes a param while running.
    If the function normally dont take a param,then it creates an exception.
    """
    root.after(200, UpdateSpeed)


# Function to display slider reading
def UpdateSpeed():
    """
    Function that gets the slider reading and displays it
    """
    # Get speed set by the slider
    Speed = SpeedSettingSlider.get()
    # Deletes value already in screen
    SpeedDisplay.delete(0, END)
    # Insert new value in screen
    SpeedDisplay.insert(0, str(Speed) + " km/h")
    return


# Create a socket


def CreateSocket(port):
    """Create socket connection and returns the socket variable"""
    # Create a socket object
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # Get local machine name
    host = socket.gethostname()
    # Connect to the server
    client_socket.connect((host, port))
    return client_socket


def SendDataToServer(socket, value):
    """Sends speed as an int value to socket"""
    if value < 0 or value > 200:
        return -1  # error: value out of range
    # create byte array to hold 4-byte integer in network byte order
    net_value = bytearray([0, 0, 0, 0])
    # extract the most significant byte and insert it into the byte array
    net_value[0] = (value >> 24) & 0xFF
    # extract the second most significant byte and insert it into the byte array
    net_value[1] = (value >> 16) & 0xFF
    # extract the third most significant byte and insert it into the byte array
    net_value[2] = (value >> 8) & 0xFF
    # extract the least significant byte and insert it into the byte array
    net_value[3] = value & 0xFF
    bytes_sent = socket.send(net_value)  # Track the number of bytes transfered
    if bytes_sent != 4:
        return -1  # error: failed to send all bytes


# Receive data from server
def ReceiveDataFromServer(socket):
    """
    Receives 3 integer values -> Distance,EngineStatus,FuelLevel
    """
    data = socket.recv(12)  # Receive 3 integers
    Distance, EngineStatus, FuelLevel = struct.unpack(
        "iii", data
    )  # Unpack the bytes into 3 integers
    Values = [Distance, EngineStatus, (FuelLevel)]
    print("Received 3 integers from socket:", Distance, EngineStatus, round(FuelLevel,2))
    return Values


def create_display_frame(root, name, row, column):
    """Creates a display"""
    # Create the frame
    frame = Frame(root, padx=10, pady=10)
    frame.grid(row=row, column=column, columnspan=3)

    # Add the label
    description_label = Label(frame, text=str(name) + ":")
    description_label.grid(row=0, column=0, padx=5, pady=5)

    # Create display entry widget
    display = Entry(frame, width=35, borderwidth=5)
    display.grid(row=0, column=1, columnspan=3, padx=10, pady=10)

    return frame, display


# Create socket
port = args.port
client_socket = CreateSocket(port)

try:


    # Init the GUI window
    root = Tk()
    root.title("DASH BOARD")
    root.geometry("600x500")

    # Start Button
    IgnitionButton = Button(
        root,
        text="Start/Stop",
        border=10,
        command=lambda: HandleButtonPress(),
        bg="red",
        fg="white",
    )
    IgnitionButton.grid(row=9, column=0, columnspan=3, padx=10, pady=10)

    # Widget to display speed
    speed_frame, SpeedDisplay = create_display_frame(root, "speed", 0, 0)
    SpeedDisplay.insert(0, "0 km/h")

    # Widget to display Odometer
    Odoframe, OdometerDisplay = create_display_frame(root, "Odometer", 3, 0)
    OdometerDisplay.insert(0, "00000" + " km")  # Initial value to display

    # Widget to display Fuel level

    FuelFrame, FuelDisplay = create_display_frame(root, "Fuel", 5, 0)
    FuelDisplay.insert(0, "0000" + " l")

    # Widget to display EngineStatus
    StatusFrame, EngineStatusDisplay = create_display_frame(root, "Engine Status", 7, 0)
    EngineStatusDisplay.insert(0, "OFF")

    # Create Slider
    SpeedSettingSlider = Scale(
        root,
        name="speed",
        from_=0,
        to=200,
        showvalue=True,
        orient=HORIZONTAL,
        length=350,
        borderwidth=5,
        command=CallUpdateSpeed,
        resolution=10,
    )
    # Initially disabled,activates on button press.
    SpeedSettingSlider.configure(state="disabled", troughcolor="black")
    SpeedSettingSlider.grid(row=2, column=1, columnspan=3, padx=10, pady=10)

    SliderLabel = Label(root, text="Adjust speed:")
    SliderLabel.grid(row=2, column=0, padx=5, pady=5)

    # Run the GUI
    root.mainloop()

except Exception as e:
    print(e.__doc__)

finally:
    # Close thread
    if network_thread.is_alive():  # If the thread is running
        network_thread.join()
    # Close the socket
    client_socket.close()
