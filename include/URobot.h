#ifndef UROBOT_H
#define UROBOT_H
#define _USE_MATH_DEFINES
#include <math.h>

#include <ur_rtde/rtde_control_interface.h>
#include <ur_rtde/rtde_receive_interface.h>

#include <thread>
#include <chrono>

#include <QDebug>

using namespace ur_rtde;
using namespace std::chrono;

class URobot
{
public:
	URobot(std::string ip_adress);

	~URobot();

	std::vector<double> getActualQ();

	void rotateAlongZ(float angle);
    void rotate2Zero();

	bool isConnected();

private:
	RTDEControlInterface rtdeControl;
	RTDEReceiveInterface rtdeReceive;

	double ToRadians(double deg);

	double ToDegree(double radian);

	double checkRange(double rad);

	void checkRangeVec(std::vector<double> & qList, std::vector<double> & new_qList);

	void showQ(const std::string preText, const std::vector<double>qList);
};
#endif
