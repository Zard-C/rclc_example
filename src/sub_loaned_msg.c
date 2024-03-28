#include <rclc/executor.h>
#include <rclc/rclc.h>
#include <std_msgs/msg/float32.h>
#include <stdio.h>

rcl_subscription_t my_sub;

void my_sub_callback(const void *msgin)
{
    const std_msgs__msg__Float32 *loaned_msg =
        (const std_msgs__msg__Float32 *)msgin; /* since the loaned_msg is token from rmw, don't modify it, use it
                                                  as an input*/
    printf("In sub_callback: loaned_msg data: %f\n", loaned_msg->data);
    // /* return loaned message to rmw*/
    // rcl_ret_t rc = rcl_return_loaned_message_from_subscription(&my_sub,
    //                                                            (void *) loaned_msg);
    // if (rc != RCL_RET_OK) {
    //   printf("Error in return loaned message. %d\n", rc);
    // }
}

int main(int argc, const char *const *argv)
{
    rcl_allocator_t allocator = rcl_get_default_allocator();
    rclc_support_t support;
    rcl_ret_t rc;

    std_msgs__msg__Float32 my_msg;
    std_msgs__msg__Float32__init(&my_msg);

    // create init options
    rc = rclc_support_init(&support, argc, argv, &allocator);
    if (rc != RCL_RET_OK)
    {
        printf("Error rclc_support_init\n");
        return -1;
    }

    // create rcl_node
    rcl_node_t my_node = rcl_get_zero_initialized_node();
    rc = rclc_node_init_default(&my_node, "demo_node_sub_loan", "", &support);
    if (rc != RCL_RET_OK)
    {
        printf("Error in rclc_node_init_default\n");
        return -1;
    }

    const char *topic_name = "TEST_LOANED_TOPIC";

    // get type support
    const rosidl_message_type_support_t *my_type_support = ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32);

    // create subscription
    my_sub = rcl_get_zero_initialized_subscription();
    rc = rclc_subscription_init_default(&my_sub, &my_node, my_type_support, topic_name);

    if (rc != RCL_RET_OK)
    {
        printf("Error in rclc_subscription_init_default.\n");
        return -1;
    }

    // configuration of rcl executor

    rclc_executor_t executor;
    executor = rclc_executor_get_zero_initialized_executor();

    unsigned int num_handles = 1; // only a subscription callback handle
    printf("Debug: number of DDS handlers: %u\n", num_handles);
    rclc_executor_init(&executor, &support.context, num_handles, &allocator);

    int ret = rcl_subscription_can_loan_messages(&my_sub);
    printf("Message %s : %s\n", "Float32", ret ? "Can Loan" : "Cannot Loan");

    if (ret)
    {
        rc = rclc_executor_add_subscription(&executor, &my_sub, &my_msg, my_sub_callback, ON_NEW_DATA);
        if (rc != RCL_RET_OK)
        {
            printf("Error in rclc_executor_add_subscription_with_context\n");
        }
        else
        {
            rclc_executor_spin(&executor);
        }
    }

    // clean up
    rc = rclc_executor_fini(&executor);
    rc += rcl_subscription_fini(&my_sub, &my_node);
    rc += rcl_node_fini(&my_node);
    rc += rclc_support_fini(&support);

    if (rc != RCL_RET_OK)
    {
        printf("Error while cleaning up.\n");
        return -1;
    }

    return 0;
}