#include <rclc/executor.h>
#include <rclc/rclc.h>
#include <std_msgs/msg/float32.h>
#include <stdio.h>

rcl_publisher_t my_pub;
rcl_timer_t my_timer;
const rosidl_message_type_support_t *my_type_support = NULL;

void timer_callback(rcl_timer_t *timer, int64_t last_call_time) {
  (void)last_call_time;
  (void)timer;

  bool ret = rcl_publisher_can_loan_messages(&my_pub);
  if (ret) {
    rmw_publisher_t *rmw_publisher = rcl_publisher_get_rmw_handle(&my_pub);
    void *loaned_msg = NULL;
    rcl_ret_t rc =
        rmw_borrow_loaned_message(rmw_publisher, my_type_support, &loaned_msg);

    if (rc == RCL_RET_OK && loaned_msg != NULL) {
      RCUTILS_LOG_INFO("Loan message from rmw success.\n");
    } else {
      RCUTILS_LOG_ERROR("Loan message from rmw failed\n");
    }
    ((std_msgs__msg__Float32 *)loaned_msg)->data = 1.0;
    // This pub function will return the loaned message to rmw.
    rc = rcl_publish_loaned_message(&my_pub, loaned_msg, NULL);
    if (rc != RCL_RET_OK) {
      RCUTILS_LOG_ERROR("Error rcl_publish_loaned_message\n");
    } else {
      RCUTILS_LOG_INFO("Publish Loaned Message Success!\n");
    }
  }
}

int main(int argc, const char *const *argv) {
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rclc_support_t support;
  rcl_ret_t rc;

  // create init options
  rc = rclc_support_init(&support, argc, argv, &allocator);
  if (rc != RCL_RET_OK) {
    printf("Error rclc_support_init\n");
    return -1;
  }

  // create rcl_node
  rcl_node_t my_node = rcl_get_zero_initialized_node();
  rc = rclc_node_init_default(&my_node, "node_pub_loan", "", &support);
  if (rc != RCL_RET_OK) {
    printf("Error in rclc_node_init_default\n");
    return -1;
  }

  const char *topic_name = "TEST_LOANED_TOPIC";
  my_type_support = ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32);

  if (my_type_support == NULL) {
    printf("Error in ROSIDL_GET_MSG_TYPE_SUPPORT\n");
    return -1;
  }

  rc = rclc_publisher_init_default(&my_pub, &my_node, my_type_support,
                                   topic_name);

  if (RCL_RET_OK != rc) {
    printf("Error in rclc_publisher_init_default %s.\n", topic_name);
    return -1;
  }

  // wait for dds to finish discovery
  rclc_sleep_ms(1000);

  // create timer,
  rcl_timer_t my_timer = rcl_get_zero_initialized_timer();
  const unsigned int timer_timeout = 1000;
  rc = rclc_timer_init_default(&my_timer, &support, RCL_MS_TO_NS(timer_timeout),
                               timer_callback);
  if (rc != RCL_RET_OK) {
    printf("Error in rclc_timer_init_default.\n");
    return -1;
  }

  // create executor
  rclc_executor_t executor = rclc_executor_get_zero_initialized_executor();
  unsigned int num_handles = 1;
  rc = rclc_executor_init(&executor, &support.context, num_handles, &allocator);
  if (rc != RCL_RET_OK) {
    printf("Error in rclc_executor_init.\n");
    return -1;
  }

  // add timer to executor
  rc = rclc_executor_add_timer(&executor, &my_timer);
  if (rc != RCL_RET_OK) {
    printf("Error in rclc_executor_add_timer.\n");
    return -1;
  }

  // spin executor
  rclc_executor_spin(&executor);

  // clean up
  rc = rclc_executor_fini(&executor);
  rc += rcl_timer_fini(&my_timer);
  rc += rcl_publisher_fini(&my_pub, &my_node);
  rc += rcl_node_fini(&my_node);
  rc += rclc_support_fini(&support);

  if (rc != RCL_RET_OK) {
    printf("Error while cleaning up\n");
    return -1;
  }

  return 0;
}
