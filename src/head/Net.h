#pragma once
#include <iostream>
#include "cpr/cpr.h"

using namespace std;


struct NetContext {
	string url;
	string method;
	cpr::Header headers;
	std::function<void(const cpr::Response r)> callback;
};


class Net
{
public:
	explicit Net(const NetContext& context);
};