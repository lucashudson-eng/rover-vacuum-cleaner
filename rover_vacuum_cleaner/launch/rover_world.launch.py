from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import SetEnvironmentVariable, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution
from launch_ros.substitutions import FindPackageShare
import os


def generate_launch_description():
    package_name = 'rover_vacuum_cleaner'
    
    ros_gz_sim_pkg_path = get_package_share_directory('ros_gz_sim')
    example_pkg_path = FindPackageShare(package_name)
    gz_launch_path = PathJoinSubstitution([ros_gz_sim_pkg_path, 'launch', 'gz_sim.launch.py'])
    
    # Caminho para o arquivo de configuração do bridge
    bridge_config_path = os.path.join(
        get_package_share_directory(package_name),
        'config',
        'ros_gz_bridge.yaml'
    )
    

    return LaunchDescription([
        SetEnvironmentVariable(
            'GZ_SIM_RESOURCE_PATH',
            PathJoinSubstitution([example_pkg_path, 'models'])
        ),
        SetEnvironmentVariable(
            'GZ_SIM_PLUGIN_PATH',
            PathJoinSubstitution([example_pkg_path, 'plugins'])
        ),
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(gz_launch_path),
            launch_arguments={
                'gz_args': PathJoinSubstitution([example_pkg_path, 'worlds/indoor_house_world.sdf']),
                'on_exit_shutdown': 'True'
            }.items(),
        ),
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(
                PathJoinSubstitution([ros_gz_sim_pkg_path, 'launch', 'gz_spawn_model.launch.py'])
            ),
            launch_arguments={
                'world': 'indoor_house_world',
                'file': PathJoinSubstitution([example_pkg_path, 'models/rover/model.sdf']),
                'entity_name': 'rover',
                'x': '1.0', 'y': '0.0', 'z': '0.05',
                'R': '0.0', 'P': '0.0', 'Y': '0.0'
            }.items(),
        ),
        Node(
            package='ros_gz_bridge',
            executable='parameter_bridge',
            parameters=[{
                'config_file': bridge_config_path,
            }],
            output='screen'
        ),
    ])
