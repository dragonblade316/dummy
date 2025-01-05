
#include <chrono>
#include <cstdlib>
#include <memory>
#include <ratio>
#include <rclcpp/executors.hpp>
#include <rclcpp/node.hpp>
#include <rclcpp/publisher.hpp>
#include <rclcpp/subscription.hpp>
#include <rclcpp/timer.hpp>
#include <rclcpp/utilities.hpp>
#include <sensor_msgs/msg/detail/joint_state__struct.hpp>
#include <std_msgs/msg/detail/int32_multi_array__struct.hpp>
#include <trajectory_msgs/msg/detail/joint_trajectory_point__struct.hpp>
#include <trajectory_msgs/msg/joint_trajectory_point.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <std_msgs/msg/int32_multi_array.hpp>
#include <rclcpp/rclcpp.hpp>
#include <cmath>
#include <math.h>


const int timer_deley = 20;

class Controller : public rclcpp::Node {
public: 
	rclcpp::Publisher<std_msgs::msg::Int32MultiArray>::SharedPtr servo_target_publisher;
	rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr joint_state_publisher;
	rclcpp::Subscription<trajectory_msgs::msg::JointTrajectoryPoint>::SharedPtr target_subscription;
	rclcpp::TimerBase::SharedPtr timer;

	trajectory_msgs::msg::JointTrajectoryPoint target;
	double current[3] = {0.0,0.0,0.0};
	std::string joint_names[3] = {
		"shoulder",
		"elbow",
		"wrist",
	};
	

	Controller() : Node("controller") {
		this->declare_parameter("max_velocity", 1000.0);

		auto target_change_callback = [this](trajectory_msgs::msg::JointTrajectoryPoint msg) -> void {
			target = msg;
		};

		auto timer_callback = [this]() -> void {
			this->send_new_targets();
			//TODO: publish joint states
			//
			
			sensor_msgs::msg::JointState msg;

			msg.header.stamp = now();

			for (int i = 0; i < 3; i++) {
				msg.name.push_back(joint_names[i]);
				//the negitive is a hack so that everything looks right in rviz. Doing a better fix would require more effort than I have time for.
				msg.position.push_back(current[i] * (M_PI/180));
			}
			joint_state_publisher->publish(msg);
		};

		target.positions = {0,0,0,0};
		target.velocities = {0,0,0,0};

		servo_target_publisher = this->create_publisher<std_msgs::msg::Int32MultiArray>("servo_targets", 10);
		joint_state_publisher = this->create_publisher<sensor_msgs::msg::JointState>("joint_states", 10);
		target_subscription = this->create_subscription<trajectory_msgs::msg::JointTrajectoryPoint>("arm_target", 10, target_change_callback);
		timer = this->create_wall_timer(std::chrono::milliseconds(timer_deley), timer_callback);
	}

	void send_new_targets() {
		double max_vel = this->get_parameter("max_velocity").as_double();


		for (int i = 0; i < 3; i++) {
			current[i] = interpolate_target(i, max_vel);
		}

		std_msgs::msg::Int32MultiArray msg;
		//There are better ways I could have done this but I would have had to google it
		for (int i = 0; i < static_cast<int>(sizeof(current)); i++) {
			msg.data.push_back(static_cast<int>(round(current[i])));
		}

		servo_target_publisher->publish(msg);
	}

	double interpolate_target(int index, double max_vel) {
		std::cout << "made it here\n";

		

		double vel;
		if (target.velocities[index] > max_vel) vel = max_vel; else vel = target.velocities[index];
		//this is to account for the 20 ms cycle time
		vel = vel * (timer_deley * 0.001);

		if (abs(current[index] - target.positions[index]) <= vel) {
			return target.positions[index];
		}

		//should probably check for 0s (and learn how vel works)

		if ((target.positions[index] - current[index]) > 0) {
			return current[index] + vel;
		} else {
			return current[index] - vel;
		}
	}

};




int main(int argc, char * argv[]) {
	rclcpp::init(argc, argv);
	rclcpp::spin(std::make_shared<Controller>());
	rclcpp::shutdown();
	return 0;
}
