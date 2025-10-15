from moveit_configs_utils import MoveItConfigsBuilder
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.substitutions import FindPackageShare
from ament_index_python.packages import get_package_share_directory
from controller_manager.launch_utils import generate_controllers_spawner_launch_description
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
        .joint_limits(file_path="config/joint_limits.yaml")
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

    # arm_ctrl = generate_controllers_spawner_launch_description(
    #     ["joint_trajectory_controller"],
    #     controller_params_files=[ros2_controllers_path],
    # )
    #
    #
    # arm_controller_spawner = Node(
    #     package="controller_manager",
    #     executable="spawner",
    #     arguments=["joint_trajectory_controller", "-c", "/controller_manager"],
    #     parameters=[{"type": "position_controllers/JointTrajectoryController"}],
    #
    # )

    arm_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["main_controller", "-c", "/controller_manager"],
        parameters=[{"type": "position_controllers/JointTrajectoryController"}],

    )

    #moveit 

    run_move_group_node = Node(
        package="moveit_ros_move_group",
        executable="move_group",
        output="screen",
        parameters=[moveit_config.to_dict()],
    )

    
    rviz_config = os.path.join(
        get_package_share_directory("moveit_config"),
        "config",
        "moveit.rviz"
    )

    # Launch RViz
    rviz_node = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        output="log",
        arguments=["-d", rviz_config],
        parameters=[
            moveit_config.robot_description,
            moveit_config.robot_description_semantic,
            moveit_config.robot_description_kinematics,
            moveit_config.planning_pipelines,
            moveit_config.joint_limits,
        ],
    )


    #dont know if the is important but whatev
    # Static TF
    static_tf = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        name="static_transform_publisher",
        output="log",
        arguments=["--frame-id", "world", "--child-frame-id", "base_link"],
    )


    return LaunchDescription(
        [
            # rviz_config_arg,
            rviz_node,
            static_tf,
            robot_state_publisher,
            run_move_group_node,
            ros2_control_node,
            joint_state_broadcaster_spawner,
            arm_controller_spawner,
            # hand_controller_spawner,
            # arm_ctrl
        ]
    )



 
