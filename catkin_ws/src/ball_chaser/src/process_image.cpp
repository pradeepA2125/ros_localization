#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
  
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    // Call the command_robot service and pass the requested motor commands
    if (!client.call(srv))
        ROS_ERROR("Failed to call service command_robot");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    int left = img.step / 3;
    int right = left * 2;
    bool ball_found = false;
    bool first_iteration = false;
        //ROS_FATAL("i'm here before entering.........................");
    for ( int i = 0; i < ((img.height)*(img.step)) ; i=i + img.step)
    { //ROS_FATAL("i'm here.........................");
       for(int j=0; j <  img.step; j=j+3)
       {
       //image data is in the format unit_8[] data, three consecutive bytes refer to one pixel.  # actual matrix data, size is (step * rows)    
         if(i == 0) {first_iteration = true;i=1;} //to avoid repeated search on same block of memory on first iteration
          //ROS_FATAL("i'm here, trying to access the image data.........................%u,%u,%u,%u",i,j,i+j,img.step*img.height);
         if ((img.data[i+j] == white_pixel) && (img.data[i+j+1] == white_pixel) && (img.data[i+j+2] == white_pixel)) 
         {
              //ROS_INFO("Found ball row: %d, column: %d", row/img.width, i%img.step);
             ball_found = true;

              if ( j < left )
              {
                  drive_robot(0.3 , 0.3);
              }
              else if ( j > right)
              {

                  drive_robot(0.3, -0.3);
              }
              else
              {
                  drive_robot( 1 , 0);
              }
              break;
         }
         //ROS_FATAL("i'm here, after accessing the image data.........................");
    
       }
       if(first_iteration == true){first_iteration = false;i=0;}
    }
      

    if ( ball_found == false )
    {
        drive_robot(0, 0);
    }
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}