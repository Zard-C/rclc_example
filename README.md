# rclc_example

Example codes as a complement to rclc/examples
ROS2 Distro: `Humble`


## Examples

1. [timer_with_context](rclc_example/src/timer_with_context.c)
2. [publish_with_loaned_msg](rclc_example/src/pub_loaned_msg.c)
3. [subscription_with_loaned_msg](rclc_example/src/sub_loaned_msg.c)

### prerequisites

To use `subscription_with_loaned_msg`, we need to add msg_loan in rclc/executor.c

```diff
diff --git a/rclc/src/rclc/executor.c b/rclc/src/rclc/executor.c
index b91aa78..873790f 100644
--- a/rclc/src/rclc/executor.c
+++ b/rclc/src/rclc/executor.c
@@ -1104,20 +1104,39 @@ _rclc_take_new_data(rclc_executor_handle_t * handle, rcl_wait_set_t * wait_set)
     case RCLC_SUBSCRIPTION_WITH_CONTEXT:
       if (wait_set->subscriptions[handle->index]) {
         rmw_message_info_t messageInfo;
-        rc = rcl_take(
-          handle->subscription, handle->data, &messageInfo,
-          NULL);
-        if (rc != RCL_RET_OK) {
-          // rcl_take might return this error even with successfull rcl_wait
+        rcl_subscription_t* sub = handle->subscription;
+        if(rcl_subscription_can_loan_messages(sub)){
+          handle->data = NULL;
+          rc = rcl_take_loaned_message(sub, &handle->data, &messageInfo, NULL); // how to return it to rmw????
+          if (rc != RCL_RET_OK) {
+          // rcl_take_loaned_message might return this error even with successfull rcl_wait
           if (rc != RCL_RET_SUBSCRIPTION_TAKE_FAILED) {
-            PRINT_RCLC_ERROR(rclc_take_new_data, rcl_take);
+            PRINT_RCLC_ERROR(rcl_take_loaned_message, rcl_take_loaned_message);
             RCUTILS_LOG_ERROR_NAMED(ROS_PACKAGE_NAME, "Error number: %d", rc);
           }
-          // invalidate that data is available, because rcl_take failed
+          // invalidate that data is available, because rcl_take_loaned_message failed
           if (rc == RCL_RET_SUBSCRIPTION_TAKE_FAILED) {
             handle->data_available = false;
           }
           return rc;
+          }
+        }
+        else {
+          rc = rcl_take(
+            handle->subscription, handle->data, &messageInfo,
+            NULL);
+          if (rc != RCL_RET_OK) {
+            // rcl_take might return this error even with successfull rcl_wait
+            if (rc != RCL_RET_SUBSCRIPTION_TAKE_FAILED) {
+              PRINT_RCLC_ERROR(rclc_take_new_data, rcl_take);
+              RCUTILS_LOG_ERROR_NAMED(ROS_PACKAGE_NAME, "Error number: %d", rc);
+            }
+            // invalidate that data is available, because rcl_take failed
+            if (rc == RCL_RET_SUBSCRIPTION_TAKE_FAILED) {
+              handle->data_available = false;
+            }
+            return rc;
+          }
         }
       }
       break;
```

This change is already on branch  [humble_add_msg_loan_in_sub_callback](https://github.com/Zard-C/rclc/pull/1) in [my fork of rclc](https://github.com/Zard-C/rclc),

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

