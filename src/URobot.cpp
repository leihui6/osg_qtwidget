#include "URobot.h"

URobot::URobot(std::string ip_adress)
	:rtdeControl(ip_adress), rtdeReceive(ip_adress)
{
	isConnected();
}
URobot::~URobot()
{
	rtdeControl.stopScript();
	rtdeControl.disconnect();

	//rtdeReceive.stopFileRecording();
	rtdeReceive.disconnect();
}

std::vector<double> URobot::getActualQ()
{
	if (isConnected()) {
		return rtdeReceive.getActualQ();
	}
	else {
		return std::vector<double>();
	}
}

void URobot::rotateAlongZ(float angle)
{
	if (!isConnected()) return;

	std::vector<double> init_q = getActualQ(), new_q;
	showQ("Current:", init_q);
	// Target to the final joint
	init_q[5] += ToRadians(angle);
	checkRangeVec(init_q, new_q);

	/**
	 * Move asynchronously in joint space to new_q, we specify asynchronous behavior by setting the async parameter to
	 * 'true'. Try to set the async parameter to 'false' to observe a default synchronous movement, which cannot be
	 * stopped by the stopJ function due to the blocking behaviour.
	 */
	showQ("Target:", new_q);
	rtdeControl.moveJ(new_q, 1.05 / 2, 1.4, false);
	std::cout << "Moving.." << std::endl;

	auto final_q = rtdeReceive.getActualQ();
	showQ("Current:", final_q);

	std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

bool URobot::isConnected()
{
    if (!rtdeControl.isConnected() || !rtdeReceive.isConnected())
	{
        qDebug() << "error: rtde connection failed";
		return false;
	}
    else {
		return true;
	}
}

double URobot::ToRadians(double deg) {
	return deg * M_PI / 180.0f;
}

double URobot::ToDegree(double radian) {
	return radian * 180 / M_PI;
}

double URobot::checkRange(double rad) {
	if (rad > 2 * M_PI) {
		return rad - 2 * M_PI;
	}
	else {
		return rad;
	}
}

void URobot::checkRangeVec(std::vector<double> & qList, std::vector<double> & new_qList) {
	new_qList.resize(qList.size(), 0.0);
	for (size_t i = 0; i < qList.size(); i++) {
		new_qList[i] = checkRange(qList[i]);
	}
}

void URobot::showQ(const std::string preText, std::vector<double>qList) {
	for (auto &q : qList)
	{
		std::cout << preText << q << " " << ToDegree(q) << " ";
	}
	std::cout << std::endl;
}

