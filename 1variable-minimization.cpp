// �i1�ϐ��j�j���[�g���@�E���z�~���@�ɂ��֐��̍ŏ��l�����߂�
// 
// ���ΏۂƂ���֐����ʂ���2�K�����\�ł���K�v������

#include <cfloat>
#include <cmath>
#include <utility>
#include <iostream>

// ��F
// f(x) = (x - 1)^4 + (x - 2)^2 - 4��(x + 3)
// f'(x) = 4(x - 1)^3 + 2(x - 2) - 2/[��(x + 3)]
// f''(x) = 12(x - 1)^2 + 2 + 1/[��(x + 3)^3]
class SampleFunction{
public:
	static double val(double x){
		return std::pow(x - 1, 4) + std::pow(x - 2, 2) - 4 * std::sqrt(x + 3);
	}
	
	static double d_val(double x){
		return 4 * std::pow(x - 1, 3) + 2 * (x - 2) - 2 / std::sqrt(x + 3);
	}
	
	static double dd_val(double x){
		return 12 * std::pow(x - 1, 2) + 2 + std::pow(x + 3, -1.5);
	}
};

// �ŏ������s���ۂ̏����l������
inline double minimization_init(double min, double max){
	if(min <= -DBL_MAX/2){
		return(max >= DBL_MAX/2 ? 0 : max);
	}else{
		return(max >= DBL_MAX/2 ? min : (max+min)/2);
	}
}

template <class Funcs>
std::pair<double, double> newton_minimize(double x_min, double x_max, double precision){
	double x = minimization_init(x_min, x_max);
	double current_value = Funcs::val(x);
	
	for(;;){
		// ��������
		double shift = Funcs::d_val(x) / Funcs::dd_val(x);
		double shift_ratio = 1.0;
		
		// �X�V�r����x�̒l�Ɗ֐��̒l
		double x_tmp;
		double value_tmp;
		
		// �J��Ԃ��l���X�V����
		for(;;){
			x_tmp = x - shift * shift_ratio;
			if(x_tmp < x_min) x_tmp = x_min;
			if(x_tmp > x_max) x_tmp = x_max;
			
			value_tmp = Funcs::val(x_tmp);
			if(value_tmp < current_value) break;
			// ����x�𓮂����Ċ֐��̒l���������Ă��܂����ꍇ�́A
			// x�𓮂������𔼕��ɂ��čĎ��s����B
			// �Ȃ��Ax�𓮂�����������̒l����������ꍇ�͂����őł��؂�
			shift_ratio *= 0.5;
			if(std::fabs(shift * shift_ratio) < precision) break;
		}
		x = x_tmp;
		if(std::fabs(current_value - value_tmp) < precision) break;
		current_value = value_tmp;
	}
	return std::make_pair(x, current_value);
}

template <class Funcs>
inline std::pair<double, double> newton_minimize(double x_min, double x_max){
	newton_minimize<Funcs>(x_min, x_max, std::sqrt(DBL_MIN));
}

template <class Funcs>
std::pair<double, double> gradient_minimize(double x_min, double x_max, double precision){
	double x = minimization_init(x_min, x_max);
	double current_value = Funcs::val(x);
	
	for(;;){
		// ��������
		double shift = Funcs::d_val(x);
		double shift_ratio = 1.0;
		
		// �X�V�r����x�̒l�Ɗ֐��̒l
		double x_tmp;
		double value_tmp;
		
		// �J��Ԃ��l���X�V����
		for(;;){
			x_tmp = x - shift * shift_ratio;
			if(x_tmp < x_min) x_tmp = x_min;
			if(x_tmp > x_max) x_tmp = x_max;
			
			value_tmp = Funcs::val(x_tmp);
			if(value_tmp < current_value) break;
			// ����x�𓮂����Ċ֐��̒l���������Ă��܂����ꍇ�́A
			// x�𓮂������𔼕��ɂ��čĎ��s����B
			// �Ȃ��Ax�𓮂�����������̒l����������ꍇ�͂����őł��؂�
			shift_ratio *= 0.5;
			if(std::fabs(shift * shift_ratio) < precision) break;
		}
		x = x_tmp;
		if(std::fabs(current_value - value_tmp) < precision) break;
		current_value = value_tmp;
	}
	return std::make_pair(x, current_value);
}

template <class Funcs>
inline std::pair<double, double> gradient_minimize(double x_min, double x_max){
	return gradient_minimize<Funcs>(x_min, x_max, std::sqrt(DBL_MIN));
}

int main(void){
	std::pair<double, double> result = newton_minimize<SampleFunction>(0.0, DBL_MAX);
	//std::pair<double, double> result = gradient_minimize<SampleFunction>(0.0, DBL_MAX);
	std::cout << "x = " << result.first << std::endl;
	std::cout << "f(x) = " << result.second << std::endl;
}


