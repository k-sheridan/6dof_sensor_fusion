/*
 * EKF.h
 *
 *  Created on: Jun 19, 2017
 *      Author: pauvsi
 */

#ifndef M7_SENSOR_FUSION_INCLUDE_M7_SENSOR_FUSION_EKF_H_
#define M7_SENSOR_FUSION_INCLUDE_M7_SENSOR_FUSION_EKF_H_

#include <ros/ros.h>
#include <eigen3/Eigen/Geometry>

#include <sensor_msgs/Imu.h>
#include <geometry_msgs/TwistWithCovarianceStamped.h>
#include <geometry_msgs/AccelWithCovarianceStamped.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <sensor_msgs/Range.h>

#include <nav_msgs/Odometry.h>

#include <tf2_ros/transform_broadcaster.h>
#include <tf2_ros/message_filter.h>
#include <tf2_ros/transform_listener.h>
#include <tf/tf.h>
#include <tf/transform_listener.h>
#include <tf/tf.h>
#include <tf/tfMessage.h>

#include <float.h>

#include "Parameters.h"
#include "Types.h"

class EKF {
public:

	std::vector<State> old_states; // used for interpolating old measurements forward to be used

	std::deque<Measurement> measurements;

	State state;

	tf::Transform base2imu; // can transform imu measurements into the base coordinate frame
	tf::Transform base2dipaCam;
	tf::Transform base2sonar;

	tf::TransformListener tf_listener; // listens for transforms

	EKF(bool test = false);
	virtual ~EKF();

	void imu_callback(const sensor_msgs::ImuConstPtr& msg);
	void mantis_callback(const geometry_msgs::PoseWithCovarianceStampedConstPtr& msg);
	void dipa_callback(const nav_msgs::OdometryConstPtr& msg);
	void sonar_callback(const sensor_msgs::RangeConstPtr& msg);

	State process(State prior, ros::Time t);

	State update(State prior, MeasurementCombination mats);
	State update(State prior, Measurement meas);

	Eigen::Matrix<double, STATE_VECTOR_SIZE, STATE_VECTOR_SIZE> computeStateTransitionF(double dt);

	//Eigen::Matrix<double, 6, STATE_VECTOR_SIZE> computeIMUMeasurementJacobian(State est);
	Eigen::Matrix<double, 5, STATE_VECTOR_SIZE> computeIMUMeasurementH();

	//Eigen::Matrix<double, 7, STATE_VECTOR_SIZE> computePOSEMeasurementJacobian(State est);
	Eigen::Matrix<double, 6, STATE_VECTOR_SIZE> computePoseMeasurementH();

	Eigen::Matrix<double, 6, STATE_VECTOR_SIZE> computeTwistMeasurementH();

	Eigen::Matrix<double, STATE_VECTOR_SIZE, STATE_VECTOR_SIZE> computeStateProcessError(double dt);

	State findClosestState(ros::Time t);

	void predictAllMeasurementsForward(ros::Time new_t);

	Measurement predictMeasurementForward(Measurement z, ros::Time new_t);

	void addMeasurement(Measurement z);
};

#endif /* M7_SENSOR_FUSION_INCLUDE_M7_SENSOR_FUSION_EKF_H_ */
