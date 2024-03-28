#include <rclc/executor.h>
#include <rclc/rclc.h>
#include <stdio.h>

/* customized timer callback type */
typedef void (*timer_callback_t)(void *context);

/* customized timer_context_t */
typedef struct demo_timer_context_s {
  rcl_timer_t timer;
  timer_callback_t callback; /* for calling  customized timer_callback */
  void *context;             /* for passing customized context */
  uint64_t period;
} demo_timer_context_t;

/* callback function to regist */
static void my_timer_callback(rcl_timer_t *timer, int64_t last_call_time) {
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    demo_timer_context_t *timer_context = (demo_timer_context_t *)timer;
    if (timer_context != NULL) {
      timer_context->callback(timer_context->context);
    }
  }
}

static void custom_timer_callback(void *context) {
  int *counter = (int *)context;
  printf("custom_timer_callback: %d\n", *counter);
  (*counter)++;
}

int main(int argc, const char *const *argv) {
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rclc_support_t support;
  rcl_ret_t rc;

  // create init_options
  rc = rclc_support_init(&support, argc, argv, &allocator);
  if (rc != RCL_RET_OK) {
    printf("Error rclc_support_init.\n");
    return -1;
  }

  // create rcl_node
  rcl_node_t my_node = rcl_get_zero_initialized_node();
  rc = rclc_node_init_default(&my_node, "node_0", "", &support);
  if (rc != RCL_RET_OK) {
    printf("Error in rclc_node_init_default\n");
    return -1;
  }

  int count = 0;

  // create a customized timer_context_t
  demo_timer_context_t timer_context;
  timer_context.timer = rcl_get_zero_initialized_timer();
  timer_context.period = 1000;
  timer_context.callback = custom_timer_callback;
  timer_context.context = &count;

  // create a timer, which will call the custom callback function with period=
  // `timer_timeout` ms
  rc = rclc_timer_init_default(&timer_context.timer, &support,
                               RCL_MS_TO_NS(timer_context.period),
                               my_timer_callback);

  if (rc != RCL_RET_OK) {
    printf("Error in rcl_timer_init_default.\n");
    return -1;
  } else {
    printf("timer initialized with period %lu ms\n", timer_context.period);
  }

  // add timer to executor, since we only have one callback function.
  size_t num_handles = 1;
  rclc_executor_t executor;
  rc = rclc_executor_init(&executor, &support.context, num_handles, &allocator);
  if (rc != RCL_RET_OK) {
    printf("Error in rclc_executor_init.\n");
    return -1;
  }

  rc = rclc_executor_add_timer(&executor, &timer_context.timer);
  if (rc != RCL_RET_OK) {
    printf("Error in rclc_executor_add_timer.\n");
    return -1;
  }

  // spin the node
  rclc_executor_spin(&executor);

  // clean up
  rc = rclc_executor_fini(&executor);
  rc += rcl_timer_fini(&timer_context.timer);
  rc += rcl_node_fini(&my_node);
  rc += rclc_support_fini(&support);

  if (rc != RCL_RET_OK) {
    printf("Error while cleaning up!\n");
    return -1;
  }

  return 0;
}
