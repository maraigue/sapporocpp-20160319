#include "numeric-csv-reader.hpp"

namespace numeric_csv_reader{
	// ファイル全体を読み込む
	bool file_get_contents(const char * filename, std::string & result){
		size_t filesize;
		
		std::ifstream ifs(filename, std::ios::in | std::ios::binary);
		if(!ifs) return false;
		
		ifs.seekg(0, std::ios_base::end);
		if(ifs.fail()) return false;
		
		filesize = ifs.tellg() - std::ifstream::pos_type(0);
		if(ifs.fail()) return false;
		
		ifs.seekg(0, std::ios_base::beg);
		if(ifs.fail()) return false;
		
		result.resize(filesize);
		ifs.read(const_cast<char *>(result.c_str()), filesize);
		if(ifs.fail()) return false;
		
		ifs.close();
		return true;
	}
	
	// 文字列をdouble型に変換する
	// 文字列全体を変換に使えた場合はtrue、そうでなかった場合はfalse
	bool str2double(const char * buf, double & val){
		char * parse_pos;
		val = std::strtod(buf, &parse_pos);
		return(*parse_pos == '\0');
	}

	// 文字列を読み込んで行列にする
	bool parse(Eigen::MatrixXd & result, const std::string & buf){
		std::size_t len = buf.length();
		if(buf[len - 1] == '\n') --len;
		
		// 行数、列数を調べる
		std::size_t num_rows = 1;
		std::size_t num_cols = 1;
		for(std::size_t n = 0; n < len; ++n){
			if(num_rows == 1 && buf[n] == ',') ++num_cols;
			if(buf[n] == '\n') ++num_rows;
		}
		result.resize(num_rows, num_cols);
		
		// 解析する
		std::size_t i = 0, j = 0;
		const char * current_ptr = buf.data();
		char * parse_ptr;
		double value;
		for(;;){
			value = std::strtod(current_ptr, &parse_ptr);
			if(parse_ptr == current_ptr){
				// 正常な数値でない
				std::cerr << "CSV parsing error: Invalid string as number: \"" << *parse_ptr << "\" (Position: " << (parse_ptr - buf.data()) << ")" << std::endl;
				return false;
			}
			
			result(i, j) = value;
			++j;
			
			if(*parse_ptr == ','){
				current_ptr = parse_ptr;
				++current_ptr;
				if(j >= num_cols){
					std::cerr << "CSV parsing error: Too many columns" << std::endl;
					return false;
				}
			}else if(*parse_ptr == '\r' || *parse_ptr == '\n' || *parse_ptr == '\0'){
				current_ptr = parse_ptr;
				if(*parse_ptr != '\0'){
					++current_ptr;
					if(parse_ptr[0] == '\r' && parse_ptr[1] == '\n') ++current_ptr;
				}
				
				if(j != num_cols){
					std::cerr << "CSV parsing error: Too few columns" << std::endl;
					return false;
				}
				
				j = 0;
				++i;
				if(*current_ptr == '\0'){
					if(i != num_rows){
						std::cerr << "CSV parsing error: Too few rows" << std::endl;
						return false;
					}
					break;
				}else{
					if(i >= num_rows){
						std::cerr << "CSV parsing error: Too many rows" << std::endl;
						return false;
					}
				}
			}else{
				std::cerr << "CSV parsing error: Invalid delimiter: '" << *parse_ptr << "' at position " << (parse_ptr - buf.data()) << std::endl;
				return false;
			}
		}
		
		return true;
	}
	
	// ファイルから読み込む
	bool read(Eigen::MatrixXd & result, const char * file){
		std::cerr << "Parsing CSV in the file \"" << file << "\"" << std::endl;
		
		std::string buf;
		if(!file_get_contents(file, buf)){
			std::cerr << "File reading error: Cannot read \"" << file << "\"" << std::endl;
			return false;
		}
		if(buf.empty()){
			std::cerr << "File reading error: File \"" << file << "\" is empty" << std::endl;
			return false;
		}
		
		return parse(result, buf.data());
	}
}
