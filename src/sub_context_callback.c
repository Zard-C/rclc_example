#include <rclc/executor.h>
#include <rclc/rclc.h>
#include <std_msgs/msg/string.h>
#include <stdio.h>

typedef void (*sub_callback_t)(const void *msgin);

typedef struct
{
    // rcl_subscription_t sub;
    sub_callback_t callback;
} sub_context_t;

static void custom_sub_callback_foo(const void *msgin)
{
    const std_msgs__msg__String *msg = (const std_msgs__msg__String *)msgin;
    if (msg == NULL)
    {
        printf("Callback: msg NULL\n");
    }
    else
    {
        printf("custom_sub_callback_foo: I heard: %s\n", msg->data.data);
    }
}
static void custom_sub_callback_bar(const void *msgin)
{
    const std_msgs__msg__String *msg = (const std_msgs__msg__String *)msgin;
    if (msg == NULL)
    {
        printf("Callback: msg NULL\n");
    }
    else
    {
        printf("custom_sub_callback_bar: I heard: %s\n", msg->data.data);
    }
}

static void custom_sub_callback_baz(const void *msgin)
{
    const std_msgs__msg__String *msg = (const std_msgs__msg__String *)msgin;
    if (msg == NULL)
    {
        printf("Callback: msg NULL\n");
    }
    else
    {
        printf("custom_sub_callback_baz: I heard: %s\n", msg->data.data);
    }
}

void my_subscriber_callback_with_context(const void *msgin, void *context_void_ptr)
{
    const std_msgs__msg__String *msg = (const std_msgs__msg__String *)msgin;
    if (msg == NULL)
    {
        printf("Callback: msg NULL\n");
        return;
    }

    if (context_void_ptr == NULL)
    {
        printf("Callback: context is empty\n");
    }
    else
    {
        sub_context_t *context_ptr = (sub_context_t *)context_void_ptr;
        if (context_ptr->callback == NULL)
        {
            printf("Callback: callback is empty\n");
            return;
        }
        context_ptr->callback(msgin);
    }
}

