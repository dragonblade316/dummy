from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution

import os

def generate_launch_description():
    desc = get_package_share_directory('dummy_description')
    bringup = get_package_share_directory('dummy_bringup')

    urdf_file  =  os.path.join(desc, 'model', 'robot.urdf')
    with open(urdf_file, 'r') as infp:
        robot_desc = infp.read()

    return LaunchDescription([
        Node(
            package='joy',
            executable='joy_node',
        ),
        Node(
            package='dummy_serial',
            executable='serial'
        ),
        # Node(
        #     package='dummy_joy_control',
        #     executable='controller'
        # ),
        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            name='robot_state_publisher',
            output='both',
            parameters=[
                {'robot_description': robot_desc},
            ]
        ),
        Node(
            package='dummy_controller',
            executable='controller'
        ),
        Node(
           package='rviz2',
           executable='rviz2',
           arguments=['-d', os.path.join(bringup, 'config', 'config.rviz')],
            # condition=IfCondition(LaunchConfiguration('rviz'))
        )
    ])

