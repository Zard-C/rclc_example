#include <rclc/executor.h>
#include <rclc/rclc.h>
#include <std_msgs/msg/float32.h>
#include <stdio.h>

rcl_publisher_t my_pub;

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
  const rosidl_message_type_support_t *my_type_support =
      ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32);

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

  rclc_sleep_ms(1000);

  // loan_msgs
  bool ret = rcl_publisher_can_loan_messages(&my_pub);
  printf("rcl_publisher is %s.\n", ret ? "loanable" : "unloanable");

  if (ret) {
    void *loaned_msg = NULL;
    rmw_publisher_t *rmw_publisher = rcl_publisher_get_rmw_handle(&my_pub);
    rc = rmw_borrow_loaned_message(rmw_publisher, my_type_support, &loaned_msg);

    if (rc == RCL_RET_OK && loaned_msg != NULL) {
      printf("Loan message from rmw success.\n");
    } else {
      printf("Loan message from rmw failed\n");
    }

    // This pub function will return the loaned message to rmw.
    rc = rcl_publish_loaned_message(&my_pub, loaned_msg, NULL);
    if (rc != RCL_RET_OK) {
      printf("Error rcl_publish_loaned_message\n");
      return -1;
    } else {
      printf("Publishing Loaned Message Success!\n");
    }
  }

  // sleep for a while
  rclc_sleep_ms(1000);

  // clean up
  rc = rcl_publisher_fini(&my_pub, &my_node);
  rc += rcl_node_fini(&my_node);
  rc += rclc_support_fini(&support);

  if (rc != RCL_RET_OK) {
    printf("Error while cleaning up\n");
    return -1;
  }

  return 0;
}
