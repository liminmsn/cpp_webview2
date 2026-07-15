#include "head/Net.h"

Net::Net(const NetContext &context)
{
	cpr::Response r;
	if (context.method == "GET")
	{
		r = cpr::Get(cpr::Url{ context.url }, context.headers);
	}
	else if (context.method == "POST")
	{
		r = cpr::Post(cpr::Url{ context.url }, context.headers);
	}
	else if (context.method == "PUT")
	{
		r = cpr::Put(cpr::Url{ context.url }, context.headers);
	}
	else if (context.method == "DELETE")
	{
		r = cpr::Delete(cpr::Url{ context.url }, context.headers);
	}
	else
	{
		throw std::invalid_argument("Unsupported HTTP method: " + context.method);
	}
	if (r.error)
	{
		throw std::runtime_error("HTTP request failed: " + r.error.message);
	}
	//cout << "Response code: " << r.status_code << endl;
	//cout << "Response body: " << r.text << endl;
	context.callback(r);
}