#pragma once
class CHidRespInterface
{
public:
	virtual void OnSensorEvent(unsigned char *buf) = 0;
	virtual void OnCommandResp(unsigned char * buf) = 0;
};