int main(int argc, const char *argv[])
{
    rcl_allocator_t allocator = rcl_get_default_allocator();
    rclc_support_t support;
    rcl_ret_t rc;

    // within main, we can create the state information our subscriptions work
    // with
    const unsigned int n_topics = 3;
    const char *topic_names[] = {"topic_foo", "topic_bar", "topic_baz"};
    sub_context_t my_contexts[] = {{custom_sub_callback_foo}, {custom_sub_callback_bar}, {custom_sub_callback_baz}};
    rcl_publisher_t my_pubs[n_topics];
    std_msgs__msg__String pub_msgs[n_topics];
    rcl_subscription_t my_subs[n_topics];
    std_msgs__msg__String sub_msgs[n_topics];

    // create init_options
    rc = rclc_support_init(&support, argc, argv, &allocator);
    if (rc != RCL_RET_OK)
    {
        printf("Error rclc_support_init.\n");
        return -1;
    }

    // create rcl_node
    rcl_node_t my_node = rcl_get_zero_initialized_node();
    rc = rclc_node_init_default(&my_node, "node_0", "executor_examples", &support);
    if (rc != RCL_RET_OK)
    {
        printf("Error in rclc_node_init_default\n");
        return -1;
    }

    const rosidl_message_type_support_t *my_type_support = ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String);

    // initialise each publisher and subscriber
    for (unsigned int i = 0; i < n_topics; i++)
    {
        rc = rclc_publisher_init_default(&(my_pubs[i]), &my_node, my_type_support, topic_names[i]);
        if (RCL_RET_OK != rc)
        {
            printf("Error in rclc_publisher_init_default %s.\n", topic_names[i]);
            return -1;
        }
        // assign message to publisher
        std_msgs__msg__String__init(&(pub_msgs[i]));
        const unsigned int PUB_MSG_CAPACITY = 40;
        pub_msgs[i].data.data = allocator.reallocate(pub_msgs[i].data.data, PUB_MSG_CAPACITY, allocator.state);
        pub_msgs[i].data.capacity = PUB_MSG_CAPACITY;
        snprintf(pub_msgs[i].data.data, pub_msgs[i].data.capacity, "Hello World! on %s", topic_names[i]);
        pub_msgs[i].data.size = strlen(pub_msgs[i].data.data);

        // create subscription
        my_subs[i] = rcl_get_zero_initialized_subscription();
        rc = rclc_subscription_init_default(&my_subs[i], &my_node, my_type_support, topic_names[i]);
        if (rc != RCL_RET_OK)
        {
            printf("Failed to create subscriber %s.\n", topic_names[i]);
            return -1;
        }
        else
        {
            printf("Created subscriber %s:\n", topic_names[i]);
        }

        // one string message for subscriber
        std_msgs__msg__String__init(&(sub_msgs[i]));
    }

    ////////////////////////////////////////////////////////////////////////////
    // Configuration of RCL Executor
    ////////////////////////////////////////////////////////////////////////////
    rclc_executor_t executor;
    executor = rclc_executor_get_zero_initialized_executor();
    // total number of handles = #subscriptions + #timers
    // Note:
    // If you need more than the default number of publisher/subscribers, etc.,
    // you need to configure the micro-ROS middleware also! See documentation in
    // the executor.h at the function rclc_executor_init() for more details.
    unsigned int num_handles = n_topics + 0;
    printf("Debug: number of DDS handles: %u\n", num_handles);
    rclc_executor_init(&executor, &support.context, num_handles, &allocator);

    // add subscriptions to executor
    for (unsigned int i = 0; i < n_topics; i++)
    {
        // create a void* pointer to any information you want in your callback
        //   make sure you cast back to the the same type before accessing it.
        sub_context_t *context_ptr = &(my_contexts[i]);
        void *context_void_ptr = (void *)context_ptr;

        // add subscription to executor
        rc = rclc_executor_add_subscription_with_context(&executor, &(my_subs[i]), &(sub_msgs[i]),
                                                         &my_subscriber_callback_with_context, // all subs here use the
                                                                                               // same callback
                                                         context_void_ptr, // equivalently: (void*) &( my_contexts[i] ),
                                                         ON_NEW_DATA);
        if (rc != RCL_RET_OK)
        {
            printf("Error in rclc_executor_add_subscription. \n");
        }
    }

    for (unsigned int tick = 0; tick < 10; tick++)
    {
        // timeout specified in nanoseconds (here 1s)
        rc = rclc_executor_spin_some(&executor, 1000 * (1000 * 1000));

        for (unsigned int i = 0; i < n_topics; i++)
        {
            // publish once for each topic
            rc = rcl_publish(&my_pubs[i], &pub_msgs[i], NULL);
            if (rc == RCL_RET_OK)
            {
                printf("\nPublished message %s\n", pub_msgs[i].data.data);
            }
            else
            {
                printf("Error publishing message %s\n", pub_msgs[i].data.data);
            }

            // capture the message in the callback
            rc = rclc_executor_spin_some(&executor, 1000 * (1000 * 1000));
        }
    }

    // clean up
    rc = rclc_executor_fini(&executor);

    for (unsigned int i = 0; i < n_topics; i++)
    {
        rc += rcl_publisher_fini(&(my_pubs[i]), &my_node);
        rc += rcl_subscription_fini(&(my_subs[i]), &my_node);
    }
    rc += rcl_node_fini(&my_node);
    rc += rclc_support_fini(&support);

    for (unsigned int i = 0; i < n_topics; i++)
    {
        std_msgs__msg__String__fini(&(pub_msgs[i]));
        std_msgs__msg__String__fini(&(sub_msgs[i]));
    }

    if (rc != RCL_RET_OK)
    {
        printf("Error while cleaning up!\n");
        return -1;
    }
    return 0;
}