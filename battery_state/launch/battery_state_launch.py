from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='battery_state',
            executable='bat_state',
            namespace='battery_ns',
            output='screen',
            parameters=[
                {'x': 5}, # rising time
                {'y' : 4.0}, # rising coeff
                {'z' : 2 },   # falling time (seconds)
                {'w' : 2.0}   # is falling coef
            ]
        )

    ])


# parametre olarak x sn de y kadar art
# parametre olarak z sn de w kadar azal

# terminalden de bool topic olarak is_charging ver True/False