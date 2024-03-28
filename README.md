# rclc_example

Example codes as a complement to rclc/examples

ROS2 Distro: `Humble`

docker image: `tonghezhang/ros2_x86:humble_lint_0.1`

## Examples

1. [timer with context](src/timer_with_context.c)
2. [publish with loaned_msg](src/pub_loaned_msg.c)
3. [subscription with loaned msg](src/sub_loaned_msg.c)
4. [subscription with context](src/sub_context_callback.c)

### prerequisites

To use `subscription_with_loaned_msg`, we need to add msg_loan in rclc/executor.c

#### rcl_take_loaned_message

[take loaned message](https://github.com/ros2/rclc/compare/humble...Zard-C:rclc:humble#diff-82aa34a48fc24ad5d3eefc4e7937b764b3ff91947b0e7394ffab7bc78def736b)

#### rcl_return_loaned_message_from_subscription

[return loaned message](https://github.com/Zard-C/rclc_example/blob/main/rclc_example/src/sub_loaned_msg.c)

This change is already on branch humble of [my fork of rclc](https://github.com/Zard-C/rclc),

currently, you can use my repo [rclc](https://github.com/Zard-C/rclc) to test this feature

build rclc from source

```bash
git clone https://github.com/Zard-C/rclc.git
git checkout humble
colcon build --packages-select rclc
```

build rclc_example from source

```bash
colcon build --packages-select rclc_example
```

use ros2's default middleware (fastrtps) zero copy feature

```bash
export FASTRTPS_DEFAULT_PROFILES_FILE=${PATH_TO_CONFIG}/fast_dds_profiles.xml
export RMW_FASTRTPS_USE_QOS_FROM_XML=1
```

replace `${PATH_TO_CONFIG}` with your own path (we have a demo in [demo_config_file](config/fastrtps/fast_dds_profiles.xml))

## Build

```bash
colcon build --packages-select rclc_example
```

## dependencies and version

[rclc](https://github.com/Zard-C/rclc)

## environment

```bash
source /opt/ros2/humble/setup.bash
source ${rclc}/install/setup.bash
source ${rclc_example}/install/setup.bash

# for using fastrtps's zero copy, in your rclc_example workspace
export FASTRTPS_DEFAULT_PROFILES_FILE=${PATH_TO_CONFIG}/fast_dds_profiles.xml
export RMW_FASTRTPS_USE_QOS_FROM_XML=1

# run in one terminal
./build/rclc_example/pub_loaned_msg

# run in another terminal
./build/rclc_example/sub_loaned_msg

```

## features

- [x] timer_with_context
- [x] publish_with_loaned_msg
- [x] subscription_with_loaned_msg
- [x] subscription_with_context
