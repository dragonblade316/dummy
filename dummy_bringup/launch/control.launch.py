from moveit_configs_utils import MoveItConfigsBuilder
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.substitutions import FindPackageShare
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
import os

def generate_launch_description():
    urdf = os.path.join(
        get_package_share_directory("moveit_config"),
        "config",
        "dummy.urdf.xacro",
    )


    moveit_config = (
        MoveItConfigsBuilder(
            "gen3", package_name="moveit_config"
        )
        .robot_description(file_path="config/dummy.urdf.xacro")
        # .robot_description()       
        .trajectory_execution(file_path="config/moveit_controllers.yaml")
        .planning_scene_monitor(
            publish_robot_description=True, publish_robot_description_semantic=True
        )
        #.planning_pipelines(
        #     pipelines=["ompl", "stomp", "pilz_industrial_motion_planner"]
        # )
        .to_moveit_configs()
    )

    robot_state_publisher = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        name="robot_state_publisher",
        output="both",
        # parameters=[{'robot_description': moveit_config.robot_description}],
        parameters=[moveit_config.robot_description]
    )

    # print(moveit_config.robot_description)


    ros2_controllers_path = os.path.join(
        get_package_share_directory("moveit_config"),
        "config",
        "ros2_controllers.yaml",
    )

    ros2_control_node = Node(
        package="controller_manager",
        executable="ros2_control_node",
        parameters=[ros2_controllers_path],
        remappings=[
            ("/controller_manager/robot_description", "/robot_description"),
        ],
        output="both",
    )

    joint_state_broadcaster_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=[
            "joint_state_broadcaster",
            "--controller-manager",
            "/controller_manager",
        ],
    )

    # arm_controller_spawner = Node(
    #     package="controller_manager",
    #     executable="spawner",
    #     arguments=["joint_trajectory_controller", "-c", "/controller_manager"],
    # )
    #
    return LaunchDescription(
        [
            # rviz_config_arg,
            # rviz_node,
            # static_tf,
            robot_state_publisher,
            # run_move_group_node,
            ros2_control_node,
            joint_state_broadcaster_spawner,
            # arm_controller_spawner,
            # hand_controller_spawner,
        ]
    )



 
