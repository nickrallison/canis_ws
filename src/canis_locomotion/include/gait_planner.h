#include <iostream>
#include <vector>
#include <queue>

#include <ros/ros.h>
#include <std_msgs/Float64.h>
#include <std_msgs/String.h>
#include <std_msgs/Bool.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/TwistStamped.h>
#include <geometry_msgs/PointStamped.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Pose.h>

#include <robot_core/Gait.h>
#include <robot_core/GaitVec.h>
#include <robot_core/PathQuat.h>


using namespace robot_core;
using namespace geometry_msgs;

enum Mode{
    still,
    walking,
    crouching,
    sitting,
    laying_down,
    recovering,
    manual
};

enum WalkMode {Halt, SR, SL, IR, IL};


class GaitPlanner {
    public:

        GaitPlanner(const ros::NodeHandle &nh_private_);

        ~GaitPlanner() = default;

        // Callback methods

        void Vel_CB(const geometry_msgs::TwistStamped::ConstPtr& twist);
        void Reset_CB(const std_msgs::Bool::ConstPtr& reset);
        void crouch_CB(const std_msgs::Bool::ConstPtr &crouch);
        void sit_CB(const std_msgs::Bool::ConstPtr &sit);
        void lay_down_CB(const std_msgs::Bool::ConstPtr &lay_down);

        /**
         * Frame_CB (Frame Callback)
         * 
         * Calculates and publishes the desired gait for the next frame
         * 
         * @param event It's just a timer.
        */
        void Frame_CB(const ros::TimerEvent& event);

        // Operation Methods

        void Init();
        std::vector<Gait> turn(double turn_rad);
        std::vector<Gait> walk(double dist);
        geometry_msgs::Pose zeroPose();
        Gait zeroGait();
        void InitializeGaits(); 

        /**
         * Computes the desired elevation of the lifted foot
         */
        Gait gait_raise_foot(Gait gait);

        // Public Variables

        double operating_freq;

        // Debugging

        void debug(std::vector<double> values, std::string message);
        void debug(std::string message);
        void print_gait(Gait gait);

    private:
        // Node handlers

        ros::NodeHandle nh_;
        ros::NodeHandle nh_private_;

        // Robot Params (Can be passed as params)

        double shoulder_length;
        double arm_length;
        double forearm_length;
        double body_width;
        double center_to_front;
        double center_to_back;
        double operating_freq; // TBD, more testing
        double walking_z;
        double step_height;
        double leg_x_offset;
        double leg_x_separation;

        // Publishers, subscribers & messages

        ros::Publisher gait_pub;
        ros::Publisher debug_pub;
        ros::Publisher test_leg_position_pub;

        ros::Subscriber vel_sub;
        ros::Subscriber reset_sub;
        ros::Subscriber crouch_sub;
        ros::Subscriber sit_sub;
        ros::Subscriber lay_down_sub;
    
        std_msgs::String debug_msg;
        std_msgs::String test_leg_position_msg;
        geometry_msgs::Pose pose_command;
        geometry_msgs::Pose pose_current;

        // #### Gait Variables ####

        WalkMode step = Halt;
        Mode mode = still;

        Gait previous_gait;
        Gait current_gait;
        Gait next_gait;
        std::queue<Gait> gaits;

        double percent_step;
        double delta_percent;
        double margin = 1.0 - 0.0005;

        double x_vel = 0;
        double theta_vel = 0;
        double delta_dist = 0;
        double delta_theta = 0;
        double step_turn_rad = 2 * M_PI / 48;

        // #### Standard Gaits ####
        
        Gait default_gait;
        Gait sr_fwd;
        Gait sl_fwd;
        Gait ir_fwd;
        Gait il_fwd;
        Gait sr_turn;
        Gait sl_turn;
        Gait ir_turn;
        Gait il_turn;
        Gait halt;

        // #### Testing ####

        double delta_angle = 0.05;
        double angle = 0.0;
        double radius = 0.05;
        double period = 2 * M_PI * radius / x_vel;
        double rot_freq = 1 / period;
        bool testing_leg_position = false;
        double percent_dist;
        double percent_theta;

};

double double_lerp(double x1, double x2, double percent);
Point point_lerp(Point p1, Point p2, double percent);
geometry_msgs::Point rotate2D(double rad, geometry_msgs::Point point);
