

#ifndef DUMMY_HARDWARE_HPP
#define DUMMY_HARDWARE_HPP


#include "hardware_interface/handle.hpp"
#include "hardware_interface/hardware_info.hpp"
#include "hardware_interface/system_interface.hpp"
#include "hardware_interface/types/hardware_interface_return_values.hpp"
#include "rclcpp/clock.hpp"
#include "rclcpp/logger.hpp"
#include "rclcpp/macros.hpp"
#include "rclcpp_lifecycle/node_interfaces/lifecycle_node_interface.hpp"
#include "rclcpp_lifecycle/state.hpp"
#include <memory>
#include <vector>
#include <chrono>
#include "serial.h"

const double RAD_TO_DEGREE = 180/3.14;

namespace hardware {
class DummyHardware : public hardware_interface::SystemInterface {
public: 	

  hardware_interface::CallbackReturn on_init(const hardware_interface::HardwareInfo & info) override;
	
  std::vector<hardware_interface::StateInterface> export_state_interfaces() override;
  std::vector<hardware_interface::CommandInterface> export_command_interfaces() override;

  hardware_interface::CallbackReturn on_configure(const rclcpp_lifecycle::State & ) override;

  hardware_interface::return_type read(const rclcpp::Time & time, const rclcpp::Duration & period) override;
  hardware_interface::return_type write(const rclcpp::Time & time, const rclcpp::Duration & period) override;

  std::shared_ptr<rclcpp::Logger> logger_;
  rclcpp::Logger get_logger() const { return *logger_; }

  int thing() {
    serialib ser;
    ser.isDeviceOpen();
    return 0;
  }

private:
  std::vector<double> cmd_joints = {0,0,0,0,0};
  std::vector<double> cmd_joints_cache = {0,0,0,0,0};
  std::vector<double> state_joints = {0,0,0,0,0};
  std::vector<double> joint_offsets;
  std::chrono::milliseconds last_message;

  serialib serial;
};

}

#endif
