#include "numeric-csv-reader.hpp"
#include <Eigen>
#include <algorithm>
#include <iostream>

constexpr const double EPSILON = 0.000000000001l;

// ���ފ֐���\��
void display_classifier(const Eigen::VectorXd & coefs, int dim, std::ostream & out){
	out << "g(x_1, ..., x_" << dim << ") =";
	for(int d = 0; d < dim; ++d){
		if(coefs(d) > 0){
			out << " + " << coefs(d) << "*x_" << (d+1);
		}else if(coefs(d) < 0){
			out << " - " << -coefs(d) << "*x_" << (d+1);
		} // else: �����\�����Ȃ�
	}
	out << " - 1" << std::endl;
}

// ���C��
int main(int argc, char ** argv){
	if(argc != 2){
		std::cerr << "Usage: compute-border [CSVfile]" << std::endl;
		return -1;
	}
	
	// �t�@�C����ǂݍ��ށB
	// �t�@�C���́A�ȉ��̍\�������Ă���CSV�ł���B
	// 
	// 1�Ԗڂ̃f�[�^�_�̃��x��,1�Ԗڂ̃f�[�^�_��x�̒l,1�Ԗڂ̃f�[�^�_��y�̒l,...
	// 2�Ԗڂ̃f�[�^�_�̃��x��,2�Ԗڂ̃f�[�^�_��x�̒l,2�Ԗڂ̃f�[�^�_��y�̒l,...
	// :
	// 
	// �u���x���v�Ƃ́A�����ł͕��ދ敪�̈Ӗ��ł���B
	// ���E���𐶐��������ʁA�Ȃ�ׂ��e�̈�ɂ����ē������x���̃f�[�^�_������
	// �W�܂�悤�ɂ������B
	Eigen::MatrixXd data;
	if(!( numeric_csv_reader::read(data, argv[1]) )){
		std::cerr << "Error found in the specified CSV file \"" << argv[1] << "\"." << std::endl;
		return -1;
	}
	
	// �ǂݍ��񂾓��e���A���x���ƍ��W�ɕ�����
	Eigen::VectorXd labels = data.block(0, 0, data.rows(), 1);
	Eigen::MatrixXd points = data.block(0, 1, data.rows(), data.cols() - 1);
	
	// �f�[�^����������(dim)�ƁA�f�[�^��(num)���m�F
	int dim = points.cols();
	int num = points.rows();
	
	// ���x����-1��1�������肵�Ă��Ȃ��̂ŁA������m�F
	for(int i = 0; i < num; ++i){
		if(labels(i) != 1.0 && labels(i) != -1.0){
			std::cerr << "Label (first column) must be either 1 or -1." << std::endl;
			return -1;
		}
	}
	
	// 2�����̏ꍇ�A
	// �E�_(p, q)�̃��x���� -1 (��) �ƕt���Ă���ꍇ�A
	//   ���ސ����ƂȂ������ ap + bq - 1 > 0 �ł���A
	//   �ŏ����������֐��� (-(ap + bq - 1))+ �ƒ�߂�B
	// �E�_(p, q)�̃��x���� +1 (��) �ƕt���Ă���ꍇ�A
	//   ���ސ����ƂȂ������ ap + bq - 1 < 0 �ł���A
	//   �ŏ����������֐��� (ap + bq - 1)+ �ƒ�߂�B
	// �������A(z)+ = max{0, z} �ł���i0�������ꍇ��0�ɂ���j�B
	// 
	// �������܂Ƃ߂āA�ŏ����������֐��� c(ap + bq - 1)+ �ƒ�߂�B
	// �������Ac�̓��x���ł���B
	// 
	// ���̏����̂��ƂŁA�֐��l���ŏ�������悤��(a, b)�����߂�B
	// 3�����ȏ�̏ꍇ�A�ϐ��̐��������邾���œ��l�ł���B
	// 
	// ���āA���̊֐���a�ɂ��čŏ������邱�Ƃ��l����ib�ɂ��Ă����l�j�B
	// (z)+ �́A��_�����Ɍq����������2�{����Ȃ�B
	// 
	// �_
	//   �_
	//     �_�Q�Q�Q�Q  c(ap + bq - 1) (cp < 0)
	//            ��a
	// 
	//             �^
	//           �^
	// �Q�Q�Q�Q�^      c(ap + bq - 1) (cp > 0)
	//            ��a
	// 
	// ���̂��߁A�P�� c(ap + bq - 1) = 0�A���Ȃ킿 a = (1 - bq)/p �ƂȂ�
	// �ꏊ��I�ׂ΂悢�B
	// �Ȃ����ۂɂ́A�u�f�[�^�_���ׂĂɂ��Ă��̊֐��𑫂����킹�����́v��
	// �ŏ������Ȃ��ƂȂ�Ȃ��̂ŁA�e�f�[�^�_(p, q)�ɂ��� (1 - bq)/p ��
	// ���߂Ă������Ƃ͂������A���� (1 - bq)/p �̂����ǂ����ŏ��l�����̂���
	// �v�Z�ł��Ȃ��ƂȂ�Ȃ��B
	// �ȉ��ł́A���� (1 - bq)/p ���u�u���[�N�|�C���g�v�ƌĂсA�S�f�[�^�_��
	// �΂���u���[�N�|�C���g���\�[�g���ĕێ����Ă������̂Ƃ���B
	// 
	// �ŏ��l�����߂�ɂ́A���֐���p����΂悢�Bg(a) = c(ap + bq - 1) ����������
	// g'(a) = cp �ɂȂ�̂ŁAf(a) = (c(ap + bq - 1))+ �̓��֐���
	// �E���� (c(ap + bq - 1))+ = 0 �Ȃ�΁Af'(a) = 0
	// �E���� (c(ap + bq - 1))+ > 0 �Ȃ�΁Af'(a) = cp
	// �ƂȂ�B
	// ���Ȃ݂ɁA���֐��͈ȉ��̂悤�ɂȂ�B
	// 
	// [���̊֐�]
	// 
	// �_    cp < 0            cp > 0    �^
	//   �_                            �^
	//     �_�Q�Q�Q�Q        �Q�Q�Q�Q�^
	//            ��a                   ��a
	//       �����֐�              �����֐�
	// 
	//                               �P�P�P cp
	//       �������� 0      ��������       0
	// �Q�Q�Q         cp
	// 
	// �Ƃ���ŁA���ۂɋ��߂������֐��́A�����S�f�[�^�_�ɑ΂��铱�֐��ɂ���
	// ���Z�������̂ł���B�܂�A
	// �Ecp < 0 �ƂȂ�u���[�N�|�C���g����������A����ȉ���a�ɂ��āA
	//   ���֐��̒l��cp�������Z����
	// �Ecp > 0 �ƂȂ�u���[�N�|�C���g����������A����ȏ��a�ɂ��āA
	//   ���֐��̒l��cp�������Z����
	// �Ƃ���Γ��֐������߂���B
	// �����A����͕s�s�����傫���i�u���[�N�|�C���g�̐�ƌ�̗����ɍs��������
	// ���֐��̒l�����Z���Ȃ��ƂȂ�Ȃ��j�B�����ŁA�K���悾�������邱�Ƃ̂ł���
	// �ȉ��̕��@���̗p����B
	// �������A�u���[�N�|�C���gb����b+1�܂ł̊Ԃ̓��֐��� d[b] �Ƃ����B
	// �EM = 0
	// �E�u���[�N�|�C���g�̏��������̂��珇�Ɉȉ��̂��Ƃ��s���B
	//   �Ecp < 0 �ƂȂ�ꍇ�AM += cp, d[b] = d[b-1] + |cp| �i|�E|�͐�Βl�j
	//   �Ecp > 0 �ƂȂ�ꍇ�Ad[b] = d[b-1] + cp
	// �E���ׂĂ�b�ɂ��āAd[b] += M
	// 
	// ���̓��֐��̒l��0���܂������ꏊ�����o����΂悢�B
	
	// �œK���̂��߂ɓ����������ϐ��ia, b�ɑ����j
	Eigen::VectorXd coefs = Eigen::VectorXd::Zero(dim);
	
	// �u���[�N�|�C���g
	// std::pair �́u�f�[�^�_�̔ԍ��A�u���[�N�|�C���g�̒l�v�̏�
	std::vector< std::pair<int, double> > breakpoints(num);
	
	// ���֐�
	std::vector< double > derivative(num);
	double derivative_base;
	
	int steps = 0;
	for(;;){
		++steps;
		std::cerr << "Computing step " << steps << "; ";
		display_classifier(coefs, dim, std::cerr);
		
		// coefs���X�V�����ʂ̂����ő�̂���
		// ���ꂪ���̒l�����������A�v�Z��ł��؂�
		double max_update = 0.0;
		
		// �e�����ɂ��āA�ȉ��̂��Ƃ��s��
		for(int d = 0; d < dim; ++d){
			// �u���[�N�|�C���g���v�Z����B
			// c(ap + bq - 1) = 0 �i2�����j�̏ꍇ�� a = (1 - bq)/p �Ƃ���΂悢��
			// ��ʂ̎������̏ꍇ�́Ac(ap + bq + b'q' + b''q'' + ... - 1) = 0 �܂�
			// a = (1 - bq - b'q' - b''q'' - ...)/p �Ƃ����v�Z�ɂȂ�B
			for(int i = 0; i < num; ++i){
				breakpoints[i].first = i;
				breakpoints[i].second = 1;
				for(int j = 0; j < dim; ++j){
					if(j == d) continue;
					breakpoints[i].second -= coefs(j) * points(i, j);
				}
				breakpoints[i].second /= points(i, d);
			}
			
			// �u���[�N�|�C���g���\�[�g����
			std::sort(breakpoints.begin(), breakpoints.end(),
				[](const std::pair<int, double> & a, const std::pair<int, double> & b){
					return(a.second < b.second);
				});
			
			// �\�[�g�������ɁA�X���i���֐��j�����߂�
			derivative_base = 0.0;
			for(int i = 0; i < num; ++i){
				// ���ܒ��ڂ��Ă���f�[�^�_
				int id = breakpoints[i].first;
				
				// ��L cp �̒l�����߂�
				double deriv_single = labels(id) * points(id, d);
				
				// ���֐��̒l�����Z����
				if(i == 0){
					derivative[i] = std::fabs(deriv_single);
				}else{
					derivative[i] = derivative[i-1] + std::fabs(deriv_single);
				}
				if(deriv_single < 0) derivative_base += deriv_single;
			}
			
			// ���֐���0�𒴂����i���ŏ��l�����Ƃ��́j�u���[�N�|�C���g��T���B
			int min_breakpoint;
			for(min_breakpoint = 0; min_breakpoint < num; ++min_breakpoint){
				if(derivative[min_breakpoint] + derivative_base >= 0.0) break;
			}
			
			if(min_breakpoint == num){
				// ����͋N���Ȃ��͂��Ȃ̂����O�̂��߃`�F�b�N
				std::cerr << "Unexpected Error!" << std::endl;
				return -1;
			}
			
			// �W���i��L�����a��b�j���X�V�B���������u���[�N�|�C���g�̒l�ɂ���
			double update = std::fabs(breakpoints[min_breakpoint].second - coefs(d));
			coefs(d) = breakpoints[min_breakpoint].second;
			
			if(update > max_update) max_update = update;
		}
		
		if(max_update < EPSILON) break;
	}
	
	std::cout << "---------- Result of training ----------" << std::endl;
	std::cout << "Classify as class y = -1 if g(x_1, ..., x_" << dim << ") > 0," << std::endl;
	std::cout << "Classify as class y = +1 if g(x_1, ..., x_" << dim << ") < 0," << std::endl;
	std::cout << "(x_i: data point)" << std::endl;
	std::cout << "where" << std::endl;
	display_classifier(coefs, dim, std::cout);
	
	return 0;
}



