# rclc_example

Example codes as a complement to rclc/examples
ROS2 Distro: `Humble`


## Examples

1. [timer_with_context](rclc_example/src/timer_with_context.c)
2. [publish_with_loaned_msg](rclc_example/src/pub_loaned_msg.c)
3. [subscription_with_loaned_msg](rclc_example/src/sub_loaned_msg.c)

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
$ git clone https://github.com/Zard-C/rclc.git
$ git checkout humble
$ colcon build --packages-select rclc
```
build rclc_example from source

```bash
$ colcon build --packages-select rclc_example
```
use ros2's default middleware (fastrtps) zero copy feature

```bash
export FASTRTPS_DEFAULT_PROFILES_FILE=${PATH_TO_CONFIG}/fast_dds_profiles.xml
export RMW_FASTRTPS_USE_QOS_FROM_XML=1
```
replace `${PATH_TO_CONFIG}` with your own path (we have a demo in [demo_config_file](config/fastrtps/fast_dds_profiles.xml))

## Build

```bash
$ colcon build --packages-select rclc_example
```
## dependencies and version

`ros2/rclc:humble` (packages-up-to rclc)

## environment

```bash
$ source /opt/ros2/humble/setup.bash
source source {rclc}/install/setup.bash
$ source install/setup.bash

# for fastrtps's zero copy, in your rclc_example workspace

export FASTRTPS_DEFAULT_PROFILES_FILE=${PATH_TO_CONFIG}/fast_dds_profiles.xml
export RMW_FASTRTPS_USE_QOS_FROM_XML=1
./build/rclc_example/sub_loaned_msg
Debug: number of DDS handlers: 1
Message Float32 : Can Loan
In sub_callback: loaned_msg data: 0.000000

```


## feature

- [x] timer_with_context
- [x] publish_with_loaned_msg
- [x] subscription_with_loaned_msg
- [ ] subscription_with_context

