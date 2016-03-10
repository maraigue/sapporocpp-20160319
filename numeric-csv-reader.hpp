#pragma once

#include <Eigen>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

namespace numeric_csv_reader{
	bool file_get_contents(const char * filename, std::string & result);
	bool str2double(const char * buf, double & val);
	bool parse(Eigen::MatrixXd & result, const std::string & buf);
	bool read(Eigen::MatrixXd & result, const char * file);
}
