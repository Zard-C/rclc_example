cmake_minimum_required(VERSION 3.8)
project(rclc_example)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclc REQUIRED)
find_package(std_msgs REQUIRED)

add_executable(timer_with_context src/timer_with_context.c)
ament_target_dependencies(timer_with_context rclc)

add_executable(pub_loaned_msg src/pub_loaned_msg.c)
ament_target_dependencies(pub_loaned_msg rclc std_msgs)

add_executable(sub_loaned_msg src/sub_loaned_msg.c)
ament_target_dependencies(sub_loaned_msg rclc std_msgs)

add_executable(sub_context_callback src/sub_context_callback.c)
ament_target_dependencies(sub_context_callback rclc std_msgs)

install(TARGETS
  timer_with_context
  pub_loaned_msg
  sub_loaned_msg
  sub_context_callback
  DESTINATION bin/${PROJECT_NAME})

install(DIRECTORY
  config
  DESTINATION share/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
