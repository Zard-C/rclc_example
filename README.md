# rclc_example

Example codes as a complement to rclc/examples

## Examples

1. [timer_with_context](rclc_example/src/timer_with_context.c)
2. [publish_with_loaned_msg](rclc_example/src/pub_loaned_msg.c)

## Build

```bash
$ colcon build --packages-select rclc_example
```
## dependencies and version

`ros2/rclc:humble` (packages-up-to)

## environment

```bash
$ source /opt/ros/humble/setup.bash
$ source install/setup.bash

# for fastrtps's zero copy
export FASTRTPS_DEFAULT_PROFILES_FILE=${PATH_TO_CONFIG}/fast_dds_profiles.xml
export RMW_FASTRTPS_USE_QOS_FROM_XML=1

```


## feature

- [x] timer_with_context
- [x] publish_with_loaned_msg
- [ ] subscription_with_loaned_msg
- [ ] subscription_with_context
