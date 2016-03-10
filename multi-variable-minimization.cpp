// �i2�ϐ��j�j���[�g���@�E���z�~���@�ɂ��֐��̍ŏ��l�����߂�
// 
// ���ΏۂƂ���֐����ʂ���2�K�����\�ł���K�v������

#include <cfloat>
#include <cmath>
#include <utility>
#include <iostream>
#include <Eigen>

// ��
// f(x, y) = x^2 + 2xy + 3y^2 + 4x + 5y + 6
// 
// d_val�ł́A�ȉ��̂��̂��܂Ƃ߂ăx�N�g���ɂ������̂�Ԃ��i���z�j
// ��f/��x = 2x + 2y + 4
// ��f/��y = 2x + 6y + 5
// 
// dd_val�ł́A�ȉ��̂��̂��܂Ƃ߂čs��ɂ������̂�Ԃ��i�w�V�A���j
// ��f^2/��x^2 = 2
// ��f^2/��y^2 = 6
// ��f^2/��x��y = 2

class SampleFunction{
public:
	static double val(const Eigen::Vector2d & input){
		return(
			input(0)*input(0) + 2*input(0)*input(1) + 3*input(1)*input(1)
			+ 4*input(0) + 5*input(1) + 6
		);
	}
	
	static Eigen::VectorXd d_val(const Eigen::Vector2d & input){
		Eigen::Vector2d result;
		result(0) = 2*input(0) + 2*input(1) + 4;
		result(1) = 2*input(0) + 6*input(1) + 5;
		return result;
	}
};

template <class Funcs>
std::pair<Eigen::Vector2d, double> newton_2d_minimize(double precision){
	Eigen::Vector2d x;
	Eigen::Matrix2d pseudo_inv_hessian = Eigen::Matrix2d::Identity();
	double current_value = Funcs::val(x);
	
	for(;;){
		// ��������
		Eigen::Vector2d shift = pseudo_inv_hessian * Funcs::d_val(x);
		double shift_norm = shift.norm();
		double shift_ratio = 1.0;
		
		// �X�V�r����x�̒l�Ɗ֐��̒l
		Eigen::Vector2d x_tmp;
		double value_tmp;
		
		// �J��Ԃ��l���X�V����
		for(;;){
			x_tmp = x - shift * shift_ratio;
			
			value_tmp = Funcs::val(x_tmp);
			if(value_tmp < current_value) break;
			// ����x�𓮂����Ċ֐��̒l���������Ă��܂����ꍇ�́A
			// x�𓮂������𔼕��ɂ��čĎ��s����B
			// �Ȃ��Ax�𓮂�����������̒l����������ꍇ�͂����őł��؂�
			shift_ratio *= 0.5;
			if(shift_ratio * shift_norm < precision) break;
		}
		if(std::fabs(current_value - value_tmp) < precision) break;
		current_value = value_tmp;
		
		// �w�V�A���̍X�V
		Eigen::Vector2d y = Funcs::d_val(x_tmp) - Funcs::d_val(x);
		Eigen::Vector2d dx = -shift_ratio * shift;
		Eigen::Matrix2d t = Eigen::Matrix2d::Identity() - y * dx.transpose() / y.dot(dx);
		pseudo_inv_hessian = t.transpose() * pseudo_inv_hessian * t + dx * dx.transpose() / y.dot(dx);
		x = x_tmp;
	}
	return std::make_pair(x, current_value);
}

template <class Funcs>
inline std::pair<Eigen::Vector2d, double> newton_2d_minimize(){
	return newton_2d_minimize<Funcs>(std::sqrt(DBL_MIN));
}

/*
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
*/

int main(void){
	std::pair<Eigen::Vector2d, double> result = newton_2d_minimize<SampleFunction>();
	//std::pair<double, double> result = gradient_minimize<SampleFunction>();
	std::cout << "x = " << result.first << std::endl;
	std::cout << "f(x) = " << result.second << std::endl;
}


