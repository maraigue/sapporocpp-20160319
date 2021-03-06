# このプログラムについて

このプログラムは、2016年3月19日の[札幌C++勉強会 #11](http://sapporocpp.connpass.com/event/26937/ "札幌C++勉強会 #11 - connpass")における、H.Hiroの発表「関数の最小値を求めることから、機械学習へ」のサンプルプログラムです。

# 動かし方

ビルド環境はMakefileのみ用意しています。これは主にEigenのパスを指定するために使っています。別の環境では適宜読み替えた上で設定してください。

-   行列演算ライブラリ「[Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page "Eigen")」をダウンロードし、適当なディレクトリに置いてください（ヘッダのみのライブラリのため、そのまま使えます）。
-   `Makefile`の`-I$(HOME)/local/eigen-eigen-b30b87236a1b/Eigen`という部分がEigenの存在する場所を指定するオプションになっているため、それを適宜書き換えてください。

# 各プログラムの解説

-  1variable-minimization.cpp
   -   1変数関数を最小化するサンプルです。ニュートン法・勾配降下法の2つを実装しています。
-  multi-variable-minimization.cpp
   -   2変数関数を最小化するサンプルです。準ニュートン法を実装しています。
-  compute-border.cpp
   -   2クラス分類境界を得るプログラムです。詳細は発表資料をご覧ください。（双対問題を利用せずに直接境界の式を得る実装にしています。なので大量のデータに対して利用する場合は効率が悪いと思われます）
