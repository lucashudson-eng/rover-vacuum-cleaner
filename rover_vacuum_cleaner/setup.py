from setuptools import find_packages, setup
import os
from glob import glob

package_name = 'rover_vacuum_cleaner'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        (os.path.join('share', package_name, 'launch'), glob('launch/*.launch.py')),
        (os.path.join('share', package_name, 'worlds'), glob('worlds/*.sdf')),
        (os.path.join('share', package_name, 'config'), glob('config/*.yaml')),
        (os.path.join('share', package_name, 'models/rover'), glob('models/rover/*.config')),
        (os.path.join('share', package_name, 'models/rover'), glob('models/rover/*.sdf')),
        (os.path.join('share', package_name, 'models/rover/meshes'), glob('models/rover/meshes/*.STL')),
        (os.path.join('share', package_name, 'models/rover/textures'), glob('models/rover/textures/*.dae')),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='Lucas Hudson',
    maintainer_email='lucashudson.eng@gmail.com',
    description='Autonomous robotic rover designed for navigation and task execution, inspired by smart vacuum cleaners.',
    license='GPL-3.0-or-later',
    entry_points={
        'console_scripts': [
        ],
    },
)
