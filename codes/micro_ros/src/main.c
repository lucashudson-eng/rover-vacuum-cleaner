#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/string.h>
#include <rclc/rclc_macros.h>

rcl_publisher_t publisher;
std_msgs__msg__String msg;

void appMain(void* arg) {
    rcl_allocator_t allocator = rcl_get_default_allocator();
    rclc_support_t support;

    // Inicializa micro-ROS
    rclc_support_init(&support, 0, NULL, &allocator);

    rcl_node_t node;
    rclc_node_init_default(&node, "micro_ros_node", "", &support);

    // Inicializa publisher
    rclc_publisher_init_default(
        &publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
        "micro_ros_chatter"
    );

    // Loop principal
    while (true) {
        msg.data.data = (char*)"Hello from ESP32!";
        msg.data.size = strlen(msg.data.data);
        msg.data.capacity = msg.data.size + 1;

        rcl_publish(&publisher, &msg, NULL);
        printf("Mensagem publicada!\n");

        sleep(1);
    }

    // Cleanup
    rcl_publisher_fini(&publisher, &node);
    rcl_node_fini(&node);
}
