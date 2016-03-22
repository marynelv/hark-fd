/*
 * CsvIO.hpp
 *
 *  Created on: 2013/05/25
 *      Author: ohtsuka
 */

#ifndef CsvIO_HPP_
#define CsvIO_HPP_

#include <fstream>
#include <string>
#include <vector>

#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

//#include <Eigen/Dense>
//#include <Eigen/Core>
#include "Array2D.hpp"

template <typename DataType> class CsvIO {
public:
	CsvIO() {}
	virtual ~CsvIO() {}

	CsvIO(const std::string& file_name){
		read_file(file_name);
	}
	
	template <typename T> CsvIO(const std::string& file_name, const Array2D<T>& data){
		write_file(file_name, data);
	}

	const Array2D<DataType>& get_data() const {
		return data_;
	}

	int read_file(const std::string& file_name){
	 	std::ifstream ifs(file_name.c_str(), std::ios::in);

	 	if(!ifs){
	 		// could not read the file
			std::cerr << "csvfile not found! " << file_name << std::endl;
			return 1;
	 	}

	 	std::string line;
	 	size_t rows = 0, cols = 0;
	 	data_.resize(0,0); // clear the contents

	 	while(std::getline(ifs, line)){
	 		std::vector<std::string> split_str;
	 		boost::algorithm::split(split_str, line, boost::algorithm::is_any_of(" ,\t"), boost::algorithm::token_compress_on);

	 		if(split_str.empty()){
	 			continue;
	 		}
	 		++rows;
	 		if(cols < split_str.size()){
	 			cols = split_str.size();
	 		}

	 		data_.resize(rows,cols);
	 		for(int n = 0, N = split_str.size(); n < N; ++n){
	 			data_(rows-1, n) = boost::lexical_cast<DataType>(split_str[n]);
	 		}
	 	}

	 	return 0;
	}
	
	template <typename T> int write_file(const std::string& file_name, const Array2D<T>& data, const std::string& sep=std::string(",")) const {
		std::ofstream ofs(file_name.c_str(), std::ios::trunc);
		
		if(!ofs){
			// could not open the file
			std::cerr << "CsvIO::write_file file open error! " << file_name << std::endl;
			return 1;
		}
		
		const int rows = data.rows();
		const int cols = data.cols();
		for(int n = 0; n < rows; ++n){
			std::vector<std::string> row_values;
			
			for(int l = 0; l < cols; ++l){
				boost::format val("%1.7f");
				val%data(n,l);
				row_values.push_back(val.str());
				//row_values.push_back(boost::lexical_cast<std::string>(data.derived()(n,l)));
			}
			
			// join with separator(, "," by default)
			ofs << boost::algorithm::join(row_values, sep) << std::endl;
		}
		
		return 0;
	}
	
	int write_file(const std::string& file_name, const std::string& sep=std::string(",")) const {
		return write_file(file_name, data_, sep);
	}
	

private:
	Array2D<DataType> data_;
};
#endif /* CsvIO_HPP_ */
