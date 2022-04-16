#include <ros/ros.h>

#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <command_to_mavros.h>
#include <px4_command/command.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/TwistStamped.h>
#include <ros/time.h>
#include <geometry_msgs/Twist.h>
#include <aruco_msgs/Detector_vel.h>
#include <Eigen/Eigen>
using namespace std;

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>全 局 变 量<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
enum Command
{
    Move_ENU,
    Move_Body,
    Hold,
    Takeoff,
    Land,
    Arm,
    Disarm,
    Failsafe_land,
    Idle
};

px4_command::command Command_now;
aruco_msgs::Detector_vel detect_vel;

float height_square =0.5;                //飞行高度
float sleep_time = 150;

void detector_vel_cb(const aruco_msgs::Detector_vel::ConstPtr& msg)
{
    detect_vel=*msg;

}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>主 函 数<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
int main(int argc, char **argv)
{
    ros::init(argc, argv, "auto_takeoff");
    ros::NodeHandle nh("~");
    ros::Rate rate(20.0);

    ros::Subscriber detector_vel_sub = nh.subscribe<aruco_msgs::Detector_vel>("/aruco_single/detector_vel", 100, detector_vel_cb);
/*    while(ros::ok()){
    ROS_INFO("flag=%d",detect_vel.flag);
    ROS_INFO("x=%f",detect_vel.vel.linear.x);
    ROS_INFO("y=%f",detect_vel.vel.linear.y);
    ROS_INFO("z=%f",detect_vel.vel.linear.z);
    ros::spinOnce();
    rate.sleep();
}*/
    // 【发布】发送给position_control.cpp的命令.
    ros::Publisher command_pub = nh.advertise<px4_command::command>("/px4/command", 10);
    geometry_msgs::TwistStamped msg_uav_vel_cmd;
    ros::Publisher local_vel_pub = nh.advertise<geometry_msgs::TwistStamped>("/mavros/setpoint_velocity/cmd_vel", 10);

    //check paramater
    int check_flag;
    //输入1,继续，其他，退出程序
    cout << "Please check the parameter and setting，1 for go on， else for quit: "<<endl;
    cin >> check_flag;
    if(check_flag != 1) return -1;

    //check arm
    int Arm_flag;
    cout<<"Whether choose to Arm? 1 for Arm, 0 for quit: "<<endl;
    cin >> Arm_flag;

    if(Arm_flag == 1)
    {
        Command_now.command = Arm;
        command_pub.publish(Command_now);
    }
    else return -1;

    //check takeoff
    int Take_off_flag;
    cout<<"Whether choose to Takeoff? 1 for Takeoff, 0 for quit"<<endl;
    cin >> Take_off_flag;
    if(Take_off_flag == 1)
    {
        Command_now.command = Takeoff;
        command_pub.publish(Command_now);
    }
    else return -1;

    //check land
    int Land_flag;
    cout<<"Whether choose to Land? 1 for Land, 0 for quit"<<endl;
    cin >> Land_flag;
    if(Land_flag == 1)
    {
        Command_now.command = Land;
        while (ros::ok())
        {
           command_pub.publish(Command_now);
           rate.sleep();
           cout << "Land"<<endl;
        }
    }
    else return -1;

    cout<<"Program finish, exiting.....";   
    return 0;  
}












