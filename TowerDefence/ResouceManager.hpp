#pragma once

#include "Manager.h"

class ResouceManager : public Manager<ResouceManager>
{
	friend class Manager<ResouceManager>;

public:
	ResouceManager();
	~ResouceManager();

private:

};

