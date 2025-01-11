#include "robot.hpp"
#include "hardware_interface/handle.hpp"
#include "hardware_interface/system_interface.hpp"
#include "hardware_interface/types/hardware_interface_return_values.hpp"
#include <cstdio>
#include <hardware_interface/types/hardware_interface_type_values.hpp>
#include <memory>
#include <rclcpp/logger.hpp>
#include <rclcpp/logging.hpp>
#include <rclcpp_lifecycle/state.hpp>
#include <rosidl_runtime_cpp/traits.hpp>
#include <pluginlib/class_list_macros.hpp>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>
#include <rclcpp/rclcpp.hpp>


#define SERIAL_PORT "/dev/ttyACM1"
#define BAUD 9600



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

	char zeros[] = "<0,0,0>";

	// Connection to serial port
	char error = serial.openDevice(SERIAL_PORT, 9600);

	if (error!=1) {
		RCLCPP_ERROR(get_logger(), "Failed to open serial port");
		return hardware_interface::CallbackReturn::ERROR;
	}
	//I apologize for nothing
	joints.push_back(0.0);
	joints.push_back(0.0);
	joints.push_back(0.0);
	
	serial.writeString(zeros);
	RCLCPP_INFO(get_logger(), "hardware ready");

	return CallbackReturn::SUCCESS;
}


std::vector<hardware_interface::StateInterface> DummyHardware::export_state_interfaces() {
	std::vector<hardware_interface::StateInterface> interfaces;

	if (info_.joints.size() > joints.size()) {
		RCLCPP_ERROR(get_logger(), "more joints than values??? num is: %i", info_.joints.size());
		for (int i = 0; i < info_.joints.size(); i++) {
			RCLCPP_INFO(get_logger(), "%s", info_.joints[i].name.c_str());
			RCLCPP_ERROR(get_logger(), "joint???");
		}
	}

	

	for (unsigned int i = 0; i < info_.joints.size(); i++) {
		interfaces.emplace_back(hardware_interface::StateInterface(info_.joints[i].name, hardware_interface::HW_IF_POSITION, &joints[i]));	
	}
	
	return interfaces;

}

std::vector<hardware_interface::CommandInterface> DummyHardware::export_command_interfaces() {
	std::vector<hardware_interface::CommandInterface> interfaces;

	for (unsigned int i = 0; i < info_.joints.size(); i++) {
		interfaces.emplace_back(hardware_interface::CommandInterface(info_.joints[i].name, hardware_interface::HW_IF_POSITION, &joints[i]));	
	}

	return interfaces;
}

hardware_interface::return_type DummyHardware::read(const rclcpp::Time & time, const rclcpp::Duration & period) {
	return hardware_interface::return_type::OK;	
}

hardware_interface::return_type DummyHardware::write(const rclcpp::Time & time, const rclcpp::Duration & period) {
	std::stringstream ss;
	ss << "<" << joints.at(0) << "," << joints.at(1) << "," << joints.at(2) << ">";
	serial.writeString(ss.str().c_str());
	
	return hardware_interface::return_type::OK;
}



