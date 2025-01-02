#pragma once

class Uploader
{
public:

	Uploader();

	void Send();
	void SendModules();
	void SendProcessos();
};

extern Uploader gUP;
