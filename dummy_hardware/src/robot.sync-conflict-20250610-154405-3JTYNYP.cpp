#include "robot.hpp"
#include "hardware_interface/handle.hpp"
#include "hardware_interface/system_interface.hpp"
#include "hardware_interface/types/hardware_interface_return_values.hpp"
#include <chrono>
#include <cstdio>
#include <exception>
#include <hardware_interface/types/hardware_interface_type_values.hpp>
#include <memory>
#include <rclcpp/logger.hpp>
#include <rclcpp/logging.hpp>
#include <rclcpp_lifecycle/state.hpp>
#include <rosidl_runtime_cpp/traits.hpp>
#include <pluginlib/class_list_macros.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <vector>
#include <rclcpp/rclcpp.hpp>


#define SERIAL_PORT "/dev/ttyACM1"
#define BAUD 115200


PLUGINLIB_EXPORT_CLASS(
    hardware::DummyHardware,
    hardware_interface::SystemInterface
)


using namespace hardware;

hardware_interface::CallbackReturn DummyHardware::on_init(const hardware_interface::HardwareInfo &info) {
	if (hardware_interface::SystemInterface::on_init(info) != hardware_interface::CallbackReturn::SUCCESS) {
		return hardware_interface::CallbackReturn::ERROR;
	}

	logger_ = std::make_shared<rclcpp::Logger>(rclcpp::get_logger("controller_manager.resource_manager.hardware_component.system.Dummy"));
	
	return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn DummyHardware::on_configure(
	const rclcpp_lifecycle::State & /*previous_state*/) {
	// setup communication with robot hardware

	/*using LibSerial::BaudRate;*/
	/*serial_port.Open("/dev/ttyACM1");*/
	/*serial_port.SetBaudRate(BaudRate::BAUD_115200);*/
	RCLCPP_INFO(get_logger(), "setting up stuff");

	char zeros[] = "<0,0,0,0,0>";

	std::stringstream raw_offsets(info_.hardware_parameters["joint_offsets"]);
	std::string token;

	while (std::getline(raw_offsets, token, ' ')) {
		joint_offsets.push_back(std::stof(token));
	}

	// Connection to serial port
	char error = serial.openDevice(SERIAL_PORT, 115200);

	if (error!=1) {
		RCLCPP_ERROR(get_logger(), "Failed to open serial port");
		return hardware_interface::CallbackReturn::ERROR;
	}
	
	serial.writeString(zeros);
	RCLCPP_INFO(get_logger(), "hardware ready");

	return CallbackReturn::SUCCESS;
}


std::vector<hardware_interface::StateInterface> DummyHardware::export_state_interfaces() {
	std::vector<hardware_interface::StateInterface> interfaces;

	if (info_.joints.size() > state_joints.size()) {
		RCLCPP_ERROR(get_logger(), "more joints than values???");
		for (int i = 0; i < info_.joints.size(); i++) {
			RCLCPP_INFO(get_logger(), "%s", info_.joints[i].name.c_str());
			RCLCPP_ERROR(get_logger(), "joint???");
		}
	}

	

	for (unsigned int i = 0; i < info_.joints.size(); i++) {
		interfaces.emplace_back(hardware_interface::StateInterface(info_.joints[i].name, hardware_interface::HW_IF_POSITION, &state_joints[i]));	
	}
	
	return interfaces;

}

std::vector<hardware_interface::CommandInterface> DummyHardware::export_command_interfaces() {
	std::vector<hardware_interface::CommandInterface> interfaces;

	for (unsigned int i = 0; i < info_.joints.size(); i++) {
		RCLCPP_INFO(get_logger(), "Command interface: %s initalized", info_.joints[i].name.c_str());
		interfaces.emplace_back(hardware_interface::CommandInterface(info_.joints[i].name, hardware_interface::HW_IF_POSITION, &cmd_joints[i]));	
	}

	return interfaces;
}

hardware_interface::return_type DummyHardware::read(const rclcpp::Time & time, const rclcpp::Duration & period) {
	return hardware_interface::return_type::OK;	
}

hardware_interface::return_type DummyHardware::write(const rclcpp::Time & time, const rclcpp::Duration & period) {
	if (cmd_joints == cmd_joints_cache) {
		return hardware_interface::return_type::OK;
	}


	
	using namespace std::chrono;

	std::chrono::milliseconds deley(20);

	if (duration_cast<milliseconds>(system_clock::now().time_since_epoch()) - last_message < deley) {
		return hardware_interface::return_type::OK;
	}
	

	std::stringstream ss;
	/*ss << "<" << cmd_joints.at(0) * RAD_TO_DEGREE << "," << cmd_joints.at(1) * RAD_TO_DEGREE << "," << cmd_joints.at(2) * RAD_TO_DEGREE << cmd_joints.at(3) * RAD_TO_DEGREE << cmd_joints.at(4) * RAD_TO_DEGREE<< ">";*/
	
	ss << "<";
	for (unsigned long i = 0; i < cmd_joints.size(); i++) {
		double offset;

		try {
			offset = joint_offsets.at(i);
		} 
		catch (std::out_of_range) {
			offset = 0;
		};

		ss << (cmd_joints.at(i) * RAD_TO_DEGREE) + offset;

		i++;
		if (i < cmd_joints.size()) ss << ",";
	}
	ss << ">";

	RCLCPP_INFO(get_logger(), "sending data: %s", ss.str().c_str());
	serial.writeString(ss.str().c_str());

	state_joints = cmd_joints;
	cmd_joints_cache = cmd_joints;
	
	last_message = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	return hardware_interface::return_type::OK;
}



