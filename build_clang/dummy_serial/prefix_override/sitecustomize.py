import sys
if sys.prefix == '/usr':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/home/dragonblade316/Programming/robotics/dummy/install_clang/dummy_serial'
