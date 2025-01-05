#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <rclcpp/executors.hpp>
#include <rclcpp/logging.hpp>
#include <rclcpp/subscription.hpp>
#include <std_msgs/msg/detail/int32_multi_array__struct.h>
#include <std_msgs/msg/detail/int32_multi_array__struct.hpp>
#include <string>
#include <algorithm>
#include <rcl/logging.h>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joy.hpp"
#include <geometry_msgs/msg/twist.hpp>
#include <std_msgs/msg/int32_multi_array.hpp>
#include <unistd.h>

using namespace std::chrono_literals;

/* This example creates a subclass of Node and uses a fancy C++11 lambda
* function to shorten the callback syntax, at the expense of making the
* code somewhat more difficult to understand at first glance. */

class Controller : public rclcpp::Node

{


public:
  int shoulder;
  int elbow;
  int wrist;

  Controller()
  : Node("controller"), count_(0)
  {
    
    
    publisher_ = this->create_publisher<std_msgs::msg::Int32MultiArray>("/servo_targets", 10);

    auto callback =
      [this](sensor_msgs::msg::Joy::UniquePtr msg) -> void {
        
        std_msgs::msg::Int32MultiArray message = std_msgs::msg::Int32MultiArray();

        RCLCPP_INFO(this->get_logger(), "made it here");

        shoulder = std::clamp(shoulder + static_cast<int>(floor(msg->axes[1])), 0, 180);
        elbow = std::clamp(elbow + static_cast<int>(floor(msg->axes[3])), 0, 180);
        wrist = std::clamp(wrist + static_cast<int>(floor(msg->axes[2])), 0, 180);

        message.data.push_back(shoulder);
        message.data.push_back(elbow);
        message.data.push_back(wrist);
        /*message.data[0] = shoulder;*/
        /*message.data[1] = elbow;*/
        /*message.data[2] = wrist;*/
        /**/
        /*count_++;*/
        // message.data = "Hello, world! " + std::to_string(this->count_++);
        // RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
        this->publisher_->publish(message);
      };
    subscription = this->create_subscription<sensor_msgs::msg::Joy>("joy", 10, callback);
  }

private:
  rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr subscription;
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<std_msgs::msg::Int32MultiArray>::SharedPtr publisher_;
  size_t count_;
};


int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Controller>());
  rclcpp::shutdown();
  return 0;
}

