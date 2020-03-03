#include <ros/ros.h>
#include <actionlib/server/simple_action_server.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <geometry_msgs/Twist.h>


class DummyActionCpp
{

protected:

  ros::NodeHandle nh_;
  ros::Publisher cmd_pub_; // publisher for commands
  actionlib::SimpleActionServer<move_base_msgs::MoveBaseAction> as_; // action server object
  std::string action_name_;

public:

  DummyActionCpp(std::string name) :
    as_(nh_, name, boost::bind(&DummyActionCpp::executeCB, this, _1), false),
    action_name_(name)
  {
    cmd_pub_ = nh_.advertise<geometry_msgs::Twist>("nav_vel", 1);
    ROS_INFO("Starting action server..");
    as_.start();
    ROS_INFO("Waiting for requests..");
  }


  // this is the method that is called when a client request the action
  void executeCB(const move_base_msgs::MoveBaseGoalConstPtr &goal)
  {
    ROS_INFO("Request received!");
    ros::Rate rate(10);
    bool success = true;

    // we will send a commands at 10 Hz for 4 seconds (40 * 1/(10 Hz) = 4 s)
    // if the action is preempted, we stop executing the action
    for (int i=0; i<40; i++)
    {
      if (as_.isPreemptRequested() || !ros::ok())
      {
        ROS_INFO("%s: Preempted", action_name_.c_str());
	as_.setPreempted();
	success = false;
	break;
      }

      geometry_msgs::Twist cmd_msg;
      cmd_msg.linear.x = 0.3;
      cmd_msg.linear.y = 0.2;
      cmd_msg.angular.z = 0.1;
      cmd_pub_.publish(cmd_msg);
      rate.sleep();

    }
    
    // send a zero command to stop the robot
    geometry_msgs::Twist zero_cmd_msg;
    cmd_pub_.publish(zero_cmd_msg);

    move_base_msgs::MoveBaseResult result;
    as_.setSucceeded(result);
    ROS_INFO("Action completed!");

  }

};



int main(int argc, char** argv)
{

  ros::init(argc, argv, "dummy_action_cpp");
  DummyActionCpp dummy_action_cpp("dummy_action_cpp");
  ros::spin();

  return 0;

}
