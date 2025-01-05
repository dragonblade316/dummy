import serial
import time
import rclpy
from rclpy.node import Node
from std_msgs.msg import Int32MultiArray



class SerialNode(Node):
    def __init__(self):
        super().__init__("SerialNode")

        self.count = 0;

        self.declare_parameter("serial_address", "/dev/ttyACM1")
        self.serial = serial.Serial(self.get_parameter("serial_address").value, 115200)
        
        # self.serial_monitor = self.create_publisher
        self.subscription = self.create_subscription(
            Int32MultiArray,
            "servo_targets",
            self.callback,
            10
        )

        self.last = time.time()
    
    def callback(self, msg): 
        if time.time() - self.last < 0.05:
            print(f"waiting {time.time() - self.last}")
            self.get_logger().debug('waiting')
            return
        self.last = time.time()

        self.count += 1
        print(self.count)
        print(msg.data[0])

        self.serial.reset_output_buffer()
        

        message = f"<{msg.data[0]},{msg.data[1]},{msg.data[2]}>"
        print(message)
        self.serial.write(str.encode(message))

        # self.serial.readline()
        
        
def main(args=None):
    rclpy.init()
    node = SerialNode()
    rclpy.spin(node)

if __name__ == '__main__':
    main()

    https://en.wikipedia.org/wiki/X86-64


    FHSz6$oV7J&$Km

