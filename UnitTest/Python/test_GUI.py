import unittest
import GUI
import tkinter as tk
from unittest.mock import MagicMock


class testGUI(unittest.TestCase):
    def setup(self):
        pass

    # def test_SendDataToServer(self):
    #     x = GUI.SendDataTOServer()

    # def test_ReceiveDataFromServer(self):
    #     Response = GUI.ReceiveDataFromServer(self.client_socket)
    #     self.assertTrue(len(Response) < 6) # Result has max 5 bytes

    def test_CreateSocket(self):
        """
        Test if type of the variable created by the function is socket
        """
        self.port = 8888
        Socket = GUI.CreateSocket(self.port)
        self.assertTrue("socket.socket" in str(type(Socket)))

    # def tearDown(self):
    #     # Clean up any resources created during the test here
    #     self.client_socket.close()

    # def test_SendDataToServer(self):
    #     GUI.SendDataToServer()

    # def test_call_update_speed_with_slider_event(self):
    #     # Create a mock root object
    #     root = tk.Tk()

    #     # Create a mock slider object
    #     slider = tk.Scale(root)

    #     # Call the CallUpdateSpeed function with a mock slider event
    #     event = MagicMock()
    #     event.widget = slider
    #     GUI.CallUpdateSpeed(event)

    #     # Verify that the slider set method was called with the correct arguments
    #     slider.set.assert_called_once_with(500)

    #     # Verify that the root.after method was called with the correct arguments
    #     root.after.assert_called_once_with(500, GUI.UpdateSpeed)

    def test_CreateDisplay(self):
        """Checks if type of A dislay created is \'Entry\'"""
        root = tk.Tk()
        FuelFrame, FuelDisplay = GUI.create_display_frame(root, "Fuel", 5, 0)
        self.assertTrue("Entry" in str(type(FuelDisplay)))
        self.assertTrue("Frame" in str(type(FuelFrame)))


if __name__ == "__main__":
    unittest.main()
