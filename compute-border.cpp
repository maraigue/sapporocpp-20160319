#include "numeric-csv-reader.hpp"
#include <Eigen>
#include <algorithm>
#include <iostream>

constexpr const double EPSILON = 0.000000000001l;

// 分類関数を表示
void display_classifier(const Eigen::VectorXd & coefs, int dim, std::ostream & out){
	out << "g(x_1, ..., x_" << dim << ") =";
	for(int d = 0; d < dim; ++d){
		if(coefs(d) > 0){
			out << " + " << coefs(d) << "*x_" << (d+1);
		}else if(coefs(d) < 0){
			out << " - " << -coefs(d) << "*x_" << (d+1);
		} // else: 何も表示しない
	}
	out << " - 1" << std::endl;
}

// メイン
int main(int argc, char ** argv){
	if(argc != 2){
		std::cerr << "Usage: compute-border [CSVfile]" << std::endl;
		return -1;
	}
	
	// ファイルを読み込む。
	// ファイルは、以下の構造をしているCSVである。
	// 
	// 1番目のデータ点のラベル,1番目のデータ点のxの値,1番目のデータ点のyの値,...
	// 2番目のデータ点のラベル,2番目のデータ点のxの値,2番目のデータ点のyの値,...
	// :
	// 
	// 「ラベル」とは、ここでは分類区分の意味である。
	// 境界線を生成した結果、なるべく各領域において同じラベルのデータ点だけが
	// 集まるようにしたい。
	Eigen::MatrixXd data;
	if(!( numeric_csv_reader::read(data, argv[1]) )){
		std::cerr << "Error found in the specified CSV file \"" << argv[1] << "\"." << std::endl;
		return -1;
	}
	
	// 読み込んだ内容を、ラベルと座標に分ける
	Eigen::VectorXd labels = data.block(0, 0, data.rows(), 1);
	Eigen::MatrixXd points = data.block(0, 1, data.rows(), data.cols() - 1);
	
	// データが何次元か(dim)と、データ数(num)を確認
	int dim = points.cols();
	int num = points.rows();
	
	// ラベルは-1か1しか仮定していないので、それを確認
	for(int i = 0; i < num; ++i){
		if(labels(i) != 1.0 && labels(i) != -1.0){
			std::cerr << "Label (first column) must be either 1 or -1." << std::endl;
			return -1;
		}
	}
	
	// 2次元の場合、
	// ・点(p, q)のラベルが -1 (赤) と付いている場合、
	//   分類成功となる条件は ap + bq - 1 > 0 であり、
	//   最小化したい関数は (-(ap + bq - 1))+ と定める。
	// ・点(p, q)のラベルが +1 (青) と付いている場合、
	//   分類成功となる条件は ap + bq - 1 < 0 であり、
	//   最小化したい関数は (ap + bq - 1)+ と定める。
	// ただし、(z)+ = max{0, z} である（0を下回る場合は0にする）。
	// 
	// これらをまとめて、最小化したい関数を c(ap + bq - 1)+ と定める。
	// ただし、cはラベルである。
	// 
	// この条件のもとで、関数値を最小化するような(a, b)を求める。
	// 3次元以上の場合、変数の数が増えるだけで同様である。
	// 
	// さて、この関数をaについて最小化することを考える（bについても同様）。
	// (z)+ は、一点を境に繋いだ半直線2本からなる。
	// 
	// ＼
	//   ＼
	//     ＼＿＿＿＿  c(ap + bq - 1) (cp < 0)
	//            →a
	// 
	//             ／
	//           ／
	// ＿＿＿＿／      c(ap + bq - 1) (cp > 0)
	//            →a
	// 
	// そのため、単に c(ap + bq - 1) = 0、すなわち a = (1 - bq)/p となる
	// 場所を選べばよい。
	// なお実際には、「データ点すべてについてこの関数を足し合わせたもの」を
	// 最小化しないとならないので、各データ点(p, q)について (1 - bq)/p を
	// 求めておくことはもちろん、その (1 - bq)/p のうちどこが最小値を取るのかを
	// 計算できないとならない。
	// 以下では、この (1 - bq)/p を「ブレークポイント」と呼び、全データ点に
	// 対するブレークポイントをソートして保持しておくものとする。
	// 
	// 最小値を求めるには、導関数を用いればよい。g(a) = c(ap + bq - 1) を微分すると
	// g'(a) = cp になるので、f(a) = (c(ap + bq - 1))+ の導関数は
	// ・もし (c(ap + bq - 1))+ = 0 ならば、f'(a) = 0
	// ・もし (c(ap + bq - 1))+ > 0 ならば、f'(a) = cp
	// となる。
	// ちなみに、導関数は以下のようになる。
	// 
	// [元の関数]
	// 
	// ＼    cp < 0            cp > 0    ／
	//   ＼                            ／
	//     ＼＿＿＿＿        ＿＿＿＿／
	//            →a                   →a
	//       ↓導関数              ↓導関数
	// 
	//                               ￣￣￣ cp
	//       ──── 0      ────       0
	// ＿＿＿         cp
	// 
	// ところで、実際に求めたい導関数は、これを全データ点に対する導関数について
	// 加算したものである。つまり、
	// ・cp < 0 となるブレークポイントがあったら、それ以下のaについて、
	//   導関数の値をcpだけ加算する
	// ・cp > 0 となるブレークポイントがあったら、それ以上のaについて、
	//   導関数の値をcpだけ加算する
	// とすれば導関数を求められる。
	// ただ、これは不都合が大きい（ブレークポイントの先と後の両方に行き来して
	// 導関数の値を加算しないとならない）。そこで、必ず先だけを見ることのできる
	// 以下の方法を採用する。
	// ただし、ブレークポイントbからb+1までの間の導関数を d[b] とかく。
	// ・M = 0
	// ・ブレークポイントの小さいものから順に以下のことを行う。
	//   ・cp < 0 となる場合、M += cp, d[b] = d[b-1] + |cp| （|・|は絶対値）
	//   ・cp > 0 となる場合、d[b] = d[b-1] + cp
	// ・すべてのbについて、d[b] += M
	// 
	// この導関数の値が0をまたいだ場所を検出すればよい。
	
	// 最適化のために動かしたい変数（a, bに相当）
	Eigen::VectorXd coefs = Eigen::VectorXd::Zero(dim);
	
	// ブレークポイント
	// std::pair は「データ点の番号、ブレークポイントの値」の順
	std::vector< std::pair<int, double> > breakpoints(num);
	
	// 導関数
	std::vector< double > derivative(num);
	double derivative_base;
	
	int steps = 0;
	for(;;){
		++steps;
		std::cerr << "Computing step " << steps << "; ";
		display_classifier(coefs, dim, std::cerr);
		
		// coefsを更新した量のうち最大のもの
		// これが一定の値を下回ったら、計算を打ち切る
		double max_update = 0.0;
		
		// 各次元について、以下のことを行う
		for(int d = 0; d < dim; ++d){
			// ブレークポイントを計算する。
			// c(ap + bq - 1) = 0 （2次元）の場合は a = (1 - bq)/p とすればよいが
			// 一般の次元数の場合は、c(ap + bq + b'q' + b''q'' + ... - 1) = 0 つまり
			// a = (1 - bq - b'q' - b''q'' - ...)/p という計算になる。
			for(int i = 0; i < num; ++i){
				breakpoints[i].first = i;
				breakpoints[i].second = 1;
				for(int j = 0; j < dim; ++j){
					if(j == d) continue;
					breakpoints[i].second -= coefs(j) * points(i, j);
				}
				breakpoints[i].second /= points(i, d);
			}
			
			// ブレークポイントをソートする
			std::sort(breakpoints.begin(), breakpoints.end(),
				[](const std::pair<int, double> & a, const std::pair<int, double> & b){
					return(a.second < b.second);
				});
			
			// ソートした順に、傾き（導関数）を求める
			derivative_base = 0.0;
			for(int i = 0; i < num; ++i){
				// いま注目しているデータ点
				int id = breakpoints[i].first;
				
				// 上記 cp の値を求める
				double deriv_single = labels(id) * points(id, d);
				
				// 導関数の値を加算する
				if(i == 0){
					derivative[i] = std::fabs(deriv_single);
				}else{
					derivative[i] = derivative[i-1] + std::fabs(deriv_single);
				}
				if(deriv_single < 0) derivative_base += deriv_single;
			}
			
			// 導関数が0を超えた（＝最小値を取るときの）ブレークポイントを探す。
			int min_breakpoint;
			for(min_breakpoint = 0; min_breakpoint < num; ++min_breakpoint){
				if(derivative[min_breakpoint] + derivative_base >= 0.0) break;
			}
			
			if(min_breakpoint == num){
				// これは起きないはずなのだが念のためチェック
				std::cerr << "Unexpected Error!" << std::endl;
				return -1;
			}
			
			// 係数（上記解説のaやb）を更新。見つかったブレークポイントの値にする
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



